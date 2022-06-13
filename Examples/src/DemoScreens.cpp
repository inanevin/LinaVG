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
#include <iostream>
#include <ctime>
#include <cstdlib>

namespace LinaVG
{
    namespace Examples
    {

        struct StarData
        {
            Vec2  m_pos        = Vec2(0.0f, 0.0f);
            Vec4  m_startCol   = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
            Vec4  m_endCol     = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
            float m_haloRadius = 0.0f;
            float m_radialSize = 0.0f;
        };

        std::vector<StarData> stars;

        void DemoScreens::Initialize()
        {
            m_defaultFont     = LinaVG::LoadFont("Resources/Fonts/SourceSansPro-Regular.ttf", false, 18);
            m_titleFont       = LinaVG::LoadFont("Resources/Fonts/SourceSansPro-Regular.ttf", true, 65);
            m_descFont        = LinaVG::LoadFont("Resources/Fonts/SourceSansPro-Regular.ttf", false, 20);
            m_textDemoFont    = LinaVG::LoadFont("Resources/Fonts/SourceSansPro-Regular.ttf", false, 30);
            m_textDemoSDFFont = LinaVG::LoadFont("Resources/Fonts/SourceSansPro-Regular.ttf", true, 40);

           //m_screenDescriptions.push_back("LinaVG supports variety of convex shapes, which can be partially or fully rounded, and all shapes also support filled & non-filled versions.");
           //m_screenDescriptions.push_back("You can use flat colors, alphas, vertical / horizontal gradients and rounded gradients. Also, textures w/ custom UV offset & tiling are supported.");
            m_screenDescriptions.push_back("TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST");
            m_screenDescriptions.push_back("TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST");
            
            m_screenDescriptions.push_back("LinaVG supports inner as well as outer outlines. Outlines also support all previous coloring and texturing options, as well as both filled & non-filled objects.");
            m_screenDescriptions.push_back("LinaVG supports single lines, multi-lines as well as bezier curves. Lines can have left/right or both caps, multi-lines can have 4 different types of joints. All lines also support outlines, coloring & texturing.");
            m_screenDescriptions.push_back("Texts support drop shadows, flat colors & vertical/horizontal gradients. SDF texts also support outlines, individual thickness as well as softness factors. LinaVG also provides alignment, wrapping & spacing options.");
            m_screenDescriptions.push_back("You can suply varying draw order to DrawXXX commands in order to support z-ordering.");
            m_screenDescriptions.push_back("You can use global clipping variables to create clipping rectangles for any shape you are drawing. Press C to toggle clipping.");
            m_screenDescriptions.push_back("Here are some examples of GUIs you can draw with LinaVG.");
            m_screenDescriptions.push_back("And since we have all that functionality, why not draw a simple retro grid.");

            // This is for Demo Screen 8, which is basically some basic retro art.
            std::srand(std::time(0));

            const int  starCount  = 5 + (std::rand() % 50);
            const Vec2 screenSize = Vec2(static_cast<float>(LinaVG::Config.m_displayWidth), static_cast<float>(LinaVG::Config.m_displayHeight));
            const Vec2 skyEnd     = Vec2(screenSize.x, screenSize.y * 0.45f);

            for (int i = 0; i < starCount; i++)
            {
                StarData star;
                star.m_pos        = Vec2(0 + (std::rand() % static_cast<int>(skyEnd.x)), 0 + (std::rand() % static_cast<int>(skyEnd.y)));
                star.m_radialSize = static_cast<float>((15 + (std::rand() % 35))) / 10.0f;
                star.m_haloRadius = static_cast<float>((5 + (std::rand() % 12)));

                const float redFactor  = static_cast<float>(5 + (std::rand() % 90)) / 255.0f;
                const float redFactor2 = static_cast<float>(5 + (std::rand() % 90)) / 255.0f;
                star.m_startCol        = Vec4(0.6f + redFactor, 0.6f, 0.6f, 0.9f);
                star.m_endCol          = Vec4(0.6f + redFactor2, 0.6f, 0.6f, 0.0f);
                stars.push_back(star);
            }
        }

