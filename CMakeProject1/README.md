## Installation on Windows

```shell
# clone the repository
gh repo clone SpicyRicecaker/cs457
# install vcpkg. Then, 
vcpkg install glew:x64-windows glm:x64-windows freeglut:x64-windows
# now open the cmakelists.txt file with visual studio open > cmake project.
# profit
```

We install to a global directory, since for some reason there's no option to exclude folders from symbol search in visual studio?? And even if we move `vcpkg` to another folder, still some completely random files show up in symbol search. Whatever, c'est la cost of using windows.

## Installation on Fedora 39 Linux (Deprecated)

### Build Dependencies

Install the mold linker.
Install cmake.
Have a recent gcc compiler that supports the c++20 standard.

### Dev Dependencies

Install the following development dependencies:

```shell
sudo dnf install mesa-libGL # for <GL/gl.h> import.
sudo dnf install mesa-libGL-devel # for <GL/gl.h> import. (Opengl)
sudo dnf install mesa-libGLU-devel # for <GL/glu.h> import. "Provides functions to simplify common matrix operations, like setting up perspective or orthographic projection matrices" (Not sure which)
sudo dnf install freeglut-devel # for <glut.h> import. "Helps with creating windows, handling input, and managing the event loop in OpenGL"
sudo dnf install glew-devel # for <GL/glew.h> import. "Provides efficient run-time mechanisms for determining which OpenGL extensions are supported on the target platform"
sudo dnf install glm-devel # for <glm/glm.hpp> import. "Linear algebra library".
```

## Compile & Run

In the root directory, run the following commands:

```shell
mkdir build
cd build
cmake ..
make -j$(nproc)
./main
```

# VSCode Specific Settings

## Extensions

It is recommended to install `GLSL Lint` and `Shader languages support` extensions to edit shader files with syntax highlighting and syntax checking. `glslang` is a dependency for `Shader languages support`, install it on Fedora as follows:

```shell
sudo dnf install glslang
```

## Run keybinding

Add the following to your VSCode `keybindings.json`:

```json
{
"key": "ctrl+f5", 
"command": "workbench.action.tasks.runTask",
"args": "Run CMake Project"
}
```

## Colorized Output

Make sure that the cmake build tasks outputs to the terminal (instead of output pane) with the following setting in `settings.json`:

```json
"cmake.buildTask": true
```

and in your `CMakeLists.txt` file, ensure the `-fdiagnostics-color=always` flag is passed to the c++ compiler

```shell
set(CMAKE_CXX_FLAGS "-fdiagnostics-color=always")
```