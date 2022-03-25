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

#include "Core/LinaGUI.hpp"
#include "Core/LinaGUIGLBackend.hpp"
#include <math.h>

namespace Lina
{
    LGDrawData GUI::g_drawData;
    LGOptions  GUI::g_options;

    void       GUI::Initialize(const LGOptions& initOptions)
    {
        g_options = initOptions;
        Backend::InitializeBackend();
    }

    void GUI::Start()
    {
        Backend::StartBackend();
    }

    void GUI::Render()
    {
        Backend::RenderBackend();
    }

    void GUI::End()
    {
        Backend::EndBackend();
        g_drawData.m_indexCounter = 0;
        g_drawData.m_indexBuffer.clear();
        g_drawData.m_vertexBuffer.clear();
    }

    float GUI::Mag(const LGVec2& v)
    {
        return sqrt(v.x * v.x + v.y * v.y);
    }

    LGVec2 GUI::Normalized(const LGVec2& v)
    {
        const float mag = Mag(v);
        return LGVec2(v.x / mag, v.y / mag);
    }

    LGVec2 GUI::Rotate90(const LGVec2& v, bool cw)
    {
        if (cw)
            return LGVec2(v.y, -v.x);
        else
            return LGVec2(-v.y, v.x);
    }

} // namespace Lina