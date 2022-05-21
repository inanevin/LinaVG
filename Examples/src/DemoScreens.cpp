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

#include "DemoScreens.hpp"
#include "Main.hpp"
#include "LinaVG.hpp"

namespace LinaVG
{
    namespace Examples
    {

        FontHandle defaultFont;
        int        drawCount, triangleCount, vertexCount = 0;

        void DemoScreens::Initialize()
        {
            defaultFont = LinaVG::LoadFont("Resources/Fonts/SourceSansPro-Regular.ttf", false, 32);
        }

        /// <summary>
        /// Draw background rect, stats and info rect.
        /// </summary>
        void DemoScreens::ShowBackground()
        {
            const Vec2   screenSize = LinaVG::Config.m_displaySize;
            StyleOptions style;

            // Draw background gradient.
            style.m_color.m_start        = Vec4(0.55f, 0.772, 0.988, 1.0f);
            style.m_color.m_end          = Vec4(0.995f, 0.764f, 0.988f, 1.0f);
            style.m_color.m_gradientType = LinaVG::GradientType::Vertical;
            style.m_isFilled             = true;
            LinaVG::DrawRect(Vec2(0.0f, 0.0f), screenSize, style, 0.0f, 0);

            // Draw stats window.
            const float statsWindowX = screenSize.x - screenSize.x * 0.18f;
            const float statsWindowY = screenSize.y * 0.05f;
            style.m_color            = Vec4(0, 0, 0, 0.5f);
            style.m_rounding         = 0.2f;
            style.m_onlyRoundTheseCorners.push_back(0);
            style.m_onlyRoundTheseCorners.push_back(3);
            LinaVG::DrawRect(Vec2(statsWindowX, statsWindowY), Vec2(screenSize.x, screenSize.y * 0.2f), style, 0.0f, 1);
            style.m_onlyRoundTheseCorners.clear();

            // Draw stats texts.
            const std::string drawCountStr     = std::to_string(drawCount);
            const std::string triangleCountStr = std::to_string(triangleCount);
            const std::string vertexCountStr   = std::to_string(vertexCount);
            const std::string wireframeStr     = Config.m_debugWireframeEnabled ? "Enabled" : "Disabled";
            const std::string fpsStr           = std::to_string(ExampleApp::Get()->GetFPS());
            const std::string frameTimeStr     = std::to_string(ExampleApp::Get()->GetFrameTimeRead()) + " ms";
            Vec2              textPosition     = Vec2(statsWindowX + 10, statsWindowY + 25);
            TextOptions       textStyle;
            textStyle.m_textScale = 0.6f;
            LinaVG::DrawTextNormal("Draw Count: " + drawCountStr, textPosition, textStyle, 0.0f, 2);
            textPosition.y += 25;
            LinaVG::DrawTextNormal("Vertex Count: " + vertexCountStr, textPosition, textStyle, 0.0f, 2);
            textPosition.y += 25;
            LinaVG::DrawTextNormal("Triangle Count: " + triangleCountStr, textPosition, textStyle, 0.0f, 2);
            textPosition.y += 25;
            LinaVG::DrawTextNormal("Wireframe: " + wireframeStr, textPosition, textStyle, 0.0f, 2);
            textPosition.y += 25;
            LinaVG::DrawTextNormal("FPS: " + fpsStr + " Frame Time: " + frameTimeStr, textPosition, textStyle, 0.0f, 2);

            // Draw semi-transparent black rectangle on the bottom of the screen.
            style.m_color    = Vec4(0, 0, 0, 0.5f);
            style.m_rounding = 0.0f;
            LinaVG::DrawRect(Vec2(0.0f, screenSize.y - screenSize.y * 0.1f), screenSize, style, 0.0f, 1);
        }

        void DemoScreens::ShowDemoScreen1_Shapes()
        {
        }
        void DemoScreens::ShowDemoScreen2_Outlines()
        {
        }
        void DemoScreens::ShowDemoScreen3_Colors()
        {
        }
        void DemoScreens::ShowDemoScreen4_Lines()
        {
        }
        void DemoScreens::ShowDemoScreen5_Texts()
        {
        }
        void DemoScreens::PreEndFrame()
        {
            drawCount     = Config.m_debugCurrentDrawCalls;
            triangleCount = Config.m_debugCurrentTriangleCount;
            vertexCount   = Config.m_debugCurrentVertexCount;
        }
    } // namespace Examples
} // namespace LinaVG