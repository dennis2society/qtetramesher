/*
 * QGLTetraMesh.cpp
 *
 *  Created on: Sep 11, 2011
 *      Author: Dennis Luebke
 */

#ifdef WIN32
#include "windows.h"
#endif
#include "GL/gl.h"
// #include "GL/glut.h"
#include "QGLTetraMesh.hpp"
#include "TetraMeshTools/GMSHMeshLoader.h"
#include "TetraMeshTools/GMSHMeshWriter.h"
#include "TetraMeshTools/TetgenWriter.h"
#include "TetraMeshTools/TriMeshLoader.h"
#include "TetraMeshTools/TriMeshWriter.h"
#include "Timer.h"

QGLTetraMesh::QGLTetraMesh()
    : drawSolid(false),
      drawTetraMesh(false),
      drawTetraSurface(false),
      drawTriangleMesh(true),
      isReady(false),
      showBoundingBox(false),
      bboxMode(0),
      drawTetraSolid(true),
      drawTetraWireframe(true),
      surfaceMode(0) {
  top = NULL;
  surf = NULL;
  surfaceColorSolid = Vec3f(0.3f, 0.3f, 0.3f);
  tetraColorSolid = Vec3f(0.5f, 0.5f, 0.1f);
  surfaceColorWireframe = Vec3f(0.1f, 0.6f, 0.1f);
  tetraColorWireframe = Vec3f(0.2f, 0.2f, 0.6f);
  oct = NULL;
}

QGLTetraMesh::~QGLTetraMesh() {
  isReady = false;
  if (top != NULL) {
    delete top;
  }
  if (surf != NULL) {
    delete surf;
  }
}

