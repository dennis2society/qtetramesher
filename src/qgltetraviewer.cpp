/*
 * qgltetraviewer.cpp
 *
 * Derived from QGLViewer to allow custom mesh drawing.
 *
 *  Created 2012
 *      Author: Dennis Luebke
 */

#include "qgltetraviewer.hpp"

#include <math.h>
#include <qtetramesher_version.h>

#include <QColorDialog>
#include <QDateTime>
#include <QFileDialog>
#include <QFontDatabase>
#include <QDialog>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QSettings>
#include <QPainter>
#include <algorithm>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QtmGLViewer/MeshData.hpp>
#include <sstream>
#include <string>

// Constructor must call the base class constructor.
QGLTetraViewer::QGLTetraViewer(QWidget *parent) : QtmGLViewer(parent) {
  tMesh = new QGLTetraMesh();
  taskAnimationTimer.setInterval(100);
  connect(&taskAnimationTimer, &QTimer::timeout, this,
          [this]() { update(); });
  noticeTimer.setSingleShot(true);
  connect(&noticeTimer, &QTimer::timeout, this, [this]() {
    noticeText.clear();
    noticeDetail.clear();
    update();
  });
  jobOutputTimer.setSingleShot(true);
  connect(&jobOutputTimer, &QTimer::timeout, this, [this]() {
    jobOutputVisible = false;
    update();
  });
}

QGLTetraViewer::~QGLTetraViewer() {
  // GPU resources must be released while the context is current
  makeCurrent();
  cleanupScene();
  delete tMesh;
  tMesh = NULL;
  doneCurrent();
}

float QGLTetraViewer::getMaxBBox() const { return _maxBBox; }

void QGLTetraViewer::initializeScene() {
  tMesh->InitGL();
  // tilted so the strip is not seen edge-on or from straight above
  QMatrix4x4 tilt;
  tilt.rotate(-45.0f, 1.0f, 0.0f, 0.0f);
  tilt.rotate(35.0f, 0.0f, 1.0f, 0.0f);
  qtm::MeshData m;
  m.append(qtm::primitives::mobiusStrip(0.8f, -0.25f, 0.25f, 400), tilt);
  mobius.upload(m, GL_TRIANGLES);

  // black ribbons along the edges, next to the strip so nothing z-fights
  qtm::MeshData outline;
  outline.append(qtm::primitives::mobiusStrip(0.8f, 0.25f, 0.27f, 400), tilt);
  outline.append(qtm::primitives::mobiusStrip(0.8f, -0.27f, -0.25f, 400),
                 tilt);
  outline.normals.clear();
  outline.colors.clear();
  mobiusOutline.upload(outline, GL_TRIANGLES);
}

void QGLTetraViewer::cleanupScene() {
  if (tMesh != NULL) tMesh->CleanupGL();
  mobius.destroy();
  mobiusOutline.destroy();
}

void QGLTetraViewer::draw(const qtm::RenderContext &context) {
  if (tMesh->IsReady()) {
    BoundingBox bb = tMesh->GetBoundingBox();
    tMesh->Draw(context);
    std::stringstream ss;
    std::string stdMSG;
    QString msg;
    int textY = 30;
    if (tMesh->GetSurface() != NULL) {
      ss << "Surface Mesh: "
         << "Vertices: " << tMesh->GetSurface()->GetVertices().size()
         << "; Triangles: " << tMesh->GetSurface()->GetTriangles().size();
      stdMSG = ss.str();
      msg = QString::fromStdString(stdMSG);
      drawText(10, 30, msg, QColor::fromRgbF(0.1f, 0.8f, 0.1f));
      ss.str("");
      _maxBBox = bb.max.x - bb.min.x;
      if (_maxBBox < (bb.max.y - bb.min.y)) _maxBBox = bb.max.y - bb.min.y;
      if (_maxBBox < (bb.max.z - bb.min.z)) _maxBBox = bb.max.z - bb.min.z;
      ss << "BBox Min:" << bb.min.x << "/" << bb.min.y << "/" << bb.min.z
         << "; BBox Max: " << bb.max.x << "/" << bb.max.y << "/" << bb.max.z
         << "; Max Value: " << _maxBBox;
      stdMSG = ss.str();
      msg = QString::fromStdString(stdMSG);
      drawText(10, 45, msg, QColor::fromRgbF(0.1f, 0.8f, 0.1f));
      ss.str("");
      textY = 60;
      const TetraTools::MeshTransform &t = tMesh->GetSurfaceTransform();
      if (t.HasOriginal() &&
          (tMesh->IsPreviewingSurfaceTransform() || !t.IsIdentity())) {
        const Vec3f r = t.GetRotation();
        const BoundingBox &ob = t.GetOriginalBoundingBox();
        const Vec3f os = ob.max - ob.min;
        const Vec3f s = bb.max - bb.min;
        const QColor c = QColor::fromRgbF(0.6f, 0.6f, 0.6f);
        drawText(10, textY,
                 QString("Transform: Scale %1; Rotation X/Y/Z: %2/%3/%4 deg")
                     .arg(t.GetScale(), 0, 'g', 4)
                     .arg(r.x)
                     .arg(r.y)
                     .arg(r.z),
                 c);
        drawText(10, textY + 15,
                 QString("Size: %1 x %2 x %3 (original: %4 x %5 x %6)")
                     .arg(s.x, 0, 'g', 4)
                     .arg(s.y, 0, 'g', 4)
                     .arg(s.z, 0, 'g', 4)
                     .arg(os.x, 0, 'g', 4)
                     .arg(os.y, 0, 'g', 4)
                     .arg(os.z, 0, 'g', 4),
                 c);
        textY += 30;
      }
    }
    if (tMesh->GetTetraMesh() != NULL) {
      ss << "Tetra Mesh: "
         << "Vertices: " << tMesh->GetTetraMesh()->GetVertices().size()
         << "; Tetras: " << tMesh->GetTetraMesh()->GetTetrahedra().size();
      stdMSG = ss.str();
      msg = QString::fromStdString(stdMSG);
      drawText(10, textY, msg, QColor::fromRgbF(0.8f, 0.8f, 0.1f));
      ss.str("");
      ss << "Tetra Mesh: "
         << "Triangles: " << tMesh->GetTetraMesh()->GetTriangles().size()
         << "; Edges: " << tMesh->GetTetraMesh()->GetEdges().size();
      stdMSG = ss.str();
      msg = QString::fromStdString(stdMSG);
      drawText(10, textY + 15, msg, QColor::fromRgbF(0.8f, 0.8f, 0.1f));
    }
  } else {
    context.drawLines(mobiusOutline, Qt::black);
    context.drawSolid(mobius, QColor());
  }
}

