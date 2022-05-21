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
#include <chrono>

#ifdef LINAVG_BACKEND_GL
#include "Backends/OpenGLGLFW.hpp"
#endif

int main(int argc, char* argv[])
{
    LinaVG::Examples::ExampleApp app;
    app.Run();
    return 0;
}

namespace LinaVG
{
    namespace Examples
    {
        ExampleApp* ExampleApp::s_exampleApp = nullptr;

        void ExampleApp::Run()
        {
            s_exampleApp = this;
            ExampleBackend exampleBackend;

            const float sizeX = 1920.0f;
            const float sizeY = 1080.0f;

            // Initialize example exampleBackend.
            exampleBackend.InitWindow(static_cast<int>(sizeX), static_cast<int>(sizeY));

            // Setup Lina VG config.
            LinaVG::Config.m_displayPos.x  = 0.0f;
            LinaVG::Config.m_displayPos.y  = 0.0f;
            LinaVG::Config.m_displaySize.x = sizeX;
            LinaVG::Config.m_displaySize.y = sizeY;
            // LinaVG::Config.m_aaMultiplier = 3;
            LinaVG::Config.m_framebufferScale.x = LinaVG::Config.m_framebufferScale.y = exampleBackend.GetFramebufferScale();
            Config.m_flipTextureUVs                                                   = true;
            LinaVG::Config.m_errorCallback                                            = [](const std::string& err) {
                std::cerr << err.c_str() << std::endl;
            };

            LinaVG::Config.m_logCallback = [](const std::string& log) {
                std::cout << log.c_str() << std::endl;
            };

            // Init LinaVG
            LinaVG::Initialize();

            LinaVG::LoadFont("Resources/Fonts/OpenSans-Regular.ttf", 64);

            float prev = exampleBackend.GetTime();

            // Application loop.
            while (!m_shouldClose)
            {
                float now   = exampleBackend.GetTime();
                m_deltaTime = now - prev;
                prev        = now;

                // Example exampleBackend input & rendering.
                exampleBackend.Poll();
                exampleBackend.Render();

                // Lina VG start frame.
                LinaVG::StartFrame();

                //  Define style options & render rect.
                LinaVG::StyleOptions opts;
                opts.m_isFilled        = true;
                opts.m_color           = LinaVG::Vec4(1, 1, 1, 1);
                opts.m_thickness       = 5.0f;
                opts.m_rounding        = 0.0f;
                opts.m_textureUVTiling = Vec2(1.0f, 1.0f);
                opts.m_textureUVOffset = Vec2(0.0f, 0.0f);
                // opts.m_outlineOptions.m_drawDirection = OutlineDrawDirection::Inwards;
                opts.m_outlineOptions.m_drawDirection = OutlineDrawDirection::Both;
                opts.m_outlineOptions.m_color         = LinaVG::Vec4Grad(LinaVG::Vec4(1, 0, 0, 1), LinaVG::Vec4(0, 0, 1, 1));
                opts.m_outlineOptions.m_thickness     = 7;
                //   opts.m_outlineOptions.m_thickness = 2.0f;
                LinaVG::DrawRect(LinaVG::Vec2(100, 200), LinaVG::Vec2(300, 500), opts, 40.0f, 0);


                TextOptions textOpts;
                textOpts.m_color.m_gradientType = GradientType::Vertical;
                textOpts.m_color.m_start        = Vec4(1, 0, 0, 1);
                textOpts.m_color.m_end          = Vec4(0, 0, 1, 1);
                textOpts.m_textScale = 1.0f;
                //textOpts.m_dropShadowOffset     = Vec2(5.0f, 5.5f);
               // textOpts.m_textScale            = 0.8f;
               // textOpts.m_outlineThickness = 1.0f;
               // textOpts.m_outlineColor = Vec4(1,1,1,1);
               LinaVG::DrawText("T", LinaVG::Vec2(300, 500), textOpts, 0, true);

                LinaVG::DrawText("SA!", LinaVG::Vec2(300, 800), textOpts, 0);
                // LinaVG::DrawText("moprngh", LinaVG::Vec2(800, 800), 1, opts, 0);
                // Lina VG Render & end frame.
                LinaVG::Render();
                LinaVG::EndFrame();

                // Backend window swap buffers.
                exampleBackend.SwapBuffers();
            }

            // Terminate Lina VG & example exampleBackend.
            LinaVG::Terminate();
            exampleBackend.Terminate();
        }

        void ExampleApp::OnHorizontalKeyCallback(float input)
        {
            LinaVG::Config.m_debugOrthoOffset.x += input * m_deltaTime * 1000;
        }

        void ExampleApp::OnVerticalKeyCallback(float input)
        {
            LinaVG::Config.m_debugOrthoOffset.y -= input * m_deltaTime * 1000;
        }

        void ExampleApp::OnSpaceCallback()
        {
        }

        void ExampleApp::OnNumKeyCallback(int key)
        {
        }

        void ExampleApp::OnFCallback()
        {
            LinaVG::Config.m_debugWireframeEnabled = !LinaVG::Config.m_debugWireframeEnabled;
        }

        void ExampleApp::OnMouseScrollCallback(float val)
        {
            LinaVG::Config.m_debugOrthoProjectionZoom -= val * m_deltaTime * 10;
        }

        void ExampleApp::OnWindowResizeCallback(int width, int height)
        {
            LinaVG::Config.m_displaySize.x = static_cast<float>(width);
            LinaVG::Config.m_displaySize.y = static_cast<float>(height);
        }
        void ExampleApp::OnWindowCloseCallback()
        {
            m_shouldClose = true;
        }
    } // namespace Examples
} // namespace LinaVG