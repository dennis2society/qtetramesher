# QtmGLViewer

QtmGLViewer is a small static library with a `QOpenGLWidget` based 3D viewer. It
replaces libQGLViewer in QTetraMesher. It uses only the OpenGL 3.3 core profile
(shaders, vertex arrays, buffers), so it also works in native Wayland sessions
where the fixed function pipeline used by libQGLViewer is not reliable.

Only Qt6 (Core, Gui, Widgets, OpenGL, OpenGLWidgets) is required. There are no
other dependencies (no GLU, GLUT, GLEW or GLM). The library is plain C++17 and
does not use any platform specific code, so it builds the same way on Linux and
Windows. It is built as a static library with position independent code.

## Inspiration

QtmGLViewer is inspired by [libQGLViewer](https://github.com/GillesDebunne/libQGLViewer)
by Gilles Debunne. Its camera behavior, input bindings and scene look follow the
libQGLViewer design, but the code is a new implementation on OpenGL 3.3 core.

## Features

* Trackball camera with the interaction of libQGLViewer
* Automatic near/far plane computation from the scene center and radius
* Scene fitting (`showEntireScene()`)
* Grid and XYZ axes (same look as libQGLViewer)
* FPS display, text overlay and timed status messages
* Perspective and orthographic projection
* Optional spinning after releasing a fast mouse drag (off by default)
* Built-in shaders: lit solid surfaces and flat colored lines
* Helpers for GPU geometry (`MeshBuffer`) and procedural geometry (`MeshData`)

## Build

The directory is a stand-alone CMake project:

```
cmake -S QtmGLViewer -B build_qtm
cmake --build build_qtm
./build_qtm/QtmGLViewerTest
```

It can also be used with `add_subdirectory(QtmGLViewer)` and linked with
`target_link_libraries(myapp PRIVATE QtmGLViewer)`. The test application is
only built if `QTMGLVIEWER_BUILD_TEST` is ON (default). When used as a
subdirectory you may want to set it to OFF.

The test application (`QtmGLViewerTest`) shows a Moebius strip with a color
gradient (hue running through the full spectrum) with the grid and
axes. All options can be toggled in a side panel. It also accepts
`--screenshot <file>` which saves an image of the viewer and exits.

## Usage

```cpp
#include <QtmGLViewer/QtmGLViewer.hpp>

class MyViewer : public QtmGLViewer {
 protected:
  void initializeScene() override { /* create GPU buffers */ }
  void cleanupScene() override { /* destroy GPU buffers */ }
  void draw(const qtm::RenderContext &ctx) override {
    ctx.drawSolid(mesh_, QColor(200, 200, 220));
    drawText(10, 30, "Hello");
  }
  qtm::MeshBuffer mesh_;
};

int main(int argc, char **argv) {
  QtmGLViewer::setDefaultSurfaceFormat();  // before QApplication
  QApplication app(argc, argv);
  ...
}
```

`setDefaultSurfaceFormat()` must be called before the `QApplication` is
created. It requests OpenGL 3.3 core profile with a 24 bit depth buffer and 4x
multisampling (if no other sample count was set in the default format). The
widget also sets this format on itself in its constructor.

### Mouse and keyboard

| Input                           | Action                         |
|---------------------------------|--------------------------------|
| Left drag                       | Rotate around the pivot point  |
| Right drag                      | Translate                      |
| Middle drag, mouse wheel        | Zoom                           |
| Left double click               | Align the view with the axes   |
| Right double click              | Center the scene               |
| Middle double click             | Show the entire scene          |
| A, G, F                         | Toggle axes, grid, FPS display |
| O                               | Toggle orthographic projection |

### Scene description

The viewer does not know what you draw. Tell it with `setSceneCenter()` and
`setSceneRadius()`. They control the pivot point, the clipping planes, the size
of grid and axes and what `showEntireScene()` fits into the window. Calling
`setSceneCenter()` also moves the pivot point there.

## Implementation details

### Classes

| Class / file                | Purpose                                              |
|-----------------------------|------------------------------------------------------|
| `QtmGLViewer`               | The widget: GL setup, frame loop, input, overlay     |
| `qtm::Camera`               | Camera frame, projection and mouse math              |
| `qtm::RenderContext`        | Per-frame matrices, built-in shaders, draw helpers   |
| `qtm::MeshBuffer`           | VAO with one VBO per attribute and an index buffer   |
| `qtm::MeshData`             | CPU geometry and primitive generators                |

### Camera

The camera is a position plus an orientation quaternion. It looks along its
local -Z axis with +Y as up vector. The view matrix is the inverse of that
frame. All the numerical behavior follows libQGLViewer so that it feels
the same:

* Rotation is the "deformed ball" trackball: the mouse positions are projected
  onto a sphere near the pivot point and onto a hyperbolic sheet further away.
  The rotation angle is `5 * asin(...)` of the sine of the angle between the two
  projected points. The camera orbits around the pivot point (the projection
  of the pivot point on the screen is the center of the ball).
* Translation scales the mouse movement so that the scene at pivot depth
  follows the cursor (perspective: `2 * tan(fov / 2) * depth / height`).
* Zoom moves the camera along the view axis. The step is proportional to the
  distance to the pivot point, but at least `0.2 * sceneRadius`, so that the
  camera does not get stuck when it is very close. The wheel uses
  `angleDelta * 8e-4` as zoom value.
* The clipping planes are `zFar = distance + sqrt(3) * radius` and
  `zNear = distance - sqrt(3) * radius`, limited from below by
  `0.005 * sqrt(3) * radius`. Because they are derived from the scene radius
  and not from the actual geometry, meshes of any size (0.01 or 1000 units)
  render without clipping as long as the scene radius is set.
* The vertical field of view is 45 degrees. `showEntireScene()` moves the
  camera backwards until the scene sphere fits horizontally and vertically.
* In orthographic mode the visible half height is `tan(fov / 2) * depth of the
  pivot point`, so switching the projection keeps the apparent size.

### Frame loop

`paintGL()` does the following in each frame:

1. Reset state that `QPainter` may have changed in the previous frame (scissor,
   stencil, blending, culling, masks), clear color and depth.
2. Update the camera screen size, compute view and projection matrices
   (`RenderContext::beginFrame()`).
3. Enable depth test (`GL_LEQUAL`) and call the virtual `draw()`.
4. Reset a few states that `draw()` may have changed and draw grid and axes.
5. Update the FPS counter (average over 20 frames, like libQGLViewer).
6. Draw the text overlay with a `QPainter` on top of the finished 3D image.

Text is not rendered with OpenGL. `drawText()` queues strings which are painted
in step 6. Everything that is queued in `draw()` is shown in the same frame.
For custom 2D painting override `drawOverlay(QPainter &)`.

The widget only repaints when needed (`update()` after input or property
changes). There is no continuous render loop, apart from the optional spinning
timer.

### GPU buffers

`MeshBuffer` owns one vertex array object, one VBO per attribute location and
optionally an index buffer (32 bit indices). Attribute locations used by the
built-in shaders:

| Location | Content        |
|----------|----------------|
| 0        | Position (vec3)|
| 1        | Normal (vec3)  |
| 2        | Color (vec3)   |

`setAttribute(location, data, count, components, divisor)` uploads any float
attribute. A divisor greater than 0 makes it a per instance attribute, which
together with `setInstanceCount()` gives instanced drawing. QTetraMesher uses
this for the solid tetrahedra: each tetrahedron is one instance that stores its
4 corners (48 bytes). The vertex shader builds the 12 vertices of the 4 faces,
shrinks the tetrahedron towards its centroid, computes face normals and hides
the instance if its centroid is outside the cutplane range. Moving the cutplane
sliders therefore only changes uniforms, nothing is re-uploaded.

Data that already lives in a flat float array (for example a
`std::vector<Vec3f>`) can be passed without copying or converting.

All `MeshBuffer` functions, including the destructor and `destroy()`, need the
OpenGL context to be current.

### GPU resource lifetime

`initializeScene()` is called after the context and the built-in resources have
been created, `cleanupScene()` before the context is destroyed. Both are called
with the context current. They run again if Qt recreates the context (for
example when the widget is reparented to another top level window), so create
all GPU resources in `initializeScene()`.

A base class destructor cannot call virtual functions of the derived class.
Therefore a subclass that owns GPU resources has to release them in its own
destructor:

```cpp
MyViewer::~MyViewer() {
  makeCurrent();
  cleanupScene();
  doneCurrent();
}
```

### Lighting and shaders

There are two built-in programs, both GLSL 330 core:

* Solid: per fragment Blinn-Phong lighting, optional per vertex color
  (`MeshBuffer::Color`), otherwise a uniform color. Two sided: back faces use
  the flipped normal, so open surfaces do not turn black from behind.
* Line: unlit, uniform or per vertex color.

The lighting function `qtmHeadlight()` can be reused in custom shaders
(`RenderContext::lightingGlsl()` returns the GLSL source, to be put in front
of your fragment shader code; `setMatrixUniforms()` sets `u_model`, `u_view`,
`u_projection` and `u_normalMatrix`).

The scene has two directional lights, both fixed in eye space (they move with
the camera):

* A headlight along the viewing direction (diffuse 0.5, specular 0.6 share).
* A softer key light from above, slightly from the left and the front, direction
  `(-0.3, 0.85, 0.45)` (diffuse 0.45, specular 0.4 share).

Together with an ambient term of 0.25, a surface facing the camera is fully lit
while surfaces tilted upwards are brighter than those tilted downwards, which
makes the shape much easier to read than with a headlight alone. Surfaces
facing away from both lights keep the ambient color. The constants are at the top
of `RenderContext.cpp` and can be changed easily.

Core profile OpenGL only guarantees lines with a width of 1 pixel, so all lines
are 1 pixel wide.

### Text and status messages

`displayMessage(text, ms)` shows a message in the lower left corner, which
disappears after the delay. The FPS counter is drawn in the upper left corner.
`drawText(x, y, text, color)` uses widget coordinates with the origin in the
upper left corner and the y value being the text baseline.

### Procedural geometry

`MeshData` and the functions in `qtm::primitives` produce geometry on the CPU:
`mobiusStrip()`, `truncatedCone()`, `arrow()` (same proportions as
`QGLViewer::drawArrow()`), `grid()`, `axisLetters()`, `appendBoxLines()` and
`triangleEdges()` (unique edge list of an indexed triangle mesh). Grid, axes and
the test strip are built with these.

## Differences to libQGLViewer

Not implemented: stereo rendering, snapshot saving, camera paths and key frames,
saving and restoring the camera state, mouse grabbers, manipulated frames
(moving objects with the mouse), the help window, and configurable mouse and key
bindings. The bindings are fixed in `QtmGLViewer.cpp`. The about/help dialogs
belong to the application.

## Files

```
QtmGLViewer/
  CMakeLists.txt
  include/QtmGLViewer/   public headers
  src/                   implementation
  test/                  test application (QtmGLViewerTest)
```