void QGLTetraViewer::beginTask(const QString &id, const QString &text,
                               const QString &hint) {
  RunningTask &task = runningTasks[id];
  task.text = text;
  task.hint = hint;
  task.timer.start();
  taskAnimationTimer.start();
  update();
}

void QGLTetraViewer::endTask(const QString &id) {
  runningTasks.erase(id);
  if (runningTasks.empty()) taskAnimationTimer.stop();
  update();
}

void QGLTetraViewer::showNotice(const QString &text, const QString &detail,
                                int durationMs) {
  noticeText = text;
  noticeDetail = detail;
  noticeTimer.start(durationMs);
  update();
}

void QGLTetraViewer::drawOverlay(QPainter &painter) {
  drawTaskOverlay(painter);
  drawNoticeOverlay(painter);
  drawJobOutputOverlay(painter);
}

namespace {
const std::size_t kJobOutputLines = 10;
}

void QGLTetraViewer::beginJobOutput() {
  jobOutputTimer.stop();
  jobOutput.clear();
  jobOutputVisible = true;
  update();
}

void QGLTetraViewer::appendJobOutput(const QString &line, bool isError,
                                     bool replaceLast) {
  if (replaceLast && !jobOutput.empty())
    jobOutput.back() = {line, isError};
  else
    jobOutput.push_back({line, isError});
  while (jobOutput.size() > kJobOutputLines) jobOutput.pop_front();
  if (jobOutputEnabled && jobOutputVisible) update();
}

void QGLTetraViewer::endJobOutput(int hideDelayMs) {
  jobOutputTimer.start(hideDelayMs);
}

void QGLTetraViewer::setJobOutputEnabled(bool enabled_) {
  jobOutputEnabled = enabled_;
  update();
}

void QGLTetraViewer::drawJobOutputOverlay(QPainter &painter) {
  if (!jobOutputEnabled || !jobOutputVisible || jobOutput.empty()) return;
  painter.save();
  QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  font.setPointSizeF(painter.font().pointSizeF() * 0.85);
  const QFontMetrics metrics(font);
  const int padding = 8;
  const int maxWidth = std::max(100, this->width() / 2 - 2 * padding);
  std::vector<QString> lines;
  int width = 0;
  for (const OutputLine &l : jobOutput) {
    lines.push_back(metrics.elidedText(l.text, Qt::ElideRight, maxWidth));
    width = std::max(width, metrics.horizontalAdvance(lines.back()));
  }
  width += 2 * padding;
  const int height = int(lines.size()) * metrics.height() + 2 * padding;
  const QRect box(this->width() - width - 10, this->height() - height - 10,
                  width, height);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(Qt::NoPen);
  painter.setBrush(QColor(0, 0, 0, 170));
  painter.drawRoundedRect(box, 6, 6);
  painter.setFont(font);
  int y = box.top() + padding;
  for (std::size_t i = 0; i < lines.size(); ++i) {
    painter.setPen(jobOutput[i].isError ? QColor(240, 130, 130)
                                        : QColor(210, 210, 210));
    painter.drawText(box.left() + padding, y + metrics.ascent(), lines[i]);
    y += metrics.height();
  }
  painter.restore();
}

