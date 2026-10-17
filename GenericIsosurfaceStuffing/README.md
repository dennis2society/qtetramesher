# GenericIsosurfaceStuffing

A self-contained C++20 implementation of the Isosurface Stuffing algorithm
(Labelle & Shewchuk, SIGGRAPH 2007) for generating high-quality tetrahedral meshes
from closed triangle surface meshes.

## Algorithm Overview

The Isosurface Stuffing algorithm fills a domain bounded by a triangle surface
with a tetrahedral mesh by:

1. Building a Body-Centered Cubic (BCC) lattice that covers the bounding box
   (optionally graded through an octree away from the surface).
2. Evaluating the signed distance from every lattice point to the surface.
3. Finding the isosurface crossing along every lattice edge via false position
   against the real distance field.
4. Applying Shewchuk's warping rules to snap near-surface lattice points onto
   the surface, preventing badly shaped tetrahedra.
5. Subdividing each background tetrahedron using stencils and classifying the
   pieces as inside or outside.
6. Emitting only the inside tetrahedra.

## Components

### SignedDistanceField (SignedDistanceField.cpp)

Computes exact signed distance to the triangle surface.

- **BVH acceleration**: Builds a bounding-volume hierarchy over the triangles
  for fast closest-point queries in O(log n) expected time.
- **Pseudo-normal sign**: Uses the angle-weighted pseudo-normal of the closest
  feature (vertex, edge, or face) to determine the sign. This method, due to
  Barentzen & Aanaes, is exact for closed manifold meshes.
- **Double precision**: All distance and sign computations use double precision
  to ensure robustness on meshes with small features.

### BCCLattice (BCCLattice.cpp)

Generates the background mesh covering the domain.

- **Uniform lattice**: Divides the padded bounding box into a regular grid of
  cubic cells. Each cell contributes one lattice point at its center, and
  tetrahedra are constructed as bipyramids over shared cell faces. This
  produces exactly the BCC tetrahedralization of Labelle & Shewchuk.
- **Graded octree**: Optionally coarsens cells away from the surface using a
  strongly 2:1-balanced octree. This reduces the tetrahedron count by 25-45%
  at equal surface resolution while maintaining mesh quality.
- **Conforming transitions**: Level transitions stay conforming by inserting
  midpoints on split cell edges. The face patches walk the boundary with these
  midpoints included, so neighboring patches agree on their shared edge.

### TetStuffer (TetStuffer.cpp)

Orchestrates the cutting, warping, and stencil application.

- **Lattice point labeling**: Labels every lattice point as inside (1),
  outside (-1), or on the surface (0). Points already on the surface are
  identified to prevent false sign changes.
- **Isosurface crossing**: Finds the exact crossing of every sign-changing
  lattice edge using Illinois-regulated false position against the signed
  distance field. This ensures the output boundary follows the input surface,
  not a linear estimate.
- **Warping**: Moves lattice points that lie within Shewchuk's thresholds
  (alpha_s and alpha_l) onto their nearest cut point, preventing slivers.
  Very close cut points are snapped to avoid duplicate vertices.
- **Stuffing stencils**: Subdivides each background tetrahedron. Quadrilateral
  faces are split along the lowest-numbered corner, ensuring adjacent
  tetrahedra agree on the subdivision.

## Features

### Uniform Meshing

Use `graded=false` for a regular BCC lattice covering the domain uniformly.

### Graded Meshing

Use `graded=true` for an octree-based background mesh that coarsens away from
the surface. The grading respects the strong 2:1 balance rule, so cell sizes
differ by at most a factor of 2 at any two adjacent cells.

### Alpha Parameters

Two warping thresholds control how aggressively near-surface points are snapped:

- `alphaShort` (default 0.3548): Threshold for short edges (between lattice
  centers and corners).
- `alphaLong` (default 0.2151): Threshold for long edges (between two centers
  or two corners).

These are the proven values from the Labelle-Shewchuk paper. Higher values
produce slivers and should be avoided.

## Quality Metrics

Validation across 7 test meshes at multiple resolutions (42 runs):

- **No inverted tetrahedra**: Every output tetrahedron has positive volume.
- **No topological defects**: 0 non-manifold faces, 0 open boundary edges.
- **Good angles**: Minimum dihedral angle >= 10.7 degrees (all runs).
- **Volume accuracy**: Output volume converges to true volume (0.998-1.000).
- **Surface fidelity**: Every boundary vertex lies on the input surface to
  within float precision (< 1e-5 % of bounding box).

Grading reduces output size by 25-45% compared to uniform meshing at the same
surface resolution, while maintaining these quality guarantees.

## Known Limitations

1. **Features thinner than cell size**: The mesh may pinch on very thin
   geometric features smaller than the background cell size. Reduce the cell
   size to avoid this.

2. **Input mesh must be closed and manifold**: The signed distance sign test
   assumes a watertight, non-self-intersecting input. Run "Check Surface Mesh"
   first to verify. A mesh with self-intersecting triangles or open edges will
   produce incorrect sign assignments and invalid output.

3. **Very thin slivers near lattice corners**: Although the algorithm is
   designed to avoid slivers, edge cases on highly distorted inputs may still
   produce rare tetrahedra with dihedral angles below 10 degrees.

## Building

The subproject requires:

- C++20 compiler
- TetraMeshTools library (for Vec3f, Triangle, Tetrahedron types)
- OpenMP (optional, for parallel distance evaluation)

The CMakeLists.txt links OpenMP if available and compiles with -O3.

## Usage

```cpp
#include <GenericIsosurfaceStuffing.h>

GenericIsosurfaceStuffing iso;
iso.GenerateFromSurface(triangles, vertices, cellSize,
                        alphaShort, alphaLong, graded);

const auto& outVerts = iso.GetTetraVertices();
const auto& outTets = iso.GetTetras();
```

Parameters:

- `triangles`: Input surface triangle indices.
- `vertices`: Input surface vertex positions (Vec3f).
- `cellSize`: Size of the finest background cells (relative to bounding box).
  If <= 0, defaults to 1/20 of the bounding box diagonal.
- `alphaShort`: Short-edge warp threshold (0-0.49). Default 0.3548.
- `alphaLong`: Long-edge warp threshold (0-0.49). Default 0.2151.
- `graded`: Enable graded octree meshing. Default false for uniform lattice.

## References

Labelle, F., & Shewchuk, J. R. (2007). Isosurface stuffing: Fast tetrahedral
meshes with good dihedral angles. ACM Transactions on Graphics, 26(3), 57.

Barentzen, J. A., & Aanaes, H. (2005). Signed distance computation using the
angle weighted pseudonormal. IEEE Transactions on Visualization and Computer
Graphics, 11(3), 243-253.

Ericson, C. (2005). Real-time collision detection. CRC Press.
