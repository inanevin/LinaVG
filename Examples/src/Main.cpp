/*
This file is a part of: LinaVG
https://github.com/inanevin/LinaVG

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2022-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Main.hpp"
#include "LinaVG.hpp"

#ifdef LINAVG_BACKEND_GL
#include "Backends/OpenGLGLFW.hpp"
#endif

int main(int argc, char* argv[])
{
    LinaVG::Examples::Application app;
    app.Run();
    return 0;
}

namespace LinaVG
{
    namespace Examples
    {
        void Application::Run()
        {
            ExampleBackend backend;

            const float sizeX = 1920.0f;
            const float sizeY = 1080.0f;

            // Initialize example backend.
            backend.InitWindow(sizeX, sizeY);

            // Setup Lina VG config.
            LinaVG::Config.m_displayPos.x       = 0.0f;
            LinaVG::Config.m_displayPos.y       = 0.0f;
            LinaVG::Config.m_displaySize.x      = sizeX;
            LinaVG::Config.m_displaySize.y      = sizeY;
            LinaVG::Config.m_framebufferScale.x = LinaVG::Config.m_framebufferScale.y = backend.GetFramebufferScale();
            Config.m_flipTextureUVs                                                   = true;
            LinaVG::Config.m_errorCallback                                            = [](const std::string& err) {
                std::cerr << err.c_str() << std::endl;
            };

            LinaVG::Config.m_logCallback = [](const std::string& log) {
                std::cout << log.c_str() << std::endl;
            };

            // Init LinaVG
            LinaVG::Initialize();

            // Application loop.
            while (!backend.m_shouldClose)
            {
                // Example backend input & rendering.
                backend.Poll();
                backend.Render();

                // Lina VG start frame.
                LinaVG::StartFrame();

                // Define style options & render rect.
                LinaVG::StyleOptions opts;
                opts.m_isFilled = true;
                opts.m_color = LinaVG::Vec4(1,0,0,1);
                opts.m_thickness                             = 5.0f;
                opts.m_rounding                              = 0.0f;
                opts.m_textureUVTiling                       = Vec2(1.0f, 1.0f);
                opts.m_textureUVOffset                       = Vec2(0.0f, 0.0f);
                opts.m_outlineOptions.m_color                = Vec4(1, 0, 0, 1);
                opts.m_color                                 = Vec4(1, 1, 1, 1);
                opts.m_outlineOptions.m_thickness = 2.0f;
                LinaVG::DrawRect(LinaVG::Vec2(100, 200), LinaVG::Vec2(300, 500), opts, 0.0f, 0.0f);

                // Lina VG Render & end frame.
                LinaVG::Render();
                LinaVG::EndFrame();

                // Backend window swap buffers.
                backend.SwapBuffers();
            }

            // Terminate Lina VG & example backend.
            LinaVG::Terminate();
            backend.Terminate();
        }
    } // namespace Examples
} // namespace LinaVG