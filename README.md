# QTetraMesher 0.8.5

<p align="center">
  <a href="https://qtm.dennis2society.de/images/QTetraMesher_0_8_5_Windows_ftetwild.jpg"><img src="https://qtm.dennis2society.de/images/QTetraMesher_0_8_5_Windows_ftetwild.jpg" alt="ScreenShot (Windows, fTetWild)" width="30%"></a>
  <a href="https://qtm.dennis2society.de/images/QTetraMesher_0_8_5_Linux_GIsoStuffing.jpg"><img src="https://qtm.dennis2society.de/images/QTetraMesher_0_8_5_Linux_GIsoStuffing.jpg" alt="ScreenShot (Linux, Generic Isosurface Stuffing)" width="31%"></a>
  <a href="https://qtm.dennis2society.de/images/QTetraMesher_0_8_5_Windows_VM.jpg"><img src="https://qtm.dennis2society.de/images/QTetraMesher_0_8_5_Windows_VM.jpg" alt="ScreenShot (Windows 11 VM, GMSH)" width="35%"></a>
</p>

This is a Qt-based program for Windows and Linux to generate tetrahedral meshes for 
finite element simulation from various surface mesh formats. It also offers a fast 
and easy-to-use mesh viewer. Several methods for tetrahedralization are available:
Delaunay Triangulation (CGAL), Tetgen, GMSH and variants of Johnathan Shewchuk's Isosurface
Stuffing algorithm (SOFA and a generic implementation).

This project is using lots of 3rd-party open-source C/C++ libraries, for this reason this work is released 
under GPL License. This includes all self-written code. 3rd-party code remains under 
its own license.

History + usage instructions: https://qtm.dennis2society.de

<b>Build instructions:</b> https://qtm.dennis2society.de/#Build


## Changelog
### 0.8.5 - 2026-10-17
* <b>New tetrahedralization methods:</b>
  - Custom GenericIsoSurfaceStuffing implementation with optional graded tetrahedra (results vary with mesh size and selected cell size) with no external dependencies
  - Improved Tetgen integration
  - GMSH integration (this one is sensitive to surface mesh quality...)
  - ftetwild integration
* Surface mesh quality check (basic test for usage with Tetgen and GMSH)
* libQGLViewer replaced by the new lightweight QtmGLViewer (QOpenGLWidget, OpenGL 3.3 core profile, shaders and GPU buffers). Works natively in Wayland.
* Two lights (headlight and a softer light from above) for better shape perception
* Visualization options (colors, wireframe/solid options, surface mode, axis/grid/bbox) are saved and restored between sessions
  - Now with different surface/inside colors for tetra meshes
* Fixed: bounding box was not updated when the surface mesh was moved to the origin
* Robustness: failed or corrupt mesh files (surface, GMSH, Tetgen) are rejected with a message and keep the current mesh instead of crashing; Generate without a loaded surface no longer crashes; 
  failed tetrahedralization keeps the surface visible
* Fixed memory leaks (octree, file stream in the GMSH loader, Tetgen parameters)
* GMSH files must be ASCII MSH version 2 (version 4 files are now detected and reported)
* Tetrahedralization no longer blocks the UI: each method runs in a separate worker process and can be canceled (also when a method hangs); a crash in a method no longer takes down the program.   
  Octree generation and the surface check run in background threads. A progress indicator is shown in the viewer
* Fixed a bug in the cutplane rendering (initial slider values were not at 0 cutting off parts of the tetrahedra mesh)
* Improved surface mesh loading including optional basic mesh repair on load
* Better about and help dialog
* Added an application icon

## Binary Releases
* <b>Windows binary release</b> (x64, Release/MinSizeRel) (portable ZIP archive or InnoSetup installer on the Releases page):
  - Compiler: Visual Studio 2022 (17.14), MSVC 19.44
  - Qt 6.12.0
  - Boost 1.92.0
  - CGAL 6.2.1 (with GMP/MPFR from the CGAL auxiliary libraries)
  - assimp 6.0.5
  - SOFA Framework 26.12.99
  - GMSH 4.15.2
  - fTetWild (git 8118f81, 2026-09-23) with oneTBB 2022.2.0, geogram 1.9.6, libigl 2.6.0, Eigen 3.4.0, spdlog 1.15.3 and fmt 11.2.0 (all statically linked except TBB)
* <b>Linux AppImage</b>
  - Qt 6.11.2
  - Boost 1.92.0
  - CGAL 6.2.1 (with GMP/MPFR from the CGAL auxiliary libraries)
  - assimp 6.0.5
  - SOFA Framework 26.12.99
  - GMSH 4.15.2
  - fTetWild (git 8118f81, 2026-09-23) with oneTBB 2022.2.0, geogram 1.9.6, libigl 2.6.0, Eigen 3.4.0, spdlog 1.15.3 and fmt 11.2.0 (all statically linked except TBB)

### 0.8.4 - 2025-04-06
* <b>Complete UI rework!</b> Bye bye *.ui files, all UI stuff is done in code because I can. And some things are incredibly tedious to do in the Qt Designer...
* Tetrameshing Options are now in the main UI instead of popup windows
* Qt6 build on Windows and at least some Linuxes
* Cutplanes in 6 directions to inspect inside mesh quality
* Windows binary release:
  - VS 2022, x64
  - Qt 6.9.0
  - assimp 5.4
  - Sofa Framework (git pull 2025-04-05)
  - boost 1.87.0

### 0.8.3 - 2023-12-16
 
 <b>Updated Windows binary build (Visual Studio 2019 Community, x64)</b>
 * Download: https://github.com/dennis2society/qtetramesher/releases/tag/0.8.3_win64_VS2019
 * Qt 5.15.2
 * CGAL 5.6, https://www.cgal.org/download.html
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
 * libQGLViewer is now included in the repo and not required as an external dependency anymore

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
 * As of 2024-06-01 this project can now be built with Qt6 (at least on Linux)
 * Runs on Windows and Linux (tested on Win7/10/11 (64 bit)) and Ubuntu 12.04/12.10/.../24.04, Arch based Manjaro >= 2024-05, Fedora >= 40 (Qt5 only)).
 * Integration of QGLViewer for easy mesh preview.
 * Surface mesh import and export for *.obj, *.ply, *.off, *.stl, 3ds (using assimp).
 * Import and export of tetrahedral meshes in (a simplified subset of the) GMSH format *.msh (compatible with the SOFA Framework).
 * Two different methods for tetrahedralization: Delaunay Triangulation (CGAL implementation) and Johnathan Shewchuk's Isosurface Stuffing: Fast Tetrahedral Meshes with Good Dihedral Angles (implementation taken from the SOFA Framework, non-graded variant only).
 * Wireframe and solid/shaded drawing.
 * 6-planes cutplane view for tetrahedral meshes.
 * Surface export of a tetrahedral mesh (GMSH: *.msh).
 * Automatic parameter guessing for tetrahedralization.

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