void QGLTetraMesh::Draw() {
  // glEnable(GL_LIGHTING);
  // glEnable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);
  static const float zero[4] = {0.0f, 0.0f, 0.0f};
  static const float diff[4] = {0.8f, 0.8f, 0.85f};
  static const float spec[4] = {0.6f, 0.6f, 0.6f};
  glMaterialfv(GL_FRONT, GL_AMBIENT, zero);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
  glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
  glMaterialfv(GL_FRONT, GL_EMISSION, zero);
  glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

  if (isReady) {
    /// draw tetra mesh
    if (top != NULL) {
      if (drawTetraWireframe) {
        glDisable(GL_LIGHTING);
        glColor3f(tetraColorWireframe.x, tetraColorWireframe.y,
                  tetraColorWireframe.z);  /// yellow wireframe
        glVertexPointer(3, GL_FLOAT, sizeof(Vec3f), &(top->GetVertices()[0]));
        // draw triangles
        glEnableClientState(GL_VERTEX_ARRAY);
        glDrawElements(GL_LINES, top->GetNumEdges() * 2, GL_UNSIGNED_INT,
                       &(top->GetEdges()[0]));
        glDisableClientState(GL_VERTEX_ARRAY);
      }
      if (drawTetraSolid) {
        glEnable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        const std::vector<Tetrahedron> &tetras = top->GetTetrahedra();
        const std::vector<Vec3f> &vertices = top->GetVertices();
        BoundingBox b = top->GetBoundingBox();
        glColor3f(tetraColorSolid.x, tetraColorSolid.y, tetraColorSolid.z);
        for (unsigned int i = 0; i < tetras.size(); ++i) {
          const Tetrahedron &t = tetras[i];
          Vec3f centroid =
              t.Centroid(vertices[t.index[0]], vertices[t.index[1]],
                         vertices[t.index[2]], vertices[t.index[3]]);
          float xDist = b.max.x - b.min.x;
          float xLowerCutDist = xDist * xLowerThreshold;
          float xUpperCutDist = xDist * xUpperThreshold;
          float yDist = b.max.y - b.min.y;
          float yLowerCutDist = yDist * yLowerThreshold;
          float yUpperCutDist = yDist * yUpperThreshold;
          float zDist = b.max.z - b.min.z;
          float zLowerCutDist = zDist * zLowerThreshold;
          float zUpperCutDist = zDist * zUpperThreshold;
          Vec3f v0 = vertices[t.index[0]];
          Vec3f v1 = vertices[t.index[1]];
          Vec3f v2 = vertices[t.index[2]];
          Vec3f v3 = vertices[t.index[3]];
          Vec3f vc0 = (v0 - centroid) * 0.95f;
          Vec3f vc1 = (v1 - centroid) * 0.95f;
          Vec3f vc2 = (v2 - centroid) * 0.95f;
          Vec3f vc3 = (v3 - centroid) * 0.95f;
          /// draw tetra when its centroid is between the front and back
          /// cutplanes
          if ((centroid.x <= (b.max.x - xLowerCutDist)) &&
              (centroid.x >= (b.max.x - xUpperCutDist)) &&
              (centroid.y <= (b.max.y - yLowerCutDist)) &&
              (centroid.y >= (b.max.y - yUpperCutDist)) &&
              (centroid.z <= (b.max.z - zLowerCutDist)) &&
              (centroid.z >= (b.max.z - zUpperCutDist))) {
            /// draw 4 correctly faced tetra triangles based on the
            /// triangles in relation to the tetrahedron's centroid
            glBegin(GL_TRIANGLES);
            Vec3f normal = (vc3 - vc0).cross((vc2 - vc0));
            normal.normalize();
            glNormal3f(normal.x, normal.y, normal.z);
            glVertex3f(centroid.x + vc0.x, centroid.y + vc0.y,
                       centroid.z + vc0.z);
            glVertex3f(centroid.x + vc3.x, centroid.y + vc3.y,
                       centroid.z + vc3.z);
            glVertex3f(centroid.x + vc2.x, centroid.y + vc2.y,
                       centroid.z + vc2.z);
            normal = (vc1 - vc3).cross((vc2 - vc3));
            normal.normalize();
            glNormal3f(normal.x, normal.y, normal.z);
            glVertex3f(centroid.x + vc3.x, centroid.y + vc3.y,
                       centroid.z + vc3.z);
            glVertex3f(centroid.x + vc1.x, centroid.y + vc1.y,
                       centroid.z + vc1.z);
            glVertex3f(centroid.x + vc2.x, centroid.y + vc2.y,
                       centroid.z + vc2.z);
            normal = (vc0 - vc1).cross((vc2 - vc1));
            normal.normalize();
            glNormal3f(normal.x, normal.y, normal.z);
            glVertex3f(centroid.x + vc1.x, centroid.y + vc1.y,
                       centroid.z + vc1.z);
            glVertex3f(centroid.x + vc0.x, centroid.y + vc0.y,
                       centroid.z + vc0.z);
            glVertex3f(centroid.x + vc2.x, centroid.y + vc2.y,
                       centroid.z + vc2.z);
            normal = (vc1 - vc0).cross((vc3 - vc0));
            normal.normalize();
            glNormal3f(normal.x, normal.y, normal.z);
            glVertex3f(centroid.x + vc0.x, centroid.y + vc0.y,
                       centroid.z + vc0.z);
            glVertex3f(centroid.x + vc1.x, centroid.y + vc1.y,
                       centroid.z + vc1.z);
            glVertex3f(centroid.x + vc3.x, centroid.y + vc3.y,
                       centroid.z + vc3.z);
            glEnd();
          }
        }
      }
    }
    /// draw surface if possible
    switch (surfaceMode) {
      case 2:  // hide
        break;
      case 1:  // wireframe
        if (surf != NULL) {
          glColor3f(surfaceColorWireframe.x, surfaceColorWireframe.y,
                    surfaceColorWireframe.z);  /// yellow wireframe
          glDisable(GL_LIGHTING);
          glVertexPointer(3, GL_FLOAT, sizeof(Vec3f),
                          &(surf->GetVertices()[0]));
          // draw triangles
          glEnableClientState(GL_VERTEX_ARRAY);
          glDrawElements(GL_LINES, surf->GetNumEdges() * 2, GL_UNSIGNED_INT,
                         &(surf->GetEdges()[0]));
          glDisableClientState(GL_VERTEX_ARRAY);
        }
        break;
      case 0:  // surface with normals
        if (surf != NULL) {
          glEnable(GL_LIGHTING);
          glEnable(GL_DEPTH_TEST);
          glEnable(GL_CULL_FACE);
          glColor3f(surfaceColorSolid.x, surfaceColorSolid.y,
                    surfaceColorSolid.z);  /// grey
          glVertexPointer(3, GL_FLOAT, sizeof(Vec3f),
                          &(surf->GetVertices()[0]));
          glNormalPointer(GL_FLOAT, sizeof(Vec3f), &(surf->GetNormals()[0]));
          // draw triangles
          glEnableClientState(GL_VERTEX_ARRAY);
          glEnableClientState(GL_NORMAL_ARRAY);
          glDrawElements(GL_TRIANGLES, surf->GetNumTriangles() * 3,
                         GL_UNSIGNED_INT, &(surf->GetTriangles()[0]));
          glDisableClientState(GL_NORMAL_ARRAY);
          glDisableClientState(GL_VERTEX_ARRAY);
        }
        break;
      default:
        break;
    }

    /// draw bounding volume
    BoundingBox bb;
    BoundingCube c;
    if (surf) {
      bb = surf->GetBoundingBox();
      c = surf->GetBoundingCube();
      if (bboxMode == 3) {
        c = surf->GetLargeBoundingCube();
      }
    }
    if (top) {
      bb = top->GetBoundingBox();
      c = top->GetBoundingCube();
      if (bboxMode == 3) {
        c = surf->GetLargeBoundingCube();
      }
    }
    switch (bboxMode) {
      case 0:  // hide bounding volume
        break;
      case 1:  // show bounding box
        DrawBoundingVolume(bb, Vec3f(0.1f, 0.8f, 0.1f));
        break;
      case 2:  // show larger bounding cube
        bb.min = Vec3f(c.center.x - c.size / 2, c.center.y - c.size / 2,
                       c.center.z - c.size / 2);
        bb.max = Vec3f(c.center.x + c.size / 2, c.center.y + c.size / 2,
                       c.center.z + c.size / 2);
        DrawBoundingVolume(bb, Vec3f(0.1f, 0.8f, 0.1f));
        break;
      default:
        break;
    }
    switch (octreeMode) {
      case 0:  // hide octree
        break;
      case 1:  // show octree
        DrawOctree(false);
        break;
      case 2:  // show octree with smallest leafs only
        DrawOctree(true);
        break;
    }
  }
}

