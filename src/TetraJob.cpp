/*
 * TetraJob.cpp
 *
 * Worker process protocol (all values in QDataStream format):
 *
 * input file:  magic "QTMI", version, method id (QString),
 *              parameters (QVariantMap), number of vertices (quint64),
 *              vertices (3 floats each), number of triangles (quint64),
 *              triangles (3 uint32 each)
 * output file: magic "QTMO", version, status (qint32, 0 = success),
 *              message (QString), used parameters (QVariantMap, e.g. the
 *              automatically chosen sizes), number of vertices (quint64),
 *              vertices, number of tetrahedra (quint64), tetrahedra
 *              (4 uint32 each)
 *
 * The output file is written with QSaveFile, so it only exists if it is
 * complete.
 */

#include "TetraJob.hpp"

#include <QCoreApplication>
#include <QDataStream>
#include <QFile>
#include <QRegularExpression>
#include <QSaveFile>
#include <QtConcurrent/QtConcurrent>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <stdexcept>
#include <thread>
#include <tuple>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "CGALTetrahedralize.h"
#include "GMSHWrapper.h"
#include "GenericIsosurfaceStuffing.h"
#include "QGLTetraMesh.hpp"
#include "SofaTetraStuffing.h"
#include "TetgenWrapper.h"
#ifdef QTM_HAVE_FTETWILD
#include "FTetWildWrapper.h"
#endif

namespace {

const quint32 kInputMagic = 0x51544D49;   // "QTMI"
const quint32 kOutputMagic = 0x51544D4F;  // "QTMO"
const quint32 kVersion = 2;
const char *kWorkerArgument = "--tetra-worker";
const char *kLoadGMSHMethod = "load_gmsh";

void prepareStream(QDataStream &stream) {
  stream.setVersion(QDataStream::Qt_6_0);
  stream.setByteOrder(QDataStream::LittleEndian);
}

template <typename T>
void writeArray(QDataStream &stream, const std::vector<T> &data) {
  stream << quint64(data.size());
  if (!data.empty())
    stream.writeRawData(reinterpret_cast<const char *>(data.data()),
                        qint64(data.size() * sizeof(T)));
}

/// reads an array written by writeArray(); refuses sizes that do not fit
/// into the file so that a damaged file cannot cause a huge allocation
template <typename T>
bool readArray(QDataStream &stream, const QFile &file, std::vector<T> &data) {
  quint64 count = 0;
  stream >> count;
  if (stream.status() != QDataStream::Ok) return false;
  const quint64 remaining = quint64(file.size() - file.pos());
  if (count > remaining / sizeof(T)) return false;
  data.resize(count);
  if (count == 0) return true;
  const qint64 bytes = qint64(count * sizeof(T));
  return stream.readRawData(reinterpret_cast<char *>(data.data()), bytes) ==
         bytes;
}

struct WorkerInput {
  QString methodId;
  QVariantMap parameters;
  std::vector<Vec3f> vertices;
  std::vector<Triangle> triangles;
};

bool writeInput(const QString &fileName, const QString &methodId,
                const QVariantMap &parameters,
                const std::vector<Vec3f> &vertices,
                const std::vector<Triangle> &triangles) {
  QSaveFile file(fileName);
  if (!file.open(QIODevice::WriteOnly)) return false;
  QDataStream stream(&file);
  prepareStream(stream);
  stream << kInputMagic << kVersion << methodId << parameters;
  writeArray(stream, vertices);
  writeArray(stream, triangles);
  return stream.status() == QDataStream::Ok && file.commit();
}

bool readInput(const QString &fileName, WorkerInput &input) {
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly)) return false;
  QDataStream stream(&file);
  prepareStream(stream);
  quint32 magic = 0, version = 0;
  stream >> magic >> version;
  if (magic != kInputMagic || version != kVersion) return false;
  stream >> input.methodId >> input.parameters;
  return stream.status() == QDataStream::Ok &&
         readArray(stream, file, input.vertices) &&
         readArray(stream, file, input.triangles);
}

bool writeOutput(const QString &fileName, qint32 status,
                 const QString &message, const QVariantMap &usedParameters,
                 const std::vector<Vec3f> &vertices,
                 const std::vector<Tetrahedron> &tetras) {
  QSaveFile file(fileName);
  if (!file.open(QIODevice::WriteOnly)) return false;
  QDataStream stream(&file);
  prepareStream(stream);
  stream << kOutputMagic << kVersion << status << message << usedParameters;
  writeArray(stream, vertices);
  writeArray(stream, tetras);
  return stream.status() == QDataStream::Ok && file.commit();
}

