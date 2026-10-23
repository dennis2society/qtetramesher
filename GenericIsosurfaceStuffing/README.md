# GenericIsosurfaceStuffing

A self-contained C++20 implementation of the Isosurface Stuffing algorithm
(Labelle & Shewchuk, SIGGRAPH 2007) for generating high-quality tetrahedral meshes
from closed triangle surface meshes.

## Algorithm Overview

The Isosurface Stuffing algorithm fills a domain bounded by a triangle surface
with a tetrahedral mesh by:

1. Building a Body-Centered Cubic (BCC) lattice that covers the bounding box
   (optionally graded through an octree away from the surface and along flat
   surface regions).
2. Evaluating the signed distance from every lattice point to the surface.
3. Finding the isosurface crossing along every lattice edge via false position
   against the real distance field.
4. Applying Shewchuk's warping rules to snap near-surface lattice points onto
   the surface, preventing badly shaped tetrahedra. Warps that would glue two
   parts together across a thin gap are rejected.
5. Subdividing each background tetrahedron using stencils and classifying the
   pieces as inside or outside.
6. Emitting only the inside tetrahedra, minus those that bridge a gap narrower
   than the cell size.
7. Post-processing: snapping boundary vertices onto sharp edges and corners,
   smoothing poorly shaped tetrahedra and removing flat boundary caps.

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
- **Segment queries**: `segmentOnSide()` and `segmentCrossesGap()` walk along a
  segment using the 1-Lipschitz property of the distance field: from a point
  with value v below the tolerance, no point closer than (tol - v) can exceed
  it, so that is a safe step (outside the solid, the gap test steps by the
  distance itself and cannot pass the re-entry surface). A segment running
  along the surface therefore takes about len/tol steps instead of crawling.
  A segment crosses a gap if it leaves the solid and re-enters it through a
  surface whose pseudo-normal faces the one it left; a chord that only bulges
  out of a concave surface does not count. This finds gaps much thinner than
  the lattice cells.

### BCCLattice (BCCLattice.cpp)

Generates the background mesh covering the domain.

- **Uniform lattice**: Divides the padded bounding box into a regular grid of
  cubic cells. Each cell contributes one lattice point at its center, and
  tetrahedra are constructed as bipyramids over shared cell faces. This
  produces exactly the BCC tetrahedralization of Labelle & Shewchuk.
- **Graded octree**: Coarsens cells using a strongly 2:1-balanced octree. The
  `grading` parameter (0..1) controls how aggressively:
  - interior cells may grow from 8x up to 128x the finest cell size;
  - cells cut by the surface may stay at 2x (grading < 0.5) or 4x the finest
    cell size where the surface is flat. Flatness is tested by comparing the
    distance field at 27 points of the cell with its trilinear interpolation
    (tolerance 0.3 * grading * cell size), so curved regions, sharp edges,
    corners and thin parts still get the finest cells.
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
  Very close cut points are snapped to avoid duplicate vertices. A warp is
  rejected (the next-nearest cut point is tried) if an edge from the new
  position to an inside neighbour would cross a gap, since warping onto the
  surface of a neighbouring part would glue both parts together.
- **Stuffing stencils**: Subdivides each background tetrahedron. Quadrilateral
  faces are split along the lowest-numbered corner, ensuring adjacent
  tetrahedra agree on the subdivision.
- **Gap bridges**: Where two parts (or the two sides of a hole) come closer
  than the cell size, a background tetrahedron can have inside corners in
  both. Output tetrahedra with an edge that crosses a gap are removed, so the
  parts stay separate.

### MeshOptimizer (MeshOptimizer.cpp)

Post-processing that only moves vertices (and removes flat boundary caps).
Every move is kept only if all tetrahedra around the vertex stay positively
oriented and above a quality floor (or no worse than before), and only if the
move does not leave the solid, so vertices cannot jump onto another part.

- **Sharp feature snapping**: Edges of the input surface whose face normals
  differ by more than `featureAngle` are sharp; corners are where three or more
  sharp edges meet or a sharp edge turns sharply. Boundary vertices within
  `featureSnap` x (mean boundary edge length) of a corner or sharp edge are
  moved onto it (corners first, closest first). Rejected moves get retried
  after their neighbourhood has been smoothed. This greatly reduces the
  aliasing of sharp edges that is typical for isosurface stuffing.
- **Feature point insertion**: Where no boundary vertex was close enough to
  be snapped, the boundary cuts the feature off (saw-tooth notches along sharp
  edges, truncated corners). Uncovered corners and points along the sharp
  edges get a new vertex that is connected to the patch of boundary triangles
  it sees, filling the notch with a fan of tetrahedra. A fill is only done if
  the patch is a disk, the new tetrahedra are positive with quality >= 0.05,
  the new boundary is at least twice as close to the surface and no new edge
  crosses the boundary elsewhere. On a single tetrahedron (data/
  single_tetra.obj) this takes the mean edge error from 0.038 to 0.0003 and
  hits all corners exactly.
