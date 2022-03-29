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

    /// <summary>
    /// Your points for the triangle must follow the given parameter order -- left, right and top edges.
    /// If you are drawing odd triangles, e.g. maxAngle > 90, rounding of the triangle might perform poorly on sharp edges.
    /// </summary>
    /// <param name="left"> Bottom left corner. </param>
    /// <param name="right"> Bottom right corner. </param>
    /// <param name="top"> Top corner. </param>
    /// <param name="style"> Style options to apply.</param>
    /// <param name="rotateAngle"> Rotate angles around the center of the triangle. </param>
    void DrawTriangleFilled(const Vec2& left, const Vec2& right, const Vec2& top, StyleOptions& style, float rotateAngle = 0.0f);
    void DrawRectFilled(const Vec2& min, const Vec2& max, StyleOptions& style, float rotateAngle = 0.0f);

    void ConvexFillVertices(int startIndex, int endIndex, Array<Index>& indices);

    void GenerateLine(const Vec2& p1, const Vec2& p2, const Vec4Grad& col, ThicknessGrad thickness);

    /// <summary>
    /// Draws an arc always clock-wise from p1 to p2.
    /// </summary>
    /// <param name="points"></param>
    /// <param name="p1"></param>
    /// <param name="p2"></param>
    /// <param name="radius"></param>
    /// <param name="segments"></param>
    /// <param name="flip"></param>
    void GetArcPoints(Array<Vec2>& points, const Vec2& p1, const Vec2& p2, Vec2 directionHintPoint = Vec2(-1.0f, -1.0f), float radius = 0.0f, float segments = 36, bool flip = false, float angleOffset = 0.0f);

    /// <summary>
    /// Rotates all the vertices in the given range.
    /// </summary>
    /// <param name="vertices"> Vertex array. </param>
    /// <param name="center"> Center of rotation. </param>
    /// <param name="startIndex"> First vertex to rotate. </param>
    /// <param name="endIndex"> Last vertex to rotate. </param>
    /// <param name="angle"> Rotation angle. </param>
    void RotateVertices(Array<Vertex>& vertices, const Vec2& center, int startIndex, int endIndex, float angle);

    namespace Internal
    {
        // No rounding, vertical or horizontal gradient
        void FillRect_NoRound_VerHorGra(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& colorTL, const Vec4& colorTR, const Vec4& colorBR, const Vec4& colorBL);

        // No rounding, single color
        void FillRect_NoRound_SC(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& color);

        // No rounding, radial gradient
        void FillRect_NoRound_RadialGra(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& startcolor, const Vec4& endColor);

        // Rounding
        void FillRect_Round(Array<Vertex>& vertices, Array<Index>& indices, Array<int>& roundedCorners, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& col, float rounding);

        // Fill rect impl.
        void FillRectData(Vertex* vertArray, bool hasCenter, const Vec2& min, const Vec2& max);

        // No rounding, vertical or horizontal gradient
        void FillTri_NoRound_VerHorGra(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec4& colorLeft, const Vec4& colorRight, const Vec4& colorTop);

        // No rounding, single color
        void FillTri_NoRound_SC(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec4& color);

        // No rounding, radial gradient
        void FillTri_NoRound_RadialGra(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec4& startColor, const Vec4& endColor);

        // Rounding
        void FillTri_Round(Array<Vertex>& vertices, Array<Index>& indices, Array<int>& onlyRoundCorners, float rotateAngle, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec4& col, float rounding);

        // Fill rect impl.
        void FillTriData(Vertex* vertArray, bool hasCenter, const Vec2& p1, const Vec2& p2, const Vec2& p3);


        float GetAngleIncrease(float rounding);
    }; // namespace Internal

} // namespace Lina2D

#endif
