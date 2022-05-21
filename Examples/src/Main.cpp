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
#include "DemoScreens.hpp"
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

            const float sizeX = 1440.0f;
            const float sizeY = 960.0f;

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

            DemoScreens demoScreens;

            // Init LinaVG
            LinaVG::Initialize();
            demoScreens.Initialize(this);

            float prevTime = exampleBackend.GetTime();

            // Application loop.
            while (!m_shouldClose)
            {
                float now   = exampleBackend.GetTime();
                m_deltaTime = now - prevTime;
                prevTime    = now;

                // Example exampleBackend input & rendering.
                exampleBackend.Poll();
                exampleBackend.Render();

                // Lina VG start frame.
                LinaVG::StartFrame();


                demoScreens.ShowBackground();

                if (m_currentDemoScreen == 0)
                    demoScreens.ShowDemoScreen1_Shapes();
                else if (m_currentDemoScreen == 1)
                    demoScreens.ShowDemoScreen2_Outlines();
                else if (m_currentDemoScreen == 2)
                    demoScreens.ShowDemoScreen3_Colors();
                else if (m_currentDemoScreen == 3)
                    demoScreens.ShowDemoScreen4_Lines();
                else if (m_currentDemoScreen == 4)
                    demoScreens.ShowDemoScreen5_Texts();

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
            m_currentDemoScreen = key;
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