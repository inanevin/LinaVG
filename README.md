![Lina](Docs/Images/linalogofull_medium.png)

## Lina 2D
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/c0c10a437a214dbf963210ed3edf3c4f)](https://www.codacy.com/gh/inanevin/LinaEngine/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=inanevin/LinaEngine&amp;utm_campaign=Badge_Grade)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](https://github.com/inanevin/LinaGUI/issues) 
![GitHub issues](https://img.shields.io/github/issues/inanevin/LinaEngine.svg)

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT) 
Check out the [Licence](LICENSE) file.

Lina2D is a 2D graphics & an immediate-mode GUI library mainly developed for [Lina Engine](https://www.github.com/inanevin/LinaEngine), however it's decoupled from any of the Lina Engine functionality. You can use Lina2D in your own projects to facilitate the low-level drawing functionality for rendering basic anti-aliased shapes or use the high-level widgets to draw immediate-mode user interfaces.

Lina2D currently only supports OpenGL backend, but feel free to contribute by implementing low-level drawing functionalities on other backends!

## Installation

```shell
# Initialize git in your desired directory.
git init

# Clone Lina Engine
git clone https://github.com/inanevin/Lina2D

# Alternatively you can clone recursively, this will download all the submodules if there exists any along with Lina Engine source code.
git clone --recursive https://github.com/inanevin/LinaGUI
```
You can generate the project files and build using; ~~**CMake**~~

```shell
# Above commands will generate project files with default generator, you can specify a generator if you want.
cmake -G "Visual Studio 17 2022"
```

After generating project files you can either open your IDE and build the ALL_BUILD project which will build all the targets or you can build the binaries from shell.

```shell
# Create a directory for binaries
mkdir bin

# Navigate to directory
cd bin

# Build the root project
cmake ../ -G "Visual Studio 17 2022" -A "x64"

# After the project files are built, you can build the project via
cmake --build . --target ALL_BUILD
```

This will build static libraries, now you can link your application to the Lina2D library & include the headers.

Check out [CMake Documentation](https://cmake.org/cmake/help/v3.2/manual/cmake.1.html) for specifying more options and configurations like project file generation directories and more.