        void DemoScreens::ShowBackground()
        {
            const Vec2   screenSize = Vec2(static_cast<float>(LinaVG::Config.m_displayWidth), static_cast<float>(LinaVG::Config.m_displayHeight));
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
                LinaVG::DrawRect(Vec2(statsWindowX, statsWindowY), Vec2(screenSize.x, screenSize.y * 0.17f), style, 0.0f, 3);
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
                LinaVG::DrawTextNormal(drawCountStr.c_str(), textPosition, textStyle, 0.0f, 4);
                textPosition.y += 25;
                LinaVG::DrawTextNormal(vertexCountStr.c_str(), textPosition, textStyle, 0.0f, 4);
                textPosition.y += 25;
                LinaVG::DrawTextNormal(triangleCountStr.c_str(), textPosition, textStyle, 0.0f, 4);
                textPosition.y += 25;
                LinaVG::DrawTextNormal(fpsStr.c_str(), textPosition, textStyle, 0.0f, 4);
            }

            // Draw semi-transparent black rectangle on the bottom of the screen.
            style.m_color      = Vec4(0, 0, 0, 0.5f);
            style.m_rounding   = 0.0f;
            const Vec2 rectMin = Vec2(0.0f, screenSize.y - screenSize.y * 0.12f);
            LinaVG::DrawRect(rectMin, screenSize, style, 0.0f, 3);

            //  // Draw a vertical dividers.
            const float  rectHeight = screenSize.y - rectMin.y;
            const float  rectWidth  = screenSize.x - rectMin.x;
            StyleOptions vertDivider;
            vertDivider.m_color = Vec4(1, 1, 1, 1);
            LinaVG::DrawLine(Vec2(rectWidth * 0.225f, rectMin.y), Vec2(rectWidth * 0.225f, screenSize.y), vertDivider, LineCapDirection::None, 0.0f, 4);
            LinaVG::DrawLine(Vec2(rectWidth * 0.725f, rectMin.y), Vec2(rectWidth * 0.725f, screenSize.y), vertDivider, LineCapDirection::None, 0.0f, 4);

            // Draw title text.
            SDFTextOptions sdfStyle;
            sdfStyle.m_font           = m_titleFont;
            const Vec2 size           = LinaVG::CalculateTextSize(m_screenTitles[ExampleApp::Get()->GetCurrentScreen() - 1], sdfStyle);
            const Vec2 titlePos       = Vec2(rectMin.x + 20, rectMin.y + rectHeight / 2.0f + size.y / 2.0f);
            sdfStyle.m_newLineSpacing = 10.0f;
            sdfStyle.m_color          = Utility::HexToVec4(0xFCAA67);
            sdfStyle.m_sdfThickness   = 0.62f;
            sdfStyle.m_sdfSoftness    = 0.5f;
            LinaVG::DrawTextSDF(m_screenTitles[ExampleApp::Get()->GetCurrentScreen() - 1], titlePos, sdfStyle, 0, 4);

            // Current screen description.
            TextOptions descText;
            descText.m_font      = m_descFont;
            descText.m_wrapWidth = rectWidth * 0.45f;
            LinaVG::DrawTextNormal(m_screenDescriptions[ExampleApp::Get()->GetCurrentScreen() - 1], Vec2(rectWidth * 0.25f, rectMin.y + 30), descText, 0, 4);

