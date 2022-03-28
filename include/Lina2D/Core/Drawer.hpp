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
    struct Line
    {
        Vertex m_vertices[4]; // 0 - start up, 1 - end up, 2 - end down, 3 - start down - cw order
        Vec2   m_center;
    };

    void DrawArc(const Vec2& p1, const Vec2& p2, const Vec4Grad& color, float radius = 0.0f, ThicknessGrad thickness = ThicknessGrad(), int segments = 36, bool flip = false);
    void DrawBezier(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec4Grad& color, ThicknessGrad thickness = ThicknessGrad(), int segments = 50);
    void DrawLine(const Vec2& p1, const Vec2& p2, const Vec4Grad& col, ThicknessGrad thickness = ThicknessGrad());
    void DrawPoint(const Vec2& p1, const Vec4& col);

    void DrawTriangleFilled(const Vec2& left, const Vec2& right, const Vec2& top, StyleOptions style = StyleOptions(), float rotateAngle = 0.0f);
    void DrawRectFilled(const Vec2& min, const Vec2& max, StyleOptions style = StyleOptions(), float rotateAngle = 0.0f);
    void DrawNGonFilled(const Vec2& start, float radius, int n, StyleOptions style = StyleOptions());

    void DrawCircleFilled(const Vec2& center, float radius, int segments = 36, StyleOptions style = StyleOptions());
    void DrawSemiCircleFilled(const Vec2& start, const Vec2& end, bool flip, int segments = 18, StyleOptions style = StyleOptions());
    void DrawSemiCircleFilled(const Vec2& center, bool flip = false, int segments = 18, StyleOptions style = StyleOptions());
    void ConvexFillVertices(int startIndex, int endIndex, Array<Index>& indices);

    void GenerateLine(const Vec2& p1, const Vec2& p2, const Vec4Grad& col, ThicknessGrad thickness);

    namespace Internal
    {
        // No rounding, vertical or horizontal gradient
        void FillRectData_NoRound(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& colorTL, const Vec4& colorTR, const Vec4& colorBR, const Vec4& colorBL);

        // No rounding, single color
        void FillRectData_NoRound(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& color);

        // No rounding, radial gradient
        void FillRectData_NoRoundCenter(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& startcolor, const Vec4& endColor);

        // Rounding
        void FillRectData_Round(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& col, float rounding);

        // Fill rect impl.
        void FillRectData(Vertex* vertArray, bool hasCenter, const Vec2& min, const Vec2& max);

        // No rounding, vertical or horizontal gradient
        void FillTri_NoRound(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec4& colorLeft, const Vec4& colorRight, const Vec4& colorTop);

        // No rounding, single color
        void FillTri_NoRound(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec4& color);

        // No rounding, radial gradient
        void FillTri_NoRoundCenter(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec4& startcolor, const Vec4& endColor);

        // Rounding
        void FillTri_Round(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec4& col, float rounding);

        // Fill rect impl.
        void FillTriData(Vertex* vertArray, bool hasCenter, const Vec2& p1, const Vec2& p2, const Vec2& p3);

        void RotateVertices(Array<Vertex>& vertices, const Vec2& center, int startIndex, int endIndex, float angle);
    }; // namespace Internal

} // namespace Lina2D

#endif
