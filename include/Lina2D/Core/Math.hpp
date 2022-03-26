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

/*
Class: Lina2DMath



Timestamp: 3/26/2022 10:42:37 AM
*/

#pragma once

#ifndef Lina2DMath_HPP
#define Lina2DMath_HPP

// Headers here.
#include "Common.hpp"

namespace Lina2D::Math
{
    static float   Mag(const Vec2& v);
    static int     GetAreaIndex(const Vec2& diff);
    static float   GetAngleFromCenter(const Vec2& center, const Vec2& point);
    static float   GetAngleBetween(const Vec2& p1, const Vec2& p2);
    static bool    IsEqual(const Vec2& v1, const Vec2& v2);
    static bool    IsEqual(const Vec4& v1, const Vec4& v2);
    static Vec2 Normalized(const Vec2& v);
    static Vec2 Rotate90(const Vec2& v, bool cw = true);
    static Vec2 GetPointOnSphere(const Vec2& center, float radius, float angle); // Angle in degrees.
    static Vec2 SampleParabola(const Vec2& p1, const Vec2& p2, const Vec2& direction, float height, float t);
    static Vec2 SampleBezier(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, float t);

    template <typename T, typename U>
    static T Lerp(const T& val1, const T& val2, const U& amt)
    {
        return (T)(val1 * ((U)(1) - amt) + val2 * amt);
    }

    static Vec4 Lerp(const Vec4& val, const Vec4& val2, float amt)
    {
        float   t  = (1.0f - amt);
        Vec4 c1 = Vec4(val.x * t, val.y * t, val.z * t, val.w * t);
        Vec4 c2 = Vec4(val2.x * amt, val2.y * amt, val2.z * amt, val2.w * amt);
        return Vec4(c1.x + c2.x, c1.y + c2.y, c1.z + c2.z, c1.w + c2.w);
    }

    template <typename T>
    static T Remap(const T& val, const T& fromLow, const T& fromHigh, const T& toLow, const T& toHigh)
    {
        return toLow + (val - fromLow) * (toHigh - toLow) / (fromHigh - fromLow);
    }

}

#endif