void QGLTetraViewer::drawNoticeOverlay(QPainter &painter) {
  if (noticeText.isEmpty()) return;
  painter.save();
  painter.setRenderHint(QPainter::Antialiasing);
  QFont textFont = painter.font();
  textFont.setBold(true);
  QFont detailFont = painter.font();
  detailFont.setPointSizeF(detailFont.pointSizeF() * 0.85);
  const QFontMetrics textMetrics(textFont);
  const QFontMetrics detailMetrics(detailFont);
  const int padding = 8;
  // a long detail line is cut to the viewer width
  const int maxWidth = std::max(100, this->width() - 40 - 2 * padding);
  const QString detail =
      detailMetrics.elidedText(noticeDetail, Qt::ElideRight, maxWidth);
  int width = textMetrics.horizontalAdvance(noticeText);
  if (!detail.isEmpty())
    width = std::max(width, detailMetrics.horizontalAdvance(detail));
  width = std::min(width, maxWidth) + 2 * padding;
  const int height = textMetrics.height() +
                     (detail.isEmpty() ? 0 : detailMetrics.height()) +
                     2 * padding;
  // top center, the task indicator uses the upper right corner
  const QRect box((this->width() - width) / 2, 10, width, height);
  painter.setPen(QPen(QColor(90, 180, 90), 1.5));
  painter.setBrush(QColor(0, 0, 0, 170));
  painter.drawRoundedRect(box, 6, 6);
  int y = box.top() + padding;
  painter.setPen(QColor(235, 235, 235));
  painter.setFont(textFont);
  painter.drawText(box.left() + padding, y + textMetrics.ascent(),
                   noticeText);
  y += textMetrics.height();
  if (!detail.isEmpty()) {
    painter.setPen(QColor(170, 170, 170));
    painter.setFont(detailFont);
    painter.drawText(box.left() + padding, y + detailMetrics.ascent(), detail);
  }
  painter.restore();
}

void QGLTetraViewer::drawTaskOverlay(QPainter &painter) {
  if (runningTasks.empty()) return;
  painter.save();
  painter.setRenderHint(QPainter::Antialiasing);
  QFont textFont = painter.font();
  QFont hintFont = textFont;
  hintFont.setPointSizeF(textFont.pointSizeF() * 0.85);
  const QFontMetrics textMetrics(textFont);
  const QFontMetrics hintMetrics(hintFont);
  const int spinner = textMetrics.height();
  const int padding = 8;
  const int gap = 6;

  // layout: one row per task, an optional hint line below its text
  struct Row {
    const RunningTask *task;
    QString text;
  };
  std::vector<Row> rows;
  int width = 0;
  int height = padding;
  for (const auto &entry : runningTasks) {
    const RunningTask &task = entry.second;
    const qint64 seconds = task.timer.elapsed() / 1000;
    const QString text = QString("%1  %2:%3")
                             .arg(task.text)
                             .arg(seconds / 60)
                             .arg(seconds % 60, 2, 10, QChar('0'));
    rows.push_back({&task, text});
    width = std::max(width, textMetrics.horizontalAdvance(text));
    if (!task.hint.isEmpty())
      width = std::max(width, hintMetrics.horizontalAdvance(task.hint));
    height += textMetrics.height() +
              (task.hint.isEmpty() ? 0 : hintMetrics.height()) + padding;
  }
  width += spinner + gap + 2 * padding;
  const QRect box(this->width() - width - 10, 10, width, height);
  painter.setPen(Qt::NoPen);
  painter.setBrush(QColor(0, 0, 0, 170));
  painter.drawRoundedRect(box, 6, 6);

  int y = box.top() + padding;
  for (const Row &row : rows) {
    // rotating arc, one turn per second
    const QRect arc(box.left() + padding, y, spinner, spinner);
    const int startAngle = -int((row.task->timer.elapsed() % 1000) * 360 / 1000);
    painter.setPen(QPen(QColor(240, 180, 60), 2.5, Qt::SolidLine, Qt::RoundCap));
    painter.setBrush(Qt::NoBrush);
    painter.drawArc(arc.adjusted(2, 2, -2, -2), startAngle * 16, 270 * 16);
    const int x = arc.right() + gap;
    painter.setPen(QColor(235, 235, 235));
    painter.setFont(textFont);
    painter.drawText(x, y + textMetrics.ascent(), row.text);
    y += textMetrics.height();
    if (!row.task->hint.isEmpty()) {
      painter.setPen(QColor(170, 170, 170));
      painter.setFont(hintFont);
      painter.drawText(x, y + hintMetrics.ascent(), row.task->hint);
      y += hintMetrics.height();
    }
    y += padding;
  }
  painter.restore();
}

