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

#include "Core/Math.hpp"

namespace LinaVG
{
    float Math::Mag(const Vec2& v)
    {
        return sqrt(v.x * v.x + v.y * v.y);
    }

    Vec2 Math::Normalized(const Vec2& v)
    {
        const float mag = Mag(v);
        return Vec2(v.x / mag, v.y / mag);
    }

    /// <summary>
    /// Inverted Y axis.
    /// </summary>
    Vec2 Math::Rotate90(const Vec2& v, bool ccw)
    {
        if (ccw)
            return Vec2(v.y, -v.x);
        else
            return Vec2(-v.y, v.x);
    }

    float Math::GetAngleFromCenter(const Vec2& center, const Vec2& point)
    {
        return LVG_RAD2DEG * (std::atan2(point.y - center.y, point.x - center.x));
    }

    float Math::GetAngleBetween(const Vec2& p1, const Vec2& p2)
    {
        return LVG_RAD2DEG * (std::atan2(p2.y, p2.x) - atan2(p1.y, p1.x));
    }

    float Math::GetAngleBetweenDirs(const Vec2& dir1, const Vec2& dir2)
    {
        return LVG_RAD2DEG * std::atan2f(dir1.x * dir2.y - dir1.y * dir2.x, dir1.x * dir2.x + dir1.y * dir2.y);
    }

    float Math::GetAngleBetweenShort(const Vec2& p1, const Vec2& p2)
    {
        float ang = LVG_RAD2DEG * (std::atan2(p2.y, p2.x) - atan2(p1.y, p1.x));

        if (ang > 180.0f)
            ang = 360.0f - ang;

        return ang;
    }

    bool Math::IsEqualMarg(const Vec2& v1, const Vec2& v2, float epsilon)
    {
        const bool x = ((v1.x - v2.x) < epsilon) && ((v2.x - v1.x) < epsilon);
        const bool y = ((v1.y - v2.y) < epsilon) && ((v2.y - v1.y) < epsilon);
        return x && y;
    }

    bool Math::IsEqualMarg(float f1, float f2, float epsilon)
    {
        return ((f1 - f2) < epsilon) && ((f2 - f1) < epsilon);
    }
    bool Math::IsEqual(const Vec2& v1, const Vec2& v2)
    {
        return (v1.x == v2.x && v1.y == v2.y);
    }
    bool Math::IsEqual(const Vec4& v1, const Vec4& v2)
    {
        return (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w);
    }

    int Math::GetAreaIndex(const Vec2& diff)
    {
        if (diff.x >= 0 && diff.y >= 0)
            return 0;
        if (diff.x >= 0 && diff.y <= 0)
            return 1;
        if (diff.x <= 0 && diff.y <= 0)
            return 2;
        if (diff.x <= 0 && diff.y >= 0)
            return 3;

        return -1;
    }

    Vec2 Math::GetPointOnCircle(const Vec2& center, float radius, float angle)
    {
        const float x = radius * std::cos(LVG_DEG2RAD * angle) + center.x;
        const float y = radius * std::sin(LVG_DEG2RAD * angle) + center.y;
        return Vec2(x, y);
    }

    Vec2 Math::SampleParabola(const Vec2& p1, const Vec2& p2, const Vec2& direction, float height, float t)
    {
        float parabolicT = t * 2.0f - 1.0f;
        float h          = (-parabolicT * parabolicT + 1) * height;

        Vec2 travelDir  = Vec2(p2.x - p1.x, p2.y - p1.y);
        Vec2 levelDir   = Vec2(p2.x - p1.x, p2.y - p2.y);
        Vec2 upAddition = Vec2(direction.x * h, direction.y * h);
        return Vec2(p1.x + (t * travelDir.x) + upAddition.x, p1.y + (t * travelDir.y) + upAddition.y);
    }

    Vec2 Math::SampleBezier(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, float t)
    {
        float tt  = t * t;
        float ttt = t * tt;
        float u   = 1.0f - t;
        float uu  = u * u;
        float uuu = u * uu;

        Vec2 B = Vec2(uuu * p0.x, uuu * p0.y);
        B.x += 3.0f * uu * t * p1.x;
        B.y += 3.0f * uu * t * p1.y;
        B.x += 3.0f * u * tt * p2.x;
        B.y += 3.0f * u * tt * p2.y;
        B.x += ttt * p3.x;
        B.y += ttt * p3.y;
        return B;
    }

