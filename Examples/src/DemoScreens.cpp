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
#define LINAVG_CUSTOM_MALLOC std::malloc
#include "Main.hpp"
#include "LinaVG.hpp"
#include "Utility/Utility.hpp"
#include <string>

namespace LinaVG
{
    namespace Examples
    {

        FontHandle  defaultFont;
        FontHandle  bigFont;
        int         drawCount, triangleCount, vertexCount = 0;
        int         currentScreen        = 0;
        std::string screenTitles[]       = {"SHAPES", "COLORS", "OUTLINE", "LINES", "TEXTS", "Z-ORDER", "CLIPPING"};
        std::string screenDescriptions[] = {
            "Shows out of the box shape types that can be drawn with LinaVG. All shapes also support filled & non-filled versions. "
        };
        void DemoScreens::Initialize()
        {
            defaultFont = LinaVG::LoadFont("Resources/Fonts/SourceSansPro-Regular.ttf", false, 18);
            bigFont     = LinaVG::LoadFont("Resources/Fonts/SourceSansPro-Regular.ttf", false, 42);
        }

        /// <summary>
        /// Draw background rect, stats and info rect.
        /// </summary>
        void DemoScreens::ShowBackground()
        {
            const Vec2   screenSize = LinaVG::Config.m_displaySize;
            StyleOptions style;

            // Draw background gradient.
            style.m_color    = Utility::HexToVec4(0x818D92);
            style.m_isFilled = true;
            LinaVG::DrawRect(Vec2(0.0f, 0.0f), screenSize, style, 0.0f, 0);

            // Draw stats window.
            const float statsWindowX = screenSize.x - screenSize.x * 0.18f;
            const float statsWindowY = screenSize.y * 0.05f;
            style.m_color            = Vec4(0, 0, 0, 0.5f);
            style.m_rounding         = 0.2f;
            style.m_onlyRoundTheseCorners.push_back(0);
            style.m_onlyRoundTheseCorners.push_back(3);
            LinaVG::DrawRect(Vec2(statsWindowX, statsWindowY), Vec2(screenSize.x, screenSize.y * 0.17f), style, 0.0f, 1);
            style.m_onlyRoundTheseCorners.clear();

            LinaVG::Internal::DrawDebugFontAtlas(Vec2(300, 500), defaultFont);

            // Draw stats texts.
            const std::string drawCountStr     = "Draw Calls: " + std::to_string(drawCount);
            const std::string triangleCountStr = "Tris Count: " + std::to_string(triangleCount);
            const std::string vertexCountStr   = "Vertex Count: " + std::to_string(vertexCount);
            const std::string frameTimeStr     = "Frame: " + std::to_string(ExampleApp::Get()->GetFrameTimeRead()) + " ms";
            const std::string fpsStr           = "FPS: " + std::to_string(ExampleApp::Get()->GetFPS()) + " " + frameTimeStr;

            Vec2           textPosition = Vec2(statsWindowX + 10, statsWindowY + 15);
            SDFTextOptions textStyle;
            textStyle.m_textScale = 0.82f;
            textStyle.m_font = defaultFont;
            LinaVG::DrawTextNormal(drawCountStr.c_str(), textPosition, textStyle, 0.0f, 2);
            textPosition.y += 25;
            LinaVG::DrawTextNormal(vertexCountStr.c_str(), textPosition, textStyle, 0.0f, 2);
            textPosition.y += 25;
            LinaVG::DrawTextNormal(triangleCountStr.c_str(), textPosition, textStyle, 0.0f, 2);
            textPosition.y += 25;
            LinaVG::DrawTextNormal(fpsStr.c_str(), textPosition, textStyle, 0.0f, 2);

            // Draw semi-transparent black rectangle on the bottom of the screen.
            style.m_color      = Vec4(0, 0, 0, 0.5f);
            style.m_rounding   = 0.0f;
            const Vec2 rectMin = Vec2(0.0f, screenSize.y - screenSize.y * 0.1f);
            LinaVG::DrawRect(rectMin, screenSize, style, 0.0f, 1);

            textStyle.m_font      = bigFont;
            Vec2 size             = LinaVG::CalculateTextSize(screenTitles[currentScreen], textStyle);
            textStyle.m_color     = Utility::HexToVec4(0xFCAA67);
            textStyle.m_alignment = TextAlignment::Right;
            LinaVG::DrawTextNormal(screenTitles[currentScreen], Vec2(screenSize.x - 20, rectMin.y + 20 + size.y / 2.0f), textStyle, 0, 2);
            
            textStyle.m_textScale = 1.0f;
            textStyle.m_font = defaultFont;
            textStyle.m_color = Vec4(1,1,1,1);
            textStyle.m_alignment = TextAlignment::Left;
            textStyle.m_wrapWidth = screenSize.x * 0.4f;
            textStyle.m_newLineSpacing = 10;
            const Vec2 descSize = LinaVG::CalculateTextSize(screenDescriptions[currentScreen], textStyle);
            LinaVG::DrawTextNormal(screenDescriptions[currentScreen], Vec2(20, rectMin.y + 20), textStyle, 0, 2);
        }

        void DemoScreens::ShowDemoScreen1_Shapes()
        {
            currentScreen = 0;
        }
        void DemoScreens::ShowDemoScreen2_Outlines()
        {
            currentScreen = 1;
        }
        void DemoScreens::ShowDemoScreen3_Colors()
        {
            currentScreen = 2;
        }
        void DemoScreens::ShowDemoScreen4_Lines()
        {
            currentScreen = 3;
        }
        void DemoScreens::ShowDemoScreen5_Texts()
        {
            currentScreen = 4;
        }
        void DemoScreens::ShowDemoScreen6_DrawOrder()
        {
            currentScreen = 5;
        }
        void DemoScreens::ShowDemoScreen7_Clipping()
        {
            currentScreen = 6;
        }

        void DemoScreens::PreEndFrame()
        {
            drawCount     = Config.m_debugCurrentDrawCalls;
            triangleCount = Config.m_debugCurrentTriangleCount;
            vertexCount   = Config.m_debugCurrentVertexCount;
        }
    } // namespace Examples
} // namespace LinaVG