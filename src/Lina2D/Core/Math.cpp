/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

#include "Lina2D/Core/Math.hpp"

namespace Lina2D::Math
{
    float Mag(const Vec2& v)
    {
        return sqrt(v.x * v.x + v.y * v.y);
    }

    Vec2 Normalized(const Vec2& v)
    {
        const float mag = Mag(v);
        return Vec2(v.x / mag, v.y / mag);
    }

    Vec2 Rotate90(const Vec2& v, bool cw)
    {
        if (cw)
            return Vec2(v.y, -v.x);
        else
            return Vec2(-v.y, v.x);
    }

    float GetAngleFromCenter(const Vec2& center, const Vec2& point)
    {
        return L2D_RAD2DEG * (std::atan2(point.y - center.y, point.x - center.x));
    }

    float GetAngleBetween(const Vec2& p1, const Vec2& p2)
    {
        return L2D_RAD2DEG * (std::atan2(p2.y, p2.x) - atan2(p1.y, p1.x));
    }

    bool IsEqual(const Vec2& v1, const Vec2& v2)
    {
        return (v1.x == v2.x && v1.y == v2.y);
    }

    bool IsEqual(const Vec4& v1, const Vec4& v2)
    {
        return (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w);
    }

    int GetAreaIndex(const Vec2& diff)
    {
        if (diff.x >= 0 && diff.y >= 0)
            return 0;
        if (diff.x >= 0 && diff.y <= 0)
            return 1;
        if (diff.x <= 0 && diff.y <= 0)
            return 2;
        if (diff.x <= 0 && diff.y >= 0)
            return 3;
    }

    Vec2 GetPointOnSphere(const Vec2& center, float radius, float angle)
    {
        float x = radius * std::cos(L2D_DEG2RAD * angle) + center.x;
        float y = radius * std::sin(L2D_DEG2RAD * angle) + center.y;
        return Vec2(x, y);
    }

    Vec2 SampleParabola(const Vec2& p1, const Vec2& p2, const Vec2& direction, float height, float t)
    {
        float parabolicT = t * 2.0f - 1.0f;
        float h          = (-parabolicT * parabolicT + 1) * height;

        Vec2 travelDir  = Vec2(p2.x - p1.x, p2.y - p1.y);
        Vec2 levelDir   = Vec2(p2.x - p1.x, p2.y - p2.y);
        Vec2 upAddition = Vec2(direction.x * h, direction.y * h);
        return Vec2(p1.x + (t * travelDir.x) + upAddition.x, p1.y + (t * travelDir.y) + upAddition.y);
    }

    Vec2 SampleBezier(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, float t)
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
}