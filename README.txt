README for QTetraMesher

Copyright (C) 2013 Dennis Luebke, qtm (at) dennis2society.de

Usage and build instructions available at http://qtm.dennis2society.de.
Sourcecode available at: https://github.com/dennis2society/qtetramesher

This is a Qt-based program for Windows and Linux to generate tetrahedral meshes for 
finite element simulation from various surface mesh formats. It also offers a fast 
and easy-to-use mesh viewer based on QGLViewer and allows basic mesh manipulations 
(currently only scaling is possible). Two different methods for tetrahedralization 
are possible: Delaunay Triangulation and Johnathan Shewchuk's Isosurface Stuffing algorithm.

As I am using lots of 3rd-party open-source C/C++ libraries, this work is released 
as GPL software. This includes all self-written code. 3rd-party code remains under 
its own license.

Changelog
  2015-12-06:
        Migration to Github due to cancellation of Google Code
  0.8.1
	- Drag & Drop supported mesh files on the main window to open files.
	- Octree generation and visualization as a preparation for own Isosurface Stuffing implementation.

Features
    - Runs on Windows and Linux (tested on Win7 (32/64 bit), WinXP (32 bit) and Ubuntu 12.04/12.10).
    - Integration of QGLViewer for easy mesh preview.
    - Surface mesh import and export for *.obj, *.ply, *.off, *.stl (using trimesh2).
    - Import and export of tetrahedral meshes in (a simplified subset of the) GMSH format *.msh 
      (compatible with the SOFA Framework).
    - Two different methods for tetrahedralization: Delaunay Triangulation (CGAL implementation) and 
      Johnathan Shewchuk's Isosurface Stuffing: Fast Tetrahedral Meshes with Good Dihedral Angles 
      (implementation taken from the SOFA Framework, non-graded variant only).
    - Wireframe and solid/shaded drawing.
    - Cutplane view for tetrahedral meshes.
    - Export the surface of a tetrahedral mesh.
    - Automatic parameter guessing for tetrahedralization.
    - Reversible scale mechanism for CGAL performance optimization.


Dependencies / 3rd Party Libraries

  This program is using the following 3rd-party libraries: (Name, Version, License, URL)

    libQGLViewer (2.3.16), GPL, http://www.libqglviewer.com/
    SOFA Framework (1.0), GPL/LGPL, http://www.sofa-framework.org/
    CGAL (4.x), GPL/LGPL, http://www.cgal.org/
    Qt (4.8.1), LGPL, http://qt-project.org/
    trimesh2 (2.11), GPL, http://gfx.cs.princeton.edu/proj/trimesh2/
    Boost (1.51), Boost Software License, http://www.boost.org/
    GMP (5.1.1), LGPL, http://gmplib.org/
    MPFR (3.1.1), LGPL, http://www.mpfr.org/

License

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