bool readOutput(const QString &fileName, qint32 &status, QString &message,
                QVariantMap &usedParameters, std::vector<Vec3f> &vertices,
                std::vector<Tetrahedron> &tetras) {
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly)) return false;
  QDataStream stream(&file);
  prepareStream(stream);
  quint32 magic = 0, version = 0;
  stream >> magic >> version;
  if (magic != kOutputMagic || version != kVersion) return false;
  stream >> status >> message >> usedParameters;
  return stream.status() == QDataStream::Ok &&
         readArray(stream, file, vertices) && readArray(stream, file, tetras);
}

/// Merges vertices with identical coordinates. Loaded meshes often have
/// split vertices where normals or texture coordinates change (e.g. at the
/// sharp edges of a cylinder). Such a surface is open in terms of its
/// indices, which makes Tetgen fail and GMSH run (nearly) forever, although
/// the surface check (which welds as well) reports a closed surface.
void weldVertices(const std::vector<Vec3f> &verts,
                  const std::vector<Triangle> &tris,
                  std::vector<Vec3f> &weldedVerts,
                  std::vector<Triangle> &weldedTris) {
  std::map<std::tuple<float, float, float>, uint32_t> seen;
  std::vector<uint32_t> remap(verts.size());
  weldedVerts.clear();
  weldedVerts.reserve(verts.size());
  for (std::size_t i = 0; i < verts.size(); ++i) {
    const auto key = std::make_tuple(verts[i].x, verts[i].y, verts[i].z);
    auto it = seen.find(key);
    if (it == seen.end()) {
      it = seen.emplace(key, uint32_t(weldedVerts.size())).first;
      weldedVerts.push_back(verts[i]);
    }
    remap[i] = it->second;
  }
  weldedTris = tris;
  for (Triangle &t : weldedTris)
    for (int k = 0; k < 3; ++k) t.index[k] = remap[t.index[k]];
}

/// runs the requested method in the worker process, usedParameters gets
/// the automatically chosen values of parameters given as 0
void runMethod(const WorkerInput &in, std::vector<Vec3f> &vertices,
               std::vector<Tetrahedron> &tetras, QVariantMap &usedParameters) {
  const QVariantMap &p = in.parameters;
  auto real = [&p](const char *key) { return p.value(key).toDouble(); };
  auto flag = [&p](const char *key) { return p.value(key).toBool(); };
  if (in.methodId == kLoadGMSHMethod) {
    GMSHWrapper w;
    if (!w.loadAsGMSH(p.value("fileName").toString().toStdString()))
      throw std::runtime_error(w.GetLastError());
    vertices = std::move(w.GetTetraVertices());
    tetras = std::move(w.GetTetras());
    return;
  }
  for (const Triangle &t : in.triangles)
    for (int k = 0; k < 3; ++k)
      if (t.index[k] >= in.vertices.size())
        throw std::runtime_error(
            "The surface mesh has invalid vertex indices.");
  std::vector<Vec3f> verts;
  std::vector<Triangle> tris;
  weldVertices(in.vertices, in.triangles, verts, tris);
  if (verts.size() != in.vertices.size())
    std::cout << "Tetra worker: merged " << in.vertices.size() - verts.size()
              << " duplicate surface vertices." << std::endl;

  if (in.methodId == "sofa") {
    SofaTetraStuffing w;
    w.GenerateFromSurface(tris, verts, float(real("size")),
                          float(real("alphaShort")), float(real("alphaLong")),
                          flag("snapToPoints"), flag("splitTetras"));
    vertices = std::move(w.GetTetraVertices());
    tetras = std::move(w.GetTetras());
  } else if (in.methodId == "cgal") {
    CGALTetrahedralize w;
    w.GenerateFromSurface(tris, verts, real("cellSize"), real("facetAngle"),
                          real("facetSize"), real("facetDistance"),
                          real("cellRadiusEdgeRatio"));
    vertices = std::move(w.GetTetraVertices());
    tetras = std::move(w.GetTetras());
  } else if (in.methodId == "generic") {
    GenericIsosurfaceStuffing w;
    w.GenerateFromSurface(tris, verts, float(real("cellSize")),
                          float(real("alphaShort")), float(real("alphaLong")),
                          flag("graded"));
    vertices = std::move(w.GetTetraVertices());
    tetras = std::move(w.GetTetras());
  } else if (in.methodId == "tetgen") {
    TetgenWrapper w;
    w.GenerateFromSurface(tris, verts, float(real("qualityBound")),
                          float(real("cellSize")), flag("usePLC"));
    usedParameters["cellSize"] = double(w.GetCellSize());
    vertices = std::move(w.GetTetraVertices());
    tetras = std::move(w.GetTetras());
  } else if (in.methodId == "gmsh") {
    GMSHWrapper w;
    w.GenerateFromSurface(tris, verts, real("meshSizeMin"),
                          real("meshSizeMax"),
                          p.value("algorithm3D").toInt());
    usedParameters["meshSizeMin"] = w.GetMeshSizeMin();
    usedParameters["meshSizeMax"] = w.GetMeshSizeMax();
    vertices = std::move(w.GetTetraVertices());
    tetras = std::move(w.GetTetras());
#ifdef QTM_HAVE_FTETWILD
  } else if (in.methodId == "ftetwild") {
    FTetWildWrapper w;
    w.GenerateFromSurface(tris, verts, real("edgeLengthRel"),
                          real("epsilonRel"), real("stopEnergy"),
                          flag("coarsen"));
    vertices = std::move(w.GetTetraVertices());
    tetras = std::move(w.GetTetras());
#endif
  } else {
    throw std::runtime_error("Unknown tetrahedralization method '" +
                             in.methodId.toStdString() + "'.");
  }
}

