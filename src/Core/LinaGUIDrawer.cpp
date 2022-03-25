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

#include "Core/LinaGUIDrawer.hpp"
#include <iostream>
#include <stdio.h>

namespace Lina
{
    void Drawer::DrawLine(const LGVec2& p1, const LGVec2& p2, const LGVec4& color, float thickness)
    {
        const float  halfThickness = thickness / 2.0f;
        const LGVec2 dir           = GUI::Normalized(LGVec2(p2.x - p1.x, p2.y - p1.y));
        const LGVec2 up            = GUI::Rotate90(dir, false);
        const LGVec2 down          = GUI::Rotate90(dir, true);
        LGVertex     p1U, p1D, p2U, p2D;
        p1U.m_pos = LGVec2(p1.x + up.x * halfThickness, p1.y + up.y * halfThickness);
        p1D.m_pos = LGVec2(p1.x + down.x * halfThickness, p1.y + down.y * halfThickness);
        p2U.m_pos = LGVec2(p2.x + up.x * halfThickness, p2.y + up.y * halfThickness);
        p2D.m_pos = LGVec2(p2.x + down.x * halfThickness, p2.y + down.y * halfThickness);
        std::cout << "P1 U: " << p1U.m_pos.x << " " << p1U.m_pos.y << " P1 D: " << p1D.m_pos.x << " " << p1D.m_pos.y << " P2 U:" << p2U.m_pos.x << " " << p2U.m_pos.y << " P2 D: " << p2D.m_pos.x << " " << p2D.m_pos.y << std::endl;
        // p1U.m_pos = LGVec2(-0.2f, 0.6f);
        // p1D.m_pos = LGVec2(0.2f, 0.6f);
        // p2U.m_pos = LGVec2(-0.2f, 0.4f);
        // p2D.m_pos = LGVec2(0.2f, 0.4f);
        p1U.m_col = color;
        p1D.m_col = color;
        p2U.m_col = color;
        p2D.m_col = color;
        GUI::g_drawData.m_vertexBuffer.insert(GUI::g_drawData.m_vertexBuffer.end(), {p1U, p2U, p2D, p1D});

        const LGIndex curr = GUI::g_drawData.m_indexCounter;
        GUI::g_drawData.m_indexBuffer.insert(GUI::g_drawData.m_indexBuffer.end(), {curr, curr + 1, curr + 3, curr + 1, curr + 2, curr + 3});
        
    }

} // namespace Lina::GUI