![Lina](Docs/Images/linalogofull_medium.png)

## Lina VG
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT) 
[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://paypal.me/inanevin) 
[![Lina Discord](https://badgen.net/discord/members/QYeTkEtRMB)](https://discord.gg/QYeTkEtRMB)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/c0c10a437a214dbf963210ed3edf3c4f)](https://www.codacy.com/gh/inanevin/LinaEngine/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=inanevin/LinaEngine&amp;utm_campaign=Badge_Grade)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](https://github.com/inanevin/LinaVG/issues) 
![GitHub issues](https://img.shields.io/github/issues/inanevin/LinaEngine.svg)



LinaVG is a 2D vector graphics library mainly developed for [Lina Engine](https://www.github.com/inanevin/LinaEngine), however it's decoupled from any of the Lina Engine functionality. You can use LinaVG in your own projects to facilitate the low-level drawing functionality for rendering filled & non-filled convex shapes, lines with features such as flat colors, gradients, textures, outlines and vector-based anti-aliasing.

LinaVG currently only supports OpenGL backend, but feel free to contribute by implementing low-level drawing functionalities on other backends!

## Installation

```shell
# Initialize git in your desired directory.
git init

# Clone Lina Engine
git clone https://github.com/inanevin/LinaVG

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

This will build static libraries, now you can link your application to the LinaVG library & include the headers.

Check out [CMake Documentation](https://cmake.org/cmake/help/v3.2/manual/cmake.1.html) for specifying more options and configurations like project file generation directories and more.