    Vec2 Math::LineIntersection(const Vec2& p00, const Vec2& p01, const Vec2& p10, const Vec2& p11)
    {
        // Line 1
        float a1 = p01.y - p00.y;
        float b1 = p00.x - p01.x;
        float c1 = a1 * (p00.x) + b1 * (p00.y);

        // Line 2
        float a2 = p11.y - p10.y;
        float b2 = p10.x - p11.x;
        float c2 = a2 * (p10.x) + b2 * (p10.y);

        float det = a1 * b2 - a2 * b1;

        // Parallel
        if (det == 0.0f)
            return p10;

        _ASSERT(det != 0.0f);

        float x = (b2 * c1 - b1 * c2) / det;
        float y = (a1 * c2 - a2 * c1) / det;

        return Vec2(x, y);
    }

    bool Math::AreLinesParallel(const Vec2& p00, const Vec2& p01, const Vec2& p10, const Vec2& p11)
    {
        // Line 1
        float a1 = p01.y - p00.y;
        float b1 = p00.x - p01.x;
        float c1 = a1 * (p00.x) + b1 * (p00.y);

        // Line 2
        float a2 = p11.y - p10.y;
        float b2 = p10.x - p11.x;
        float c2 = a2 * (p10.x) + b2 * (p10.y);

        float det = a1 * b2 - a2 * b1;

        // Parallel
        return det == 0.0f;
    }

    Vec2 Math::RotateAround(const Vec2& point, const Vec2& center, float angle)
    {
        const float angleRads = LVG_DEG2RAD * angle;
        float       x         = std::cos(angleRads) * (point.x - center.x) - std::sin(angleRads) * (point.y - center.y) + center.x;
        float       y         = std::sin(angleRads) * (point.x - center.x) + std::cos(angleRads) * (point.y - center.y) + center.y;
        return Vec2(x, y);
    }

    Vec2 Math::Abs(const Vec2& v)
    {
        return Vec2(Abs(v.x), Abs(v.y));
    }

    Vec2 Math::Clamp(const Vec2& v, const Vec2& min, const Vec2& max)
    {
        return Vec2(Clamp(v.x, min.x, max.x), Clamp(v.y, min.y, max.y));
    }

    Vec2 Math::ScalePoint(const Vec2& p, const Vec2& center, float scale)
    {
        const Vec2 dir          = Math::Normalized(Vec2(p.x - center.x, p.y - center.y));
        const Vec2 aroundOrigin = Vec2(p.x - center.x, p.y - center.y);
        const Vec2 scaled       = Vec2(aroundOrigin.x * scale, aroundOrigin.y * scale);
        return Vec2(scaled.x + center.x, scaled.y + center.y);
    }

    Vec2 Math::GetVertexAverage(const Vec2& point, const Vec2& previousPoint, const Vec2& nextPoint)
    {
        const Vec2 toNext = Math::Normalized(Vec2(nextPoint.x - point.x, nextPoint.y - point.y));
        const Vec2 toPrev = Math::Normalized(Vec2(previousPoint.x - point.x, previousPoint.y - point.y));
        return Math::Normalized(Vec2(-toNext.x - toPrev.x, -toNext.y - toPrev.y));
    }

    Vec2 Math::GetVertexNormal(const Vec2& point, const Vec2& previousPoint, const Vec2& nextPoint, bool ccw)
    {
        if (previousPoint.x == -1.0f && previousPoint.y == -1.0f)
        {
            const Vec2 toNext = Math::Normalized(Vec2(nextPoint.x - point.x, nextPoint.y - point.y));
            return Math::Normalized(Math::Rotate90(toNext, ccw));
        }

        if (nextPoint.x == -1.0f && nextPoint.y == -1.0f)
        {
            const Vec2 fromPrev = Math::Normalized(Vec2(point.x - previousPoint.x, point.y - previousPoint.y));
            return Math::Normalized(Math::Rotate90(fromPrev, ccw));
        }

        const Vec2 toNext   = Math::Normalized(Vec2(nextPoint.x - point.x, nextPoint.y - point.y));
        const Vec2 fromPrev = Math::Normalized(Vec2(point.x - previousPoint.x, point.y - previousPoint.y));

        const Vec2 toNextNormal       = Math::Rotate90(toNext, ccw);
        const Vec2 fromPreviousNormal = Math::Rotate90(fromPrev, ccw);
        return Math::Normalized(Vec2(toNextNormal.x + fromPreviousNormal.x, toNextNormal.y + fromPreviousNormal.y));
    }