void QGLTetraViewer::showAbout() {
  QMessageBox::about(this, tr("About QTetraMesher"), helpString());
}

void QGLTetraViewer::showHelp() {
  QDialog dialog(this);
  dialog.setWindowTitle(tr("QTetraMesher Help"));
  dialog.resize(680, 620);
  QVBoxLayout *layout = new QVBoxLayout(&dialog);
  QTextBrowser *browser = new QTextBrowser(&dialog);
  browser->setOpenExternalLinks(true);
  browser->setHtml(helpHtml());
  layout->addWidget(browser);
  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close);
  connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
  layout->addWidget(buttons);
  dialog.exec();
}

QString QGLTetraViewer::helpHtml() const {
  return tr(
      "<h2>QTetraMesher Help</h2>"
      "<h3>Quick start</h3>"
      "<ol>"
      "<li>Load a surface mesh (File &gt; Load Surface Mesh) or an existing "
      "tetrahedral mesh (File &gt; Load GMSH / Load Tetgen).</li>"
      "<li>Select a tetra mesh method in the options panel and adjust its "
      "parameters.</li>"
      "<li>Press <i>Generate Tetrahedra</i>. The generation runs in the "
      "background (shown in the upper right corner of the viewer), the "
      "program stays usable. The button turns into <i>Cancel Generation</i> "
      "and stops a method that takes too long. The result is shown in the "
      "viewer and the status line reports the vertex and tetra counts.</li>"
      "<li>Export the result with File &gt; Export GMSH or Export Tetgen.</li>"
      "</ol>"
      "<h3>Loading and exporting</h3>"
      "<table cellspacing=\"4\">"
      "<tr><td><b>Load Surface Mesh</b></td><td>OBJ, STL, OFF, Collada (DAE), 3DS, "
      "PLY and AC3D files. Only the surface is loaded; the mesh is moved so "
      "that its bounding box is centered at the origin.</td></tr>"
      "<tr><td><b>Load GMSH</b></td><td>A *.msh file containing a tetrahedral "
      "mesh. The surface of the volume mesh is shown as the surface "
      "mesh. The file is loaded in the background; 'Cancel Loading' stops "
      "it.</td></tr>"
      "<tr><td><b>Load Tetgen</b></td><td>Select the *.node file. The matching "
      "*.ele file must be in the same folder and have the same base "
      "name.</td></tr>"
      "<tr><td><b>Export Surface</b></td><td>Saves the surface mesh as OBJ, "
      "STL, Collada, 3DS or PLY, depending on the extension you choose.</td></tr>"
      "<tr><td><b>Export GMSH</b></td><td>Saves the tetrahedral mesh as a "
      "*.msh file. Choose the file type GMSH 4.1 (current format) or GMSH "
      "2.2 (for older programs that cannot read 4.x).</td></tr>"
      "<tr><td><b>Export Tetgen</b></td><td>Saves the tetrahedral mesh as a "
      "*.node and *.ele pair. Choose the base name; both files are written "
      "next to each other.</td></tr>"
      "</table>"
      "<h3>Tetra mesh methods</h3>"
      "<table cellspacing=\"4\">"
      "<tr><td><b>SOFA TetraStuffing</b></td><td>Isosurface stuffing from the "
      "SOFA framework (Shewchuk's algorithm). Parameters: TetraSize, AlphaShort "
      "and AlphaLong, Split Tetras and Snap To Points.</td></tr>"
      "<tr><td><b>CGAL Delaunay Triangulation</b></td><td>Delaunay refinement "
      "of the surface with CGAL. Parameters: CellSize, Facet Angle, Facet Size, "
      "Facet Distance and Cell2Edge Ratio.</td></tr>"
      "<tr><td><b>Generic Isosurface Stuffing</b></td><td>Isosurface stuffing "
      "implemented for this program. Parameters: CellSize (smallest tetra "
      "size), AlphaShort, AlphaLong, Grading (0 = uniform; higher values use "
      "larger tetrahedra in the interior and along flat surface parts), Sharp "
      "Edge Angle and Edge Snapping (boundary vertices are moved onto sharp "
      "edges and corners where the tetrahedra stay valid).</td></tr>"
      "<tr><td><b>Tetgen</b></td><td>Constrained Delaunay tetrahedralization. "
      "Parameters: Quality Bound, Cell Size (0 = automatic) and Use PLC.</td>"
      "</tr>"
      "<tr><td><b>GMSH Tetrahedralize</b></td><td>Meshing with GMSH. Parameters: "
      "Mesh Size Min and Max (0 = automatic) and the 3D algorithm (Delaunay or "
      "HXT).</td></tr>"
      "<tr><td><b>fTetWild Tetrahedralize</b></td><td>Robust tetrahedral "
      "meshing that also works with imperfect surfaces (optional module, only "
      "available if QTetraMesher was built with fTetWild). Parameters: Edge "
      "Length and Envelope (relative to the bounding box diagonal), Stop "
      "Energy and Coarsen.</td></tr>"
      "</table>"
      "<p>Tetgen and GMSH need a closed surface without self-intersections. Use "
      "<i>Check Surface Mesh</i> in the surface options to find open edges, "
      "intersecting triangles and other defects before meshing.</p>"
      "<h3>Visualization</h3>"
      "<ul>"
      "<li><b>Surface</b>: solid, wireframe or hidden. Surface and wireframe "
      "colors can be changed with the color buttons.</li>"
      "<li><b>Surface Transform</b>: scale (logarithmic slider, 0.0001 to "
      "10000) and rotate a loaded surface mesh around its center. The "
      "original is kept, <i>Reset Transform</i> restores it. A generated "
      "tetra mesh is removed when the surface changes. Export Surface saves "
      "the transformed mesh.</li>"
      "<li><b>TetraMesh</b>: draw the wireframe and/or the solid tetrahedra, "
      "with their own colors. Tetrahedra on the surface share one color; with "
      "<i>Color Interior by Depth</i> the inner ones fade from the outer to "
      "the inner color the deeper they lie.</li>"
      "<li><b>Cutplanes X, Y, Z</b>: drag the handles to show only the "
      "tetrahedra whose centroid lies between the two planes. This is the way "
      "to look inside a mesh.</li>"
      "<li><b>Octree</b>: generate an octree from the surface with the given "
      "depth and show all cells or only the leaves.</li>"
      "<li><b>View menu</b>: show or hide the axes, the grid, the bounding "
      "box and the console output of a running tetra mesh generation (lower "
      "right corner).</li>"
      "</ul>"
      "<h3>Viewer controls</h3>"
      "<table cellspacing=\"4\">"
      "<tr><td>Left drag</td><td>Rotate around the scene center</td></tr>"
      "<tr><td>Right drag</td><td>Move the view</td></tr>"
      "<tr><td>Middle drag or mouse wheel</td><td>Zoom</td></tr>"
      "<tr><td>Left double click</td><td>Align the view with the axes</td></tr>"
      "<tr><td>Right double click</td><td>Center the scene</td></tr>"
      "<tr><td>Middle double click</td><td>Show the entire scene</td></tr>"
      "<tr><td>A / G / F / O</td><td>Toggle axes / grid / FPS display / "
      "orthographic projection</td></tr>"
      "</table>"
      "<h3>Settings</h3>"
      "<p>Colors, the visualization checkboxes and the view options are saved "
      "when the program closes and restored at the next start.</p>"
      "<p>Usage and build instructions: <a "
      "href=\"https://qtm.dennis2society.de\">qtm.dennis2society.de</a><br>"
      "Source code: <a href=\"https://github.com/dennis2society/qtetramesher\">"
      "github.com/dennis2society/qtetramesher</a></p>");
}