void QGLTetraMesh::DrawBoundingVolume(const BoundingBox &bb, Vec3f color_) {
  glColor3f(color_.x, color_.y, color_.z);  /// green bbox
  glDisable(GL_LIGHTING);
  glBegin(GL_LINES);
  glVertex3f(bb.min.x, bb.min.y, bb.min.z);
  glVertex3f(bb.max.x, bb.min.y, bb.min.z);
  glVertex3f(bb.min.x, bb.min.y, bb.min.z);
  glVertex3f(bb.min.x, bb.max.y, bb.min.z);
  glVertex3f(bb.min.x, bb.min.y, bb.min.z);
  glVertex3f(bb.min.x, bb.min.y, bb.max.z);
  glVertex3f(bb.min.x, bb.max.y, bb.min.z);
  glVertex3f(bb.max.x, bb.max.y, bb.min.z);
  glVertex3f(bb.max.x, bb.min.y, bb.min.z);
  glVertex3f(bb.max.x, bb.max.y, bb.min.z);
  glVertex3f(bb.min.x, bb.max.y, bb.min.z);
  glVertex3f(bb.min.x, bb.max.y, bb.max.z);
  glVertex3f(bb.max.x, bb.max.y, bb.max.z);
  glVertex3f(bb.min.x, bb.max.y, bb.max.z);
  glVertex3f(bb.max.x, bb.max.y, bb.max.z);
  glVertex3f(bb.max.x, bb.min.y, bb.max.z);
  glVertex3f(bb.max.x, bb.max.y, bb.max.z);
  glVertex3f(bb.max.x, bb.max.y, bb.min.z);
  glVertex3f(bb.max.x, bb.min.y, bb.max.z);
  glVertex3f(bb.min.x, bb.min.y, bb.max.z);
  glVertex3f(bb.max.x, bb.min.y, bb.min.z);
  glVertex3f(bb.max.x, bb.min.y, bb.max.z);
  glVertex3f(bb.min.x, bb.min.y, bb.max.z);
  glVertex3f(bb.min.x, bb.max.y, bb.max.z);
  glEnd();
}

void QGLTetraMesh::DrawTetraTriangle(const unsigned int triangleIndex,
                                     const Vec3f &tetraCentroid,
                                     const Vec3f &color) {
  const Triangle &t = top->GetTriangles()[triangleIndex];
  // funny way to calculate the normal from the distance from the tetrahedron's
  // centroid to the triangle's centroid
  glBegin(GL_TRIANGLES);
  glColor3f(color.x, color.y, color.z);
  Vec3f v0 = (top->GetVertices()[t.index[0]] - tetraCentroid);  //*0.95f;
  Vec3f v1 = (top->GetVertices()[t.index[1]] - tetraCentroid);  //*0.95f;
  Vec3f v2 = (top->GetVertices()[t.index[2]] - tetraCentroid);  //*0.95f;
  Vec3f normal = (v0 + v1 + v2);
  normal.normalize();
  glNormal3f(normal.x, normal.y, normal.z);
  glVertex3f(tetraCentroid.x + v0.x, tetraCentroid.y + v0.y,
             tetraCentroid.z + v0.z);
  glVertex3f(tetraCentroid.x + v1.x, tetraCentroid.y + v1.y,
             tetraCentroid.z + v1.z);
  glVertex3f(tetraCentroid.x + v2.x, tetraCentroid.y + v2.y,
             tetraCentroid.z + v2.z);
  glEnd();
}