/// Ends the worker as soon as the main program is gone. The main program
/// keeps the worker's stdin pipe open (it never writes to it); when it exits
/// or crashes, the operating system closes the pipe and the read returns.
/// Without this, a crashed main program would leave a possibly long running
/// worker behind.
void startParentWatchdog() {
  std::thread([]() {
    char buffer[64];
#ifdef _WIN32
    HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
    DWORD bytesRead = 0;
    while (ReadFile(input, buffer, sizeof(buffer), &bytesRead, NULL) &&
           bytesRead > 0) {
    }
#else
    while (::read(0, buffer, sizeof(buffer)) > 0) {
    }
#endif
    std::cerr << "Tetra worker: main program is gone, exiting." << std::endl;
    std::_Exit(4);
  }).detach();
}

}  // namespace

bool TetraWorker::isWorkerInvocation(int argc, char *argv[]) {
  return argc >= 2 && std::strcmp(argv[1], kWorkerArgument) == 0;
}

int TetraWorker::run(int argc, char *argv[]) {
  // only used for the correct (unicode) decoding of the file names
  QCoreApplication app(argc, argv);
  const QStringList args = QCoreApplication::arguments();
  if (args.size() < 4) {
    std::cerr << "Usage: QTetraMesher " << kWorkerArgument
              << " <input file> <output file>" << std::endl;
    return 2;
  }
  const QString inputFile = args.at(2);
  const QString outputFile = args.at(3);
  // stdout is a pipe here, which stdio would buffer in large blocks; the
  // main program shows the output live
  std::setvbuf(stdout, nullptr, _IONBF, 0);
  startParentWatchdog();

  qint32 status = 1;
  QString message;
  std::vector<Vec3f> vertices;
  std::vector<Tetrahedron> tetras;
  QVariantMap usedParameters;
  WorkerInput input;
  if (!readInput(inputFile, input)) {
    message = "The worker process could not read its input file.";
  } else {
    try {
      runMethod(input, vertices, tetras, usedParameters);
      if (vertices.empty() || tetras.empty())
        message = "The method did not generate any tetrahedra.";
      else
        status = 0;
    } catch (const std::exception &e) {
      message = QString::fromStdString(e.what());
    } catch (int e) {
      // Tetgen reports errors by throwing an int
      message = QString("The method failed with error code %1.").arg(e);
    } catch (...) {
      message = "The method failed with an unknown error.";
    }
  }
  if (status != 0) {
    vertices.clear();
    tetras.clear();
    std::cerr << "Tetra worker: " << message.toStdString() << std::endl;
  }
  std::cout.flush();
  if (!writeOutput(outputFile, status, message, usedParameters, vertices,
                   tetras)) {
    std::cerr << "Tetra worker: could not write the output file."
              << std::endl;
    return 3;
  }
  return status == 0 ? 0 : 1;
}