            // Draw controls info
            TextOptions controlsText;
            controlsText.m_font      = m_descFont;
            controlsText.m_textScale = 0.8f;
            LinaVG::DrawTextNormal("Press num keys [1-8] to switch between demo screens.", Vec2(rectWidth * 0.725f + 20, rectMin.y + 20), controlsText, 0, 4);
            LinaVG::DrawTextNormal("Press P to toggle performance stats.", Vec2(rectWidth * 0.725f + 20, rectMin.y + 40), controlsText, 0, 4);
            LinaVG::DrawTextNormal("Press F to toggle wireframe rendering.", Vec2(rectWidth * 0.725f + 20, rectMin.y + 60), controlsText, 0, 4);
            LinaVG::DrawTextNormal("Press R to start/stop rotation.", Vec2(rectWidth * 0.725f + 20, rectMin.y + 80), controlsText, 0, 4);
            LinaVG::DrawTextNormal("Press E to reset rotation.", Vec2(rectWidth * 0.725f + 20, rectMin.y + 100), controlsText, 0, 4);
        }

        void DemoScreens::ShowDemoScreen1_Shapes()
        {
            const Vec2 screenSize = Vec2(static_cast<float>(LinaVG::Config.m_displayWidth), static_cast<float>(LinaVG::Config.m_displayHeight));

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
            const Vec2 screenSize = Vec2(static_cast<float>(LinaVG::Config.m_displayWidth), static_cast<float>(LinaVG::Config.m_displayHeight));
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
            const Vec2 screenSize = Vec2(static_cast<float>(LinaVG::Config.m_displayWidth), static_cast<float>(LinaVG::Config.m_displayHeight));
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
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 0.0f, 245.0f, 1);

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
            const Vec2       screenSize = Vec2(static_cast<float>(LinaVG::Config.m_displayWidth), static_cast<float>(LinaVG::Config.m_displayHeight));
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
            defaultStyle.m_color                      = Vec4(0.5f, 0.75f, 0.33f, 1.0f);
            defaultStyle.m_color.m_gradientType       = GradientType::Horizontal;
            defaultStyle.m_outlineOptions.m_thickness = 2.0f;
            defaultStyle.m_outlineOptions.m_color     = Vec4(0, 0, 0, 1);
            LinaVG::DrawBezier(startPos, Vec2(startPos.x + 200, startPos.y + 200), Vec2(startPos.x + 500, startPos.y - 200), Vec2(startPos.x + 700, startPos.y), defaultStyle, lineCap, jointType, 1, 100);

            jointType = LineJointType::Miter;
            startPos.y += 120;
            defaultStyle.m_color.m_start              = Vec4(0.5f, 1.0f, 1.0f, 1.0f);
            defaultStyle.m_color.m_end                = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
            defaultStyle.m_outlineOptions.m_thickness = 0.0f;
            defaultStyle.m_textureHandle              = 0;
            defaultStyle.m_thickness.m_start          = 2.0f;
            defaultStyle.m_thickness.m_end            = 16.0f;
            LinaVG::DrawBezier(startPos, Vec2(startPos.x + 200, startPos.y + 200), Vec2(startPos.x + 500, startPos.y - 200), Vec2(startPos.x + 700, startPos.y), defaultStyle, lineCap, jointType, 1, 100);

            TextOptions t;

            startPos.y += 120;
            lineCap                                       = LineCapDirection::None;
            defaultStyle.m_outlineOptions.m_textureHandle = ExampleApp::Get()->GetCheckeredTexture();
            defaultStyle.m_textureHandle                  = 0;
            defaultStyle.m_outlineOptions.m_thickness     = 7.0f;
            defaultStyle.m_thickness                      = 15.0f;

