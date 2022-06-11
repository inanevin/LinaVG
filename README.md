![Lina](Docs/Images/linalogofull_medium.png)

## Lina VG
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT) 
[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://paypal.me/inanevin) 
[![Lina Discord](https://badgen.net/discord/members/QYeTkEtRMB)](https://discord.gg/QYeTkEtRMB)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/c0c10a437a214dbf963210ed3edf3c4f)](https://www.codacy.com/gh/inanevin/LinaEngine/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=inanevin/LinaEngine&amp;utm_campaign=Badge_Grade)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](https://github.com/inanevin/LinaVG/issues) 
![GitHub issues](https://img.shields.io/github/issues/inanevin/LinaEngine.svg)

LinaVG is a 2D vector graphics library providing low-level functionality to draw variety of anti-aliased convex shapes, along with traditional and Signed-Distance-Field (SDF) text rendering. LinaVG also provides rich styling options including gradients, outlines, drop shadows, varying thickness, filled & non-filled shapes, text alignment/spacing and many more!

LinaVG does not provide any window management or input functionality. It's not a GUI library, but rather focused on lower-level shape rendering. It assumes your application already has a graphics rendering backend and an application loop setup. With that in mind, you can use LinaVG to build both retained and immediate mode GUI applications/libraries. Currently only OpenGL backend is supported, however actual rendering API code is rather minimal, so feel free to contribute by implementing low-level drawing functionalities on other backends!

LinaVG was initially made for [Lina Engine](https://www.github.com/inanevin/LinaEngine), however this library is completely decoupled it. You can use LinaVG in your own projects with minimal dependencies. 