TetraJob::TetraJob(QObject *parent) : QObject(parent) {
  connect(&watcher, &QFutureWatcher<std::shared_ptr<Result>>::finished, this,
          &TetraJob::collected);
}

TetraJob::~TetraJob() {
  if (process) {
    disconnect(process, nullptr, this, nullptr);
    process->kill();
    process->waitForFinished(5000);
  }
  // the collector thread only uses its own data, but must not outlive us
  watcher.waitForFinished();
}

bool TetraJob::start(const QString &methodId_, const QString &methodName_,
                     const QVariantMap &parameters_,
                     const std::vector<Vec3f> &vertices_,
                     const std::vector<Triangle> &triangles_,
                     QString *error_) {
  auto fail = [error_](const QString &message) {
    if (error_) *error_ = message;
    return false;
  };
  if (vertices_.empty() || triangles_.empty())
    return fail("The surface mesh is empty.");
  return launch(methodId_, methodName_, parameters_, vertices_, triangles_,
                error_);
}

bool TetraJob::startLoadGMSH(const QString &fileName_, QString *error_) {
  return launch(kLoadGMSHMethod, "Load GMSH", {{"fileName", fileName_}}, {},
                {}, error_);
}

bool TetraJob::isLoading() const {
  return isRunning() && currentMethodId == kLoadGMSHMethod;
}

bool TetraJob::launch(const QString &methodId_, const QString &methodName_,
                      const QVariantMap &parameters_,
                      const std::vector<Vec3f> &vertices_,
                      const std::vector<Triangle> &triangles_,
                      QString *error_) {
  auto fail = [error_](const QString &message) {
    if (error_) *error_ = message;
    return false;
  };
  if (isRunning()) return fail("A tetrahedralization is already running.");

  tempDir = std::make_unique<QTemporaryDir>();
  if (!tempDir->isValid())
    return fail("Could not create a temporary directory.");
  const QString inputFile = tempDir->filePath("input.bin");
  outputFile = tempDir->filePath("output.bin");
  if (!writeInput(inputFile, methodId_, parameters_, vertices_, triangles_)) {
    tempDir.reset();
    return fail("Could not write the input file for the worker process.");
  }

  process = new QProcess(this);
  // the worker's console output (Tetgen, GMSH, ...) is read and echoed to
  // our console; its stdin stays an open pipe, the worker exits when the
  // pipe closes
  process->setProcessChannelMode(QProcess::SeparateChannels);
  stdoutChannel = OutputChannel();
  stderrChannel = OutputChannel();
  connect(process, &QProcess::readyReadStandardOutput, this,
          [this]() { readProcessOutput(false); });
  connect(process, &QProcess::readyReadStandardError, this,
          [this]() { readProcessOutput(false); });
  connect(process, &QProcess::finished, this, &TetraJob::processFinished);
  connect(process, &QProcess::errorOccurred, this, &TetraJob::processError);

  state = State::Running;
  cancelRequested = false;
  currentMethodId = methodId_;
  currentMethodName = methodName_;
  timer.start();
  process->start(QCoreApplication::applicationFilePath(),
                 {kWorkerArgument, inputFile, outputFile});
  return true;
}

void TetraJob::cancel() {
  if (state == State::Idle) return;
  cancelRequested = true;
  // during the collection the result is simply discarded
  if (state == State::Running && process) process->kill();
}

void TetraJob::processError(QProcess::ProcessError error) {
  // all other errors are followed by finished()
  if (error != QProcess::FailedToStart || state != State::Running) return;
  auto r = std::make_unique<Result>();
  r->error = "Could not start the worker process: " + process->errorString();
  process->deleteLater();
  process = nullptr;
  finish(std::move(r));
}