            std::vector<Vec2> points;
            points.push_back(startPos);
            points.push_back(Vec2(startPos.x + 200, startPos.y));
            points.push_back(Vec2(startPos.x + 200, startPos.y + 300));
            points.push_back(Vec2(startPos.x + 600, startPos.y + 300));
            points.push_back(Vec2(startPos.x + 700, startPos.y));
            LinaVG::DrawLines(&points[0], static_cast<int>(points.size()), defaultStyle, lineCap, jointType, 1);
        }

        void DemoScreens::ShowDemoScreen5_Texts()
        {
            const Vec2  screenSize = Vec2(static_cast<float>(LinaVG::Config.m_displayWidth), static_cast<float>(LinaVG::Config.m_displayHeight));
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
            sdfOpts.m_sdfSoftness  = 2.0f;
            sdfOpts.m_color        = Vec4(0.1f, 0.8f, 0.1f, 1.0f);
            LinaVG::DrawTextSDF("Smoother text", startPos, sdfOpts, m_rotateAngle, 1);

            startPos.y += 50;
            sdfOpts.m_color               = Vec4(1, 1, 1, 1);
            sdfOpts.m_sdfThickness        = 0.6f;
            sdfOpts.m_sdfSoftness         = 0.5f;
            sdfOpts.m_sdfOutlineThickness = 0.1f;
            sdfOpts.m_sdfOutlineColor     = Vec4(0, 0, 0, 1);
            LinaVG::DrawTextSDF("Outlined SDF text", startPos, sdfOpts, m_rotateAngle, 1);

            startPos.y += 50;
            sdfOpts.m_sdfThickness        = 0.8f;
            sdfOpts.m_sdfSoftness         = 0.5f;
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
            const Vec2   screenSize = Vec2(static_cast<float>(LinaVG::Config.m_displayWidth), static_cast<float>(LinaVG::Config.m_displayHeight));
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
                LinaVG::DrawRect(startPos, Vec2(startPos.x + 120, startPos.y + 120), opts, 0.0f, i);

                std::string orderStr = std::to_string(i / 2);
                LinaVG::DrawTextNormal(orderStr, Vec2(startPos.x + 5, startPos.y + 25), textOpts, 0.0f, i + 1);

                startPos.x += 20;
                startPos.y += 20;
            }
        }

        void DemoScreens::ShowDemoScreen7_Clipping()
        {
            const Vec2 screenSize = Vec2(static_cast<float>(LinaVG::Config.m_displayWidth), static_cast<float>(LinaVG::Config.m_displayHeight));
            Vec2       startPos   = Vec2(screenSize.x * 0.5f, screenSize.y * 0.5f);
            const Vec2 size       = Vec2(500, 500);

            StyleOptions opts;
            opts.m_isFilled = true;

            const Vec2 min = Vec2(startPos.x - size.x / 2.0f, startPos.y - size.y / 2.0f);
            const Vec2 max = Vec2(startPos.x + size.x / 2.0f, startPos.y + size.y / 2.0f);

            if (m_clippingEnabled)
            {
                Config.m_clipPosX  = static_cast<BackendHandle>(min.x);
                Config.m_clipPosY  = static_cast<BackendHandle>(min.y);
                Config.m_clipSizeX = static_cast<BackendHandle>(size.x);
                Config.m_clipSizeY = static_cast<BackendHandle>(size.y);
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

            Config.m_clipPosX  = 0;
            Config.m_clipPosY  = 0;
            Config.m_clipSizeX = 0;
            Config.m_clipSizeY = 0;
        }

        void DemoScreens::ShowDemoScreen8_Animated()
        {
            const Vec2 screenSize = Vec2(static_cast<float>(LinaVG::Config.m_displayWidth), static_cast<float>(LinaVG::Config.m_displayHeight));

            auto drawSinBezier = [](const Vec2& pos) {
                StyleOptions     defaultStyle;
                LineJointType    jointType;
                LineCapDirection lineCap = LineCapDirection::None;

                static float controlPos1Y = 140;
                static float controlPos2Y = -140;

                controlPos1Y = std::sin(ExampleApp::Get()->GetElapsed() * 2) * 200;
                controlPos2Y = std::cos(ExampleApp::Get()->GetElapsed() * 2) * 200;

                jointType                        = LineJointType::Miter;
                defaultStyle.m_color.m_start     = Vec4(0.5f, 1.0f, 1.0f, 1.0f);
                defaultStyle.m_color.m_end       = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
                defaultStyle.m_textureHandle     = 0;
                defaultStyle.m_thickness.m_start = 2.0f;
                defaultStyle.m_thickness.m_end   = 16.0f;
                LinaVG::DrawBezier(pos, Vec2(pos.x + 200, pos.y + controlPos1Y), Vec2(pos.x + 400, pos.y + controlPos2Y), Vec2(pos.x + 600, pos.y), defaultStyle, lineCap, jointType, 1, 100);
            };

            auto drawLoadingBar1 = [&](const Vec2& pos) {
                StyleOptions background;
                background.m_outlineOptions.m_color     = Vec4(0, 0, 0, 1);
                background.m_outlineOptions.m_thickness = 0.5f;
                background.m_rounding                   = 0.2f;
                background.m_color                      = Vec4(0.2f, 0.2f, 0.2f, 0.2f);
                LinaVG::DrawRect(pos, Vec2(pos.x + 600, pos.y + 25), background, 0.0f, 1);

                StyleOptions fill;
                fill.m_color = Vec4(0.6f, 0.2f, 0.35f, 1.0f);

                static float fillX = 0.0f;

                if (fillX < 595)
                {
                    fillX += ExampleApp::Get()->GetFrameTime() * 80;
                }
                else
                    fillX = 0.0f;
                LinaVG::DrawRect(Vec2(pos.x + 1, pos.y + 1), Vec2(pos.x + fillX, pos.y + 24), fill, 0.0f, 2);

                TextOptions textOpts;
                textOpts.m_font = m_defaultFont;
                std::string loadingStr = "Loading " + std::to_string(fillX / 600.0f);
                LinaVG::DrawTextNormal(loadingStr, Vec2(pos.x, pos.y + 26), textOpts, 0.0f, 1);

            };

            drawSinBezier(Vec2(screenSize.x * 0.05f, screenSize.y * 0.15f));
            drawLoadingBar1(Vec2(screenSize.x * 0.05f, screenSize.y * 0.35f));
        }

        void DemoScreens::ShowDemoScreen9_Final()
        {
            const Vec2 screenSize = Vec2(static_cast<float>(LinaVG::Config.m_displayWidth), static_cast<float>(LinaVG::Config.m_displayHeight));

            // Sky
            StyleOptions sky;
            const Vec2   skyEnd        = Vec2(screenSize.x, screenSize.y * 0.65f);
            sky.m_color.m_start        = Utility::HexToVec4(0x41295a);
            sky.m_color.m_end          = Vec4(0.44f, 0.1f, 0.16f, 1.0f);
            sky.m_color.m_gradientType = GradientType::Vertical;
            LinaVG::DrawRect(Vec2(0.0f, 0.0f), skyEnd, sky, 0.0f, 1);

            // Sun
            static float sunRotation = 0.0f;
            sunRotation += ExampleApp::Get()->GetFrameTime() * 5.0f;

            StyleOptions sun;
            const Vec2   sunCenter     = Vec2(screenSize.x / 2.0f, screenSize.y * 0.4f);
            sun.m_color.m_start        = Utility::HexToVec4(0xfeb47b);
            sun.m_color.m_end          = Vec4(0.84f, 0.35f, 0.26f, 1.0f);
            sun.m_color.m_gradientType = GradientType::RadialCorner;
            sun.m_color.m_radialSize   = 0.9f;
            LinaVG::DrawCircle(sunCenter, 200, sun, 72, sunRotation, 0.0f, 360.0f, 3);

            // Horizon line
            StyleOptions horizon;
            horizon.m_thickness = 4.0f;
            horizon.m_color     = Vec4(0.08f, 0.08f, 0.08f, 1.0f);
            LinaVG::DrawLine(Vec2(0.0f, skyEnd.y), Vec2(screenSize.x, skyEnd.y), horizon, LineCapDirection::None, 0.0f, 30);

            // Ground plane
            StyleOptions groundPlaneStyle;
            const Vec2   gridStart                  = Vec2(0.0f, skyEnd.y);
            groundPlaneStyle.m_isFilled             = true;
            groundPlaneStyle.m_color.m_gradientType = GradientType::Vertical;
            groundPlaneStyle.m_color.m_start        = Vec4(0.122f, 0.112, 0.28f, 1.0f);
            groundPlaneStyle.m_color.m_end          = Vec4(0.05f, 0.05f, 0.12f, 1.0f);
            LinaVG::DrawRect(gridStart, Vec2(screenSize.x, screenSize.y), groundPlaneStyle, 0.0f, 1);

            // Ground plane grid Y
            Vec2        currentGrid    = gridStart;
            const int   lineCountY     = 8;
            const float gridLeftY      = screenSize.y - gridStart.y;
            const float gridYIncrement = gridLeftY / static_cast<float>(lineCountY);
            for (int i = 0; i < lineCountY; i++)
            {
                StyleOptions gridLine;
                gridLine.m_color     = Vec4(1.0f, 1.0f, 1.0f, 0.35f);
                gridLine.m_thickness = 2.0f;
                LinaVG::DrawLine(currentGrid, Vec2(screenSize.x, currentGrid.y), gridLine, LineCapDirection::None, 0.0f, 2);
                currentGrid.y += gridYIncrement;
            }

            // Ground plane grid X
            const int   lineCountX     = 20;
            const float gridLeftX      = screenSize.x;
            const float gridXIncrement = gridLeftX / static_cast<float>(lineCountX);
            currentGrid                = gridStart;

            const int   center       = lineCountX / 2;
            const float skewMax      = 1600.0f;
            const float tLimitMax    = static_cast<float>(lineCountX);
            const float tLimitCenter = static_cast<float>(center);
            for (int i = 0; i < lineCountX; i++)
            {
                StyleOptions gridLine;
                float        a = 0.0f;

                if (i < center)
                    a = Math::Remap(static_cast<float>(i), 0.0f, tLimitCenter, 0.2f, 0.5f);
                else
                    a = Math::Remap(static_cast<float>(i), tLimitCenter, tLimitMax, 0.5f, 0.2f);

                gridLine.m_color     = Vec4(1.0f, 1.0f, 1.0f, a);
                gridLine.m_thickness = 2.0f;

                const float skew = Math::Remap(static_cast<float>(i), 0.0f, tLimitMax, -skewMax, skewMax);
                LinaVG::DrawLine(currentGrid, Vec2(currentGrid.x + skew, screenSize.y), gridLine, LineCapDirection::None, 0.0f, 2);
                currentGrid.x += gridXIncrement;
            }

            StyleOptions bgMounts;
            bgMounts.m_color.m_start = Vec4(0, 0, 0, 1);
            bgMounts.m_color.m_end   = Vec4(0.04f, 0.04f, 0.12f, 1.0f);
            Vec2 triLeft             = Vec2(-screenSize.x * 0.1f, gridStart.y);
            Vec2 triRight            = Vec2(screenSize.x * 0.24f, gridStart.y);
            Vec2 triTop              = Vec2(screenSize.x * 0.12f, gridStart.y - screenSize.y * 0.18f);
            LinaVG::DrawTriangle(triTop, triRight, triLeft, bgMounts, 0.0f, 3);

            triLeft.x += screenSize.x * 0.2f;
            triRight.x += screenSize.x * 0.12f;
            triTop.x += screenSize.x * 0.1f;
            triTop.y += screenSize.y * 0.02f;
            LinaVG::DrawTriangle(triTop, triRight, triLeft, bgMounts, 0.0f, 4);

            triRight.x += screenSize.x * 0.12f;
            triTop.x += screenSize.x * 0.1f;
            triTop.y += screenSize.y * 0.02f;
            LinaVG::DrawTriangle(triTop, triRight, triLeft, bgMounts, 0.0f, 5);

            // Right
            triRight = Vec2(screenSize.x + screenSize.x * 0.1f, gridStart.y);
            triLeft  = Vec2(screenSize.x - screenSize.x * 0.29f, gridStart.y);
            triTop   = Vec2(screenSize.x - screenSize.x * 0.16f, gridStart.y - screenSize.y * 0.18f);
            LinaVG::DrawTriangle(triTop, triRight, triLeft, bgMounts, 0.0f, 3);

            triLeft.x -= screenSize.x * 0.2f;
            triTop.x += screenSize.x * 0.1f;
            triTop.y += screenSize.y * 0.02f;
            LinaVG::DrawTriangle(triTop, triRight, triLeft, bgMounts, 0.0f, 4);

            triLeft.x  = screenSize.x * 0.6f;
            triRight.x = screenSize.x * 0.9f;
            triTop.x   = screenSize.x * 0.7f;
            triTop.y += screenSize.y * 0.02f;
            LinaVG::DrawTriangle(triTop, triRight, triLeft, bgMounts, 0.0f, 5);

            StyleOptions fgMounts;
            fgMounts.m_color.m_start = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
            fgMounts.m_color.m_end   = Vec4(0.1f, 0.1f, 0.2f, 1.0f);
            triLeft                  = Vec2(-screenSize.x * 0.1f, gridStart.y);
            triRight                 = Vec2(screenSize.x * 0.24f, gridStart.y);
            triTop                   = Vec2(screenSize.x * 0.12f, gridStart.y - screenSize.y * 0.1f);
            LinaVG::DrawTriangle(triTop, triRight, triLeft, fgMounts, 0.0f, 6);

            triLeft.x += screenSize.x * 0.2f;
            triTop.x += screenSize.x * 0.1f;
            triRight.x += screenSize.x * 0.05f;
            LinaVG::DrawTriangle(triTop, triRight, triLeft, fgMounts, 0.0f, 7);

            fgMounts.m_color.m_start = Vec4(0.05f, 0.05f, 0.05f, 1);
            fgMounts.m_color.m_end   = Vec4(0.1f, 0.01f, 0.13f, 1.0f);
            triLeft                  = triRight;
            triRight.x += screenSize.x * 0.05f;
            LinaVG::DrawTriangle(triTop, triRight, triLeft, fgMounts, 0.0f, 8);

            // Right
            fgMounts.m_color.m_start = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
            fgMounts.m_color.m_end   = Vec4(0.1f, 0.1f, 0.2f, 1.0f);
            triLeft                  = Vec2(screenSize.x - screenSize.x * 0.1f, gridStart.y);
            triRight                 = Vec2(screenSize.x, gridStart.y);
            triTop                   = Vec2(screenSize.x - screenSize.x * 0.12f, gridStart.y - screenSize.y * 0.1f);
            LinaVG::DrawTriangle(triTop, triRight, triLeft, fgMounts, 0.0f, 7);

            fgMounts.m_color.m_start = Vec4(0, 0, 0, 1);
            fgMounts.m_color.m_end   = Vec4(0.09f, 0.04f, 0.12f, 1.0f);
            triRight                 = triLeft;
            triLeft.x -= screenSize.x * 0.1f;
            LinaVG::DrawTriangle(triTop, triRight, triLeft, fgMounts, 0.0f, 7);

            // Stars
            const Vec2 starsMax = Vec2(skyEnd.x, skyEnd.y * 0.8f);

            for (int i = 0; i < stars.size(); i++)
            {
                StyleOptions star;
                star.m_color.m_start        = stars[i].m_startCol;
                star.m_color.m_end          = stars[i].m_endCol;
                star.m_color.m_radialSize   = stars[i].m_radialSize;
                star.m_color.m_gradientType = GradientType::Radial;

                const float sin      = std::sin(ExampleApp::Get()->GetElapsed() * 2.0f);
                const float haloSize = stars[i].m_haloRadius + sin + static_cast<float>((std::rand() % 100)) / 100.0f;
                LinaVG::DrawCircle(stars[i].m_pos, haloSize, star, 36, 0.0f, 0.0f, 360.0f, 2);

                // star.m_color = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
                // LinaVG::DrawCircle(starPos, starRadius, star, 36, 0.0f, 0.0f, 360.0f, 4);
            }
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