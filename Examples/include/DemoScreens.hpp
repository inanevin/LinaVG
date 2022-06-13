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

#ifndef DEMOS_HPP
#define DEMOS_HPP

#include "Core/Common.hpp"
#include <vector>

namespace LinaVG
{
    namespace Examples
    {
        class DemoScreens
        {
        public:
            void Initialize();

            void ShowBackground();
            void ShowDemoScreen1_Shapes();
            void ShowDemoScreen2_Colors();
            void ShowDemoScreen3_Outlines();
            void ShowDemoScreen4_Lines();
            void ShowDemoScreen5_Texts();
            void ShowDemoScreen6_DrawOrder();
            void ShowDemoScreen7_Clipping();
            void ShowDemoScreen8_Animated();
            void ShowDemoScreen9_Final();
            void PreEndFrame();

            bool                     m_clippingEnabled = true;
            bool                     m_statsWindowOn   = true;
            bool                     m_rotate          = false;
            float                    m_rotateAngle     = 0.0f;
            FontHandle               m_defaultFont     = 0;
            FontHandle               m_titleFont       = 0;
            FontHandle               m_descFont        = 0;
            FontHandle               m_textDemoFont    = 0;
            FontHandle               m_textDemoSDFFont = 0;
            int                      m_drawCount       = 0;
            int                      m_triangleCount   = 0;
            int                      m_vertexCount     = 0;
            std::vector<std::string> m_screenTitles    = {"SHAPES", "COLORS", "OUTLINES", "LINES", "TEXTS", "Z-ORDER", "CLIPPING", "ANIMATED", "FINAL"};
            std::vector<std::string> m_screenDescriptions;
        };
    } // namespace Examples
} // namespace LinaVG
#endif