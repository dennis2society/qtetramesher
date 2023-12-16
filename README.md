# QTetraMesher 0.8.3

![ScreenShot](https://qtm.dennis2society.de/images/qtm_win_with_octree_small.jpg )
![ScreenShot](https://qtm.dennis2society.de/images/qtm_with_stuffing_options_small.jpg )

This is a Qt-based program for Windows and Linux to generate tetrahedral meshes for 
finite element simulation from various surface mesh formats. It also offers a fast 
and easy-to-use mesh viewer based on QGLViewer and allows basic mesh manipulations 
(currently only scaling is possible). Two different methods for tetrahedralization 
are possible: Delaunay Triangulation and two variants of Johnathan Shewchuk's Isosurface
Stuffing algorithm.

As I am using lots of 3rd-party open-source C/C++ libraries, this work is released 
as GPL software. This includes all self-written code. 3rd-party code remains under 
its own license.

Usage and build instructions are available at http://qtm.dennis2society.de.
Sourcecode available at: https://github.com/dennis2society/qtetramesher

## Changelog
### 0.8.3 - 2023-12-16
 
 <b>Updated Windows binary build (Visual Studio 2019 Community, x64)</b>
 * Download: https://github.com/dennis2society/qtetramesher/releases/tag/0.8.3_win64_VS2019
 * Qt 5.15.2
 * CGAL 4.12, https://www.cgal.org/download.html
 * Boost 1.83.0, https://www.boost.org/users/download/
 * QGLViewer 2.9.1, https://github.com/GillesDebunne/libQGLViewer
 * SOFA Framework (git checkout 2023-12-15), https://github.com/sofa-framework/sofa
 * Assimp - Asset Importer Library (git checkout 2023-10-15), https://github.com/assimp/assimp
 * I hate how big the zip file is (30 MB, 80 MB extracted...), but at least it comes with a ~30MB SW OpenGL library added automatically by the Qt deployment script.
 
 <b>Functional updates:</b>
 * Full CMake build! All projects can now be built with CMake. (Pretty easy on Linux, but a PITA on Windows...)
 * Significantly improved mesh surface mesh loading + speed:
 * Replaced old trimesh2 C library with Assimp library
 * New mesh formats available for import
 * Fixed surface normal generation (damn.. the old one was so slow...)
 * The QGLViewer library is now included in the repo

### 0.8.2 - 2018-05-27
 * Updated Windows binary build (Visual Studio 2015, x64)
   * Qt5.10.1
   * CGAL 4.12 (built with Eigen3)
   * Boost 1.64
   * SOFA 17.12
   * https://github.com/dennis2society/qtetramesher/releases/tag/0.8.2_win64_vs2015
 * Binary release for Ubuntu 18.04 x64 (The release README.txt explains which libraries must be installed via apt)
   * https://github.com/dennis2society/qtetramesher/releases/tag/0.8.2_Ubuntu_18.04_x64

### 0.8.1 (Source only) - 2015-12-06
 * Migration to Github due to cancellation of Google Code

### 0.8.1 - sometimes in 2014... (Googlecode's history is lost)
 * Drag & Drop supported mesh files on the main window to open files.
 * Octree generation and visualization as a preparation for own Isosurface Stuffing implementation.

## Features
 * Runs on Windows and Linux (tested on Win7 (32/64 bit), WinXP (32 bit) and Ubuntu 12.04/12.10/.../15.10 and Arch based Manjaro 2015-12).
 * Integration of QGLViewer for easy mesh preview.
 * Surface mesh import and export for *.obj, *.ply, *.off, *.stl (using trimesh2).
 * Import and export of tetrahedral meshes in (a simplified subset of the) GMSH format *.msh (compatible with the SOFA Framework).
 * Two different methods for tetrahedralization: Delaunay Triangulation (CGAL implementation) and Johnathan Shewchuk's Isosurface Stuffing: Fast Tetrahedral Meshes with Good Dihedral Angles (implementation taken from the SOFA Framework, non-graded variant only).
 * Wireframe and solid/shaded drawing.
 * Cutplane view for tetrahedral meshes.
 * Export the surface of a tetrahedral mesh.
 * Automatic parameter guessing for tetrahedralization.
 * Reversible scale mechanism for CGAL performance optimization.

## License

This program is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free 
Software Foundation, either version 3 of the License, or (at your option) any 
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see here: http://www.gnu.org/licenses/gpl.html

Copyright (C) 2013-2024 Dennis Luebke, qtm (at) dennis2society.de
