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
#include "Core/Math.hpp"
#include "Utility/Utility.hpp"
#include <string>

namespace LinaVG
{
    namespace Examples
    {
        void DemoScreens::Initialize()
        {
            m_defaultFont     = LinaVG::LoadFont("Resources/Fonts/SourceSansPro-Regular.ttf", false, 18);
            m_titleFont       = LinaVG::LoadFont("Resources/Fonts/SourceSansPro-Regular.ttf", true, 65);
            m_descFont        = LinaVG::LoadFont("Resources/Fonts/SourceSansPro-Regular.ttf", false, 20);
            m_textDemoFont    = LinaVG::LoadFont("Resources/Fonts/SourceSansPro-Regular.ttf", false, 30);
            m_textDemoSDFFont = LinaVG::LoadFont("Resources/Fonts/SourceSansPro-Regular.ttf", true, 40);

            m_screenDescriptions.push_back("LinaVG supports variety of convex shapes, which can be partially or fully rounded, and all shapes also support filled & non-filled versions.");
            m_screenDescriptions.push_back("You can use flat colors, alphas, vertical / horizontal gradients and rounded gradients. Also, textures w/ custom UV offset & tiling are supported.");
            m_screenDescriptions.push_back("LinaVG supports inner as well as outer outlines. Outlines also support all previous coloring and texturing options, as well as both filled & non-filled objects.");
            m_screenDescriptions.push_back("LinaVG supports single lines, multi-lines as well as bezier curves. Lines can have left/right or both caps, multi-lines can have 4 different types of joints. All lines also support outlines, coloring & texturing.");
            m_screenDescriptions.push_back("Texts support drop shadows, flat colors & vertical/horizontal gradients. SDF texts also support outlines, individual thickness as well as softness factors. LinaVG also provides alignment, wrapping & spacing options.");
            m_screenDescriptions.push_back("You can suply varying draw order to DrawXXX commands in order to support z-ordering.");
            m_screenDescriptions.push_back("You can use global clipping variables to create clipping rectangles for any shape you are drawing. Press C to toggle clipping.");
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

                Vec2           textPosition = Vec2(statsWindowX + 10, statsWindowY + 25);
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
            const Vec2 rectMin = Vec2(0.0f, screenSize.y - screenSize.y * 0.12f);
            LinaVG::DrawRect(rectMin, screenSize, style, 0.0f, 1);

            //  // Draw a vertical dividers.
            const float  rectHeight = screenSize.y - rectMin.y;
            const float  rectWidth  = screenSize.x - rectMin.x;
            StyleOptions vertDivider;
            vertDivider.m_color = Vec4(1, 1, 1, 1);
            LinaVG::DrawLine(Vec2(rectWidth * 0.225f, rectMin.y), Vec2(rectWidth * 0.225f, screenSize.y), vertDivider, LineCapDirection::None, 0.0f, 2);
            LinaVG::DrawLine(Vec2(rectWidth * 0.725f, rectMin.y), Vec2(rectWidth * 0.725f, screenSize.y), vertDivider, LineCapDirection::None, 0.0f, 2);

            // Draw title text.
            SDFTextOptions sdfStyle;
            sdfStyle.m_font           = m_titleFont;
            const Vec2 size           = LinaVG::CalculateTextSize(m_screenTitles[ExampleApp::Get()->GetCurrentScreen() - 1], sdfStyle);
            const Vec2 titlePos       = Vec2(rectMin.x + 20, rectMin.y + rectHeight / 2.0f + size.y / 2.0f);
            sdfStyle.m_newLineSpacing = 10.0f;
            sdfStyle.m_color          = Utility::HexToVec4(0xFCAA67);
            sdfStyle.m_sdfThickness   = 0.62f;
            sdfStyle.m_sdfSoftness    = 0.05f;
            LinaVG::DrawTextSDF(m_screenTitles[ExampleApp::Get()->GetCurrentScreen() - 1], titlePos, sdfStyle, 0, 2);

            // Current screen description.
            TextOptions descText;
            descText.m_font      = m_descFont;
            descText.m_wrapWidth = rectWidth * 0.45f;
            LinaVG::DrawTextNormal(m_screenDescriptions[ExampleApp::Get()->GetCurrentScreen() - 1], Vec2(rectWidth * 0.25f, rectMin.y + 30), descText, 0, 2);

            // Draw controls info
            TextOptions controlsText;
            controlsText.m_font      = m_descFont;
            controlsText.m_textScale = 0.8f;
            LinaVG::DrawTextNormal("Press num keys [0-9] to switch between demo screens.", Vec2(rectWidth * 0.725f + 20, rectMin.y + 20), controlsText, 0, 2);
            LinaVG::DrawTextNormal("Press P to toggle performance stats.", Vec2(rectWidth * 0.725f + 20, rectMin.y + 40), controlsText, 0, 2);
            LinaVG::DrawTextNormal("Press F to toggle wireframe rendering.", Vec2(rectWidth * 0.725f + 20, rectMin.y + 60), controlsText, 0, 2);
            LinaVG::DrawTextNormal("Press R to start/stop rotation.", Vec2(rectWidth * 0.725f + 20, rectMin.y + 80), controlsText, 0, 2);
            LinaVG::DrawTextNormal("Press E to reset rotation.", Vec2(rectWidth * 0.725f + 20, rectMin.y + 100), controlsText, 0, 2);
        }

