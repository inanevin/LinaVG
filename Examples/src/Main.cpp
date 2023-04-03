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
#include <iostream>
#include <chrono>
#include "Backends/OpenGLGLFW.hpp"

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

            const unsigned int sizeX = 1440;
            const unsigned int sizeY = 960;

            // Initialize example exampleBackend.
            exampleBackend.InitWindow(static_cast<int>(sizeX), static_cast<int>(sizeY));

            // Setup Lina VG config.
            LinaVG::Config.displayPosX   = 0;
            LinaVG::Config.displayPosY   = 0;
            LinaVG::Config.displayWidth  = sizeX;
            LinaVG::Config.displayHeight = sizeY;
            LinaVG::Config.clipPosX = LinaVG::Config.clipPosY = 0;
            LinaVG::Config.clipSizeX                            = sizeX;
            LinaVG::Config.clipSizeY                            = sizeY;

            LinaVG::Config.errorCallback = [](const std::string& err) {
                std::cerr << err.c_str() << std::endl;
            };

            LinaVG::Config.logCallback = [](const std::string& log) {
                std::cout << log.c_str() << std::endl;
            };

            m_checkeredTexture = exampleBackend.CreateTexture("Resources/Textures/Checkered.png");
            m_linaTexture      = exampleBackend.CreateTexture("Resources/Textures/Lina.png");

            // Init LinaVG
            LinaVG::Initialize();
            m_demoScreens.Initialize();

            float prevTime    = exampleBackend.GetTime();
            float lastFPSTime = exampleBackend.GetTime();
            int   frameCount  = 0;

            // Application loop.
            while (!m_shouldClose)
            {
                float now   = exampleBackend.GetTime();
                m_deltaTime = now - prevTime;
                prevTime    = now;
                m_elapsedTime += m_deltaTime;

                if (now > lastFPSTime + 1.0f)
                {
                    m_fps           = frameCount;
                    frameCount      = 0;
                    lastFPSTime     = now;
                    m_deltaTimeRead = m_deltaTime;
                }

                // Example exampleBackend input & rendering.
                exampleBackend.Poll();
                exampleBackend.Render();

                // Lina VG start frame.
                LinaVG::StartFrame();

                // Setup style.
                StyleOptions style;
                style.outlineOptions.thickness     = 2.0f;
                style.outlineOptions.drawDirection = OutlineDrawDirection::Inwards;

                // Same options as style.m_color
                style.outlineOptions.color.start = Vec4(1, 0, 0, 1);
                style.outlineOptions.color.end   = Vec4(0, 0, 1, 1);

                // Same options as style.m_textureXXX
                style.outlineOptions.textureHandle   = ExampleApp::Get()->GetCheckeredTexture();
                style.outlineOptions.textureUVOffset = Vec2(0.0f, 0.0f);
                style.outlineOptions.textureUVTiling = Vec2(1.0f, 1.0f);

                m_demoScreens.ShowBackground();

                if (m_currentDemoScreen == 1)
                    m_demoScreens.ShowDemoScreen1_Shapes();
                else if (m_currentDemoScreen == 2)
                    m_demoScreens.ShowDemoScreen2_Colors();
                else if (m_currentDemoScreen == 3)
                    m_demoScreens.ShowDemoScreen3_Outlines();
                else if (m_currentDemoScreen == 4)
                    m_demoScreens.ShowDemoScreen4_Lines();
                else if (m_currentDemoScreen == 5)
                    m_demoScreens.ShowDemoScreen5_Texts();
                else if (m_currentDemoScreen == 6)
                    m_demoScreens.ShowDemoScreen6_DrawOrder();
                else if (m_currentDemoScreen == 7)
                    m_demoScreens.ShowDemoScreen7_Clipping();
                else if (m_currentDemoScreen == 8)
                    m_demoScreens.ShowDemoScreen8_Animated();
                else if (m_currentDemoScreen == 9)
                    m_demoScreens.ShowDemoScreen9_Final();

                // Flush everything we've drawn so far to the screen.
                LinaVG::Render();

                // Let demo screens know we're ending this frame.
                m_demoScreens.PreEndFrame();

                // Let LinaVG know we are ending this frame.
                LinaVG::EndFrame();

                // Backend window swap buffers.
                exampleBackend.SwapBuffers();
                frameCount++;
            }

            // Terminate Lina VG & example exampleBackend.
            m_demoScreens.Terminate();
            LinaVG::Terminate();
            exampleBackend.Terminate();
        }

        void ExampleApp::OnHorizontalKeyCallback(float input)
        {
            LinaVG::Config.debugOrthoOffset.x += input * m_deltaTime * 1000;
        }

        void ExampleApp::OnVerticalKeyCallback(float input)
        {
            LinaVG::Config.debugOrthoOffset.y -= input * m_deltaTime * 1000;
        }

        void ExampleApp::OnNumKeyCallback(int key)
        {
            if (key > 0 && key < 10)
                m_currentDemoScreen = key;

            if (key == 7)
                m_demoScreens.m_clippingEnabled = true;
        }

        void ExampleApp::OnPCallback()
        {
            m_demoScreens.m_statsWindowOn = !m_demoScreens.m_statsWindowOn;
        }

        void ExampleApp::OnRCallback()
        {
            m_demoScreens.m_rotate = !m_demoScreens.m_rotate;
        }

        void ExampleApp::OnFCallback()
        {
            LinaVG::Config.debugWireframeEnabled = !LinaVG::Config.debugWireframeEnabled;
        }

        void ExampleApp::OnCCallback()
        {
            if (m_currentDemoScreen == 7)
            {
                m_demoScreens.m_clippingEnabled = !m_demoScreens.m_clippingEnabled;
            }
        }

        void ExampleApp::OnECallback()
        {
            m_demoScreens.m_rotateAngle = 0.0f;
        }

        void ExampleApp::OnMouseScrollCallback(float val)
        {
            LinaVG::Config.debugOrthoProjectionZoom -= val * m_deltaTime * 10;
        }

        void ExampleApp::OnWindowResizeCallback(int width, int height)
        {
            LinaVG::Config.displayWidth  = static_cast<BackendHandle>(width);
            LinaVG::Config.displayHeight = static_cast<BackendHandle>(height);
            LinaVG::Config.clipSizeX     = static_cast<BackendHandle>(width);
            LinaVG::Config.clipSizeY     = static_cast<BackendHandle>(height);
        }
        void ExampleApp::OnWindowCloseCallback()
        {
            m_shouldClose = true;
        }
    } // namespace Examples
} // namespace LinaVG