QString QGLTetraViewer::helpString() const {
  const QString version(PROJECT_VER);
  int year = QDate::currentDate().year();
  QString text("<h2>QTetraMesher</h2>");
  text += "Version: ";
  text += version;
  text += "<br>";
  text += "<br>";
  text += "&copy; 2013 - " + QString::number(year) +
          ", Dennis L&uuml;bke, qtm (at) dennis2society.de";
  text += "<br>";
  text += "<br>";
  text +=
      "This is a Qt-based program for Windows and Linux to generate "
      "tetrahedral meshes for finite element simulation from various surface "
      "mesh formats. It also offers a fast and easy-to-use mesh viewer and "
      "allows basic mesh manipulation.<br />"
      "<a href=\"https://github.com/dennis2society/qtetramesher\">"
      "https://github.com/dennis2society/qtetramesher</a><br /><br />"
      "Tetrahedralization methods:<br />"
      "Delaunay Triangulation (CGAL)<br />"
      "Isosurface Stuffing (Johnathan Shewchuk's algorithm, "
      "<a href=\"https://people.eecs.berkeley.edu/~jrs/papers/stuffing.pdf\">"
      "paper</a>) in SOFA Framework and in a generic implementation written for "
      "this program (with optional graded tetrahedra)<br />"
      "Tetgen, GMSH and fTetWild (optional)<br /><br />"
      "<b>Third-party libraries</b><br />"
      "<ul>"
      "<li><a href=\"https://www.qt.io\">Qt 6</a>: user interface, OpenGL "
      "context</li>"
      "<li><a href=\"https://www.assimp.org\">Assimp</a>: loading and saving "
      "surface meshes (OBJ, STL, OFF, Collada, 3DS, PLY, AC3D)</li>"
      "<li><a href=\"https://www.cgal.org\">CGAL</a>: Delaunay "
      "tetrahedralization</li>"
      "<li><a href=\"https://github.com/sofa-framework/sofa\">SOFA "
      "Framework</a>: isosurface stuffing (MeshTetraStuffing)</li>"
      "<li><a href=\"https://wias-berlin.de/software/tetgen/\">TetGen</a> "
      "by Hang Si: constrained Delaunay tetrahedralization</li>"
      "<li><a href=\"https://gmsh.info\">GMSH</a> by Christophe Geuzaine and "
      "Jean-Francois Remacle: 3D mesh generation</li>"
      "<li><a href=\"https://github.com/wildmeshing/fTetWild\">fTetWild</a> "
      "by Yixin Hu et al. (optional module): robust "
      "tetrahedral meshing</li>"
      "</ul>"
      "Generic Isosurface Stuffing is part of this program and was written for "
      "it.<br />";
  text += "<br />";
  text +=
      "Usage/build instructions available at <a "
      "href=\"http://qtm.dennis2society.de\">qtm.dennis2society.de</a>.<br />";
  text += "<br />";
  text +=
      "Sourcecode available at <a href=\"https://github.com/dennis2society/"
      "qtetramesher\">QTetraMesher at Github</a>.<br />";
  text += "<br />";
  text +=
      "<pre>This program is free software: you can redistribute it and/or "
      "modify it<br />"
      "under the terms of the GNU General Public License as published by the "
      "Free<br />"
      "Software Foundation, either version 3 of the License, or (at your "
      "option) any<br />"
      "later version.<br />"
      "This program is distributed in the hope that it will be useful,<br />"
      "but WITHOUT ANY WARRANTY; without even the implied warranty of<br />"
      "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the<br />"
      "GNU General Public License for more details.<br />"
      "You should have received a copy of the GNU General Public License<br />"
      "along with this program.  If not, see here: <a "
      "href=\"http://www.gnu.org/licenses/gpl.html\">GPL License</a>.";
  text += "<br>";
  return text;
}