- **Quality smoothing**: Vertices of tetrahedra with poor quality are relaxed;
  interior vertices towards the centroid of their neighbours, boundary
  vertices along the surface (projected back with the distance field).
- **Cap removal**: Flat tetrahedra lying on the surface with two or three
  boundary faces are removed if the boundary stays manifold and the surface
  does not move away from the input (thin tetrahedra filling a sharp edge are
  kept).

## Features

### Uniform Meshing

Use `grading=0` for a regular BCC lattice covering the domain uniformly.

### Graded Meshing

Use `grading` in (0, 1] for an octree-based background mesh. Higher values
give larger interior tetrahedra and larger tetrahedra along flat surface
regions. The grading respects the strong 2:1 balance rule, so cell sizes
differ by at most a factor of 2 at any two adjacent cells; on small or thin
models this limits how much coarsening is possible. With grading enabled the
smallest allowed cell size is diag/800 instead of diag/400.

### Sharp Features

Use `featureAngle` (degrees, 0 = off) and `featureSnap` (0..1) to make the
boundary follow sharp edges and corners of the input (see MeshOptimizer).

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

The figures above were measured for the uniform lattice and the original
interior-only grading. With the more aggressive grading (coarse cells along
flat surface regions), feature snapping and gap handling, measured on a
rotated cube, a fan model and two interlocked tori (double_torus.stl, gap
narrower than the cell size):

- No inverted tetrahedra, closed manifold boundary in all runs.
- Fan: 227k tetrahedra (uniform) -> 103k (grading 0.5) -> 88k (grading 1.0).
- Worst tetrahedron quality (6*sqrt(2)*V / l_rms^3, 1 = regular) drops from
  about 0.25-0.30 (uniform) to about 0.06-0.09 with grading, and to 0.03-0.18
  next to unresolved gaps.
- Rotated cube (edge length 2) with feature snapping: mean distance of the
  sharp edges to the boundary drops from 0.047 to 0.0016-0.0024; volume error
  from 1% to 0.05%.
- Interlocked tori: the parts stay separate at all tested cell sizes (before:
  glued together by a few bridging tetrahedra).

## Performance

The log ends with the time spent in each stage (distance field, lattice,
labels, edges, cut points, warping, stencils, gap check, optimization).

- Distance evaluations run in parallel with OpenMP: lattice labels, cut
  points, warp checks, the gap check and the read-only scans of the
  optimizer (quality checks, snap candidates, feature point coverage).
- The octree refinement decisions are made level by level in parallel; the
  leaves are collected depth first afterwards, so the result is the same as
  with a sequential traversal and does not depend on the number of threads.
- The 2:1 balance needs a single pass: split cells append their children to
  the list being processed, and those check their own neighbours.
- The gap check skips edges that cannot reach the outside (Lipschitz bound
  from their ends) before deduplication, so only edges near the surface are
  traced.
- Vertex moves in the optimizer stay sequential because each one depends on
  the previous ones.

Compared to the first version with gap handling and feature snapping, this
made the method 2.5-9x faster with the same output in all test cases but one
(the double torus at 2% cell size got 8 more of 65k tetrahedra, as the gap
test samples slightly different points), e.g. the fan model at
2% cell size 4.1 s -> 0.45 s and the elephant at 1% cell size
4.8 s -> 0.78 s (20 threads).

## Known Limitations

1. **Features thinner than cell size**: The mesh may pinch on very thin
   geometric features smaller than the background cell size. Reduce the cell
   size to avoid this. Gaps between parts that are narrower than the cell size
   no longer glue the parts together, but the parts may lose a little material
   where they almost touch.

2. **Input mesh must be closed and manifold**: The signed distance sign test
   assumes a watertight, non-self-intersecting input. Run "Check Surface Mesh"
   first to verify. A mesh with self-intersecting triangles or open edges will
   produce incorrect sign assignments and invalid output.

3. **Very thin slivers near lattice corners**: Although the algorithm is
   designed to avoid slivers, edge cases on highly distorted inputs may still
   produce rare tetrahedra with dihedral angles below 10 degrees. Graded
   meshes with coarse surface cells have weaker quality than the uniform
   lattice (see Quality Metrics); use grading 0 where the guarantees matter.

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
                        alphaShort, alphaLong, grading,
                        featureAngle, featureSnap);

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
- `grading`: 0 = uniform lattice, (0, 1] = graded octree; higher values
  coarsen more. Default 0.
- `featureAngle`: Sharp edge angle in degrees, 0 = no feature snapping.
  Default 0.
- `featureSnap`: 0..1, snap radius as a fraction of the local boundary edge
  length. Default 0.

## References

Labelle, F., & Shewchuk, J. R. (2007). Isosurface stuffing: Fast tetrahedral
meshes with good dihedral angles. ACM Transactions on Graphics, 26(3), 57.

Barentzen, J. A., & Aanaes, H. (2005). Signed distance computation using the
angle weighted pseudonormal. IEEE Transactions on Visualization and Computer
Graphics, 11(3), 243-253.

Ericson, C. (2005). Real-time collision detection. CRC Press.
