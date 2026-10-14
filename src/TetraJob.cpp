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
 *              message (QString), number of vertices (quint64), vertices,
 *              number of tetrahedra (quint64), tetrahedra (4 uint32 each)
 *
 * The output file is written with QSaveFile, so it only exists if it is
 * complete.
 */

#include "TetraJob.hpp"

#include <QCoreApplication>
#include <QDataStream>
#include <QFile>
#include <QSaveFile>
#include <QtConcurrent/QtConcurrent>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <thread>

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

namespace {

const quint32 kInputMagic = 0x51544D49;   // "QTMI"
const quint32 kOutputMagic = 0x51544D4F;  // "QTMO"
const quint32 kVersion = 1;
const char *kWorkerArgument = "--tetra-worker";

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
                 const QString &message, const std::vector<Vec3f> &vertices,
                 const std::vector<Tetrahedron> &tetras) {
  QSaveFile file(fileName);
  if (!file.open(QIODevice::WriteOnly)) return false;
  QDataStream stream(&file);
  prepareStream(stream);
  stream << kOutputMagic << kVersion << status << message;
  writeArray(stream, vertices);
  writeArray(stream, tetras);
  return stream.status() == QDataStream::Ok && file.commit();
}

bool readOutput(const QString &fileName, qint32 &status, QString &message,
                std::vector<Vec3f> &vertices,
                std::vector<Tetrahedron> &tetras) {
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly)) return false;
  QDataStream stream(&file);
  prepareStream(stream);
  quint32 magic = 0, version = 0;
  stream >> magic >> version;
  if (magic != kOutputMagic || version != kVersion) return false;
  stream >> status >> message;
  return stream.status() == QDataStream::Ok &&
         readArray(stream, file, vertices) && readArray(stream, file, tetras);
}

/// runs the requested method in the worker process
void runMethod(const WorkerInput &in, std::vector<Vec3f> &vertices,
               std::vector<Tetrahedron> &tetras) {
  const QVariantMap &p = in.parameters;
  auto real = [&p](const char *key) { return p.value(key).toDouble(); };
  auto flag = [&p](const char *key) { return p.value(key).toBool(); };
  const std::vector<Triangle> &tris = in.triangles;
  const std::vector<Vec3f> &verts = in.vertices;

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
    vertices = std::move(w.GetTetraVertices());
    tetras = std::move(w.GetTetras());
  } else if (in.methodId == "gmsh") {
    GMSHWrapper w;
    w.GenerateFromSurface(tris, verts, real("meshSizeMin"),
                          real("meshSizeMax"),
                          p.value("algorithm3D").toInt());
    vertices = std::move(w.GetTetraVertices());
    tetras = std::move(w.GetTetras());
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
  startParentWatchdog();

  qint32 status = 1;
  QString message;
  std::vector<Vec3f> vertices;
  std::vector<Tetrahedron> tetras;
  WorkerInput input;
  if (!readInput(inputFile, input)) {
    message = "The worker process could not read its input file.";
  } else {
    try {
      runMethod(input, vertices, tetras);
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
  if (!writeOutput(outputFile, status, message, vertices, tetras)) {
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
  if (isRunning()) return fail("A tetrahedralization is already running.");
  if (vertices_.empty() || triangles_.empty())
    return fail("The surface mesh is empty.");

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
  // the worker's console output (Tetgen, GMSH, ...) goes to our console;
  // its stdin stays an open pipe, the worker exits when the pipe closes
  process->setProcessChannelMode(QProcess::ForwardedChannels);
  connect(process, &QProcess::finished, this, &TetraJob::processFinished);
  connect(process, &QProcess::errorOccurred, this, &TetraJob::processError);

  state = State::Running;
  cancelRequested = false;
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
  watcher.setFuture(QtConcurrent::run([file, exitCode, elapsed]() {
    auto r = std::make_shared<Result>();
    r->elapsedMs = elapsed;
    qint32 status = 1;
    QString message;
    std::vector<Vec3f> vertices;
    std::vector<Tetrahedron> tetras;
    if (!readOutput(file, status, message, vertices, tetras)) {
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
  r->topology = std::move(shared->topology);
  if (cancelRequested) {
    r->ok = false;
    r->canceled = true;
    r->topology.reset();
  }
  finish(std::move(r));
}

void TetraJob::finish(std::unique_ptr<Result> result_) {
  state = State::Idle;
  tempDir.reset();  // removes the input and output files
  result = std::move(result_);
  emit finished();
}