void QGLTetraViewer::ToggleTetraVis(bool drawWireframe_, bool drawSolid_) {
  tMesh->ToggleTetraMesh(drawWireframe_, drawSolid_);
  // updateGL();
  update();
}

void QGLTetraViewer::ToggleTriangleVis(int i) {
  tMesh->ToggleTriangleMesh(i);
  update();
}

void QGLTetraViewer::ToggleOctreeVis(int i) {
  tMesh->ToggleOctreeVis(i);
  update();
}

void QGLTetraViewer::ToggleBBox(int i) {
  tMesh->ToggleBBox(i);
  update();
}

void QGLTetraViewer::ShowStatusMessage(const QString &msg_, int duration_) {
  displayMessage(msg_, duration_);
}

void QGLTetraViewer::updateSceneFromMesh() {
  BoundingBox bb = tMesh->GetBoundingBox();
  _maxBBox = bb.max.x - bb.min.x;
  if (_maxBBox < (bb.max.y - bb.min.y)) _maxBBox = bb.max.y - bb.min.y;
  if (_maxBBox < (bb.max.z - bb.min.z)) _maxBBox = bb.max.z - bb.min.z;
  this->setSceneCenter(QVector3D(0, 0, 0));
  this->setSceneRadius(_maxBBox);
}

bool QGLTetraViewer::transformSurface(float scale_, float xDeg_, float yDeg_,
                                      float zDeg_, bool fitView_) {
  if (!tMesh->SetSurfaceTransform(scale_, xDeg_, yDeg_, zDeg_)) return false;
  updateSceneFromMesh();
  if (fitView_) showEntireScene();
  update();
  return true;
}

bool QGLTetraViewer::previewSurfaceTransform(float scale_, float xDeg_,
                                             float yDeg_, float zDeg_) {
  if (!tMesh->PreviewSurfaceTransform(scale_, xDeg_, yDeg_, zDeg_))
    return false;
  updateSceneFromMesh();
  update();
  return true;
}

void QGLTetraViewer::generateOctree(int d) {
  std::cout << "Generating Octree from surface..." << std::endl;
  tMesh->generateOctree((unsigned int)d);
}

namespace {

const char *kLastLoadDirKey = "paths/lastLoadDir";
const char *kGmshExportVersionKey = "export/gmshVersion";

/// folder of the last file chosen in one of the load dialogs; falls back to the
/// example data bundled in an AppImage, else empty (= Qt's default)
QString lastLoadDir() {
  const QString dir = QSettings().value(kLastLoadDirKey).toString();
  if (!dir.isEmpty() && QDir(dir).exists()) return dir;
  const QString appDir = qEnvironmentVariable("APPDIR");
  if (appDir.isEmpty()) return QString();
  const QString dataDir = appDir + "/usr/share/qtetramesher/data";
  return QDir(dataDir).exists() ? dataDir : QString();
}

/// stores the folder of fileName_ in the persistent settings
void rememberLoadDir(const QString &fileName_) {
  if (fileName_.isEmpty()) return;
  QSettings().setValue(kLastLoadDirKey, QFileInfo(fileName_).absolutePath());
}

}  // namespace

