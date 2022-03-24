/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

/*
Class: Application

Central application class, responsible for managing all the engines like input, physics, rendering etc.
as well as defining the game loop.

Timestamp: 12/29/2018 10:43:46 PM
*/

#pragma once
#ifndef LinaGUI_HPP
#define LinaGUI_HPP

#include <vector>


#ifdef LINAGUI_CUSTOM_LINE_SHADER
    #define DONTINIT_LINE_SHADER
#endif

namespace Lina::GUI
{
    struct LGVec4
    {
        LGVec4(){};
        LGVec4(float x, float y, float z, float w)
            : x(x), y(y), z(z), w(w){};
        LGVec4(const LGVec4& v)
        {
            this->x = v.x;
            this->y = v.y;
            this->z = v.z;
        }

        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float w = 0.0f;
    };

    struct LGVec2
    {
        LGVec2(){};
        LGVec2(float x, float y)
            : x(x), y(y){};
        LGVec2(const LGVec2& v)
        {
            x = v.x;
            y = v.y;
        }

        float x = 0.0f;
        float y = 0.0f;
    };

    struct LGVertex
    {
        LGVec2 m_pos;
        LGVec2 m_uv;
        LGVec4 m_col;
    };

    typedef unsigned int LGIndex;

    struct LGDrawData
    {
        std::vector<LGVertex> m_vertexBuffer;
        std::vector<LGIndex>  m_indexBuffer;
        LGIndex               m_indexCounter = 0;
    };

    extern LGDrawData g_drawData;

    struct LGInitOptions
    {
        char* m_customLineFragShader = nullptr;
        char* m_customLineVertexShader = nullptr;
    };

    // ************************************************
    // *                   CONTROLS                   *
    // ************************************************
    static void Initialize(const LGInitOptions& initOptions);
    static void Start();
    static void Render();
    static void End();

    // ************************************************
    // *                   MATH                       *
    // ************************************************

    static float  Mag(const LGVec2& v);
    static LGVec2 Normalized(const LGVec2& v);
    static LGVec2 Rotate90(const LGVec2& v, bool cw = true);

}; // namespace Lina::GUI

#endif
