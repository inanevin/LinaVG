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

#pragma once

#include "Utility/Utility.hpp"

namespace LinaVG
{
    namespace Utility
    {
        int GetTextCharacterSize(const char* text)
        {
            int size = 0;
            while (text[size] != '\0')
                size++;
            return size;
        }

        Vec4 HexToVec4(int hex)
        {
            Vec4 rgbColor;
            rgbColor.x = ((hex >> 16) & 0xFF) / 255.0f;
            rgbColor.y = ((hex >> 8) & 0xFF) / 255.0f;
            rgbColor.z = ((hex)&0xFF) / 255.0f;
            rgbColor.w = 1.0f;
            return rgbColor;
        }
    } // namespace Utility
} // namespace LinaVG
