<br/>
<p align="center">
  <img src="https://user-images.githubusercontent.com/3519379/173690417-be2e171d-3f08-4afc-bb48-252b2316f140.png">
</p>
<br/>

<div align="center">
  
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT) 
[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://paypal.me/inanevin) 
[![Lina Discord](https://badgen.net/discord/members/QYeTkEtRMB)](https://discord.gg/QYeTkEtRMB)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/59f8c18dd1a24b478fe6459344252e3c)](https://www.codacy.com/gh/inanevin/LinaVG/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=inanevin/LinaVG&amp;utm_campaign=Badge_Grade)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](https://github.com/inanevin/LinaVG/issues) 
![GitHub issues](https://img.shields.io/github/issues/inanevin/LinaEngine.svg)
  
</div>


LinaVG is a 2D vector graphics library providing low-level functionality to draw variety of anti-aliased convex shapes & lines, along with traditional and Signed-Distance-Field (SDF) text rendering. LinaVG also provides rich styling options including gradients, outlines, drop shadows, varying thickness, filled & non-filled shapes, text alignment/spacing and many more!

![image](https://user-images.githubusercontent.com/3519379/173692019-f624a50f-5ae5-41e2-bed2-8a8e1b2c9f47.png)

LinaVG currently only supports OpenGL backend, however the actual rendering API is rather similar, so feel free to add your own backends! This process is explained in [Wiki](https://github.com/inanevin/LinaVG/wiki) in detail. Additionally, it's possible to access the internal vector buffers for any kind of custom rendering functionality.

# What

LinaVG's purpose is to provide you with an easy way to do low-level anti-aliased shape, line and text rendering. It does not provide any window management or input functionality, and it's not a GUI library. It assumes your application already has a graphics rendering backend and an application loop setup. 

With that in mind, you can use LinaVG to build both retained and immediate mode GUI applications/libraries.

LinaVG was initially made for [Lina Engine](https://www.github.com/inanevin/LinaEngine), however this library is completely decoupled from it. You can use LinaVG in your own projects with minimal dependencies. 

# Features

![lina](https://user-images.githubusercontent.com/3519379/173691256-289869b6-7fa9-462b-b2ef-28d6680c7bdd.gif)


## Shapes

* Rectangle, triangle, ngon, circle, half-circle, arcs, user-defined convex shapes
* All shapes can be filled & non-filled
* Flat colors, vertical/horizontal/radial gradients
* Customizable thickness
* Textures, custom UV offsets, custom UV tiling
* Shape rounding, only rounding particular corners if desired
* Custom rotation

## Outlines

* Inner outlines, outer outlines & both
* Customizable outline thickness
* Flat colors, vertical/horizontal/radial gradients
* Textures, custom UV offsets, custom UV tiling

## AA

* Vector-based anti-aliasing borders
* Framebuffer scaled AA thickness
* User-defined AA multipliers

## Lines

* Single lines
* Multi lines
* Bezier curves
* Line caps: Left, right & both
* Customizable line cap rounding
* Line Joints: Vertex Average, Miter, Bevel, Bevel Round
* All outline options apply to lines as well.
* Custom rotation only on single lines

## Fonts

* FreeType font loading
* SDF fonts
* Font atlases, atlas merging
* Custom glyph-ranges
* Unicode support

## Texts

* Traditional anti-aliased bitmap glyph rendering
* Flat colors, vertical/horizontal gradients
* Drop shadows, customizable drop shadow color, customizable offsets.
* Character spacing
* Line spacing
* Word-wrapping
* Text alignment: Left, right & center
* Custom rotation

## SDF

* All text options apply to SDF texts.
* SDF thickness
* SDF softness
* SDF outlines

## Utility

* Custom draw orders, z-sorting
* Rect clipping
* Exposed configs, such as; garbage collection intervals, buffer reserves, AA params, line joint limits, texture flipping, debug functionality


# Installation

[Releases](https://github.com/inanevin/LinaVG/releases) contain pre-built binaries for linking against LinaVG. Your application first needs to link against Glad and FreeType libraries distributed alongside with LinaVG release. Simply link against those two, then against LinaVG and you are good to go. If you plan to use LinaVG within your source-code and compile it along with your application, again you still need to link against those two, then include the header & source files of LinaVG in your application.

If you choose to compile custom versions of said dependencies, their respective versions are available on [Lina Dependencies](https://github.com/inanevin/LinaDependencies/tree/linavg) repository.

Note: LinaVG requires C++ 17 features.

# Demo Application

You can download this whole repository and generate the project files using CMake to run the example application, demonstrating all capabilities of LinaVG.

```shell

# Clone repo
git clone https://github.com/inanevin/LinaVG

# Create target dir & cd
mkdir build_x64
cd build_x64

# Build LinaVG w/ GL backend & examples
cmake ../ -DLINAVG_BUILD_EXAMPLES=ON -DLINAVG_BACKEND_GL=ON -G "Visual Studio 17 2022" -A "x64"

# After the project files are generated, you can build the project via
cmake --build . --target ALL_BUILD

```

CMake build process downloads the pre-built dependencies during configuration. If you choose to build & run the demo application using another method, remember to get the required dependency binaries from [Lina Dependencies](https://github.com/inanevin/LinaDependencies/tree/linavg) repository.

# Quick Demonstration

Below is a bare-minimum implementation steps for drawing with LinaVG. As said in the home page, it's assumed that your application already has a graphics rendering backend setup and running, of course along with a window with a valid context.
```cpp

// Include LinaVG
#include "LinaVG.hpp"

// Configure display properties based on your application window
LinaVG::Config.displayPosX   = 0;
LinaVG::Config.displayPosY   = 0;
LinaVG::Config.displayWidth  = myDisplayWidth;
LinaVG::Config.displayHeight = myDisplayHeight;

// Initialize LinaVG
LinaVG::Initialize();

// Your application loop
while (m_applicationRunning)
{

    // Let LinaVG know that we are starting to render.
    LinaVG::StartFrame();

    // Setup style, give a gradient color from red to blue.
    StyleOptions style;
    style.isFilled      = true;
    style.color.start = Vec4(1, 0, 0, 1);
    style.color.end   = Vec4(0, 0, 1, 1);

    // Draw a 200x200 rectangle starting from 300, 300.
    const Vec2 min = Vec2(300, 300);
    const Vec2 max = Vec2(500, 500);
    LinaVG::DrawRect(min, max, style);

    // Finally, flush all the buffers and render to screen.
    LinaVG::Render();

    // Let LinaVG know that we are finishing this frame
    LinaVG::EndFrame();
}

// Terminate LinaVG before exiting your application.
LinaVG::Terminate();

```

And that's basically it! Now you should have this on your screen, easy peasy.

![1](https://user-images.githubusercontent.com/3519379/173247621-4f38cbe2-308f-4cd7-aa9f-8da150d83780.png)

There are a lot more to LinaVG in regards to usage, configuration, other shapes/lines/texts and styling. Check out the rest of the Wiki or the example application to learn about them all.

# License

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT) 
[LICENSE.MD](https://github.com/inanevin/LinaVG/blob/master/LICENSE)

# Contributing

Any contributions and PR are welcome.

# Support

You can join [Lina Engine's Discord channel](https://discord.gg/QYeTkEtRMB) to talk about the Lina Project.
[![Lina Discord](https://badgen.net/discord/members/QYeTkEtRMB)](https://discord.gg/QYeTkEtRMB)
