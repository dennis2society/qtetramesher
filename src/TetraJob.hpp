/*
 * TetraJob.hpp
 *
 * Runs a tetrahedralization method without blocking the user interface.
 *
 * The method runs in a separate worker process: QTetraMesher started again
 * with "--tetra-worker <input> <output>". The surface mesh and the parameters
 * are passed in a temporary file, the result comes back in another one.
 * Using a process instead of a thread allows a real cancel (the process is
 * killed, which also works when a 3rd-party library hangs), and a crash or
 * memory leak inside a library cannot affect the main program.
 *
 * After the worker has finished, the result is read and its topology
 * (edges, triangles, ...) is built in a background thread, so that large
 * meshes do not freeze the user interface either.
 *
 * Loading a GMSH file uses the same mechanism (startLoadGMSH()), as reading
 * and preparing large meshes takes a while, too.
 */

#ifndef TETRA_JOB_HPP
#define TETRA_JOB_HPP

#include <QElapsedTimer>
#include <QFutureWatcher>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QTemporaryDir>
#include <QVariantMap>
#include <memory>
#include <vector>

#include "TetraMeshTools/GeometryTypes.h"
#include "TetraMeshTools/TetrahedronTopology.h"

namespace TetraWorker {
/// true if the program was started as a tetrahedralization worker
bool isWorkerInvocation(int argc, char *argv[]);
/// entry point of the worker process, returns the process exit code
int run(int argc, char *argv[]);
}  // namespace TetraWorker

class TetraJob : public QObject {
  Q_OBJECT

 public:
  struct Result {
    bool ok = false;
    bool canceled = false;
    QString error;
    qint64 elapsedMs = 0;
    /// values the method chose for parameters given as 0 (= auto)
    QVariantMap usedParameters;
    std::unique_ptr<TetraTools::TetrahedronTopology> topology;
    /// only set by startLoadGMSH(): the surface of the loaded tetra mesh
    std::unique_ptr<TetraTools::TriangleTopology> surface;
  };

  explicit TetraJob(QObject *parent = nullptr);
  /// kills a running worker and waits for the result collection
  ~TetraJob() override;

  bool isRunning() const { return state != State::Idle; }
  QString methodName() const { return currentMethodName; }

  /// Starts the worker. The surface data is copied, it may change after
  /// this call returns. Returns false (and sets error_) if the worker could
  /// not be started.
  bool start(const QString &methodId_, const QString &methodName_,
             const QVariantMap &parameters_,
             const std::vector<Vec3f> &vertices_,
             const std::vector<Triangle> &triangles_, QString *error_);

  /// Starts loading the tetrahedra of a GMSH .msh file in the worker. The
  /// result also contains the surface of the loaded mesh.
  bool startLoadGMSH(const QString &fileName_, QString *error_);

  /// true while the running job loads a file instead of generating tetras
  bool isLoading() const;

  /// Stops a running job. finished() is emitted with a canceled result.
  void cancel();

  /// The result of the last job, valid after finished() was emitted.
  std::unique_ptr<Result> takeResult() { return std::move(result); }

 signals:
  void finished();
  /// A line of the worker's console output. replaceLast is set if the
  /// previous line of this channel ended with a lone '\r' (progress output).
  void outputLine(const QString &line, bool isError, bool replaceLast);

 private slots:
  void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
  void processError(QProcess::ProcessError error);
  void collected();

 private:
  enum class State { Idle, Running, Collecting };

  bool launch(const QString &methodId_, const QString &methodName_,
              const QVariantMap &parameters_,
              const std::vector<Vec3f> &vertices_,
              const std::vector<Triangle> &triangles_, QString *error_);
  void finish(std::unique_ptr<Result> result_);

  struct OutputChannel {
    QByteArray pending;  // bytes after the last line end
    bool replaceNext = false;
  };
  /// echoes data to our console and emits the complete lines in it
  void forwardOutput(const QByteArray &data, bool isError, bool flush);
  void readProcessOutput(bool flush);

  State state = State::Idle;
  OutputChannel stdoutChannel;
  OutputChannel stderrChannel;
  bool cancelRequested = false;
  QString currentMethodId;
  QString currentMethodName;
  QProcess *process = nullptr;
  std::unique_ptr<QTemporaryDir> tempDir;
  QString outputFile;
  QElapsedTimer timer;
  QFutureWatcher<std::shared_ptr<Result>> watcher;
  std::unique_ptr<Result> result;
};

#endif  // TETRA_JOB_HPP