        void DemoScreens::ShowDemoScreen1_Shapes()
        {
            const Vec2 screenSize = LinaVG::Config.m_displaySize;

            StyleOptions defaultStyle;
            defaultStyle.m_isFilled = true;
            Vec2 startPos           = Vec2(screenSize.x * 0.05f, screenSize.y * 0.05f);

            //*************************** ROW 1 ***************************/

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

            //*************************** ROW 2 ***************************/

            // Triangle filled
            startPos.x = screenSize.x * 0.05f;
            startPos.y += 200;
            defaultStyle.m_isFilled = true;
            defaultStyle.m_rounding = 0.0f;
            LinaVG::DrawTriangle(Vec2(startPos.x + 75, startPos.y), Vec2(startPos.x + 150, startPos.y + 150), Vec2(startPos.x, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Triangle non filled
            startPos.x += 200;
            defaultStyle.m_isFilled  = false;
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

            //*************************** ROW 3 ***************************/

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

            //*************************** ROW 4 ***************************/

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
        void DemoScreens::ShowDemoScreen2_Colors()
        {
            const Vec2 screenSize = LinaVG::Config.m_displaySize;
            Vec2       startPos   = Vec2(screenSize.x * 0.05f, screenSize.y * 0.05f);

            StyleOptions defaultStyle;
            defaultStyle.m_isFilled = true;

            //*************************** ROW 1 ***************************/

            // Single color
            defaultStyle.m_color = LinaVG::Utility::HexToVec4(0x212738);
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Single color
            startPos.x += 200;
            defaultStyle.m_color    = LinaVG::Utility::HexToVec4(0x06A77D);
            defaultStyle.m_rounding = 0.5f;
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Single color
            startPos.x += 200;
            defaultStyle.m_color    = LinaVG::Utility::HexToVec4(0xF1A208);
            defaultStyle.m_rounding = 0.5f;
            LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 7, defaultStyle, m_rotateAngle, 1);

            // Single color
            startPos.x += 200;
            defaultStyle.m_color = LinaVG::Utility::HexToVec4(0xFEFAE0);
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 0.0f, 360.0f, 1);

            //*************************** ROW 2 ***************************/

            // Horizontal gradient
            startPos.x = screenSize.x * 0.05f;
            startPos.y += 200;
            defaultStyle.m_rounding      = 0.0f;
            defaultStyle.m_color.m_start = Vec4(1.0f, 0.2f, 0.2f, 1.0f);
            defaultStyle.m_color.m_end   = Vec4(0.2f, 0.2f, 1.0f, 1.0f);
            LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 8, defaultStyle, m_rotateAngle, 1);

            // Horizontal gradient.
            startPos.x += 200;
            defaultStyle.m_rounding      = 0.0f;
            defaultStyle.m_color.m_start = Vec4(0.2f, 0.2f, 1.0f, 1.0f);
            defaultStyle.m_color.m_end   = Vec4(1.0f, 0.2f, 0.2f, 1.0f);
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Vertical gradient
            startPos.x += 200;
            defaultStyle.m_color.m_gradientType = GradientType::Vertical;
            defaultStyle.m_color.m_start        = Vec4(1.0f, 1.0f, 0.0f, 1.0f);
            defaultStyle.m_color.m_end          = Vec4(0.0f, 1.0f, 1.0f, 1.0f);
            LinaVG::DrawTriangle(Vec2(startPos.x + 75, startPos.y), Vec2(startPos.x + 150, startPos.y + 150), Vec2(startPos.x, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Vertical gradient.
            startPos.x += 200;
            defaultStyle.m_color.m_gradientType = GradientType::Vertical;
            defaultStyle.m_color.m_start        = Vec4(1.0f, 1.0f, 0.0f, 1.0f);
            defaultStyle.m_color.m_end          = Vec4(0.0f, 1.0f, 1.0f, 1.0f);
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 0.0f, 360.0f, 1);

            //*************************** ROW 3 ***************************/

            // Radial
            startPos.x = screenSize.x * 0.05f;
            startPos.y += 200;
            defaultStyle.m_color.m_start        = Vec4(0.2f, 0.2f, 0.9f, 1.0f);
            defaultStyle.m_color.m_end          = Vec4(0.9f, 0.2f, 0.9f, 1.0f);
            defaultStyle.m_color.m_gradientType = GradientType::Radial;
            defaultStyle.m_color.m_radialSize   = 1.4f;
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 0.0f, 360.0f, 1);

            // Radial
            startPos.x += 200;
            defaultStyle.m_color.m_start        = Vec4(0.2f, 0.2f, 0.9f, 1.0f);
            defaultStyle.m_color.m_end          = Vec4(0.9f, 0.2f, 0.9f, 1.0f);
            defaultStyle.m_color.m_gradientType = GradientType::Radial;
            defaultStyle.m_color.m_radialSize   = 1.4f;
            LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 7, defaultStyle, m_rotateAngle, 1);

