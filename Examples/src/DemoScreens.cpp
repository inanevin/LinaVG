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
            Vec2  pos        = Vec2(0.0f, 0.0f);
            Vec4  startCol   = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
            Vec4  endCol     = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
            float haloRadius = 0.0f;
            float radialSize = 0.0f;
        };

        std::vector<StarData> stars;

        void DemoScreens::Initialize()
        {
            m_defaultFont     = LinaVG::LoadFont("Resources/Fonts/NotoSans-Regular.ttf", false, 18);
            m_titleFont       = LinaVG::LoadFont("Resources/Fonts/SourceSansPro-Regular.ttf", true, 52);
            m_descFont        = LinaVG::LoadFont("Resources/Fonts/NotoSans-Regular.ttf", false, 20);
            m_textDemoFont    = LinaVG::LoadFont("Resources/Fonts/NotoSans-Regular.ttf", false, 30);
            m_textDemoSDFFont = LinaVG::LoadFont("Resources/Fonts/NotoSans-Regular.ttf", true, 40);

            m_screenDescriptions.push_back("LinaVG supports variety of convex shapes, which can be partially or fully rounded, and all shapes also support filled & non-filled versions.");
            m_screenDescriptions.push_back("You can use flat colors, alphas, vertical / horizontal gradients and rounded gradients. Also, textures w/ custom UV offset & tiling are supported.");
            m_screenDescriptions.push_back("LinaVG supports inner as well as outer outlines. Outlines also support all previous coloring and texturing options, as well as both filled & non-filled objects.");
            m_screenDescriptions.push_back("LinaVG supports single lines, multi-lines as well as bezier curves. Lines can have left/right or both caps, multi-lines can have 4 different types of joints. All lines also support outlines, coloring & texturing.");
            m_screenDescriptions.push_back("Texts support drop shadows, flat colors & vertical/horizontal gradients. SDF texts also support outlines, individual thickness as well as softness factors. LinaVG also provides alignment, wrapping & spacing options.");
            m_screenDescriptions.push_back("You can suply varying draw order to DrawXXX commands in order to support z-ordering.");
            m_screenDescriptions.push_back("You can use global clipping variables to create clipping rectangles for any shape you are drawing. Press C to toggle clipping.");
            m_screenDescriptions.push_back("Here are some examples of animated shapes you can draw with LinaVG.");
            m_screenDescriptions.push_back("And since we have all that functionality, why not draw a simple retro grid.");

            // This is for Demo Screen 8, which is basically some basic retro art.
            std::srand(static_cast<unsigned int>(std::time(0)));

            const int  starCount  = 5 + (std::rand() % 50);
            const Vec2 screenSize = Vec2(static_cast<float>(LinaVG::Config.displayWidth), static_cast<float>(LinaVG::Config.displayHeight));
            const Vec2 skyEnd     = Vec2(screenSize.x, screenSize.y * 0.45f);

            for (int i = 0; i < starCount; i++)
            {
                StarData star;
                star.pos        = Vec2(0.0f + static_cast<float>(std::rand() % static_cast<int>(skyEnd.x)), 0.0f + static_cast<float>(std::rand() % static_cast<int>(skyEnd.y)));
                star.radialSize = static_cast<float>((15 + (std::rand() % 35))) / 10.0f;
                star.haloRadius = static_cast<float>((5 + (std::rand() % 12)));

                const float redFactor  = static_cast<float>(5 + (std::rand() % 90)) / 255.0f;
                const float redFactor2 = static_cast<float>(5 + (std::rand() % 90)) / 255.0f;
                star.startCol          = Vec4(0.6f + redFactor, 0.6f, 0.6f, 0.9f);
                star.endCol            = Vec4(0.6f + redFactor2, 0.6f, 0.6f, 0.0f);
                stars.push_back(star);
            }
        }

        void DemoScreens::ShowBackground()
        {
            const Vec2   screenSize = Vec2(static_cast<float>(LinaVG::Config.displayWidth), static_cast<float>(LinaVG::Config.displayHeight));
            StyleOptions style;

            // Draw background gradient.
            style.color    = Utility::HexToVec4(0x818D92);
            style.isFilled = true;
            LinaVG::DrawRect(Vec2(0.0f, 0.0f), screenSize, style, 0.0f, 0);

            // Draw stats window.
            if (m_statsWindowOn)
            {
                const float statsWindowX = screenSize.x - screenSize.x * 0.18f;
                const float statsWindowY = screenSize.y * 0.05f;
                style.color              = Vec4(0, 0, 0, 0.5f);
                style.rounding           = 0.2f;
                style.onlyRoundTheseCorners.push_back(0);
                style.onlyRoundTheseCorners.push_back(3);
                LinaVG::DrawRect(Vec2(statsWindowX, statsWindowY), Vec2(screenSize.x, screenSize.y * 0.17f), style, 0.0f, 3);
                style.onlyRoundTheseCorners.clear();

                // Draw stats texts.
                const std::string drawCountStr     = "Draw Calls: " + std::to_string(m_drawCount);
                const std::string triangleCountStr = "Tris Count: " + std::to_string(m_triangleCount);
                const std::string vertexCountStr   = "Vertex Count: " + std::to_string(m_vertexCount);
                const std::string frameTimeStr     = "Frame: " + std::to_string(ExampleApp::Get()->GetFrameTimeRead()) + " ms";
                const std::string fpsStr           = "FPS: " + std::to_string(ExampleApp::Get()->GetFPS()) + " " + frameTimeStr;

                Vec2           textPosition = Vec2(statsWindowX + 10, statsWindowY + 12);
                SDFTextOptions textStyle;
                textStyle.textScale = 0.82f;
                textStyle.font      = m_defaultFont;
                LinaVG::DrawTextNormal(drawCountStr.c_str(), textPosition, textStyle, 0.0f, 4);
                textPosition.y += 25;
                LinaVG::DrawTextNormal(vertexCountStr.c_str(), textPosition, textStyle, 0.0f, 4);
                textPosition.y += 25;
                LinaVG::DrawTextNormal(triangleCountStr.c_str(), textPosition, textStyle, 0.0f, 4);
                textPosition.y += 25;
                LinaVG::DrawTextNormal(fpsStr.c_str(), textPosition, textStyle, 0.0f, 4);
            }

            // Draw semi-transparent black rectangle on the bottom of the screen.
            style.color        = Vec4(0, 0, 0, 0.5f);
            style.rounding     = 0.0f;
            const Vec2 rectMin = Vec2(0.0f, screenSize.y - screenSize.y * 0.12f);
            LinaVG::DrawRect(rectMin, screenSize, style, 0.0f, 3);

            //  // Draw a vertical dividers.
            const float  rectHeight = screenSize.y - rectMin.y;
            const float  rectWidth  = screenSize.x - rectMin.x;
            StyleOptions vertDivider;
            vertDivider.color = Vec4(1, 1, 1, 1);
            LinaVG::DrawLine(Vec2(rectWidth * 0.225f, rectMin.y), Vec2(rectWidth * 0.225f, screenSize.y), vertDivider, LineCapDirection::None, 0.0f, 4);
            LinaVG::DrawLine(Vec2(rectWidth * 0.725f, rectMin.y), Vec2(rectWidth * 0.725f, screenSize.y), vertDivider, LineCapDirection::None, 0.0f, 4);

            // Draw title text.
            SDFTextOptions sdfStyle;
            sdfStyle.font           = m_titleFont;
            const Vec2 size         = LinaVG::CalculateTextSize(m_screenTitles[ExampleApp::Get()->GetCurrentScreen() - 1], sdfStyle);
            const Vec2 titlePos     = Vec2(rectMin.x + 20, rectMin.y + rectHeight / 2.0f - size.y / 2.0f);
            sdfStyle.newLineSpacing = 10.0f;
            sdfStyle.color          = Utility::HexToVec4(0xFCAA67);
            sdfStyle.sdfThickness   = 0.62f;
            sdfStyle.sdfSoftness    = 0.5f;
            sdfStyle.spacing        = 3.0f;
            LinaVG::DrawTextSDF(m_screenTitles[ExampleApp::Get()->GetCurrentScreen() - 1], titlePos, sdfStyle, 0, 4);

            // Current screen description.
            TextOptions descText;
            descText.font      = m_descFont;
            descText.wrapWidth = rectWidth * 0.45f;
            LinaVG::DrawTextNormal(m_screenDescriptions[ExampleApp::Get()->GetCurrentScreen() - 1], Vec2(rectWidth * 0.25f, rectMin.y + 20), descText, 0, 4);

            // Draw version text.
            TextOptions versionText;
            versionText.font       = m_descFont;
            versionText.color      = Vec4(1.0f, 1.0f, 1.0f, 0.55f);
            versionText.textScale  = 0.8f;
            versionText.alignment  = TextAlignment::Right;
            std::string versionStr = "Inan Evin, LinaVG v";
            versionStr += std::to_string(LINAVG_VERSION_MAJOR) + "." + std::to_string(LINAVG_VERSION_MINOR) + "." + std::to_string(LINAVG_VERSION_PATCH);
            LinaVG::DrawTextNormal(versionStr, Vec2(screenSize.x - 10, rectMin.y - 30), versionText, 0.0f, 1);

            // Draw controls info
            TextOptions controlsText;
            controlsText.font      = m_descFont;
            controlsText.textScale = 0.8f;
            LinaVG::DrawTextNormal("Num keys[1-9]: switch screen", Vec2(rectWidth * 0.725f + 20, rectMin.y + 10), controlsText, 0, 4);
            LinaVG::DrawTextNormal("P: toggle performance stats.", Vec2(rectWidth * 0.725f + 20, rectMin.y + 30), controlsText, 0, 4);
            LinaVG::DrawTextNormal("F: toggle wireframe rendering.", Vec2(rectWidth * 0.725f + 20, rectMin.y + 50), controlsText, 0, 4);
            LinaVG::DrawTextNormal("R: start/stop rotation.", Vec2(rectWidth * 0.725f + 20, rectMin.y + 70), controlsText, 0, 4);
            LinaVG::DrawTextNormal("E: reset rotation.", Vec2(rectWidth * 0.725f + 20, rectMin.y + 90), controlsText, 0, 4);
        }

        void DemoScreens::ShowDemoScreen1_Shapes()
        {
            const Vec2 screenSize = Vec2(static_cast<float>(LinaVG::Config.displayWidth), static_cast<float>(LinaVG::Config.displayHeight));

            StyleOptions defaultStyle;
            defaultStyle.isFilled = true;
            Vec2 startPos         = Vec2(screenSize.x * 0.05f, screenSize.y * 0.05f);

            //*************************** ROW 1 ***************************/

            // Rect - filled
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // fRect - nonfilled
            startPos.x += 200;
            defaultStyle.isFilled = false;
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Rect partially rounded - non filled
            startPos.x += 200;
            defaultStyle.isFilled = false;
            defaultStyle.rounding = 0.5f;
            defaultStyle.onlyRoundTheseCorners.push_back(0);
            defaultStyle.onlyRoundTheseCorners.push_back(3);
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);
            defaultStyle.onlyRoundTheseCorners.clear();

            // Rect fully rounded - filled
            startPos.x += 200;
            defaultStyle.isFilled = true;
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            //*************************** ROW 2 ***************************/

            // Triangle filled
            startPos.x = screenSize.x * 0.05f;
            startPos.y += 200;
            defaultStyle.isFilled = true;
            defaultStyle.rounding = 0.0f;
            LinaVG::DrawTriangle(Vec2(startPos.x + 75, startPos.y), Vec2(startPos.x + 150, startPos.y + 150), Vec2(startPos.x, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Triangle non filled
            startPos.x += 200;
            defaultStyle.isFilled  = false;
            defaultStyle.thickness = 5.0f;
            LinaVG::DrawTriangle(Vec2(startPos.x + 75, startPos.y), Vec2(startPos.x + 150, startPos.y + 150), Vec2(startPos.x, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Triangle non filled partially rounded
            startPos.x += 200;
            defaultStyle.rounding = 0.2f;
            defaultStyle.onlyRoundTheseCorners.push_back(0);
            LinaVG::DrawTriangle(Vec2(startPos.x + 75, startPos.y), Vec2(startPos.x + 150, startPos.y + 150), Vec2(startPos.x, startPos.y + 150), defaultStyle, m_rotateAngle, 1);
            defaultStyle.onlyRoundTheseCorners.clear();

            // Triangle filled & fully rounded
            startPos.x += 200;
            defaultStyle.rounding = 0.4f;
            defaultStyle.isFilled = true;
            LinaVG::DrawTriangle(Vec2(startPos.x + 75, startPos.y), Vec2(startPos.x + 150, startPos.y + 150), Vec2(startPos.x, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            //*************************** ROW 3 ***************************/

            // Full circle filled
            startPos.x = screenSize.x * 0.05f;
            startPos.y += 200;
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 0.0f, 360.0f, 2);

            // Half circle non filled
            startPos.x += 200;
            defaultStyle.isFilled = false;
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 0.0f, 360.0f, 2);

            // Arc filled
            startPos.x += 200;
            defaultStyle.isFilled = true;
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 100.0f, 360.0f, 2);

            // Arc
            startPos.x += 200;
            defaultStyle.isFilled = true;
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 300.0f, 330.0f, 2);

            //*************************** ROW 4 ***************************/

            // Ngon - 6
            startPos.x = screenSize.x * 0.05f;
            startPos.y += 200;
            LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 6, defaultStyle, m_rotateAngle, 2);

            // Ngon - 8
            startPos.x += 200;
            defaultStyle.isFilled = false;
            LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 8, defaultStyle, m_rotateAngle, 2);

            // Convex
            startPos.x += 200;
            defaultStyle.isFilled = true;
            std::vector<Vec2> points;
            points.push_back(startPos);
            points.push_back(Vec2(startPos.x + 150, startPos.y));
            points.push_back(Vec2(startPos.x - 50, startPos.y + 150));
            points.push_back(Vec2(startPos.x + 100, startPos.y + 150));
            LinaVG::DrawConvex(&points[0], 4, defaultStyle, m_rotateAngle, 2);

            // Convex
            startPos.x += 200;
            defaultStyle.isFilled = false;
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
            const Vec2 screenSize = Vec2(static_cast<float>(LinaVG::Config.displayWidth), static_cast<float>(LinaVG::Config.displayHeight));
            Vec2       startPos   = Vec2(screenSize.x * 0.05f, screenSize.y * 0.05f);

            StyleOptions defaultStyle;
            defaultStyle.isFilled = true;

            //*************************** ROW 1 ***************************/

            // Single m_color
            defaultStyle.color = LinaVG::Utility::HexToVec4(0x212738);
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Single m_color
            startPos.x += 200;
            defaultStyle.color    = LinaVG::Utility::HexToVec4(0x06A77D);
            defaultStyle.rounding = 0.5f;
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Single m_color
            startPos.x += 200;
            defaultStyle.color    = LinaVG::Utility::HexToVec4(0xF1A208);
            defaultStyle.rounding = 0.5f;
            LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 7, defaultStyle, m_rotateAngle, 1);

            // Single m_color
            startPos.x += 200;
            defaultStyle.color = LinaVG::Utility::HexToVec4(0xFEFAE0);
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 0.0f, 360.0f, 1);

            //*************************** ROW 2 ***************************/

            // Horizontal gradient
            startPos.x = screenSize.x * 0.05f;
            startPos.y += 200;
            defaultStyle.rounding    = 0.0f;
            defaultStyle.color.start = Vec4(1.0f, 0.2f, 0.2f, 1.0f);
            defaultStyle.color.end   = Vec4(0.2f, 0.2f, 1.0f, 1.0f);
            LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 8, defaultStyle, m_rotateAngle, 1);

            // Horizontal gradient.
            startPos.x += 200;
            defaultStyle.rounding    = 0.0f;
            defaultStyle.color.start = Vec4(0.2f, 0.2f, 1.0f, 1.0f);
            defaultStyle.color.end   = Vec4(1.0f, 0.2f, 0.2f, 1.0f);
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Vertical gradient
            startPos.x += 200;
            defaultStyle.color.gradientType = GradientType::Vertical;
            defaultStyle.color.start        = Vec4(1.0f, 1.0f, 0.0f, 1.0f);
            defaultStyle.color.end          = Vec4(0.0f, 1.0f, 1.0f, 1.0f);
            LinaVG::DrawTriangle(Vec2(startPos.x + 75, startPos.y), Vec2(startPos.x + 150, startPos.y + 150), Vec2(startPos.x, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Vertical gradient.
            startPos.x += 200;
            defaultStyle.color.gradientType = GradientType::Vertical;
            defaultStyle.color.start        = Vec4(1.0f, 1.0f, 0.0f, 1.0f);
            defaultStyle.color.end          = Vec4(0.0f, 1.0f, 1.0f, 1.0f);
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 0.0f, 360.0f, 1);

            //*************************** ROW 3 ***************************/

            // Radial
            startPos.x = screenSize.x * 0.05f;
            startPos.y += 200;
            defaultStyle.color.start        = Vec4(0.2f, 0.2f, 0.9f, 1.0f);
            defaultStyle.color.end          = Vec4(0.9f, 0.2f, 0.9f, 1.0f);
            defaultStyle.color.gradientType = GradientType::Radial;
            defaultStyle.color.radialSize   = 1.4f;
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 0.0f, 360.0f, 1);

            // Radial
            startPos.x += 200;
            defaultStyle.color.start        = Vec4(0.2f, 0.2f, 0.9f, 1.0f);
            defaultStyle.color.end          = Vec4(0.9f, 0.2f, 0.9f, 1.0f);
            defaultStyle.color.gradientType = GradientType::Radial;
            defaultStyle.color.radialSize   = 1.4f;
            LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 7, defaultStyle, m_rotateAngle, 1);

            // Radial Corner
            startPos.x += 200;
            defaultStyle.color.start        = Vec4(0.2f, 0.2f, 1.0f, 1.0f);
            defaultStyle.color.end          = Vec4(1.0f, 0.2f, 0.2f, 1.0f);
            defaultStyle.color.gradientType = GradientType::RadialCorner;
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Radial corner
            startPos.x += 200;
            defaultStyle.color.start        = Vec4(0.2f, 0.2f, 1.0f, 1.0f);
            defaultStyle.color.end          = Vec4(1.0f, 0.2f, 0.2f, 1.0f);
            defaultStyle.color.gradientType = GradientType::RadialCorner;
            LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 7, defaultStyle, m_rotateAngle, 1);

            //*************************** ROW 4 ***************************/

            // Textured rect
            startPos.x = screenSize.x * 0.05f;
            startPos.y += 200;
            defaultStyle.textureHandle = ExampleApp::Get()->GetCheckeredTexture();
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Tiling
            startPos.x += 200;
            defaultStyle.textureUVTiling = Vec2(2, 2);
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 0.0f, 360.0f, 1);

            // Lina Logo
            startPos.x += 200;
            defaultStyle.textureUVTiling = Vec2(1, 1);
            defaultStyle.textureHandle   = ExampleApp::Get()->GetLinaLogoTexture();
            LinaVG::DrawImage(ExampleApp::Get()->GetLinaLogoTexture(), Vec2(startPos.x + 75, startPos.y + 75), Vec2(150, 150), m_rotateAngle, 1);

            // Lina Logo
            startPos.x += 200;
            LinaVG::DrawImage(ExampleApp::Get()->GetLinaLogoTexture(), Vec2(startPos.x + 75, startPos.y + 75), Vec2(150, 150), m_rotateAngle, 1, Vec2(2, 2));
        }

        void DemoScreens::ShowDemoScreen3_Outlines()
        {
            const Vec2 screenSize = Vec2(static_cast<float>(LinaVG::Config.displayWidth), static_cast<float>(LinaVG::Config.displayHeight));
            Vec2       startPos   = Vec2(screenSize.x * 0.05f, screenSize.y * 0.05f);

            StyleOptions defaultStyle;
            defaultStyle.isFilled = true;

            //*************************** ROW 1 ***************************/

            // Filled
            defaultStyle.thickness                = 5.0f;
            defaultStyle.outlineOptions.color     = Vec4(0, 0, 0, 1);
            defaultStyle.outlineOptions.thickness = 3.0f;
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Non filled outer
            startPos.x += 200;
            defaultStyle.outlineOptions.color.start = Vec4(1, 0, 0, 1);
            defaultStyle.outlineOptions.color.end   = Vec4(0, 0, 1, 1);
            defaultStyle.isFilled                   = false;
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Non filled inner
            startPos.x += 200;
            defaultStyle.outlineOptions.color         = Vec4(0, 0.5f, 0, 1);
            defaultStyle.outlineOptions.drawDirection = OutlineDrawDirection::Inwards;
            defaultStyle.isFilled                     = false;
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Both
            startPos.x += 200;
            defaultStyle.outlineOptions.color.start   = Vec4(1, 0, 0, 1);
            defaultStyle.outlineOptions.color.end     = Vec4(0, 0, 1, 1);
            defaultStyle.outlineOptions.drawDirection = OutlineDrawDirection::Both;
            defaultStyle.isFilled                     = false;
            LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            //*************************** ROW 2 ***************************/

            // Filled
            startPos.x = screenSize.x * 0.05f;
            startPos.y += 200;
            defaultStyle.outlineOptions.drawDirection = OutlineDrawDirection::Outwards;
            defaultStyle.rounding                     = 0.4f;
            defaultStyle.isFilled                     = true;
            defaultStyle.thickness                    = 8.0f;
            defaultStyle.outlineOptions.color         = Vec4(0, 0, 0, 1);
            defaultStyle.outlineOptions.thickness     = 5.0f;
            defaultStyle.outlineOptions.textureHandle = ExampleApp::Get()->GetCheckeredTexture();
            defaultStyle.color                        = Vec4(0.7f, 0.1f, 0.1f, 1.0f);
            LinaVG::DrawTriangle(Vec2(startPos.x + 75, startPos.y), Vec2(startPos.x + 150, startPos.y + 150), Vec2(startPos.x, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Non filled outer
            startPos.x += 200;
            defaultStyle.outlineOptions.color.start = Vec4(1, 0, 0, 1);
            defaultStyle.outlineOptions.color.end   = Vec4(0, 0, 1, 1);
            defaultStyle.isFilled                   = false;
            LinaVG::DrawTriangle(Vec2(startPos.x + 75, startPos.y), Vec2(startPos.x + 150, startPos.y + 150), Vec2(startPos.x, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Non filled inner
            startPos.x += 200;
            defaultStyle.outlineOptions.color         = Vec4(0, 0.5f, 0, 1);
            defaultStyle.outlineOptions.drawDirection = OutlineDrawDirection::Inwards;
            defaultStyle.isFilled                     = false;
            LinaVG::DrawTriangle(Vec2(startPos.x + 75, startPos.y), Vec2(startPos.x + 150, startPos.y + 150), Vec2(startPos.x, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            // Both
            startPos.x += 200;
            defaultStyle.outlineOptions.color.start   = Vec4(1, 0, 0, 1);
            defaultStyle.outlineOptions.color.end     = Vec4(0, 0, 1, 1);
            defaultStyle.outlineOptions.drawDirection = OutlineDrawDirection::Both;
            defaultStyle.isFilled                     = false;
            LinaVG::DrawTriangle(Vec2(startPos.x + 75, startPos.y), Vec2(startPos.x + 150, startPos.y + 150), Vec2(startPos.x, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

            //*************************** ROW 3 ***************************/

            // Filled
            startPos.x = screenSize.x * 0.05f;
            startPos.y += 200;
            defaultStyle.outlineOptions.drawDirection = OutlineDrawDirection::Outwards;
            defaultStyle.rounding                     = 0.4f;
            defaultStyle.isFilled                     = true;
            defaultStyle.thickness                    = 8.0f;
            defaultStyle.outlineOptions.color         = Vec4(0, 0, 0, 1);
            defaultStyle.outlineOptions.thickness     = 2.0f;
            defaultStyle.outlineOptions.textureHandle = 0;
            defaultStyle.color                        = Vec4(1, 1, 1, 1);
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 0.0f, 360.0f, 1);

            // outer
            startPos.x += 200;
            defaultStyle.outlineOptions.color.start = Vec4(1, 0, 0, 1);
            defaultStyle.outlineOptions.color.end   = Vec4(0, 0, 1, 1);
            defaultStyle.color.gradientType         = GradientType::Radial;
            defaultStyle.color.start                = Vec4(0.5f, 1.0f, 0.5f, 1.0f);
            defaultStyle.isFilled                   = true;
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 0.0f, 245.0f, 1);

            // Non filled inner
            startPos.x += 200;
            defaultStyle.outlineOptions.color         = Vec4(0, 0.5f, 0, 1);
            defaultStyle.outlineOptions.drawDirection = OutlineDrawDirection::Inwards;
            defaultStyle.isFilled                     = false;
            defaultStyle.color.gradientType           = GradientType::Vertical;
            defaultStyle.color                        = Vec4(1, 1, 1, 1);
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 90.0f, 360.0f, 1);

            // Both
            startPos.x += 200;
            defaultStyle.outlineOptions.color.start   = Vec4(1, 0, 0, 1);
            defaultStyle.outlineOptions.color.end     = Vec4(0, 0, 1, 1);
            defaultStyle.outlineOptions.drawDirection = OutlineDrawDirection::Both;
            defaultStyle.isFilled                     = false;
            LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 180.0f, 360.0f, 1);

            //*************************** ROW 4 ***************************/

            // Filled
            startPos.x = screenSize.x * 0.05f;
            startPos.y += 200;
            defaultStyle.outlineOptions.thickness     = 12.0f;
            defaultStyle.outlineOptions.drawDirection = OutlineDrawDirection::Outwards;
            defaultStyle.rounding                     = 0.4f;
            defaultStyle.isFilled                     = true;
            defaultStyle.thickness                    = 8.0f;
            defaultStyle.outlineOptions.color         = Vec4(0, 0, 0, 1);
            LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 7, defaultStyle, m_rotateAngle, 1);

            // outer
            startPos.x += 200;
            defaultStyle.outlineOptions.color.start = Vec4(1, 0, 0, 1);
            defaultStyle.outlineOptions.color.end   = Vec4(0, 0, 1, 1);
            defaultStyle.color.gradientType         = GradientType::Radial;
            defaultStyle.isFilled                   = false;
            LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 7, defaultStyle, m_rotateAngle, 1);

            // Non filled inner
            startPos.x += 200;
            defaultStyle.outlineOptions.color         = Vec4(0, 0.5f, 0, 1);
            defaultStyle.outlineOptions.drawDirection = OutlineDrawDirection::Inwards;
            defaultStyle.isFilled                     = false;
            defaultStyle.color.gradientType           = GradientType::Vertical;
            defaultStyle.color                        = Vec4(1, 1, 1, 1);
            LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 7, defaultStyle, m_rotateAngle, 1);

            // Both
            startPos.x += 200;
            defaultStyle.outlineOptions.color.start   = Vec4(1, 0, 0, 1);
            defaultStyle.outlineOptions.color.end     = Vec4(0, 0, 1, 1);
            defaultStyle.outlineOptions.drawDirection = OutlineDrawDirection::Both;
            defaultStyle.isFilled                     = false;
            LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 7, defaultStyle, m_rotateAngle, 1);
        }

        void DemoScreens::ShowDemoScreen4_Lines()
        {
            const Vec2       screenSize = Vec2(static_cast<float>(LinaVG::Config.displayWidth), static_cast<float>(LinaVG::Config.displayHeight));
            Vec2             startPos   = Vec2(screenSize.x * 0.05f, screenSize.y * 0.05f);
            StyleOptions     defaultStyle;
            LineCapDirection lineCap   = LineCapDirection::None;
            LineJointType    jointType = LineJointType::Miter;

            defaultStyle.thickness = 15.0f;
            defaultStyle.color     = Vec4(1, 1, 1, 1);
            LinaVG::DrawLine(startPos, Vec2(startPos.x + 700, startPos.y), defaultStyle, lineCap, m_rotateAngle, 1);

            lineCap = LineCapDirection::Left;
            startPos.y += 30;
            defaultStyle.color.start = Vec4(1.0f, 0.1f, 0.1f, 1.0f);
            defaultStyle.color.end   = Vec4(0.0f, 0.1f, 1.0f, 1.0f);
            LinaVG::DrawLine(startPos, Vec2(startPos.x + 700, startPos.y), defaultStyle, lineCap, m_rotateAngle, 1);

            lineCap = LineCapDirection::Right;
            startPos.y += 30;
            defaultStyle.color.gradientType = GradientType::Vertical;
            LinaVG::DrawLine(startPos, Vec2(startPos.x + 700, startPos.y), defaultStyle, lineCap, m_rotateAngle, 1);

            lineCap = LineCapDirection::Both;
            startPos.y += 30;
            defaultStyle.color.gradientType = GradientType::Radial;
            LinaVG::DrawLine(startPos, Vec2(startPos.x + 700, startPos.y), defaultStyle, lineCap, m_rotateAngle, 1);

            jointType = LineJointType::Miter;
            startPos.y += 120;
            defaultStyle.color                    = Vec4(0.5f, 0.75f, 0.33f, 1.0f);
            defaultStyle.color.gradientType       = GradientType::Horizontal;
            defaultStyle.outlineOptions.thickness = 2.0f;
            defaultStyle.outlineOptions.color     = Vec4(0, 0, 0, 1);
            LinaVG::DrawBezier(startPos, Vec2(startPos.x + 200, startPos.y + 200), Vec2(startPos.x + 500, startPos.y - 200), Vec2(startPos.x + 700, startPos.y), defaultStyle, lineCap, jointType, 1, 100);

            jointType = LineJointType::Miter;
            startPos.y += 120;
            defaultStyle.color.start              = Vec4(0.5f, 1.0f, 1.0f, 1.0f);
            defaultStyle.color.end                = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
            defaultStyle.outlineOptions.thickness = 0.0f;
            defaultStyle.textureHandle            = 0;
            defaultStyle.thickness.start          = 2.0f;
            defaultStyle.thickness.end            = 16.0f;
            LinaVG::DrawBezier(startPos, Vec2(startPos.x + 200, startPos.y + 200), Vec2(startPos.x + 500, startPos.y - 200), Vec2(startPos.x + 700, startPos.y), defaultStyle, lineCap, jointType, 1, 100);

            TextOptions t;

            startPos.y += 120;
            lineCap                                   = LineCapDirection::None;
            defaultStyle.outlineOptions.textureHandle = ExampleApp::Get()->GetCheckeredTexture();
            defaultStyle.textureHandle                = 0;
            defaultStyle.outlineOptions.thickness     = 7.0f;
            defaultStyle.thickness                    = 15.0f;

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
            const Vec2  screenSize = Vec2(static_cast<float>(LinaVG::Config.displayWidth), static_cast<float>(LinaVG::Config.displayHeight));
            Vec2        startPos   = Vec2(screenSize.x * 0.05f, screenSize.y * 0.05f);
            TextOptions textOpts;
            textOpts.font = m_textDemoFont;
            LinaVG::DrawTextNormal("This is a normal text.", startPos, textOpts, m_rotateAngle, 1);

            startPos.x += 350;
            textOpts.dropShadowOffset = Vec2(2, 2);
            LinaVG::DrawTextNormal("Drop shadow.", startPos, textOpts, m_rotateAngle, 1);

            startPos.x += 350;
            textOpts.color.start = Vec4(1, 0, 0, 1);
            textOpts.color.start = Vec4(0, 0, 1, 1);
            LinaVG::DrawTextNormal("Gradient color.", startPos, textOpts, m_rotateAngle, 1);

            startPos.x = screenSize.x * 0.05f;
            startPos.y += 50;
            textOpts.wrapWidth       = 100;
            textOpts.dropShadowColor = Vec4(1, 0, 0, 1);
            textOpts.color           = Vec4(1, 1, 1, 1);
            LinaVG::DrawTextNormal("This is a wrapped text with a colored drop shadow.", startPos, textOpts, m_rotateAngle, 1);

            startPos.x += 365;
            textOpts.wrapWidth          = 100;
            textOpts.alignment          = TextAlignment::Center;
            textOpts.dropShadowOffset   = Vec2(0.0f, 0.0f);
            textOpts.color.start        = Vec4(0.6f, 0.6f, 0.6f, 1);
            textOpts.color.end          = Vec4(1, 1, 1, 1);
            textOpts.color.gradientType = GradientType::Vertical;
            const Vec2 size             = LinaVG::CalculateTextSize("Center alignment and vertical gradient.", textOpts);
            startPos.x += size.x / 2.0f;
            LinaVG::DrawTextNormal("Center alignment and vertical gradient.", startPos, textOpts, m_rotateAngle, 1);

            startPos.x += 335;
            textOpts.color     = Vec4(0.8f, 0.1f, 0.1f, 1.0f);
            textOpts.alignment = TextAlignment::Right;
            const Vec2 size2   = LinaVG::CalculateTextSize("Same, but it's right alignment", textOpts);
            startPos.x += size.x;
            LinaVG::DrawTextNormal("Same, but it's right alignment", startPos, textOpts, m_rotateAngle, 1);

            startPos.x = screenSize.x * 0.05f;
            startPos.y += 370;
            textOpts.spacing   = 10;
            textOpts.wrapWidth = 0.0f;
            textOpts.alignment = TextAlignment::Left;
            textOpts.color     = Vec4(1, 1, 1, 1);
            LinaVG::DrawTextNormal("And this is a normal text with higher spacing.", startPos, textOpts, m_rotateAngle, 1);

            startPos.y += 70;
            startPos.x                       = screenSize.x * 0.05f;
            const float    beforeSDFStartPos = startPos.y;
            SDFTextOptions sdfOpts;
            sdfOpts.font         = m_textDemoSDFFont;
            sdfOpts.sdfThickness = 0.55f;
            LinaVG::DrawTextSDF("An SDF text.", startPos, sdfOpts, m_rotateAngle, 1);

            startPos.y += 50;
            sdfOpts.sdfThickness = 0.6f;
            sdfOpts.color.start  = Vec4(1, 0, 0, 1);
            sdfOpts.color.end    = Vec4(0, 0, 1, 1);
            LinaVG::DrawTextSDF("Thicker SDF text", startPos, sdfOpts, m_rotateAngle, 1);

            startPos.y += 50;
            sdfOpts.sdfThickness = 0.7f;
            sdfOpts.sdfSoftness  = 2.0f;
            sdfOpts.color        = Vec4(0.1f, 0.8f, 0.1f, 1.0f);
            LinaVG::DrawTextSDF("Smoother text", startPos, sdfOpts, m_rotateAngle, 1);

            startPos.y += 50;
            sdfOpts.color               = Vec4(1, 1, 1, 1);
            sdfOpts.sdfThickness        = 0.6f;
            sdfOpts.sdfSoftness         = 0.5f;
            sdfOpts.sdfOutlineThickness = 0.1f;
            sdfOpts.sdfOutlineColor     = Vec4(0, 0, 0, 1);
            LinaVG::DrawTextSDF("Outlined SDF text", startPos, sdfOpts, m_rotateAngle, 1);

            startPos.y += 50;
            sdfOpts.sdfThickness        = 0.8f;
            sdfOpts.sdfSoftness         = 0.5f;
            sdfOpts.sdfOutlineThickness = 0.3f;
            sdfOpts.sdfOutlineColor     = Vec4(0, 0, 0, 1);
            LinaVG::DrawTextSDF("Thicker outline.", startPos, sdfOpts, m_rotateAngle, 1);

            startPos.y += 50;
            sdfOpts.sdfDropShadowThickness = 0.6f;
            sdfOpts.dropShadowOffset       = Vec2(5, 5);
            sdfOpts.sdfOutlineThickness    = 0.0f;
            sdfOpts.sdfThickness           = 0.6f;
            LinaVG::DrawTextSDF("Drop shadow.", startPos, sdfOpts, m_rotateAngle, 1);

            startPos.y = beforeSDFStartPos;
            startPos.x += 930;
            sdfOpts.sdfDropShadowThickness = 0.0f;
            sdfOpts.sdfOutlineThickness    = 0.0f;
            sdfOpts.sdfThickness           = 0.6f;
            sdfOpts.wrapWidth              = 450;
            sdfOpts.newLineSpacing         = 10.0f;
            sdfOpts.alignment              = TextAlignment::Right;
            LinaVG::DrawTextSDF("This is an SDF, wrapped and right aligned text, with higher line spacing.", startPos, sdfOpts, m_rotateAngle, 1);
        }

        void DemoScreens::ShowDemoScreen6_DrawOrder()
        {
            const Vec2   screenSize = Vec2(static_cast<float>(LinaVG::Config.displayWidth), static_cast<float>(LinaVG::Config.displayHeight));
            Vec2         startPos   = Vec2(screenSize.x * 0.05f, screenSize.y * 0.05f);
            StyleOptions opts;
            opts.color                    = Vec4(0, 0, 0, 1);
            opts.isFilled                 = true;
            opts.outlineOptions.thickness = 1.0f;
            opts.outlineOptions.color     = Vec4(0, 0, 0, 1);

            TextOptions textOpts;
            textOpts.textScale = 1.5f;
            textOpts.font      = m_defaultFont;

            const Vec4 minCol = Vec4(0.7f, 0.1f, 0.1f, 1.0f);
            const Vec4 maxCol = Vec4(0.1f, 0.1f, 0.8f, 1.0f);
            Vec4       col    = minCol;
            for (int i = 1; i < 50; i += 2)
            {
                col        = LinaVG::Math::Lerp(minCol, maxCol, static_cast<float>(i) / 50.0f);
                opts.color = col;
                LinaVG::DrawRect(startPos, Vec2(startPos.x + 120, startPos.y + 120), opts, 0.0f, i);

                std::string orderStr = std::to_string(i / 2);
                LinaVG::DrawTextNormal(orderStr, Vec2(startPos.x + 5, startPos.y + 25), textOpts, 0.0f, i + 1);

                startPos.x += 20;
                startPos.y += 20;
            }
        }

        void DemoScreens::ShowDemoScreen7_Clipping()
        {
            const Vec2 screenSize = Vec2(static_cast<float>(LinaVG::Config.displayWidth), static_cast<float>(LinaVG::Config.displayHeight));
            Vec2       startPos   = Vec2(screenSize.x * 0.5f, screenSize.y * 0.5f);
            const Vec2 size       = Vec2(500, 500);

            StyleOptions opts;
            opts.isFilled = true;

            const Vec2 min = Vec2(startPos.x - size.x / 2.0f, startPos.y - size.y / 2.0f);
            const Vec2 max = Vec2(startPos.x + size.x / 2.0f, startPos.y + size.y / 2.0f);

            if (m_clippingEnabled)
            {
                Config.clipPosX  = static_cast<BackendHandle>(min.x);
                Config.clipPosY  = static_cast<BackendHandle>(min.y);
                Config.clipSizeX = static_cast<BackendHandle>(size.x);
                Config.clipSizeY = static_cast<BackendHandle>(size.y);
            }

            // Main rect.
            opts.color = Vec4(0, 0, 0, 1);
            LinaVG::DrawRect(min, max, opts, 0.0f, 1);

            // Clipped rect.
            opts.color = Vec4(0.8f, 0.1f, 0.2f, 1.0f);
            LinaVG::DrawRect(Vec2(min.x - 100, min.y - 100), Vec2(min.x + 100, min.y + 100), opts, 0.0f, 2);

            // Clipped circle.
            LinaVG::DrawCircle(max, 75, opts, 36, 0.0f, 0.0f, 360.0f, 2);

            TextOptions textOpts;
            textOpts.font = m_defaultFont;
            LinaVG::DrawTextNormal("This text is clipped by the black rectangle.", Vec2(min.x - 50, min.y + 250), textOpts, 0.0f, 2);

            Config.clipPosX  = 0;
            Config.clipPosY  = 0;
            Config.clipSizeX = 0;
            Config.clipSizeY = 0;
        }

        void DemoScreens::ShowDemoScreen8_Animated()
        {
            const Vec2 screenSize = Vec2(static_cast<float>(LinaVG::Config.displayWidth), static_cast<float>(LinaVG::Config.displayHeight));

            auto drawSinBezier = [](const Vec2& pos) {
                StyleOptions     defaultStyle;
                LineJointType    jointType;
                LineCapDirection lineCap = LineCapDirection::None;

                static float controlPos1Y = 140;
                static float controlPos2Y = -140;

                controlPos1Y = std::sin(ExampleApp::Get()->GetElapsed() * 2) * 200;
                controlPos2Y = std::cos(ExampleApp::Get()->GetElapsed() * 2) * 200;

                jointType                    = LineJointType::Miter;
                defaultStyle.color.start     = Vec4(0.5f, 1.0f, 1.0f, 1.0f);
                defaultStyle.color.end       = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
                defaultStyle.textureHandle   = 0;
                defaultStyle.thickness.start = 2.0f;
                defaultStyle.thickness.end   = 16.0f;
                LinaVG::DrawBezier(pos, Vec2(pos.x + 200, pos.y + controlPos1Y), Vec2(pos.x + 400, pos.y + controlPos2Y), Vec2(pos.x + 600, pos.y), defaultStyle, lineCap, jointType, 1, 100);
            };

            auto drawLoadingBar1 = [&](const Vec2& pos) {
                StyleOptions background;
                background.outlineOptions.color     = Vec4(0, 0, 0, 1);
                background.outlineOptions.thickness = 0.5f;
                background.rounding                 = 0.2f;
                background.color                    = Vec4(0.2f, 0.2f, 0.2f, 0.2f);
                LinaVG::DrawRect(pos, Vec2(pos.x + 600, pos.y + 25), background, 0.0f, 1);

                StyleOptions fill;
                fill.color = Vec4(0.6f, 0.2f, 0.35f, 1.0f);

                static float fillX = 0.0f;

                if (fillX < 595)
                    fillX += ExampleApp::Get()->GetFrameTime() * 80;
                else
                    fillX = 0.0f;
                LinaVG::DrawRect(Vec2(pos.x + 1, pos.y + 1), Vec2(pos.x + fillX, pos.y + 24), fill, 0.0f, 2);

                TextOptions textOpts;
                textOpts.font          = m_defaultFont;
                std::string loadingStr = "Loading " + std::to_string(fillX / 600.0f);
                const Vec2  txtSize    = LinaVG::CalculateTextSize(loadingStr, textOpts);

                LinaVG::DrawTextNormal(loadingStr, Vec2(pos.x + 300 - txtSize.x / 2.0f, pos.y + 12.5f - txtSize.y / 2.0f), textOpts, 0.0f, 2);
            };

            auto drawLoadingBar2 = [&](const Vec2& pos) {
                StyleOptions background;
                background.rounding = 0.8f;
                background.color    = Vec4(0.1f, 0.1f, 0.1f, 0.8f);
                LinaVG::DrawRect(pos, Vec2(pos.x + 600, pos.y + 20), background, 0.0f, 1);

                static float fillX = 0.0f;

                if (fillX < 595)
                    fillX += ExampleApp::Get()->GetFrameTime() * 80;
                else
                    fillX = 0.0f;

                const float  t = fillX / 600.0f;
                StyleOptions fill;
                fill.rounding    = background.rounding;
                fill.color.start = Vec4(0.8f, 0.8f, 0.2f, 1.0f);
                fill.color.end   = Vec4(0.8f * t, 0.2f, 0.2f, 1.0f);
                LinaVG::DrawRect(pos, Vec2(pos.x + fillX, pos.y + 20), fill, 0.0f, 2);

                TextOptions textOpts;
                textOpts.font          = m_defaultFont;
                std::string loadingStr = "Loading " + std::to_string(t);
                const Vec2  txtSize    = LinaVG::CalculateTextSize(loadingStr, textOpts);

                LinaVG::DrawTextNormal(loadingStr, Vec2(pos.x, pos.y + 30), textOpts, 0.0f, 2);
            };

            auto drawLoadingCircle1 = [&](const Vec2& pos) {
                StyleOptions opts;
                opts.isFilled = false;

                static float rotate1 = 0.0f;
                static float rotate2 = 0.0f;
                static float rotate3 = 0.0f;

                rotate1 += ExampleApp::Get()->GetFrameTime() * 35.0f;
                rotate2 = rotate1 * 2.5f + 90.0f;
                rotate3 = rotate2 * 3.5f + 9.0f;
                LinaVG::DrawCircle(pos, 50, opts, 60, rotate1, 180.0f, 360.0f, 1);
                LinaVG::DrawCircle(pos, 40, opts, 60, rotate2, 0.0f, 180.0f, 1);
                LinaVG::DrawCircle(pos, 30, opts, 60, rotate3, 180.0f, 360.0f, 1);

                TextOptions textOpts;
                textOpts.font = m_defaultFont;
                LinaVG::DrawTextNormal("Loading", Vec2(pos.x - 28.0f, pos.y + 80), textOpts, 0.0f, 2);
            };

            auto drawLoadingRect = [&](const Vec2& pos) {
                const Vec2 totalSize = Vec2(450, 116);

                StyleOptions opts;
                opts.color = Vec4(0.1f, 0.1f, 0.1f, 0.65f);

                LinaVG::DrawRect(pos, Vec2(pos.x + totalSize.x, pos.y + totalSize.y), opts, 0.0f, 1);

                const Vec2 smallRectSize = Vec2(45, 50);

                Vec2 usedPos = pos;
                usedPos.x += 4.5f;
                usedPos.y += 4.5f;

                static float t = 0.0f;

                t += ExampleApp::Get()->GetFrameTime() * 10;

                if (t > 18.0f)
                    t = 0.0f;

                int rectCounter = 0;
                for (int j = 0; j < 2; j++)
                {
                    for (int i = 0; i < 9; i++)
                    {
                        const float maxLimit = static_cast<float>(rectCounter + 1);
                        const float minLimit = static_cast<float>(rectCounter);

                        StyleOptions smallRect;

                        if (t > minLimit && t < maxLimit)
                            smallRect.color = Vec4(0.2f, 0.1f, 0.6f, 0.85f);
                        else
                            smallRect.color = Vec4(0.1f, 0.1f, 0.1f, 0.55f);

                        LinaVG::DrawRect(usedPos, Vec2(usedPos.x + smallRectSize.x, usedPos.y + smallRectSize.y), smallRect, 0.0f, 2);
                        usedPos.x += smallRectSize.x + 4.5f;
                        rectCounter++;
                    }
                    usedPos = pos;
                    usedPos.x += 4.5f;
                    usedPos.y += smallRectSize.y + 10.0f;
                }

                TextOptions textOpts;
                textOpts.font = m_defaultFont;
                LinaVG::DrawTextNormal("Loading", Vec2(pos.x, pos.y + 130), textOpts, 0.0f, 2);
            };

            auto drawMovingTri = [&](const Vec2& startPos) {
                static float movingX = 0.0f;

                const float sin = std::sin(ExampleApp::Get()->GetElapsed() * 0.85f) * 250.0f;
                movingX         = 250.0f + sin;

                const Vec2 top   = Vec2(startPos.x + 60 + movingX, startPos.y - 60);
                const Vec2 left  = Vec2(startPos.x + movingX, startPos.y + 60);
                const Vec2 right = Vec2(startPos.x + 120 + movingX, startPos.y + 60);

                StyleOptions style;
                style.outlineOptions.color     = Vec4(0.1f, 0.1f, 0.1f, 0.7f);
                style.outlineOptions.thickness = 2.0f;
                style.color.start              = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
                style.color.end                = Vec4(0.0f, 0.0f, 1.0f, 1.0f);

                static float triangleRotate = 0.0f;
                triangleRotate += ExampleApp::Get()->GetFrameTime() * 45.0f;
                LinaVG::DrawTriangle(top, right, left, style, triangleRotate, 1);
            };

            auto drawProperty = [&](const Vec2& pos, int index) {
                TextOptions textOpts;
                textOpts.font      = m_descFont;
                textOpts.textScale = 0.7f;
                std::string str    = "Property_" + std::to_string(index);
                LinaVG::DrawTextNormal(str, pos, textOpts, 0.0f, 3);

                StyleOptions opts;
                opts.color                    = Vec4(0.05f, 0.05f, 0.05f, 0.9f);
                opts.rounding                 = 0.2f;
                opts.outlineOptions.color     = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
                opts.outlineOptions.thickness = 0.1f;
                LinaVG::DrawRect(Vec2(pos.x + 150, pos.y), Vec2(pos.x + 380, pos.y + 22), opts, 0.0f, 3);
            };
            auto propertyWindow = [&](const Vec2& pos) {
                const Vec2   size = Vec2(400, 180);
                StyleOptions bg;
                StyleOptions title;
                bg.color    = Utility::HexToVec4(0x242424);
                title.color = Utility::HexToVec4(0x111111);
                LinaVG::DrawRect(pos, Vec2(pos.x + size.x, pos.y + size.y), bg, 0.0f, 1);
                LinaVG::DrawRect(Vec2(pos.x + 1, pos.y + 1), Vec2(pos.x + size.x - 1, pos.y + 25), title, 0.0f, 2);

                TextOptions textOpts;
                textOpts.font      = m_descFont;
                textOpts.textScale = 0.85f;
                LinaVG::DrawTextNormal("Demo Title", Vec2(pos.x + 10, pos.y + 3.5f), textOpts, 0.0f, 3);

                Vec2 usedPos = Vec2(pos.x + 10, pos.y + 35);
                drawProperty(usedPos, 0);
                usedPos.y += 30.0f;
                drawProperty(usedPos, 1);
                usedPos.y += 30.0f;
                drawProperty(usedPos, 2);
                usedPos.y += 40.0f;

                StyleOptions buttonRect;
                buttonRect.outlineOptions.thickness = 1.0f;
                buttonRect.rounding                 = 0.2f;
                buttonRect.color                    = Vec4(0.1f, 0.1f, 0.1f, 1.0f);
                buttonRect.outlineOptions.thickness = 0.3f;
                buttonRect.outlineOptions.color     = Vec4(0.3f, 0.3f, 0.4f, 1.0f);

                const Vec2 buttonSize = Vec2(200, 25);
                const Vec2 startPos   = Vec2(pos.x + size.x / 2.0f - buttonSize.x / 2.0f, usedPos.y);
                LinaVG::DrawRect(startPos, Vec2(startPos.x + buttonSize.x, startPos.y + buttonSize.y), buttonRect, 0.0f, 2);
                textOpts.textScale  = 0.7f;
                const Vec2 textSize = LinaVG::CalculateTextSize("Button", textOpts);
                const Vec2 textPos  = Vec2(startPos.x + buttonSize.x / 2.0f - textSize.x / 2.0f, startPos.y + buttonSize.y / 2.0f - textSize.y);
                LinaVG::DrawTextNormal("Button", textPos, textOpts, 0.0f, 3);
            };

            drawSinBezier(Vec2(screenSize.x * 0.05f, screenSize.y * 0.15f));
            drawLoadingBar1(Vec2(screenSize.x * 0.05f, screenSize.y * 0.35f));
            drawLoadingBar2(Vec2(screenSize.x * 0.05f, screenSize.y * 0.4f));
            drawLoadingCircle1(Vec2(screenSize.x * 0.08f, screenSize.y * 0.55f));
            drawLoadingRect(Vec2(screenSize.x * 0.15f, screenSize.y * 0.495f));
            drawMovingTri(Vec2(screenSize.x * 0.05f, screenSize.y * 0.75f));
            propertyWindow(Vec2(screenSize.x * 0.63f, screenSize.y * 0.1f));

            // Checkered rects
            static float colorLerp = 0.0f;
            colorLerp              = std::sin(ExampleApp::Get()->GetElapsed() * 2.0f) + 1.0f;
            StyleOptions opts;
            opts.textureHandle              = ExampleApp::Get()->GetCheckeredTexture();
            opts.outlineOptions.thickness   = 4.0f;
            opts.outlineOptions.color.start = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
            opts.outlineOptions.color.end   = Vec4(1.0f * (1.0f - colorLerp), 0.0f, 1.0f * colorLerp, 1.0f);
            Vec2 start                      = Vec2(screenSize.x * 0.63f, screenSize.y * 0.35f);
            LinaVG::DrawRect(start, Vec2(start.x + 150, start.y + 150), opts, 0.0f, 1.0f);

            // Rect 2
            start.x += 245;
            static float offSetX = 0.0f;
            offSetX += ExampleApp::Get()->GetFrameTime() * 0.3f;
            opts.textureUVOffset = Vec2(offSetX, 0.0f);
            LinaVG::DrawRect(start, Vec2(start.x + 150, start.y + 150), opts, 0.0f, 1.0f);

            // Pulsing ngon
            static float thickness = 0.0f;
            thickness              = std::sin(ExampleApp::Get()->GetElapsed() * 3.0f) * 2.0f + 3.0f;
            float        colT      = LinaVG::Math::Remap(thickness, 1.0f, 5.0f, 1.0f, 0.0f);
            StyleOptions ngon;
            ngon.outlineOptions.thickness = thickness;
            ngon.outlineOptions.color     = Vec4(1.0f, 1.0f * colT, 1.0f * (colT / 2.0f), 1.0f);
            ngon.color                    = LinaVG::Utility::HexToVec4(0x212738);
            start.x                       = screenSize.x * 0.77f;
            start.y += 250.0f;
            LinaVG::DrawNGon(start, 70.0f, 7, ngon, 0.0f, 1.0f);

            // SDF Text
            static float colLerp1 = 0.0f, colLerp2 = 0.0f;
            colLerp1 = std::sin(ExampleApp::Get()->GetElapsed() * 2.5f) + 1.0f;
            colLerp2 = std::sin(ExampleApp::Get()->GetElapsed() * 1.25f) + 1.0f;

            SDFTextOptions sdf;
            sdf.font = m_titleFont;
            start.y += 170.0f;
            start.x                 = screenSize.x * 0.6f;
            sdf.sdfThickness        = 0.72f;
            sdf.sdfOutlineColor     = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
            sdf.sdfOutlineThickness = 0.1f;
            sdf.textScale           = 2.0f;
            sdf.spacing             = 9.0f;
            sdf.color.start         = Vec4(0.0f, 0.8f * colLerp1, 1.0f * colLerp2, 1.0f);
            sdf.color.end           = Vec4(0.1f, 0.1f, 0.85f * colLerp2, 1.0f);
            LinaVG::DrawTextSDF("SDF TEXT", start, sdf, 0.0f, 1.0f);
        }

        void DemoScreens::ShowDemoScreen9_Final()
        {
            const Vec2 screenSize = Vec2(static_cast<float>(LinaVG::Config.displayWidth), static_cast<float>(LinaVG::Config.displayHeight));

            // Sky
            StyleOptions sky;
            const Vec2   skyEnd    = Vec2(screenSize.x, screenSize.y * 0.65f);
            sky.color.start        = Utility::HexToVec4(0x41295a);
            sky.color.end          = Vec4(0.44f, 0.1f, 0.16f, 1.0f);
            sky.color.gradientType = GradientType::Vertical;
            LinaVG::DrawRect(Vec2(0.0f, 0.0f), skyEnd, sky, 0.0f, 1);

            // Sun
            static float sunRotation = 0.0f;
            sunRotation += ExampleApp::Get()->GetFrameTime() * 5.0f;

            StyleOptions sun;
            const Vec2   sunCenter = Vec2(screenSize.x / 2.0f, screenSize.y * 0.4f);
            sun.color.start        = Utility::HexToVec4(0xfeb47b);
            sun.color.end          = Vec4(0.84f, 0.35f, 0.26f, 1.0f);
            sun.color.gradientType = GradientType::RadialCorner;
            sun.color.radialSize   = 0.9f;
            LinaVG::DrawCircle(sunCenter, 200, sun, 72, sunRotation, 0.0f, 360.0f, 3);

            // Horizon line
            StyleOptions horizon;
            horizon.thickness = 4.0f;
            horizon.color     = Vec4(0.08f, 0.08f, 0.08f, 1.0f);
            LinaVG::DrawLine(Vec2(0.0f, skyEnd.y), Vec2(screenSize.x, skyEnd.y), horizon, LineCapDirection::None, 0.0f, 30);

            // Ground plane
            StyleOptions groundPlaneStyle;
            const Vec2   gridStart              = Vec2(0.0f, skyEnd.y);
            groundPlaneStyle.isFilled           = true;
            groundPlaneStyle.color.gradientType = GradientType::Vertical;
            groundPlaneStyle.color.start        = Vec4(0.122f, 0.112f, 0.28f, 1.0f);
            groundPlaneStyle.color.end          = Vec4(0.05f, 0.05f, 0.12f, 1.0f);
            LinaVG::DrawRect(gridStart, Vec2(screenSize.x, screenSize.y), groundPlaneStyle, 0.0f, 1);

            // Ground plane grid Y
            Vec2        currentGrid    = gridStart;
            const int   lineCountY     = 8;
            const float gridLeftY      = screenSize.y - gridStart.y;
            const float gridYIncrement = gridLeftY / static_cast<float>(lineCountY);
            for (int i = 0; i < lineCountY; i++)
            {
                StyleOptions gridLine;
                gridLine.color     = Vec4(1.0f, 1.0f, 1.0f, 0.35f);
                gridLine.thickness = 2.0f;
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

                gridLine.color     = Vec4(1.0f, 1.0f, 1.0f, a);
                gridLine.thickness = 2.0f;

                const float skew = Math::Remap(static_cast<float>(i), 0.0f, tLimitMax, -skewMax, skewMax);
                LinaVG::DrawLine(currentGrid, Vec2(currentGrid.x + skew, screenSize.y), gridLine, LineCapDirection::None, 0.0f, 2);
                currentGrid.x += gridXIncrement;
            }

            StyleOptions bgMounts;
            bgMounts.color.start = Vec4(0, 0, 0, 1);
            bgMounts.color.end   = Vec4(0.04f, 0.04f, 0.12f, 1.0f);
            Vec2 triLeft         = Vec2(-screenSize.x * 0.1f, gridStart.y);
            Vec2 triRight        = Vec2(screenSize.x * 0.24f, gridStart.y);
            Vec2 triTop          = Vec2(screenSize.x * 0.12f, gridStart.y - screenSize.y * 0.18f);
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
            fgMounts.color.start = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
            fgMounts.color.end   = Vec4(0.1f, 0.1f, 0.2f, 1.0f);
            triLeft              = Vec2(-screenSize.x * 0.1f, gridStart.y);
            triRight             = Vec2(screenSize.x * 0.24f, gridStart.y);
            triTop               = Vec2(screenSize.x * 0.12f, gridStart.y - screenSize.y * 0.1f);
            LinaVG::DrawTriangle(triTop, triRight, triLeft, fgMounts, 0.0f, 6);

            triLeft.x += screenSize.x * 0.2f;
            triTop.x += screenSize.x * 0.1f;
            triRight.x += screenSize.x * 0.05f;
            LinaVG::DrawTriangle(triTop, triRight, triLeft, fgMounts, 0.0f, 7);

            fgMounts.color.start = Vec4(0.05f, 0.05f, 0.05f, 1);
            fgMounts.color.end   = Vec4(0.1f, 0.01f, 0.13f, 1.0f);
            triLeft              = triRight;
            triRight.x += screenSize.x * 0.05f;
            LinaVG::DrawTriangle(triTop, triRight, triLeft, fgMounts, 0.0f, 8);

            // Right
            fgMounts.color.start = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
            fgMounts.color.end   = Vec4(0.1f, 0.1f, 0.2f, 1.0f);
            triLeft              = Vec2(screenSize.x - screenSize.x * 0.1f, gridStart.y);
            triRight             = Vec2(screenSize.x, gridStart.y);
            triTop               = Vec2(screenSize.x - screenSize.x * 0.12f, gridStart.y - screenSize.y * 0.1f);
            LinaVG::DrawTriangle(triTop, triRight, triLeft, fgMounts, 0.0f, 7);

            fgMounts.color.start = Vec4(0, 0, 0, 1);
            fgMounts.color.end   = Vec4(0.09f, 0.04f, 0.12f, 1.0f);
            triRight             = triLeft;
            triLeft.x -= screenSize.x * 0.1f;
            LinaVG::DrawTriangle(triTop, triRight, triLeft, fgMounts, 0.0f, 7);

            // Stars
            const Vec2 starsMax = Vec2(skyEnd.x, skyEnd.y * 0.8f);

            for (int i = 0; i < stars.size(); i++)
            {
                StyleOptions star;
                star.color.start        = stars[i].startCol;
                star.color.end          = stars[i].endCol;
                star.color.radialSize   = stars[i].radialSize;
                star.color.gradientType = GradientType::Radial;

                const float sin      = std::sin(ExampleApp::Get()->GetElapsed() * 2.0f);
                const float haloSize = stars[i].haloRadius + sin + static_cast<float>((std::rand() % 100)) / 100.0f;
                LinaVG::DrawCircle(stars[i].pos, haloSize, star, 36, 0.0f, 0.0f, 360.0f, 2);

                // star.m_color = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
                // LinaVG::DrawCircle(starPos, starRadius, star, 36, 0.0f, 0.0f, 360.0f, 4);
            }
        }

        void DemoScreens::PreEndFrame()
        {
            if (m_rotate)
                m_rotateAngle += ExampleApp::Get()->GetFrameTime() * 20;

            m_drawCount     = Config.debugCurrentDrawCalls;
            m_triangleCount = Config.debugCurrentTriangleCount;
            m_vertexCount   = Config.debugCurrentVertexCount;
        }
    } // namespace Examples
} // namespace LinaVG