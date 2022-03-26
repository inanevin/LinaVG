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
Class: Draw



Timestamp: 3/24/2022 10:57:37 PM
*/

#pragma once

#ifndef Lina2DDrawer_HPP
#define Lina2DDrawer_HPP

// Headers here.
#include "Common.hpp"

namespace Lina2D
{
    class Drawer
    {
    public:
        Drawer(){};

        ~Drawer() = default;

        void SetLineJointType(JointType type);
        void SetFeatheringType(FeatherType type);
        void SetFeatheringDistance(float distance);
        void DrawArc(const Vec2& p1, const Vec2& p2, const Vec4Grad& color, float radius = 0.0f, ThicknessGrad thickness = ThicknessGrad(), int segments = 36, bool flip = false);
        void DrawBezier(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec4Grad& color, ThicknessGrad thickness = ThicknessGrad(), int segments = 50);
        void DrawLine(const Vec2& p1, const Vec2& p2, const Vec4Grad& col, ThicknessGrad thickness = ThicknessGrad());

    private:
        friend class Renderer;

        void CheckStartLineJoint();
        void ResetLineJointData();

        Vertex* m_lastLineVtx1   = nullptr;
        Vertex* m_lastLineVtx2   = nullptr;
        Vec2    m_lastLineDir    = Vec2(0.0f, 0.0f);
        bool       m_isJoiningLines = false;
    };

} // namespace Lina2D

#endif
