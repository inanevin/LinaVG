<br/>
<p align="center">
  <img src="https://user-images.githubusercontent.com/3519379/173690417-be2e171d-3f08-4afc-bb48-252b2316f140.png">
</p>
<br/>

<div align="center">
  
[![License](https://img.shields.io/badge/License-BSD_2--Clause-orange.svg)](https://opensource.org/licenses/BSD-2-Clause)
[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://paypal.me/inanevin) 
[![Lina Discord](https://badgen.net/discord/members/QYeTkEtRMB)](https://discord.gg/QYeTkEtRMB)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](https://github.com/inanevin/LinaVG/issues) 
![GitHub issues](https://img.shields.io/github/issues/inanevin/LinaEngine.svg)
  
</div>


LinaVG is a 2D vector graphics library providing low-level functionality to draw variety of anti-aliased convex shapes & lines, along with traditional and Signed-Distance-Field (SDF) text rendering. LinaVG also provides rich styling options including gradients, outlines, drop shadows, varying thickness, filled & non-filled shapes, text alignment/spacing and many more!

![image](https://user-images.githubusercontent.com/3519379/173692019-f624a50f-5ae5-41e2-bed2-8a8e1b2c9f47.png)

LinaVG does not provide a "rendering" functionality on its own. It generates buffers according to your draw calls and sends them back to a custom backend you determine in batches of optimized buffers.

That being said, the example project provides an example OpenGL backend you can basically copy and paste. Also [Lina Engine](https://github.com/inanevin/LinaEngine) uses LinaVG and have examples for custom DirectX12 backend. Please check [Wiki](https://github.com/inanevin/LinaVG/wiki) for more details.

# What

LinaVG's purpose is to provide you with an easy way to do low-level anti-aliased shape, line and text rendering. It does not provide any window management or input functionality, and it's not a GUI library. It assumes your application already has a graphics rendering backend and an application loop setup. 

With that in mind, you can use LinaVG to build both retained and immediate mode GUI applications/libraries.

LinaVG was initially made for [Lina Engine](https://www.github.com/inanevin/LinaEngine), however this library is completely decoupled from it. You can use LinaVG in your own projects with minimal dependencies. 

LinaVG is tested on:

- MSVC 14.36.xxx
- Clang 17.0.1
  
# Thread-safety

LinaVG is thread-safe as long as no multiple threads modify the same LinaVG::Drawer object. Only global functions are not thread-safe at the moment are those used for loading fonts, which require wrapping around a mutex.

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

Download a release from [Releases](https://github.com/inanevin/LinaVG/releases).

Recommended way of using LinaVG is to link it together with your application using CMake. Alternatively, you can build the CMake project yourself and link to produced binaries, but please mind that you also have to link to the produced FreeType binaries if you choose to do it this way.

Use ```LINAVG_BUILD_EXAMPLES``` option to build the example project.

```shell
cmake DLINAVG_BUILD_EXAMPLES=ON
```

Use ```LINAVG_DISABLE_TEXT_SUPPORT``` option to skip text support and FreeType dependency.

```shell
cmake DLINAVG_DISABLE_TEXT_SUPPORT=ON
```

Note: LinaVG requires C++ 17 features.

# Quick Demonstration

Below is a bare-minimum implementation steps for drawing with LinaVG. As said in the home page, it's assumed that your application already has a graphics rendering backend setup and running, of course along with a window with a valid context.
```cpp

#include "LinaVG.hpp"

LinaVG::Drawer lvgDrawer;
lvgDrawer.GetCallbacks().drawDefault = std::bind(&MyRenderingBackend::DrawDefault, &myRenderer, std::placeholders::_1);

// Your application loop
while (m_applicationRunning)
{
    // Setup style, give a gradient color from red to blue.
    StyleOptions style;
    style.isFilled      = true;
    style.color.start = Vec4(1, 0, 0, 1);
    style.color.end   = Vec4(0, 0, 1, 1);

    // Draw a 200x200 rectangle starting from 300, 300.
    const Vec2 min = Vec2(300, 300);
    const Vec2 max = Vec2(500, 500);
    lvgDrawer.DrawRect(min, max, style);

    lvgDrawer.FlushBuffers();
    lvgDrawer.ResetFrame();
}

```

And that's basically it!

![1](https://user-images.githubusercontent.com/3519379/173247621-4f38cbe2-308f-4cd7-aa9f-8da150d83780.png)

There are a lot more to LinaVG in regards to usage, configuration, other shapes/lines/texts and styling. Check out the rest of the Wiki or the example application to learn about them all.

# [License (BSD 2-clause)](http://opensource.org/licenses/BSD-2-Clause)

<a href="http://opensource.org/licenses/BSD-2-Clause" target="_blank">
<img align="right" src="https://opensource.org/wp-content/uploads/2022/10/osi-badge-dark.svg" width="100" height="130">
</a>

	Copyright [2022-] Inan Evin
	
	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:
	
	   1. Redistributions of source code must retain the above copyright notice, this
	      list of conditions and the following disclaimer.
	
	   2. Redistributions in binary form must reproduce the above copyright notice,
	      this list of conditions and the following disclaimer in the documentation
	      and/or other materials provided with the distribution.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
	INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
	BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
	OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
	OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
	OF THE POSSIBILITY OF SUCH DAMAGE.

# Contributing

Any contributions and PR are welcome.

# Support

[![Lina Discord](https://badgen.net/discord/members/QYeTkEtRMB)](https://discord.gg/QYeTkEtRMB)

You can join [Lina Engine's Discord channel](https://discord.gg/QYeTkEtRMB) to talk about the Lina Project.
