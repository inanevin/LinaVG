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
        void DemoScreens::Initialize()
        {
            m_defaultFont = LinaVG::LoadFont("Resources/Fonts/SourceSansPro-Regular.ttf", false, 18);
            m_titleFont   = LinaVG::LoadFont("Resources/Fonts/SourceSansPro-Regular.ttf", true, 65);
            m_descFont    = LinaVG::LoadFont("Resources/Fonts/SourceSansPro-Regular.ttf", false, 20);
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
            if (m_statsWindowOn)
            {
                const float statsWindowX = screenSize.x - screenSize.x * 0.18f;
                const float statsWindowY = screenSize.y * 0.05f;
                style.m_color            = Vec4(0, 0, 0, 0.5f);
                style.m_rounding         = 0.2f;
                style.m_onlyRoundTheseCorners.push_back(0);
                style.m_onlyRoundTheseCorners.push_back(3);
                LinaVG::DrawRect(Vec2(statsWindowX, statsWindowY), Vec2(screenSize.x, screenSize.y * 0.17f), style, 0.0f, 1);
                style.m_onlyRoundTheseCorners.clear();

                // Draw stats texts.
                const std::string drawCountStr     = "Draw Calls: " + std::to_string(m_drawCount);
                const std::string triangleCountStr = "Tris Count: " + std::to_string(m_triangleCount);
                const std::string vertexCountStr   = "Vertex Count: " + std::to_string(m_vertexCount);
                const std::string frameTimeStr     = "Frame: " + std::to_string(ExampleApp::Get()->GetFrameTimeRead()) + " ms";
                const std::string fpsStr           = "FPS: " + std::to_string(ExampleApp::Get()->GetFPS()) + " " + frameTimeStr;

                Vec2           textPosition = Vec2(statsWindowX + 10, statsWindowY + 15);
                SDFTextOptions textStyle;
                textStyle.m_textScale = 0.82f;
                textStyle.m_font      = m_defaultFont;
                LinaVG::DrawTextNormal(drawCountStr.c_str(), textPosition, textStyle, 0.0f, 2);
                textPosition.y += 25;
                LinaVG::DrawTextNormal(vertexCountStr.c_str(), textPosition, textStyle, 0.0f, 2);
                textPosition.y += 25;
                LinaVG::DrawTextNormal(triangleCountStr.c_str(), textPosition, textStyle, 0.0f, 2);
                textPosition.y += 25;
                LinaVG::DrawTextNormal(fpsStr.c_str(), textPosition, textStyle, 0.0f, 2);
            }

            // Draw semi-transparent black rectangle on the bottom of the screen.
            style.m_color      = Vec4(0, 0, 0, 0.5f);
            style.m_rounding   = 0.0f;
            const Vec2 rectMin = Vec2(0.0f, screenSize.y - screenSize.y * 0.1f);
            LinaVG::DrawRect(rectMin, screenSize, style, 0.0f, 1);

            // Draw a vertical dividers.
            const float  rectHeight = screenSize.y - rectMin.y;
            const float  rectWidth  = screenSize.x - rectMin.x;
            StyleOptions vertDivider;
            vertDivider.m_color = Vec4(1, 1, 1, 1);
            LinaVG::DrawLine(Vec2(rectWidth * 0.225f, rectMin.y), Vec2(rectWidth * 0.225f, screenSize.y), vertDivider, LineCapDirection::None, 0.0f, 2);
            LinaVG::DrawLine(Vec2(rectWidth * 0.725f, rectMin.y), Vec2(rectWidth * 0.725f, screenSize.y), vertDivider, LineCapDirection::None, 0.0f, 2);

            // Draw title text.
            SDFTextOptions sdfStyle;
            sdfStyle.m_font           = m_titleFont;
            const Vec2 size           = LinaVG::CalculateTextSize(m_screenTitles[m_currentScreen], sdfStyle);
            const Vec2 titlePos       = Vec2(rectMin.x + 20, rectMin.y + rectHeight / 2.0f - size.y / 2.0f);
            sdfStyle.m_newLineSpacing = 10.0f;
            sdfStyle.m_color          = Utility::HexToVec4(0xFCAA67);
            sdfStyle.m_sdfThickness   = 0.62f;
            sdfStyle.m_sdfSoftness    = 0.05f;
            LinaVG::DrawTextSDF(m_screenTitles[m_currentScreen], titlePos, sdfStyle, 0, 2);

            // Current screen description.
            TextOptions descText;
            descText.m_font           = m_descFont;
            descText.m_wrapWidth      = rectWidth * 0.45f;
            descText.m_newLineSpacing = 8.0f;
            LinaVG::DrawTextNormal(m_screenDescriptions[m_currentScreen], Vec2(rectWidth * 0.25f, rectMin.y + 20), descText, 0, 2);

            // Draw controls info
            TextOptions controlsText;
            controlsText.m_font      = m_descFont;
            controlsText.m_textScale = 0.8f;
            LinaVG::DrawTextNormal("Press num keys [0-9] to switch between demo screens.", Vec2(rectWidth * 0.725f + 20, rectMin.y + 10), controlsText, 0, 2);
            LinaVG::DrawTextNormal("Press P to toggle performance stats.", Vec2(rectWidth * 0.725f + 20, rectMin.y + 40), controlsText, 0, 2);
            LinaVG::DrawTextNormal("Press F to toggle wireframe rendering.", Vec2(rectWidth * 0.725f + 20, rectMin.y + 70), controlsText, 0, 2);
            LinaVG::DrawTextNormal("Press R to start/stop rotation.", Vec2(rectWidth * 0.725f + 20, rectMin.y + 100), controlsText, 0, 2);
        }

        void DemoScreens::ShowDemoScreen1_Shapes()
        {
            m_currentScreen = 0;
            const Vec2   screenSize = LinaVG::Config.m_displaySize;

            StyleOptions defaultStyle;
            defaultStyle.m_isFilled = true;
            Vec2 startPos = Vec2(screenSize.x *0.05f, screenSize.y * 0.05f);

            // Rect - filled
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // fRect - nonfilled
            startPos.x += 200;
            defaultStyle.m_isFilled = false;
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Rect partially rounded - non filled
            startPos.x += 200;
            defaultStyle.m_isFilled = false;
            defaultStyle.m_rounding = 0.5f;
            defaultStyle.m_onlyRoundTheseCorners.push_back(0);
            defaultStyle.m_onlyRoundTheseCorners.push_back(3);
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);
            defaultStyle.m_onlyRoundTheseCorners.clear();

            // Rect fully rounded - filled
            startPos.x += 200;
            defaultStyle.m_isFilled = true;
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Triangle filled
            startPos.x = screenSize.x * 0.05f;
            startPos.y += 200;
            defaultStyle.m_isFilled = true;
            defaultStyle.m_rounding = 0.0f;
            LinaVG::DrawTriangle(Vec2(startPos.x + 75, startPos.y), Vec2(startPos.x + 150, startPos.y + 150), Vec2(startPos.x, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Triangle non filled
            startPos.x += 200;
            defaultStyle.m_isFilled = false;
            defaultStyle.m_thickness = 5.0f;
            LinaVG::DrawTriangle(Vec2(startPos.x + 75, startPos.y), Vec2(startPos.x + 150, startPos.y + 150), Vec2(startPos.x, startPos.y + 150), defaultStyle, m_rotateAngle, 1);
          
            // Triangle non filled partially rounded
            startPos.x += 200;
            defaultStyle.m_rounding = 0.2f;
            defaultStyle.m_onlyRoundTheseCorners.push_back(0);
            LinaVG::DrawTriangle(Vec2(startPos.x + 75, startPos.y), Vec2(startPos.x + 150, startPos.y + 150), Vec2(startPos.x, startPos.y + 150), defaultStyle, m_rotateAngle, 1);
            defaultStyle.m_onlyRoundTheseCorners.clear();

            // Triangle filled & fully rounded
            startPos.x += 200;
            defaultStyle.m_rounding = 0.4f;
            defaultStyle.m_isFilled = true;
            LinaVG::DrawTriangle(Vec2(startPos.x + 75, startPos.y), Vec2(startPos.x + 150, startPos.y + 150), Vec2(startPos.x, startPos.y + 150), defaultStyle, m_rotateAngle, 1);
        
            // Full circle filled
            startPos.x = screenSize.x * 0.05f;
            startPos.y += 200;
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 0.0f, 360.0f, 2);
        
            // Half circle non filled
            startPos.x += 200;
            defaultStyle.m_isFilled = false;
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 0.0f, 360.0f, 2);

            // Arc filled
            startPos.x += 200;
            defaultStyle.m_isFilled = true;
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 100.0f, 360.0f, 2);

            // Arc
            startPos.x += 200;
            defaultStyle.m_isFilled = true;
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 300.0f, 330.0f, 2);

            // Ngon - 6
            startPos.x = screenSize.x * 0.05f;
            startPos.y += 200;
            LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 6, defaultStyle, m_rotateAngle, 2);

            // Ngon - 8
            startPos.x += 200;
            defaultStyle.m_isFilled = false;
            LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 8, defaultStyle, m_rotateAngle, 2);

            // Convex
            startPos.x += 200;
            defaultStyle.m_isFilled = true;
            std::vector<Vec2> points;
            points.push_back(startPos);
            points.push_back(Vec2(startPos.x + 150, startPos.y));
            points.push_back(Vec2(startPos.x - 50, startPos.y + 150));
            points.push_back(Vec2(startPos.x + 100, startPos.y + 150));
            LinaVG::DrawConvex(&points[0], 4, defaultStyle, m_rotateAngle, 2);

            // Convex
            startPos.x += 200;
            defaultStyle.m_isFilled = false;
            points.clear();
            points.push_back(startPos);
            points.push_back(Vec2(startPos.x + 150, startPos.y));
            points.push_back(Vec2(startPos.x + 100, startPos.y + 150));
            points.push_back(Vec2(startPos.x - 50, startPos.y + 150));
            LinaVG::DrawConvex(&points[0], 4, defaultStyle, m_rotateAngle, 2);
            points.clear();
        }
        void DemoScreens::ShowDemoScreen2_Outlines()
        {
            m_currentScreen = 1;
        }
        void DemoScreens::ShowDemoScreen3_Colors()
        {
            m_currentScreen = 2;
        }
        void DemoScreens::ShowDemoScreen4_Lines()
        {
            m_currentScreen = 3;
        }
        void DemoScreens::ShowDemoScreen5_Texts()
        {
            m_currentScreen = 4;
        }
        void DemoScreens::ShowDemoScreen6_DrawOrder()
        {
            m_currentScreen = 5;
        }
        void DemoScreens::ShowDemoScreen7_Clipping()
        {
            m_currentScreen = 6;
        }

        void DemoScreens::PreEndFrame()
        {
            if(m_rotate)
                m_rotateAngle += ExampleApp::Get()->GetFrameTime() * 20;

            m_drawCount     = Config.m_debugCurrentDrawCalls;
            m_triangleCount = Config.m_debugCurrentTriangleCount;
            m_vertexCount   = Config.m_debugCurrentVertexCount;
        }
    } // namespace Examples
} // namespace LinaVG