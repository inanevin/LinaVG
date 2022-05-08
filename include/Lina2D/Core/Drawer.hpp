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
        Vec2 m_points[4]; // Clock-wise, starting from top-left
    };
    
    struct LineTriangle
    {
        int m_indices[3];
    };

    struct RectOverrideData
    {
        bool overrideRectPositions = false;
        Vec2 m_p1                  = Vec2(0,0);
        Vec2 m_p2                  = Vec2(0,0);
        Vec2 m_p3                  = Vec2(0,0);
        Vec2 m_p4                  = Vec2(0,0);
    };

    enum class LineJointType
    {
        Miter,
        Bevel,
        BevelRound,
    };

    enum class LineCapDirection
    {
        None,
        Left,
        Right,
        Both
    };

    extern RectOverrideData g_rectOverrideData;

    void DrawBezier(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec4Grad& color, ThicknessGrad thickness = ThicknessGrad(), int segments = 50);

    /// <summary>
    /// Draws a single point with the given color & position.
    /// </summary>
    void DrawPoint(const Vec2& p1, const Vec4& col);

    void DrawLine(const Vec2& p1, const Vec2& p2, StyleOptions& style, LineCapDirection cap = LineCapDirection::None, float rotateAngle = 0.0f, int drawOrder = 0);

    void DrawLines(Vec2* points, int count, StyleOptions& style, LineCapDirection cap = LineCapDirection::None, LineJointType jointType = LineJointType::Miter, int drawOrder = 0, bool uniformUVs = true);

    /// <summary>
    /// Your points for the triangle must follow the given parameter order -- left, right and top edges.
    /// If you are drawing odd triangles, e.g. maxAngle > 90, rounding of the triangle might perform poorly on sharp edges.
    /// </summary>
    /// <param name="left"> Bottom left corner. </param>
    /// <param name="right"> Bottom right corner. </param>
    /// <param name="top"> Top corner. </param>
    /// <param name="style"> Style options to apply.</param>
    /// <param name="rotateAngle"> Rotate angles around the center of the triangle. </param>
    void DrawTriangle(const Vec2& top, const Vec2& right, const Vec2& left, StyleOptions& style, float rotateAngle = 0.0f, int drawOrder = 0);

    /// <summary>
    /// Draws a filled rectangle between min & max with the given style options & rotation angle.
    /// </summary>
    void DrawRect(const Vec2& min, const Vec2& max, StyleOptions& style, float rotateAngle = 0.0f, int drawOrder = 0);

    /// <summary>
    /// Draws a convex polygon with N corners. !Rounding options do not apply to NGons!
    /// </summary>
    void DrawNGon(const Vec2& center, float radius, int n, StyleOptions& style, float rotateAngle = 0.0f, int drawOrder = 0);

    /// <summary>
    /// Draws the given set of points. !Rounding options do not apply!
    /// If you are not going to fill the convex shape (styling options -> m_isFilled), then prefer using DrawLines instead of this so that you can use proper line joints.
    /// </summary>
    void DrawConvex(Vec2* points, int size, StyleOptions& style, float rotateAngle = 0.0f, int drawOrder = 0);

    /// <summary>
    /// Draws a filled circle with the given radius & center.
    /// You can change the start and end angles to create a filled semi-circle or a filled arc.
    /// It's recommended to send angles always in the range of 0.0f - 360.0f. This method will try to auto-convert if not.
    /// Segments are clamped to 6 - 360 (360 being perfect circle, 8 being 6-gon)
    /// Higher the segment cause more weight on the performance. 18-54 is a good range for balance.
    /// Always recommended to use segments that leave no remainder when 360 is divided by it.
    /// !Rounding options have no effect.!
    /// </summary>
    void DrawCircle(const Vec2& center, float radius, StyleOptions& style, int segments = 36, float rotateAngle = 0.0f, float startAngle = 0.0f, float endAngle = 360.0f, int drawOrder = 0);

    /// <summary>
    /// Triangulates & fills the index array given a start and end vertex index.
    /// </summary>
    void ConvexFillVertices(int startIndex, int endIndex, Array<Index>& indices, bool skipLastTriangle = false);
    
    /// <summary>
    /// Fills convex shapes without the assumption of a center vertex. Used for filling outer areas of non-filled shapes.
    /// <param name="startIndex"> First vertex - start of the border.</param>
    /// <param name="endIndex"> Last vertex - end of the border.</param>
    /// </summary>
    void ConvexExtrudeVertices(DrawBuffer* buf, const Vec2& center, int startIndex, int endIndex, float thickness, bool skipEndClosing = false);

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
    void RotatePoints(Vec2* points, int size, const Vec2& center, float angle);

    namespace Internal
    {
        // No rounding, vertical or horizontal gradient
        void FillRect_NoRound_VerHorGra(DrawBuffer* buf, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& colorTL, const Vec4& colorTR, const Vec4& colorBR, const Vec4& colorBL, StyleOptions& opts, int drawOrder);

        // No rounding, single color
        void FillRect_NoRound_SC(DrawBuffer* buf, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& color, StyleOptions& opts, int drawOrder);

        // No rounding, radial gradient
        void FillRect_NoRound_RadialGra(DrawBuffer* buf, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& startcolor, const Vec4& endColor, StyleOptions& opts, int drawOrder);

        // Rounding
        void FillRect_Round(DrawBuffer* buf, Array<int>& roundedCorners, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& col, float rounding, StyleOptions& opts, int drawOrder);

        // Fill rect impl.
        void FillRectData(Vertex* vertArray, bool hasCenter, const Vec2& min, const Vec2& max);

        // No rounding, vertical or horizontal gradient
        void FillTri_NoRound_VerHorGra(DrawBuffer* buf, float rotateAngle, const Vec2& p3, const Vec2& p2, const Vec2& p1, const Vec4& colorLeft, const Vec4& colorRight, const Vec4& colorTop, StyleOptions& opts, int drawOrder);

        // No rounding, single color
        void FillTri_NoRound_SC(DrawBuffer* buf, float rotateAngle, const Vec2& p3, const Vec2& p2, const Vec2& p1, const Vec4& color, StyleOptions& opts, int drawOrder);

        // No rounding, radial gradient
        void FillTri_NoRound_RadialGra(DrawBuffer* buf, float rotateAngle, const Vec2& p3, const Vec2& p2, const Vec2& p1, const Vec4& startColor, const Vec4& endColor, StyleOptions& opts, int drawOrder);

        // Rounding
        void FillTri_Round(DrawBuffer* buf, Array<int>& onlyRoundCorners, float rotateAngle, const Vec2& p3, const Vec2& p2, const Vec2& p1, const Vec4& col, float rounding, StyleOptions& opts, int drawOrder);

        // Fill rect impl.
        void FillTriData(Vertex* vertArray, bool hasCenter, bool calculateUV, const Vec2& p1, const Vec2& p2, const Vec2& p3);

        // No rounding, single color
        void FillNGon_SC(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int n, const Vec4& color, StyleOptions& opts, int drawOrder);

        // No rounding, horizontal or vertical gradient
        void FillNGon_VerHorGra(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int n, const Vec4& colorStart, const Vec4& colorEnd, bool isHor, StyleOptions& opts, int drawOrder);

        // No rounding, radial gradient
        void FillNGon_RadialGra(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int n, const Vec4& colorStart, const Vec4& colorEnd, StyleOptions& opts, int drawOrder);

        // Fill NGon imp
        void FillNGonData(Array<Vertex>&, bool hasCenter, const Vec2& center, float radius, int n);

        // Single color
        void FillCircle_SC(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int segments, const Vec4& color, float startAngle, float endAngle, StyleOptions& opts, int drawOrder);

        // Vertical or horizontal gradinet.
        void FillCircle_VerHorGra(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int segments, const Vec4& colorStart, const Vec4& colorEnd, bool isHor, float startAngle, float endAngle, StyleOptions& opts, int drawOrder);

        // Radial gradient
        void FillCircle_RadialGra(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int segments, const Vec4& colorStart, const Vec4& colorEnd, float startAngle, float endAngle, StyleOptions& opts, int drawOrder);

        // Fill circle impl
        void FillCircleData(Array<Vertex>& v, bool hasCenter, const Vec2& center, float radius, int segments, float startAngle, float endAngle);

        // Single color
        void FillConvex_SC(DrawBuffer* buf, float rotateAngle, Vec2* points, int size, const Vec2& center, const Vec4& color, StyleOptions& opts, int drawOrder);

        // Vertical horizontal gradient
        void FillConvex_VerHorGra(DrawBuffer* buf, float rotateAngle, Vec2* points, int size, const Vec2& center, const Vec4& colorStart, const Vec4& colorEnd, bool isHor, StyleOptions& opts, int drawOrder);

        // Radial gradient.
        void FillConvex_RadialGra(DrawBuffer* buf, float rotateAngle, Vec2* points, int size, const Vec2& center, const Vec4& colorStart, const Vec4& colorEnd, StyleOptions& opts, int drawOrder);

      
        /// Triangle bounding box.
        void GetTriangleBoundingBox(const Vec2& p1, const Vec2& p2, const Vec2& p3, Vec2& outMin, Vec2& outMax);

        /// <summary>
        /// Fills bounding box information for given points/vertices.
        /// </summary>
        void GetConvexBoundingBox(Vec2* points, int size, Vec2& outMin, Vec2& outMax);
        void GetConvexBoundingBox(Vertex* points, int size, Vec2& outMin, Vec2& outMax);
        void GetConvexBoundingBox(DrawBuffer* buf, int startIndex, int endIndex, Vec2& outMin, Vec2& outMax);

        /// <summary>
        /// Auto-calculate UVs for the vertices defined by start & end indices using 2D bounding-box.
        /// </summary>
        void CalculateVertexUVs(DrawBuffer* buf, int startIndex, int endIndex);

        // Angle increment based on rounding value.
        float GetAngleIncrease(float rounding);

        /// <summary>
        /// Returns the direction vector going from the center of the arc towards it's middle angle.
        /// </summary>
        Vec2 GetArcDirection(const Vec2& center, float radius, float startAngle, float endAngle);

        Line CalculateLine(const Vec2& p1, const Vec2& p2, StyleOptions& style);
        void DrawLine(Line& line, StyleOptions& style, float rotateAngle);

        /// <summary>
        /// Draws an outline (or AA) around the vertices given, following the specific draw order via index array.
        /// Used for semi-circles, arcs, lines and alike.
        /// </summary>
        /// <returns></returns>
        DrawBuffer* DrawOutlineAroundShape(DrawBuffer* sourceBuffer, StyleOptions& opts, int* indicesOrder, int vertexCount, float defThickness, bool ccw = false, int drawOrder = 0, bool isAAOutline = false);

        /// <summary>
        /// Draws an outline (or AA) around a convex shape.
        /// Vertex count need to be without including the center vertex.
        /// </summary>
        /// <param name="sourceBuffer"></param>
        /// <param name="vertexCount"></param>
        /// <param name="opts"></param>
        /// <returns></returns>
        DrawBuffer* DrawOutline(DrawBuffer* sourceBuffer, StyleOptions& opts, int vertexCount, bool skipEnds = false, int drawOrder = 0, bool isAAOutline = false, bool reverseDrawDir = false);

    }; // namespace Internal

} // namespace Lina2D

#endif