void QGLTetraMesh::DrawTetrahedron(const unsigned int tetraIndex) {
  const Tetrahedron &t = top->GetTetrahedra()[tetraIndex];
  Vec3f centroid = Tetrahedron::Centroid(
      top->GetVertices()[t.index[0]], top->GetVertices()[t.index[1]],
      top->GetVertices()[t.index[2]], top->GetVertices()[t.index[3]]);
  /// experimental tetra-drawing as GL_TRIANGLE_STRIP
  Vec3f v0 = (top->GetVertices()[t.index[0]] - centroid) * 0.8f;
  Vec3f v1 = (top->GetVertices()[t.index[1]] - centroid) * 0.8f;
  Vec3f v2 = (top->GetVertices()[t.index[2]] - centroid) * 0.8f;
  Vec3f v3 = (top->GetVertices()[t.index[3]] - centroid) * 0.8f;
  Vec3f color = Vec3f(0.2f, 0.1f, 0.2f);
  Vec3f normal;
  glColor3f(color.x, color.y, color.z);
  glBegin(GL_TRIANGLE_STRIP);
  normal = Vec3f(v0.x - centroid.x, v0.y - centroid.y, v0.z - centroid.z);
  normal.normalize();
  glNormal3f(normal.x, normal.y, normal.z);
  glVertex3f(centroid.x + v0.x, centroid.y + v0.y, centroid.z + v0.z);
  color = Vec3f(0.4f, 0.1f, 0.4f);
  glColor3f(color.x, color.y, color.z);
  normal = Vec3f(v1.x - centroid.x, v1.y - centroid.y, v1.z - centroid.z);
  normal.normalize();
  glNormal3f(normal.x, normal.y, normal.z);
  glVertex3f(centroid.x + v1.x, centroid.y + v1.y, centroid.z + v1.z);
  color = Vec3f(0.6f, 0.1f, 0.6f);
  glColor3f(color.x, color.y, color.z);
  normal = Vec3f(v2.x - centroid.x, v2.y - centroid.y, v2.z - centroid.z);
  normal.normalize();
  glNormal3f(normal.x, normal.y, normal.z);
  glVertex3f(centroid.x + v2.x, centroid.y + v2.y, centroid.z + v2.z);
  color = Vec3f(0.8f, 0.8f, 0.8f);
  glColor3f(color.x, color.y, color.z);
  normal = Vec3f(v3.x - centroid.x, v3.y - centroid.y, v3.z - centroid.z);
  normal.normalize();
  glNormal3f(normal.x, normal.y, normal.z);
  glVertex3f(centroid.x + v3.x, centroid.y + v3.y, centroid.z + v3.z);
  color = Vec3f(0.2f, 0.1f, 0.2f);
  glColor3f(color.x, color.y, color.z);
  normal = Vec3f(v0.x - centroid.x, v0.y - centroid.y, v0.z - centroid.z);
  normal.normalize();
  glNormal3f(normal.x, normal.y, normal.z);
  glVertex3f(centroid.x + v0.x, centroid.y + v0.y, centroid.z + v0.z);
  color = Vec3f(0.4f, 0.1f, 0.4f);
  glColor3f(color.x, color.y, color.z);
  normal = Vec3f(v1.x - centroid.x, v1.y - centroid.y, v1.z - centroid.z);
  normal.normalize();
  glNormal3f(normal.x, normal.y, normal.z);
  glVertex3f(centroid.x + v1.x, centroid.y + v1.y, centroid.z + v1.z);
  glEnd();
}

void QGLTetraMesh::DrawOctree(const bool leafsOnyly_) {
  if (oct != NULL) {
    OctreeNode *root = oct->getRootNode();
    const std::vector<OctreeNode *> &leafs = root->getLeafs(leafsOnyly_);
    for (unsigned int k = 0; k < leafs.size(); ++k) {
      OctreeNode *on = leafs[k];
      BoundingBox bt;
      bt.min = on->getMinBC();
      bt.max = on->getMaxBC();
      DrawBoundingVolume(bt, Vec3f(0.8f, 0.8f, 0.1f));
    }
  }
}