    Vec2 Math::GetVertexNormalFlatCheck(const Vec2& point, const Vec2& previousPoint, const Vec2& nextPoint, bool ccw)
    {
        const Vec2 toNext   = Math::Normalized(Vec2(nextPoint.x - point.x, nextPoint.y - point.y));
        const Vec2 fromPrev = Math::Normalized(Vec2(point.x - previousPoint.x, point.y - previousPoint.y));

        if (Math::IsEqualMarg(toNext, fromPrev))
        {
            const Vec2 fromPreviousNormal = Math::Rotate90(fromPrev, ccw);
            return Math::Normalized(Vec2(toNext.x + fromPreviousNormal.x, toNext.y + fromPreviousNormal.y));
        }
        const Vec2 toNextNormal       = Math::Rotate90(toNext, ccw);
        const Vec2 fromPreviousNormal = Math::Rotate90(fromPrev, ccw);
        return Math::Normalized(Vec2(toNextNormal.x + fromPreviousNormal.x, toNextNormal.y + fromPreviousNormal.y));
    }

    float Math::InverseLerp(float a, float b, float v)
    {
        if (a == b)
            return 0;
        else if (b < a)
        {
            float temp = b;
            b          = a;
            a          = temp;
        }
        return (v - a) / (b - a);
    }

    Vec2 Math::GetPolygonCentroid(Vec2* vertices, int vertexCount)
    {
        Vec2  centroid   = {0, 0};
        float signedArea = 0.0f;
        float x0         = 0.0f; // Current vertex X
        float y0         = 0.0f; // Current vertex Y
        float x1         = 0.0f; // Next vertex X
        float y1         = 0.0f; // Next vertex Y
        float a          = 0.0f; // Partial signed area

        // For all vertices except last
        int i = 0;
        for (i = 0; i < vertexCount - 1; ++i)
        {
            x0 = vertices[i].x;
            y0 = vertices[i].y;
            x1 = vertices[i + 1].x;
            y1 = vertices[i + 1].y;
            a  = x0 * y1 - x1 * y0;
            signedArea += a;
            centroid.x += (x0 + x1) * a;
            centroid.y += (y0 + y1) * a;
        }

        // Do last vertex separately to avoid performing an expensive
        // modulus operation in each iteration.
        x0 = vertices[i].x;
        y0 = vertices[i].y;
        x1 = vertices[0].x;
        y1 = vertices[0].y;
        a  = x0 * y1 - x1 * y0;
        signedArea += a;
        centroid.x += (x0 + x1) * a;
        centroid.y += (y0 + y1) * a;

        signedArea *= 0.5f;
        centroid.x /= (6.0f * signedArea);
        centroid.y /= (6.0f * signedArea);

        return centroid;
    }

    Vec2 Math::GetPolygonCentroidFast(Vec2* points, int size)
    {
        Vec2 total = Vec2(0, 0);

        for (int i = 0; i < size; i++)
        {
            total.x += points[i].x;
            total.y += points[i].y;
        }

        total.x /= (float)size;
        total.y /= (float)size;
        return total;
    }

    float Math::Abs(float f)
    {
        return f < 0.0f ? -f : f;
    }

    float Math::Clamp(float f, float min, float max)
    {
        if (f < min)
            return min;
        if (f > max)
            return max;
        return f;
    }

    int Math::Clamp(int i, int min, int max)
    {
        if (i < min)
            return min;
        if (i > max)
            return max;
        return i;
    }

  
    Vec4 Math::Lerp(const Vec4& val, const Vec4& val2, float amt)
    {
        float t  = (1.0f - amt);
        Vec4  c1 = Vec4(val.x * t, val.y * t, val.z * t, val.w * t);
        Vec4  c2 = Vec4(val2.x * amt, val2.y * amt, val2.z * amt, val2.w * amt);
        return Vec4(c1.x + c2.x, c1.y + c2.y, c1.z + c2.z, c1.w + c2.w);
    }

    Vec2 Math::Lerp(const Vec2& val, const Vec2& val2, float amt)
    {
        float t  = (1.0f - amt);
        Vec2  c1 = Vec2(val.x * t, val.y * t);
        Vec2  c2 = Vec2(val2.x * amt, val2.y * amt);
        return Vec2(c1.x + c2.x, c1.y + c2.y);
    }
} // namespace LinaVG