bool QGLTetraViewer::loadSurface() {
  const QString s = QFileDialog::getOpenFileName(
      this, "Select mesh File", lastLoadDir(),
      tr("Supported Files (OBJ, STL, OFF, Collada, 3DS, PLY, AC) (*.obj *.stl "
         "*.off *.dae *.3ds *.ply *.ac)"));
  std::cout << "Loading Surface Mesh... " << s.toStdString() << std::endl;
  if (s.isEmpty()) return false;
  rememberLoadDir(s);
  ShowStatusMessage("Loading Surface...");
  if (!tMesh->LoadSurface(s.toStdString(), repairOnLoad)) {
    ShowStatusMessage("Loading the surface mesh failed...", 10000);
    QMessageBox::warning(this, tr("Load Surface Mesh"),
                         tr("Could not load the surface mesh from\n%1\n\n"
                            "The file may be missing, damaged, empty or in an "
                            "unsupported format.")
                             .arg(s));
    return false;
  }
  BoundingBox bb = tMesh->GetBoundingBox();
  Vec3f bboxCenter = bb.min + ((bb.max - bb.min) / 2.0f);
  tMesh->translateSurfaceMesh(bboxCenter * -1.0);
  tMesh->BeginSurfaceTransform();
  if (tMesh->WasSurfaceRepaired())
    showNotice("Surface mesh was repaired on load",
               QString::fromStdString(tMesh->GetSurfaceRepairSummary()));
  showLoadedMesh(s, "Surface mesh successfully loaded...");
  return true;
}

QString QGLTetraViewer::selectGMSHFile() {
  const QString s = QFileDialog::getOpenFileName(
      this, "Select GMSH File", lastLoadDir(), "GMSH mesh file (*.msh)");
  if (!s.isEmpty()) rememberLoadDir(s);
  return s;
}

void QGLTetraViewer::showLoadedMesh(const QString &fileName_,
                                    const QString &message_) {
  currentFileName = fileName_;
  updateSceneFromMesh();
  this->showEntireScene();
  this->ShowStatusMessage(message_, 10000);
  emit onLoad();
}

bool QGLTetraViewer::loadTetgen() {
  QString s = QFileDialog::getOpenFileName(this, "Select Tetgen .node file",
                                           lastLoadDir(),
                                           "Tetgen node file (*.node)");
  if (s.isEmpty()) return false;
  rememberLoadDir(s);
  // Strip the .node extension - TetgenWrapper::loadAsTetgen expects the base name
  QFileInfo fi(s);
  QString basePath = fi.dir().filePath(fi.completeBaseName());
  std::cout << "Loading Tetgen Mesh... " << basePath.toStdString() << std::endl;
  ShowStatusMessage("Loading Tetgen...");
  if (!tMesh->LoadTetgen(basePath.toStdString())) {
    ShowStatusMessage("Loading the Tetgen mesh failed...", 10000);
    QMessageBox::warning(
        this, tr("Load Tetgen"),
        tr("Could not load a tetrahedral mesh from\n%1\n\n"
           "The matching .ele file must be in the same folder.")
            .arg(s));
    return false;
  }
  showLoadedMesh(s, "Tetgen mesh successfully loaded...");
  return true;
}

void QGLTetraViewer::saveGMSH() {
  if (tMesh == NULL) {
    ShowStatusMessage(
        "Saving GMSH not possible! No tetrahedral mesh present...", 10000);
    return;
  }
  if (tMesh->GetTetraMesh() != NULL) {
    // one filter per MSH version; 2.2 is for readers that do not support 4.x
    const QString filter41 = "GMSH 4.1 (*.msh)";
    const QString filter22 = "GMSH 2.2, legacy (*.msh)";
    QString selectedFilter =
        QSettings().value(kGmshExportVersionKey).toDouble() == 2.2 ? filter22
                                                                   : filter41;
    QString s = QFileDialog::getSaveFileName(this, "Select GMSH Output File",
                                             "", filter41 + ";;" + filter22,
                                             &selectedFilter);
    if (s.isEmpty()) return;
    const double mshVersion = (selectedFilter == filter22) ? 2.2 : 4.1;
    QSettings().setValue(kGmshExportVersionKey, mshVersion);
    // GMSH picks the output format by the extension
    if (QFileInfo(s).suffix().compare("msh", Qt::CaseInsensitive) != 0)
      s += ".msh";
    std::cout << "Saving GMSH " << mshVersion
              << " to file: " << s.toStdString() << std::endl;
    if (!tMesh->SaveGMSH(s.toStdString(), mshVersion)) {
      ShowStatusMessage("Saving GMSH failed...", 10000);
    }
  } else {
    ShowStatusMessage("Saving GMSH failed! No tetrahedral mesh present...",
                      10000);
  }
}

