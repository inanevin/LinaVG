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

    /// <summary>
    /// Draws a single point with the given color & position.
    /// </summary>
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
    void DrawTriangle(const Vec2& left, const Vec2& right, const Vec2& top, StyleOptions& style, float rotateAngle = 0.0f);

    /// <summary>
    /// Draws a filled rectangle between min & max with the given style options & rotation angle.
    /// </summary>
    void DrawRect(const Vec2& min, const Vec2& max, StyleOptions& style, float rotateAngle = 0.0f);

    /// <summary>
    /// Draws a convex polygon with N corners. !Rounding options do not apply to NGons!
    /// </summary>
    void DrawNGon(const Vec2& center, float radius, int n, StyleOptions& style, float rotateAngle = 0.0f);

    /// <summary>
    /// Draws the given set of points. !Rounding options do not apply!
    /// If you are not going to fill the convex shape (styling options -> m_isFilled), then prefer using DrawLines instead of this so that you can use proper line joints.
    /// </summary>
    void DrawConvex(Vec2* points, int size, StyleOptions& style, float rotateAngle = 0.0f);

    /// <summary>
    /// Draws a filled circle with the given radius & center.
    /// You can change the start and end angles to create a filled semi-circle or a filled arc.
    /// It's recommended to send angles always in the range of 0.0f - 360.0f. This method will try to auto-convert if not.
    /// Segments are clamped to 6 - 360 (360 being perfect circle, 8 being 6-gon)
    /// Higher the segment cause more weight on the performance. 18-54 is a good range for balance.
    /// Always recommended to use segments that leave no remainder when 360 is divided by it.
    /// !Rounding options have no effect.!
    /// </summary>
    void DrawCircle(const Vec2& center, float radius, StyleOptions& style, int segments = 36, float rotateAngle = 0.0f, float startAngle = 0.0f, float endAngle = 360.0f);

    /// <summary>
    /// Triangulates & fills the index array given a start and end vertex index.
    /// </summary>
    void ConvexFillVertices(int startIndex, int endIndex, Array<Index>& indices, bool skipLastTriangle = false);

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
        void FillRect_NoRound_VerHorGra(DrawBuffer* buf, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& colorTL, const Vec4& colorTR, const Vec4& colorBR, const Vec4& colorBL, StyleOptions& opts);

        // No rounding, single color
        void FillRect_NoRound_SC(DrawBuffer* buf, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& color, StyleOptions& opts);

        // No rounding, radial gradient
        void FillRect_NoRound_RadialGra(DrawBuffer* buf, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& startcolor, const Vec4& endColor, StyleOptions& opts);

        // Rounding
        void FillRect_Round(DrawBuffer* buf, Array<int>& roundedCorners, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& col, float rounding, StyleOptions& opts);

        // Fill rect impl.
        void FillRectData(Vertex* vertArray, bool hasCenter, const Vec2& min, const Vec2& max);

        // No rounding, vertical or horizontal gradient
        void FillTri_NoRound_VerHorGra(DrawBuffer* buf, float rotateAngle, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec4& colorLeft, const Vec4& colorRight, const Vec4& colorTop, StyleOptions& opts);

        // No rounding, single color
        void FillTri_NoRound_SC(DrawBuffer* buf, float rotateAngle, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec4& color, StyleOptions& opts);

        // No rounding, radial gradient
        void FillTri_NoRound_RadialGra(DrawBuffer* buf, float rotateAngle, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec4& startColor, const Vec4& endColor, StyleOptions& opts);

        // Rounding
        void FillTri_Round(DrawBuffer* buf, Array<int>& onlyRoundCorners, float rotateAngle, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec4& col, float rounding, StyleOptions& opts);

        // Fill rect impl.
        void FillTriData(Vertex* vertArray, bool hasCenter, bool calculateUV, const Vec2& p1, const Vec2& p2, const Vec2& p3);

        // No rounding, single color
        void FillNGon_SC(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int n, const Vec4& color, StyleOptions& opts);

        // No rounding, horizontal or vertical gradient
        void FillNGon_VerHorGra(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int n, const Vec4& colorStart, const Vec4& colorEnd, bool isHor, StyleOptions& opts);

        // No rounding, radial gradient
        void FillNGon_RadialGra(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int n, const Vec4& colorStart, const Vec4& colorEnd, StyleOptions& opts);

        // Fill NGon imp
        void FillNGonData(Array<Vertex>&, bool hasCenter, const Vec2& center, float radius, int n);

        // Single color
        void FillCircle_SC(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int segments, const Vec4& color, float startAngle, float endAngle, StyleOptions& opts);

        // Vertical or horizontal gradinet.
        void FillCircle_VerHorGra(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int segments, const Vec4& colorStart, const Vec4& colorEnd, bool isHor, float startAngle, float endAngle, StyleOptions& opts);

        // Radial gradient
        void FillCircle_RadialGra(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int segments, const Vec4& colorStart, const Vec4& colorEnd, float startAngle, float endAngle, StyleOptions& opts);

        // Fill circle impl
        void FillCircleData(Array<Vertex>& v, bool hasCenter, const Vec2& center, float radius, int segments, float startAngle, float endAngle);

        // Single color
        void FillConvex_SC(DrawBuffer* buf, float rotateAngle, Vec2* points, int size, const Vec2& center, const Vec4& color, StyleOptions& opts);

        // Vertical horizontal gradient
        void FillConvex_VerHorGra(DrawBuffer* buf, float rotateAngle, Vec2* points, int size, const Vec2& center, const Vec4& colorStart, const Vec4& colorEnd, bool isHor, StyleOptions& opts);

        // Radial gradient.
        void FillConvex_RadialGra(DrawBuffer* buf, float rotateAngle, Vec2* points, int size, const Vec2& center, const Vec4& colorStart, const Vec4& colorEnd, StyleOptions& opts);

        /// <summary>
        /// Returns the centroid of a given polygon.
        /// https://stackoverflow.com/questions/2792443/finding-the-centroid-of-a-polygon
        /// </summary>
        /// <returns></returns>
        Vec2 GetPolygonCentroid(Vec2* points, int size);

        // Triangle bounding box.
        void GetTriangleBoundingBox(const Vec2& p1, const Vec2& p2, const Vec2& p3, Vec2& outMin, Vec2& outMax);

        // fill min & max to the bounding box of the given convex points.
        void GetConvexBoundingBox(Vec2* points, int size, Vec2& outMin, Vec2& outMax);

        // Angle increment based on rounding value.
        float GetAngleIncrease(float rounding);

        /// <summary>
        /// Extrudes the outer vertices of a convex shape & draws vertex color outline around it. !Copies the given vertices into a seperate buffer.!
        /// </summary>
        /// <param name="srcVertices">Original buffer to copy from.</param>
        /// <param name="destVertices">Destination draw buffer.</param>
        /// <param name="destIndices">Destination draw buffer.</param>
        /// <param name="startIndex">Must be the first outer vertex of the shape. !Should not be the center if exists! </param>
        /// <param name="endIndex">Must be the last outer vertex of the shape.</param>
        /// <param name="convexCenter">Centroid of the convex shape.</param>
        /// <param name="options">Outline options</param>
        // void ConvexOutline(Array<Vertex>& srcVertices, int startIndex, int endIndex, const Vec2& convexCenter, OutlineOptions& options, bool skipEndClosing = false);

        /// <summary>
        /// Returns the direction vector going from the center of the arc towards it's middle angle.
        /// </summary>
        Vec2 GetArcDirection(const Vec2& center, float radius, float startAngle, float endAngle);

        void CircleOutlineClosing(Vertex& c, StyleOptions& opts, int totalSize, float radius, float startAngle, float endAngle);

        /// <summary>
        /// Fills convex shapes without the assumption of a center vertex. Used for filling outer areas of non-filled shapes.
        /// <param name="startIndex"> First vertex - start of the border.</param>
        /// <param name="endIndex"> Last vertex - end of the border.</param>
        /// </summary>
        void ExtrudeAndFillShapeBorders(DrawBuffer* buf, const Vec2& center, int startIndex, int endIndex, float thickness, bool skipEndClosing = false);


        void DrawOutline(DrawBuffer* sourceBuffer, const Vec2& enter, int startIndex, int endIndex, OutlineOptions& opts, bool isFilled, bool skipEndClosing = false, bool swapPrevNext = false);

    }; // namespace Internal

} // namespace Lina2D

#endif