void TetraJob::processFinished(int exitCode,
                               QProcess::ExitStatus exitStatus) {
  const qint64 elapsed = timer.elapsed();
  readProcessOutput(true);
  process->deleteLater();
  process = nullptr;
  if (cancelRequested) {
    auto r = std::make_unique<Result>();
    r->canceled = true;
    r->elapsedMs = elapsed;
    finish(std::move(r));
    return;
  }
  if (exitStatus == QProcess::CrashExit) {
    auto r = std::make_unique<Result>();
    r->error = "The tetrahedralization crashed. The surface mesh is probably "
               "not suitable for this method.";
    r->elapsedMs = elapsed;
    finish(std::move(r));
    return;
  }
  // read the result and build the topology in a background thread
  state = State::Collecting;
  const QString file = outputFile;
  const bool buildSurface = (currentMethodId == kLoadGMSHMethod);
  watcher.setFuture(QtConcurrent::run([file, exitCode, elapsed,
                                       buildSurface]() {
    auto r = std::make_shared<Result>();
    r->elapsedMs = elapsed;
    qint32 status = 1;
    QString message;
    std::vector<Vec3f> vertices;
    std::vector<Tetrahedron> tetras;
    if (!readOutput(file, status, message, r->usedParameters, vertices,
                    tetras)) {
      r->error = QString("The worker process ended (exit code %1) without "
                         "a valid result.")
                     .arg(exitCode);
      return r;
    }
    if (status != 0) {
      r->error = message;
      return r;
    }
    try {
      r->topology = QGLTetraMesh::BuildTetraTopology(vertices, tetras);
    } catch (const std::exception &e) {
      r->error = QString::fromStdString(e.what());
      return r;
    }
    if (!r->topology) {
      r->error = "The generated tetrahedral mesh is invalid.";
      return r;
    }
    if (buildSurface) {
      r->surface = std::make_unique<TetraTools::TriangleTopology>();
      r->surface->Init(r->topology->GetVertices(),
                       r->topology->GetSurfaceTriangles(), true);
      r->surface->GenerateNormals();
    }
    r->ok = true;
    return r;
  }));
}

void TetraJob::collected() {
  std::shared_ptr<Result> shared = watcher.result();
  auto r = std::make_unique<Result>();
  r->ok = shared->ok;
  r->error = shared->error;
  r->elapsedMs = shared->elapsedMs;
  r->usedParameters = shared->usedParameters;
  r->topology = std::move(shared->topology);
  r->surface = std::move(shared->surface);
  if (cancelRequested) {
    r->ok = false;
    r->canceled = true;
    r->topology.reset();
    r->surface.reset();
  }
  finish(std::move(r));
}

void TetraJob::readProcessOutput(bool flush) {
  if (!process) return;
  forwardOutput(process->readAllStandardOutput(), false, flush);
  forwardOutput(process->readAllStandardError(), true, flush);
}

void TetraJob::forwardOutput(const QByteArray &data, bool isError,
                             bool flush) {
  if (!data.isEmpty()) {
    std::FILE *console = isError ? stderr : stdout;
    std::fwrite(data.constData(), 1, size_t(data.size()), console);
    std::fflush(console);
  }
  static const QRegularExpression ansiEscape("\x1b\\[[0-9;?]*[A-Za-z]");
  OutputChannel &channel = isError ? stderrChannel : stdoutChannel;
  channel.pending += data;
  auto emitLine = [&](const QByteArray &bytes, bool endedWithCR) {
    QString line = QString::fromLocal8Bit(bytes);
    line.remove(ansiEscape);
    line.replace('\t', "    ");
    if (!line.trimmed().isEmpty() || endedWithCR)
      emit outputLine(line, isError, channel.replaceNext);
    channel.replaceNext = endedWithCR;
  };
  qsizetype start = 0;
  for (qsizetype i = 0; i < channel.pending.size(); ++i) {
    const char c = channel.pending.at(i);
    if (c != '\n' && c != '\r') continue;
    // a '\r' as the last byte may be the first half of "\r\n"
    if (c == '\r' && i + 1 == channel.pending.size() && !flush) break;
    const bool crlf = c == '\r' && i + 1 < channel.pending.size() &&
                      channel.pending.at(i + 1) == '\n';
    emitLine(channel.pending.mid(start, i - start), c == '\r' && !crlf);
    if (crlf) ++i;
    start = i + 1;
  }
  channel.pending.remove(0, start);
  if (flush && !channel.pending.isEmpty()) {
    emitLine(channel.pending, false);
    channel.pending.clear();
  }
}

void TetraJob::finish(std::unique_ptr<Result> result_) {
  state = State::Idle;
  tempDir.reset();  // removes the input and output files
  result = std::move(result_);
  emit finished();
}