void QGLTetraMesh::UpdateTetraMesh(std::vector<Vec3f> &verts_,
                                   std::vector<Tetrahedron> &tetras_) {
  isReady = false;
  if (top != NULL) {
    delete top;
    top = NULL;
  }
  if (verts_.size() == 0 || tetras_.size() == 0) {
    isReady = false;
    return;
  }
  top = new TetraTools::TetrahedronTopology();
  top->Init(verts_, tetras_, true);
  isReady = true;
}

void QGLTetraMesh::ClearSurface() {
  if (surf != NULL) {
    delete surf;
    surf = NULL;
  }
  if (oct != NULL) {
    delete oct;
  }
}

void QGLTetraMesh::LoadGMSH(const std::string &fileName_) {
  isReady = false;
  if (top != NULL) {
    delete top;
    top = NULL;
  }
  if (surf != NULL) {
    delete surf;
    surf = NULL;
  }
  if (oct != NULL) {
    delete oct;
    oct = NULL;
  }
  Timer t;
  t.start();
  TetraTools::GMSHMeshLoader *gloader = new TetraTools::GMSHMeshLoader();
  if (!gloader->Load(fileName_)) {
    std::cerr << "Error loading GMSH file '" << fileName_ << "'." << std::endl;
    delete gloader;
    return;
  }
  top = new TetraTools::TetrahedronTopology();
  top->Init(gloader->GetVertices(), gloader->GetTetras(), true);
  surf = new TetraTools::TriangleTopology();
  surf->Init(top->GetVertices(), top->GetSurfaceTriangles(), true);
  surf->GenerateNormals();
  isReady = true;
  t.stop();
  std::cout << "Finished loading GMSH TetraMesh in "
            << t.getElapsedTimeInMilliSec() << " ms." << std::endl;
  delete gloader;
}

void QGLTetraMesh::LoadSurface(const std::string &fileName_) {
  isReady = false;
  if (surf != NULL) {
    delete surf;
    surf = NULL;
  }
  if (top != NULL) {
    delete top;
    top = NULL;
  }
  if (oct != NULL) {
    delete oct;
    oct = NULL;
  }
  Timer t;
  t.start();
  TetraTools::TriMeshLoader gloader;
  if (!gloader.loadFile(fileName_)) {
    std::cerr << "Error loading file '" << fileName_ << "'." << std::endl;
    return;
  }
  surf = new TetraTools::TriangleTopology();
  surf->Init(gloader.GetVertices(), gloader.GetTriangles(), true);
  surf->GenerateNormals();
  isReady = true;
  t.stop();
  std::cout << "Finished loading SurfaceMesh in "
            << t.getElapsedTimeInMilliSec() << " ms." << std::endl;
}

void QGLTetraMesh::LoadTetgen(const std::string &fileName_) {
  isReady = false;
  if (top != NULL) {
    delete top;
    top = NULL;
  }
  if (surf != NULL) {
    delete surf;
    surf = NULL;
  }
  if (oct != NULL) {
    delete oct;
    oct = NULL;
  }
  Timer t;
  t.start();
  TetraTools::GMSHMeshLoader *gloader = new TetraTools::GMSHMeshLoader();
  if (!gloader->Load(fileName_)) {
    std::cerr << "Error loading Tetgen file '" << fileName_ << "'."
              << std::endl;
    delete gloader;
    return;
  }
  top = new TetraTools::TetrahedronTopology();
  top->Init(gloader->GetVertices(), gloader->GetTetras(), true);
  surf = new TetraTools::TriangleTopology();
  surf->Init(top->GetVertices(), top->GetSurfaceTriangles(), true);
  surf->GenerateNormals();
  isReady = true;
  t.stop();
  std::cout << "Finished loading Tetgen TetraMesh in "
            << t.getElapsedTimeInMilliSec() << " ms." << std::endl;
  delete gloader;
}

bool QGLTetraMesh::SaveGMSH(const std::string &fileName_) {
  Timer t;
  t.start();
  TetraTools::GMSHMeshWriter gwriter;
  bool success =
      (gwriter.SaveToFile(fileName_, top->GetVertices(), top->GetTetrahedra()));
  t.stop();
  std::cout << "Finished saving GMSH in " << t.getElapsedTimeInMilliSec()
            << " ms." << std::endl;
  return success;
}