void QGLTetraViewer::saveTetgen() {
  if (tMesh == NULL) {
    ShowStatusMessage(
        "Saving Tetgen not possible! No tetrahedral mesh present...", 10000);
    return;
  }
  if (tMesh->GetTetraMesh() != NULL) {
    const QString filter = "Tetgen node file (*.node)";
    const QString s = QFileDialog::getSaveFileName(
        this, "Select Tetgen Output File prefix", "", filter);
    if (s.isEmpty()) return;
    // Strip the .node extension (if present) while keeping the directory —
    // TetgenWrapper::saveAsTetgen() expects a path/basename prefix without
    // any extension and will append ".node"/".ele" itself.
    QFileInfo fi(s);
    QString basePath = fi.dir().filePath(fi.completeBaseName());
    std::string outfileNames = basePath.toStdString() + ".node/.ele";
    std::cout << "Saving Tetgen to file: " << outfileNames << std::endl;
    if (!tMesh->SaveTetgen(basePath.toStdString())) {
      ShowStatusMessage("Saving Tetgen failed...", 10000);
    }
  } else {
    ShowStatusMessage("Saving Tetgen failed! No tetrahedral mesh present...",
                      10000);
  }
}

void QGLTetraViewer::saveSurface() {
  if (tMesh == NULL) {
    ShowStatusMessage("Saving Surface not possible! No mesh present...", 10000);
    return;
  }
  if (tMesh->GetSurface() != NULL) {
    QString selfilter =
        tr("OBJ (*.obj);; STL (*.stl);; Collada(*.dae);; "
           "3DS (*.3ds);; PLY (*.ply)");
    QString selectedFilter;
    QString s = QFileDialog::getSaveFileName(
        this, "Select Surface Output File", "", selfilter, &selectedFilter);
    if (s.isEmpty()) return;
    // without an extension use the one of the selected filter, e.g. "*.stl"
    if (QFileInfo(s).suffix().isEmpty()) {
      const int start = selectedFilter.indexOf("*.");
      const int end = selectedFilter.indexOf(')', start);
      s += (start >= 0 && end > start)
               ? selectedFilter.mid(start + 1, end - start - 1)
               : QString(".obj");
    }
    std::cout << "Saving Surface to file: " << s.toStdString() << std::endl;
    if (!tMesh->SaveSurface(s.toStdString(),
                            QFileInfo(s).suffix().toStdString()))
      ShowStatusMessage("Saving Surface failed...", 10000);
  } else {
    ShowStatusMessage("Saving Surface failed! No surface mesh present...",
                      10000);
  }
}

void QGLTetraViewer::selectSurfaceColor() {
  QColor c =
      QColorDialog::getColor(tMesh->GetSurfaceColor(), this, "Surface Color",
                             QColorDialog::DontUseNativeDialog);
  if (c.isValid()) {
    tMesh->SetSurfaceSolidColor(c.red(), c.green(), c.blue());
    update();
  }
}

void QGLTetraViewer::selectSurfaceColorWireframe() {
  QColor c = QColorDialog::getColor(tMesh->GetSurfaceWireframeColor(), this,
                                    "Surface Wireframe Color",
                                    QColorDialog::DontUseNativeDialog);
  if (c.isValid()) {
    tMesh->SetSurfaceWireframeColor(c.red(), c.green(), c.blue());
    update();
  }
}

void QGLTetraViewer::selectTetraColor() {
  QColor c =
      QColorDialog::getColor(tMesh->GetTetraColor(), this, "Tetra Solid Color",
                             QColorDialog::DontUseNativeDialog);
  if (c.isValid()) {
    tMesh->SetTetraSolidColor(c.red(), c.green(), c.blue());
    update();
  }
}

void QGLTetraViewer::selectTetraOuterColor() {
  QColor c = QColorDialog::getColor(tMesh->GetTetraOuterColor(), this,
                                    "Outer Tetra Color",
                                    QColorDialog::DontUseNativeDialog);
  if (c.isValid()) {
    tMesh->SetTetraOuterColor(c.red(), c.green(), c.blue());
    update();
  }
}

void QGLTetraViewer::selectTetraInnerColor() {
  QColor c = QColorDialog::getColor(tMesh->GetTetraInnerColor(), this,
                                    "Inner Tetra Color",
                                    QColorDialog::DontUseNativeDialog);
  if (c.isValid()) {
    tMesh->SetTetraInnerColor(c.red(), c.green(), c.blue());
    update();
  }
}

void QGLTetraViewer::selectTetraColorWireframe() {
  QColor c = QColorDialog::getColor(tMesh->GetTetraWireframeColor(), this,
                                    "Tetra Wireframe Color",
                                    QColorDialog::DontUseNativeDialog);
  if (c.isValid()) {
    tMesh->SetTetraWireframeColor(c.red(), c.green(), c.blue());
    update();
  }
}