            // Radial Corner
            startPos.x += 200;
            defaultStyle.m_color.m_start        = Vec4(0.2f, 0.2f, 1.0f, 1.0f);
            defaultStyle.m_color.m_end          = Vec4(1.0f, 0.2f, 0.2f, 1.0f);
            defaultStyle.m_color.m_gradientType = GradientType::RadialCorner;
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Radial corner
            startPos.x += 200;
            defaultStyle.m_color.m_start        = Vec4(0.2f, 0.2f, 1.0f, 1.0f);
            defaultStyle.m_color.m_end          = Vec4(1.0f, 0.2f, 0.2f, 1.0f);
            defaultStyle.m_color.m_gradientType = GradientType::RadialCorner;
            LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 7, defaultStyle, m_rotateAngle, 1);

            //*************************** ROW 4 ***************************/

            // Textured rect
            startPos.x = screenSize.x * 0.05f;
            startPos.y += 200;
            defaultStyle.m_textureHandle = ExampleApp::Get()->GetCheckeredTexture();
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Tiling
            startPos.x += 200;
            defaultStyle.m_textureUVTiling = Vec2(2, 2);
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 0.0f, 360.0f, 1);

            // Lina Logo
            startPos.x += 200;
            defaultStyle.m_textureUVTiling = Vec2(1, 1);
            defaultStyle.m_textureHandle   = ExampleApp::Get()->GetLinaLogoTexture();
            LinaVG::DrawImage(ExampleApp::Get()->GetLinaLogoTexture(), Vec2(startPos.x + 75, startPos.y + 75), Vec2(150, 150), m_rotateAngle, 1);

            // Lina Logo
            startPos.x += 200;
            LinaVG::DrawImage(ExampleApp::Get()->GetLinaLogoTexture(), Vec2(startPos.x + 75, startPos.y + 75), Vec2(150, 150), m_rotateAngle, 1, Vec2(2, 2));
        }
        void DemoScreens::ShowDemoScreen3_Outlines()
        {
            const Vec2 screenSize = LinaVG::Config.m_displaySize;
            Vec2       startPos   = Vec2(screenSize.x * 0.05f, screenSize.y * 0.05f);

            StyleOptions defaultStyle;
            defaultStyle.m_isFilled = true;

            //*************************** ROW 1 ***************************/

            // Filled
            defaultStyle.m_thickness                  = 5.0f;
            defaultStyle.m_outlineOptions.m_color     = Vec4(0, 0, 0, 1);
            defaultStyle.m_outlineOptions.m_thickness = 3.0f;
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Non filled outer
            startPos.x += 200;
            defaultStyle.m_outlineOptions.m_color.m_start = Vec4(1, 0, 0, 1);
            defaultStyle.m_outlineOptions.m_color.m_end   = Vec4(0, 0, 1, 1);
            defaultStyle.m_isFilled                       = false;
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Non filled inner
            startPos.x += 200;
            defaultStyle.m_outlineOptions.m_color         = Vec4(0, 0.5f, 0, 1);
            defaultStyle.m_outlineOptions.m_drawDirection = OutlineDrawDirection::Inwards;
            defaultStyle.m_isFilled                       = false;
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Both
            startPos.x += 200;
            defaultStyle.m_outlineOptions.m_color.m_start = Vec4(1, 0, 0, 1);
            defaultStyle.m_outlineOptions.m_color.m_end   = Vec4(0, 0, 1, 1);
            defaultStyle.m_outlineOptions.m_drawDirection = OutlineDrawDirection::Both;
            defaultStyle.m_isFilled                       = false;
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            //*************************** ROW 2 ***************************/

            // Filled
            startPos.x = screenSize.x * 0.05f;
            startPos.y += 200;
            defaultStyle.m_outlineOptions.m_drawDirection = OutlineDrawDirection::Outwards;
            defaultStyle.m_rounding                       = 0.4f;
            defaultStyle.m_isFilled                       = true;
            defaultStyle.m_thickness                      = 8.0f;
            defaultStyle.m_outlineOptions.m_color         = Vec4(0, 0, 0, 1);
            defaultStyle.m_outlineOptions.m_thickness     = 5.0f;
            defaultStyle.m_outlineOptions.m_textureHandle = ExampleApp::Get()->GetCheckeredTexture();
            defaultStyle.m_color                          = Vec4(0.7f, 0.1f, 0.1f, 1.0f);
            LinaVG::DrawTriangle(Vec2(startPos.x + 75, startPos.y), Vec2(startPos.x + 150, startPos.y + 150), Vec2(startPos.x, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Non filled outer
            startPos.x += 200;
            defaultStyle.m_outlineOptions.m_color.m_start = Vec4(1, 0, 0, 1);
            defaultStyle.m_outlineOptions.m_color.m_end   = Vec4(0, 0, 1, 1);
            defaultStyle.m_isFilled                       = false;
            LinaVG::DrawTriangle(Vec2(startPos.x + 75, startPos.y), Vec2(startPos.x + 150, startPos.y + 150), Vec2(startPos.x, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Non filled inner
            startPos.x += 200;
            defaultStyle.m_outlineOptions.m_color         = Vec4(0, 0.5f, 0, 1);
            defaultStyle.m_outlineOptions.m_drawDirection = OutlineDrawDirection::Inwards;
            defaultStyle.m_isFilled                       = false;
            LinaVG::DrawTriangle(Vec2(startPos.x + 75, startPos.y), Vec2(startPos.x + 150, startPos.y + 150), Vec2(startPos.x, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Both
            startPos.x += 200;
            defaultStyle.m_outlineOptions.m_color.m_start = Vec4(1, 0, 0, 1);
            defaultStyle.m_outlineOptions.m_color.m_end   = Vec4(0, 0, 1, 1);
            defaultStyle.m_outlineOptions.m_drawDirection = OutlineDrawDirection::Both;
            defaultStyle.m_isFilled                       = false;
            LinaVG::DrawTriangle(Vec2(startPos.x + 75, startPos.y), Vec2(startPos.x + 150, startPos.y + 150), Vec2(startPos.x, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            //*************************** ROW 3 ***************************/

            // Filled
            startPos.x = screenSize.x * 0.05f;
            startPos.y += 200;
            defaultStyle.m_outlineOptions.m_drawDirection = OutlineDrawDirection::Outwards;
            defaultStyle.m_rounding                       = 0.4f;
            defaultStyle.m_isFilled                       = true;
            defaultStyle.m_thickness                      = 8.0f;
            defaultStyle.m_outlineOptions.m_color         = Vec4(0, 0, 0, 1);
            defaultStyle.m_outlineOptions.m_thickness     = 2.0f;
            defaultStyle.m_outlineOptions.m_textureHandle = 0;
            defaultStyle.m_color                          = Vec4(1, 1, 1, 1);
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 0.0f, 360.0f, 1);

            // outer
            startPos.x += 200;
            defaultStyle.m_outlineOptions.m_color.m_start = Vec4(1, 0, 0, 1);
            defaultStyle.m_outlineOptions.m_color.m_end   = Vec4(0, 0, 1, 1);
            defaultStyle.m_color.m_gradientType           = GradientType::Radial;
            defaultStyle.m_color.m_start                  = Vec4(0.5f, 1.0f, 0.5f, 1.0f);
            defaultStyle.m_isFilled                       = true;
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 0.45f, 245.0f, 1);

            // Non filled inner
            startPos.x += 200;
            defaultStyle.m_outlineOptions.m_color         = Vec4(0, 0.5f, 0, 1);
            defaultStyle.m_outlineOptions.m_drawDirection = OutlineDrawDirection::Inwards;
            defaultStyle.m_isFilled                       = false;
            defaultStyle.m_color.m_gradientType           = GradientType::Vertical;
            defaultStyle.m_color                          = Vec4(1, 1, 1, 1);
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 90.0f, 360.0f, 1);

            // Both
            startPos.x += 200;
            defaultStyle.m_outlineOptions.m_color.m_start = Vec4(1, 0, 0, 1);
            defaultStyle.m_outlineOptions.m_color.m_end   = Vec4(0, 0, 1, 1);
            defaultStyle.m_outlineOptions.m_drawDirection = OutlineDrawDirection::Both;
            defaultStyle.m_isFilled                       = false;
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 180.0f, 360.0f, 1);

            //*************************** ROW 4 ***************************/

            // Filled
            startPos.x = screenSize.x * 0.05f;
            startPos.y += 200;
            defaultStyle.m_outlineOptions.m_thickness     = 12.0f;
            defaultStyle.m_outlineOptions.m_drawDirection = OutlineDrawDirection::Outwards;
            defaultStyle.m_rounding                       = 0.4f;
            defaultStyle.m_isFilled                       = true;
            defaultStyle.m_thickness                      = 8.0f;
            defaultStyle.m_outlineOptions.m_color         = Vec4(0, 0, 0, 1);
            LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 7, defaultStyle, m_rotateAngle, 1);

            // outer
            startPos.x += 200;
            defaultStyle.m_outlineOptions.m_color.m_start = Vec4(1, 0, 0, 1);
            defaultStyle.m_outlineOptions.m_color.m_end   = Vec4(0, 0, 1, 1);
            defaultStyle.m_color.m_gradientType           = GradientType::Radial;
            defaultStyle.m_isFilled                       = false;
            LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 7, defaultStyle, m_rotateAngle, 1);

            // Non filled inner
            startPos.x += 200;
            defaultStyle.m_outlineOptions.m_color         = Vec4(0, 0.5f, 0, 1);
            defaultStyle.m_outlineOptions.m_drawDirection = OutlineDrawDirection::Inwards;
            defaultStyle.m_isFilled                       = false;
            defaultStyle.m_color.m_gradientType           = GradientType::Vertical;
            defaultStyle.m_color                          = Vec4(1, 1, 1, 1);
            LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 7, defaultStyle, m_rotateAngle, 1);

            // Both
            startPos.x += 200;
            defaultStyle.m_outlineOptions.m_color.m_start = Vec4(1, 0, 0, 1);
            defaultStyle.m_outlineOptions.m_color.m_end   = Vec4(0, 0, 1, 1);
            defaultStyle.m_outlineOptions.m_drawDirection = OutlineDrawDirection::Both;
            defaultStyle.m_isFilled                       = false;
            LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 7, defaultStyle, m_rotateAngle, 1);
        }

        void DemoScreens::ShowDemoScreen4_Lines()
        {
            const Vec2       screenSize = LinaVG::Config.m_displaySize;
            Vec2             startPos   = Vec2(screenSize.x * 0.05f, screenSize.y * 0.05f);
            StyleOptions     defaultStyle;
            LineCapDirection lineCap   = LineCapDirection::None;
            LineJointType    jointType = LineJointType::Miter;

            defaultStyle.m_thickness = 15.0f;
            defaultStyle.m_color     = Vec4(1, 1, 1, 1);
            LinaVG::DrawLine(startPos, Vec2(startPos.x + 700, startPos.y), defaultStyle, lineCap, m_rotateAngle, 1);

            lineCap = LineCapDirection::Left;
            startPos.y += 30;
            defaultStyle.m_color.m_start = Vec4(1.0f, 0.1f, 0.1f, 1.0f);
            defaultStyle.m_color.m_end   = Vec4(0.0f, 0.1f, 1.0f, 1.0f);
            LinaVG::DrawLine(startPos, Vec2(startPos.x + 700, startPos.y), defaultStyle, lineCap, m_rotateAngle, 1);

            lineCap = LineCapDirection::Right;
            startPos.y += 30;
            defaultStyle.m_color.m_gradientType = GradientType::Vertical;
            LinaVG::DrawLine(startPos, Vec2(startPos.x + 700, startPos.y), defaultStyle, lineCap, m_rotateAngle, 1);

            lineCap = LineCapDirection::Both;
            startPos.y += 30;
            defaultStyle.m_color.m_gradientType = GradientType::Radial;
            LinaVG::DrawLine(startPos, Vec2(startPos.x + 700, startPos.y), defaultStyle, lineCap, m_rotateAngle, 1);

            jointType = LineJointType::Miter;
            startPos.y += 120;
            defaultStyle.m_color.m_start              = Vec4(0.5f, 1.0f, 1.0f, 1.0f);
            defaultStyle.m_color.m_end                = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
            defaultStyle.m_color.m_gradientType       = GradientType::Horizontal;
            defaultStyle.m_outlineOptions.m_thickness = 2.0f;
            defaultStyle.m_outlineOptions.m_color     = Vec4(0, 0, 0, 1);
            LinaVG::DrawBezier(startPos, Vec2(startPos.x + 200, startPos.y + 200), Vec2(startPos.x + 500, startPos.y - 200), Vec2(startPos.x + 700, startPos.y), defaultStyle, lineCap, jointType, 1, false);

            jointType = LineJointType::Miter;
            startPos.y += 120;
            defaultStyle.m_outlineOptions.m_thickness = 0.0f;
            defaultStyle.m_textureHandle              = ExampleApp::Get()->GetCheckeredTexture();
            LinaVG::DrawBezier(startPos, Vec2(startPos.x + 200, startPos.y + 200), Vec2(startPos.x + 500, startPos.y - 200), Vec2(startPos.x + 700, startPos.y), defaultStyle, lineCap, jointType, 1, true, 100);

            startPos.y += 120;
            lineCap                                       = LineCapDirection::None;
            defaultStyle.m_outlineOptions.m_textureHandle = ExampleApp::Get()->GetCheckeredTexture();
            defaultStyle.m_textureHandle                  = 0;
            defaultStyle.m_outlineOptions.m_thickness     = 7.0f;
            std::vector<Vec2> points;
            points.push_back(startPos);
            points.push_back(Vec2(startPos.x + 200, startPos.y));
            points.push_back(Vec2(startPos.x + 200, startPos.y + 300));
            points.push_back(Vec2(startPos.x + 600, startPos.y + 300));
            points.push_back(Vec2(startPos.x + 700, startPos.y));
            LinaVG::DrawLines(&points[0], points.size(), defaultStyle, lineCap, jointType, 1, true);
        }

        void DemoScreens::ShowDemoScreen5_Texts()
        {
            const Vec2  screenSize = LinaVG::Config.m_displaySize;
            Vec2        startPos   = Vec2(screenSize.x * 0.05f, screenSize.y * 0.05f);
            TextOptions textOpts;
            textOpts.m_font = m_textDemoFont;
            LinaVG::DrawTextNormal("This is a normal text.", startPos, textOpts, m_rotateAngle, 1);

            startPos.x += 300;
            textOpts.m_dropShadowOffset = Vec2(2, 2);
            LinaVG::DrawTextNormal("Drop shadow.", startPos, textOpts, m_rotateAngle, 1);

            startPos.x += 300;
            textOpts.m_color.m_start = Vec4(1, 0, 0, 1);
            textOpts.m_color.m_start = Vec4(0, 0, 1, 1);
            LinaVG::DrawTextNormal("Gradient color.", startPos, textOpts, m_rotateAngle, 1);

            startPos.x = screenSize.x * 0.05f;
            startPos.y += 50;
            textOpts.m_wrapWidth       = 100;
            textOpts.m_dropShadowColor = Vec4(1, 0, 0, 1);
            textOpts.m_color           = Vec4(1, 1, 1, 1);
            LinaVG::DrawTextNormal("This is a wrapped text with a colored drop shadow.", startPos, textOpts, m_rotateAngle, 1);

            startPos.x += 300;
            textOpts.m_wrapWidth            = 100;
            textOpts.m_alignment            = TextAlignment::Center;
            textOpts.m_dropShadowOffset     = Vec2(0.0f, 0.0f);
            textOpts.m_color.m_start        = Vec4(0.6f, 0.6f, 0.6f, 1);
            textOpts.m_color.m_end          = Vec4(1, 1, 1, 1);
            textOpts.m_color.m_gradientType = GradientType::Vertical;
            const Vec2 size                 = LinaVG::CalculateTextSize("Center alignment and vertical gradient.", textOpts);
            startPos.x += size.x / 2.0f;
            LinaVG::DrawTextNormal("Center alignment and vertical gradient.", startPos, textOpts, m_rotateAngle, 1);

            startPos.x += 300;
            textOpts.m_color     = Vec4(0.8f, 0.1f, 0.1f, 1.0f);
            textOpts.m_alignment = TextAlignment::Right;
            const Vec2 size2     = LinaVG::CalculateTextSize("Same, but it's right alignment", textOpts);
            startPos.x += size.x;
            LinaVG::DrawTextNormal("Same, but it's right alignment", startPos, textOpts, m_rotateAngle, 1);

            startPos.x = screenSize.x * 0.05f;
            startPos.y += 330;
            textOpts.m_spacing   = 10;
            textOpts.m_wrapWidth = 0.0f;
            textOpts.m_alignment = TextAlignment::Left;
            textOpts.m_color     = Vec4(1, 1, 1, 1);
            LinaVG::DrawTextNormal("And this is a normal text with higher spacing.", startPos, textOpts, m_rotateAngle, 1);

            startPos.y += 90;
            startPos.x                       = screenSize.x * 0.05f;
            const float    beforeSDFStartPos = startPos.y;
            SDFTextOptions sdfOpts;
            sdfOpts.m_font         = m_textDemoSDFFont;
            sdfOpts.m_sdfThickness = 0.55f;
            LinaVG::DrawTextSDF("An SDF text.", startPos, sdfOpts, m_rotateAngle, 1);

            startPos.y += 50;
            sdfOpts.m_sdfThickness  = 0.6f;
            sdfOpts.m_color.m_start = Vec4(1, 0, 0, 1);
            sdfOpts.m_color.m_end   = Vec4(0, 0, 1, 1);
            LinaVG::DrawTextSDF("Thicker SDF text", startPos, sdfOpts, m_rotateAngle, 1);

            startPos.y += 50;
            sdfOpts.m_sdfThickness = 0.7f;
            sdfOpts.m_sdfSoftness  = 0.2f;
            sdfOpts.m_color        = Vec4(0.1f, 0.8f, 0.1f, 1.0f);
            LinaVG::DrawTextSDF("Smoother text", startPos, sdfOpts, m_rotateAngle, 1);

            startPos.y += 50;
            sdfOpts.m_color               = Vec4(1, 1, 1, 1);
            sdfOpts.m_sdfThickness        = 0.6f;
            sdfOpts.m_sdfSoftness         = 0.05f;
            sdfOpts.m_sdfOutlineThickness = 0.1f;
            sdfOpts.m_sdfOutlineColor     = Vec4(0, 0, 0, 1);
            LinaVG::DrawTextSDF("Outlined SDF text", startPos, sdfOpts, m_rotateAngle, 1);

            startPos.y += 50;
            sdfOpts.m_sdfThickness        = 0.8f;
            sdfOpts.m_sdfSoftness         = 0.05f;
            sdfOpts.m_sdfOutlineThickness = 0.3f;
            sdfOpts.m_sdfOutlineColor     = Vec4(0, 0, 0, 1);
            LinaVG::DrawTextSDF("Thicker outline.", startPos, sdfOpts, m_rotateAngle, 1);

            startPos.y += 50;
            sdfOpts.m_sdfDropShadowThickness = 0.6f;
            sdfOpts.m_dropShadowOffset       = Vec2(5, 5);
            sdfOpts.m_sdfOutlineThickness    = 0.0f;
            sdfOpts.m_sdfThickness           = 0.6f;
            LinaVG::DrawTextSDF("Drop shadow.", startPos, sdfOpts, m_rotateAngle, 1);

            startPos.y = beforeSDFStartPos;
            startPos.x += 550;
            sdfOpts.m_sdfDropShadowThickness = 0.0f;
            sdfOpts.m_sdfOutlineThickness    = 0.0f;
            sdfOpts.m_sdfThickness           = 0.6f;
            sdfOpts.m_wrapWidth              = 150;
            sdfOpts.m_alignment              = TextAlignment::Right;
            LinaVG::DrawTextSDF("This is an SDF, wrapped and right aligned text.", startPos, sdfOpts, m_rotateAngle, 1);

            startPos.y = beforeSDFStartPos;
            startPos.x += 300;
            sdfOpts.m_wrapWidth      = 150;
            sdfOpts.m_alignment      = TextAlignment::Right;
            sdfOpts.m_newLineSpacing = 15;
            LinaVG::DrawTextSDF("Same, but a higher new line spacing.", startPos, sdfOpts, m_rotateAngle, 1);
        }
        void DemoScreens::ShowDemoScreen6_DrawOrder()
        {
            const Vec2   screenSize = LinaVG::Config.m_displaySize;
            Vec2         startPos   = Vec2(screenSize.x * 0.05f, screenSize.y * 0.05f);
            StyleOptions opts;
            opts.m_color                      = Vec4(0, 0, 0, 1);
            opts.m_isFilled                   = true;
            opts.m_outlineOptions.m_thickness = 1.0f;
            opts.m_outlineOptions.m_color     = Vec4(0, 0, 0, 1);

            TextOptions textOpts;
            textOpts.m_textScale = 1.5f;
            textOpts.m_font      = m_defaultFont;

            const Vec4 minCol = Vec4(0.7f, 0.1f, 0.1f, 1.0f);
            const Vec4 maxCol = Vec4(0.1f, 0.1f, 0.8f, 1.0f);
            Vec4       col    = minCol;
            for (int i = 1; i < 50; i += 2)
            {
                col          = LinaVG::Math::Lerp(minCol, maxCol, static_cast<float>(i) / 50.0f);
                opts.m_color = col;
                LinaVG::DrawRect(startPos, Vec2(startPos.x + 120, startPos.y + 120), opts, m_rotateAngle, i);

                std::string orderStr = std::to_string(i / 2);
                LinaVG::DrawTextNormal(orderStr, Vec2(startPos.x + 5, startPos.y + 25), textOpts, m_rotateAngle, i + 1);

                startPos.x += 20;
                startPos.y += 20;
            }
        }
        void DemoScreens::ShowDemoScreen7_Clipping()
        {
            const Vec2 screenSize = LinaVG::Config.m_displaySize;
            Vec2       startPos   = Vec2(screenSize.x * 0.5f, screenSize.y * 0.5f);
            const Vec2 size       = Vec2(500, 500);

            StyleOptions opts;
            opts.m_isFilled = true;

            const Vec2 min = Vec2(startPos.x - size.x / 2.0f, startPos.y - size.y / 2.0f);
            const Vec2 max = Vec2(startPos.x + size.x / 2.0f, startPos.y + size.y / 2.0f);

            if (m_clippingEnabled)
            {
                Config.m_clipPosX  = min.x;
                Config.m_clipPosY  = min.y;
                Config.m_clipSizeX = size.x;
                Config.m_clipSizeY = size.y;
            }

            // Main rect.
            opts.m_color = Vec4(0, 0, 0, 1);
            LinaVG::DrawRect(min, max, opts, 0.0f, 1);

            // Clipped rect.
            opts.m_color = Vec4(0.8f, 0.1f, 0.2f, 1.0f);
            LinaVG::DrawRect(Vec2(min.x - 100, min.y - 100), Vec2(min.x + 100, min.y + 100), opts, 0.0f, 2);

            // Clipped circle.
            LinaVG::DrawCircle(max, 75, opts, 36, 0.0f, 0.0f, 360.0f, 2);

            TextOptions textOpts;
            textOpts.m_font = m_defaultFont;
            LinaVG::DrawTextNormal("This text is clipped by the black rectangle.", Vec2(min.x - 50, min.y + 250), textOpts, 0.0f, 2);

            Config.m_clipPosX  = 0.0f;
            Config.m_clipPosY  = 0.0f;
            Config.m_clipSizeX = 0.0f;
            Config.m_clipSizeY = 0.0f;
        }

        void DemoScreens::PreEndFrame()
        {
            if (m_rotate)
                m_rotateAngle += ExampleApp::Get()->GetFrameTime() * 20;

            m_drawCount     = Config.m_debugCurrentDrawCalls;
            m_triangleCount = Config.m_debugCurrentTriangleCount;
            m_vertexCount   = Config.m_debugCurrentVertexCount;
        }
    } // namespace Examples
} // namespace LinaVG