bool QGLTetraMesh::SaveTetgen(const std::string &fileName_) {
  Timer t;
  t.start();
  TetraTools::TetgenWriter twriter;
  bool success =
      (twriter.SaveToFile(fileName_, top->GetVertices(), top->GetTetrahedra()));
  t.stop();
  std::cout << "Finished saving Tetgen in " << t.getElapsedTimeInMilliSec()
            << " ms." << std::endl;
  return success;
}

bool QGLTetraMesh::SaveSurface(const std::string &fileName_,
                               const std::string extension_) {
  Timer t;
  t.start();
  TetraTools::TriMeshWriter twriter;
  bool success = twriter.writeFile(fileName_, extension_, surf->GetVertices(),
                                   surf->GetTriangles());
  t.stop();
  std::cout << "Finished saving Surface in " << t.getElapsedTimeInMilliSec()
            << " ms." << std::endl;
  return success;
}

void QGLTetraMesh::scalex10() {
  isReady = false;
  if (surf == NULL) {
    std::cerr << "Error on scale! Surface mesh is empty..." << std::endl;
    return;
  } else {
    for (unsigned int i = 0; i < surf->GetVertices().size(); ++i) {
      Vec3f &v = surf->GetVertices()[i];
      v *= 10.0f;
    }
    surf->GenerateBoundingBox();
  }
  if (top == NULL) {
    std::cerr << "Error on scale! Tetra mesh is empty..." << std::endl;
  } else {
    std::vector<Vec3f> tv;
    std::vector<Tetrahedron> tt;
    for (unsigned int i = 0; i < top->GetVertices().size(); ++i) {
      Vec3f v = top->GetVertices()[i];
      v *= 10.0f;
      tv.push_back(v);
    }
    for (unsigned int i = 0; i < top->GetTetrahedra().size(); ++i) {
      tt.push_back(top->GetTetrahedra()[i]);
    }
    UpdateTetraMesh(tv, tt);
  }
  isReady = true;
}

void QGLTetraMesh::scalex01() {
  isReady = false;
  if (surf == NULL) {
    std::cerr << "Error on scale! Surface mesh is empty..." << std::endl;
    return;
  } else {
    for (unsigned int i = 0; i < surf->GetVertices().size(); ++i) {
      Vec3f &v = surf->GetVertices()[i];
      v *= 0.1f;
    }
    surf->GenerateBoundingBox();
  }
  if (top == NULL) {
    std::cerr << "Error on scale! Tetra mesh is empty..." << std::endl;
  } else {
    std::vector<Vec3f> tv;
    std::vector<Tetrahedron> tt;
    for (unsigned int i = 0; i < top->GetVertices().size(); ++i) {
      Vec3f v = top->GetVertices()[i];
      v *= 0.1f;
      tv.push_back(v);
    }
    for (unsigned int i = 0; i < top->GetTetrahedra().size(); ++i) {
      tt.push_back(top->GetTetrahedra()[i]);
    }
    UpdateTetraMesh(tv, tt);
  }
  isReady = true;
}

void QGLTetraMesh::generateOctree(const unsigned int depth_) {
  if (oct != NULL) {
    delete oct;
    oct = NULL;
  }
  if (surf == NULL) {
    std::cerr << "Error on Octree generation: Surface is empty..." << std::endl;
    return;
  }
  Timer t;
  t.start();
  oct = new Octree(depth_, &(surf->GetVertices()), &(surf->GetTriangles()));
  t.stop();
  std::cout << "Octree Level " << oct->getDepth() << " generated in "
            << t.getElapsedTimeInMilliSec() << " ms." << std::endl;
}

void QGLTetraMesh::SetXRange(int lower, int upper) {
  xLowerThreshold = (float)lower / 100.0f;
  xUpperThreshold = (float)upper / 100.0f;
}

void QGLTetraMesh::SetYRange(int lower, int upper) {
  yLowerThreshold = (float)lower / 100.0f;
  yUpperThreshold = (float)upper / 100.0f;
}

void QGLTetraMesh::SetZRange(int lower, int upper) {
  zLowerThreshold = (float)lower / 100.0f;
  zUpperThreshold = (float)upper / 100.0f;
}

void QGLTetraMesh::translateSurfaceMesh(const Vec3f offset_) {
  for (auto &it : surf->GetVertices()) {
    it += offset_;
  }
}
