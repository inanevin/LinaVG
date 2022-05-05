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

#include "Lina2D/Core/Drawer.hpp"
#include "Lina2D/Core/Math.hpp"
#include "Lina2D/Core/Renderer.hpp"
#include "Lina2D/Core/Internal.hpp"
#include "Lina2D/Core/GLBackend.hpp"
#include <iostream>
#include <stdio.h>

namespace Lina2D
{
    RectOverrideData g_rectOverrideData;

    void DrawBezier(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec4Grad& color, ThicknessGrad thickness, int segments)
    {

        const bool  useGradientColor     = !Math::IsEqual(color.m_start, color.m_end);
        const float useGradientThickness = thickness.m_start != thickness.m_end;

        if (segments < 5)
            segments = 5;

        Vec2  lastPoint      = p0;
        Vec4  startColor     = color.m_start;
        Vec4  endColor       = color.m_end;
        float startThickness = thickness.m_start;
        float endThickness   = thickness.m_end;
        for (int i = 1; i < segments; i++)
        {
            float      t = Math::Remap((float)i, 0.0f, (float)segments, 0.0f, 1.0f);
            const Vec2 p = Math::SampleBezier(p0, p1, p2, p3, t);

            if (useGradientColor)
            {
                endColor = Math::Lerp(color.m_start, color.m_end, t);
            }

            if (useGradientThickness)
                endThickness = Math::Lerp(thickness.m_start, thickness.m_end, t);

            // DrawLine(lastPoint, p, {startColor, endColor}, {startThickness, endThickness});

            if (useGradientColor)
                startColor = endColor;

            if (useGradientThickness)
                startThickness = endThickness;
            lastPoint = p;
        }
    }

    void DrawPoint(const Vec2& p1, const Vec4& col)
    {
#ifdef NONDEBUG
        StyleOptions style;
        style.m_color        = col;
        const float distance = Config.m_framebufferScale.x / 2.0f;
        DrawRect(Vec2(p1.x - distance, p1.y - distance), Vec2(p1.x + distance, p1.y + distance), style);
#else
        const float distance = Config.m_framebufferScale.x * 0.5f;
        Vertex      p1u, p2u, p1d, p2d;
        p1u.m_pos.x = p1.x - distance;
        p1u.m_pos.y = p1.y + distance;
        p2u.m_pos.x = p1.x + distance;
        p2u.m_pos.y = p1.y + distance;
        p1d.m_pos.x = p1.x - distance;
        p1d.m_pos.y = p1.y - distance;
        p2d.m_pos.x = p1.x + distance;
        p2d.m_pos.y = p1.y - distance;
        p1u.m_col   = col;
        p2u.m_col   = col;
        p2d.m_col   = col;
        p1d.m_col   = col;

        auto&       buf  = Internal::g_rendererData.GetDefaultBuffer(0);
        const Index curr = buf.m_vertexBuffer.m_size;
        buf.PushVertex(p1u);
        buf.PushVertex(p2u);
        buf.PushVertex(p2d);
        buf.PushVertex(p1d);

        buf.PushIndex(curr);
        buf.PushIndex(curr + 1);
        buf.PushIndex(curr + 3);
        buf.PushIndex(curr + 1);
        buf.PushIndex(curr + 2);
        buf.PushIndex(curr + 3);
#endif
    }

    void DrawLine(const Vec2& p1, const Vec2& p2, StyleOptions& style, LineCapDirection cap, float rotateAngle, int drawOrder)
    {
        Line         l = Internal::CalculateLine(p1, p2, style);
        StyleOptions s = StyleOptions(style);
        s.m_isFilled   = true;

        if (cap == LineCapDirection::Left || cap == LineCapDirection::Both)
        {
            s.m_onlyRoundTheseCorners.push_back(0);
            s.m_onlyRoundTheseCorners.push_back(3);
            s.m_rounding = 2.0f;
        }

        if (cap == LineCapDirection::Right || cap == LineCapDirection::Both)
        {
            s.m_onlyRoundTheseCorners.push_back(1);
            s.m_onlyRoundTheseCorners.push_back(2);
            s.m_rounding = 2.0f;
        }

        const Vec2 up                            = Math::Normalized(Math::Rotate90(Vec2(p2.x - p1.x, p2.y - p1.y), true));
        g_rectOverrideData.m_p1                  = Vec2(p1.x + up.x * style.m_thickness.m_start / 2.0f, p1.y + up.y * style.m_thickness.m_start / 2.0f);
        g_rectOverrideData.m_p4                  = Vec2(p1.x - up.x * style.m_thickness.m_start / 2.0f, p1.y - up.y * style.m_thickness.m_start / 2.0f);
        g_rectOverrideData.m_p2                  = Vec2(p2.x + up.x * style.m_thickness.m_end / 2.0f, p2.y + up.y * style.m_thickness.m_end / 2.0f);
        g_rectOverrideData.m_p3                  = Vec2(p2.x - up.x * style.m_thickness.m_end / 2.0f, p2.y - up.y * style.m_thickness.m_end / 2.0f);
        g_rectOverrideData.overrideRectPositions = true;
        DrawRect(g_rectOverrideData.m_p1, g_rectOverrideData.m_p3, s, rotateAngle);
        g_rectOverrideData.overrideRectPositions = false;
    }

    void DrawLines(Vec2* points, int count, StyleOptions& style, LineCapDirection cap, LineJointType jointType, float rotateAngle, int drawOrder)
    {
    }

    void DrawTriangle(const Vec2& top, const Vec2& right, const Vec2& left, StyleOptions& style, float rotateAngle, int drawOrder)
    {
        // NR - SC - def buf
        // NR - SC - text
        // NR - VH - DEF
        // NR - VH - text

        if (style.m_rounding == 0.0f)
        {
            if (Math::IsEqual(style.m_color.m_start, style.m_color.m_end))
            {
                if (style.m_textureHandle == 0)
                    Internal::FillTri_NoRound_SC(&Internal::g_rendererData.GetDefaultBuffer(drawOrder), rotateAngle, top, right, left, style.m_color.m_start, style, drawOrder);
                else
                {

                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                    Internal::FillTri_NoRound_SC(&buf, rotateAngle, top, right, left, style.m_color.m_start, style, drawOrder);
                }
            }
            else
            {
                if (style.m_color.m_gradientType == GradientType::Horizontal)
                {
                    // Horizontal, non rounded
                    if (style.m_textureHandle == 0)
                        Internal::FillTri_NoRound_VerHorGra(&Internal::g_rendererData.GetDefaultBuffer(drawOrder), rotateAngle, top, right, left, style.m_color.m_start, style.m_color.m_end, style.m_color.m_end, style, drawOrder);
                    else
                    {
                        TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                        Internal::FillTri_NoRound_VerHorGra(&buf, rotateAngle, top, right, left, style.m_color.m_start, style.m_color.m_end, style.m_color.m_end, style, drawOrder);
                    }
                }
                else if (style.m_color.m_gradientType == GradientType::Vertical)
                {
                    // Vertical, non rounded
                    if (style.m_textureHandle == 0)
                        Internal::FillTri_NoRound_VerHorGra(&Internal::g_rendererData.GetDefaultBuffer(drawOrder), rotateAngle, top, right, left, style.m_color.m_end, style.m_color.m_end, style.m_color.m_start, style, drawOrder);
                    else
                    {
                        TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                        Internal::FillTri_NoRound_VerHorGra(&buf, rotateAngle, top, right, left, style.m_color.m_end, style.m_color.m_end, style.m_color.m_start, style, drawOrder);
                    }
                }
                else if (style.m_color.m_gradientType == GradientType::Radial || style.m_color.m_gradientType == GradientType::RadialCorner)
                {
                    // Radial, non rounded
                    if (style.m_textureHandle == 0)
                    {
                        GradientDrawBuffer& buf = Internal::g_rendererData.GetGradientBuffer(style.m_color, drawOrder);
                        Internal::FillTri_NoRound_RadialGra(&buf, rotateAngle, top, right, left, style.m_color.m_start, style.m_color.m_end, style, drawOrder);
                    }
                    else
                    {
                        TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                        Internal::FillTri_NoRound_RadialGra(&buf, rotateAngle, top, right, left, style.m_color.m_start, style.m_color.m_end, style, drawOrder);
                    }
                }
            }
        }
        else
        {
            if (Math::IsEqual(style.m_color.m_start, style.m_color.m_end))
            {
                // Rounded, single color.
                if (style.m_textureHandle == 0)
                    Internal::FillTri_Round(&Internal::g_rendererData.GetDefaultBuffer(drawOrder), style.m_onlyRoundTheseCorners, rotateAngle, top, right, left, style.m_color.m_start, style.m_rounding, style, drawOrder);
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                    Internal::FillTri_Round(&buf, style.m_onlyRoundTheseCorners, rotateAngle, top, right, left, style.m_color.m_start, style.m_rounding, style, drawOrder);
                }
            }
            else
            {
                // Rounded, gradient.
                if (style.m_textureHandle == 0)
                {
                    GradientDrawBuffer& buf = Internal::g_rendererData.GetGradientBuffer(style.m_color, drawOrder);
                    Internal::FillTri_Round(&buf, style.m_onlyRoundTheseCorners, rotateAngle, top, right, left, style.m_color.m_start, style.m_rounding, style, drawOrder);
                }
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                    Internal::FillTri_Round(&buf, style.m_onlyRoundTheseCorners, rotateAngle, top, right, left, style.m_color.m_start, style.m_rounding, style, drawOrder);
                }
            }
        }
    }

    void DrawRect(const Vec2& min, const Vec2& max, StyleOptions& style, float rotateAngle, int drawOrder)
    {

        if (style.m_rounding == 0.0f)
        {
            // Single color
            if (Math::IsEqual(style.m_color.m_start, style.m_color.m_end))
            {
                if (style.m_textureHandle == 0)
                    Internal::FillRect_NoRound_SC(&Internal::g_rendererData.GetDefaultBuffer(drawOrder), rotateAngle, min, max, style.m_color.m_start, style, drawOrder);
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                    Internal::FillRect_NoRound_SC(&buf, rotateAngle, min, max, style.m_color.m_start, style, drawOrder);
                }
            }
            else
            {
                if (style.m_color.m_gradientType == GradientType::Horizontal)
                {
                    // Horizontal, non rounded
                    if (style.m_textureHandle == 0)
                    {
                        Internal::FillRect_NoRound_VerHorGra(&Internal::g_rendererData.GetDefaultBuffer(drawOrder), rotateAngle, min, max,
                                                             style.m_color.m_start, style.m_color.m_end, style.m_color.m_end, style.m_color.m_start, style, drawOrder);
                    }
                    else
                    {
                        TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                        Internal::FillRect_NoRound_VerHorGra(&buf, rotateAngle, min, max,
                                                             style.m_color.m_start, style.m_color.m_end, style.m_color.m_end, style.m_color.m_start, style, drawOrder);
                    }
                }
                else if (style.m_color.m_gradientType == GradientType::Vertical)
                {
                    // Vertical, non rounded
                    if (style.m_textureHandle == 0)
                    {
                        Internal::FillRect_NoRound_VerHorGra(&Internal::g_rendererData.GetDefaultBuffer(drawOrder), rotateAngle, min, max,
                                                             style.m_color.m_start, style.m_color.m_start, style.m_color.m_end, style.m_color.m_end, style, drawOrder);
                    }
                    else
                    {
                        TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                        Internal::FillRect_NoRound_VerHorGra(&buf, rotateAngle, min, max,
                                                             style.m_color.m_start, style.m_color.m_start, style.m_color.m_end, style.m_color.m_end, style, drawOrder);
                    }
                }
                else if (style.m_color.m_gradientType == GradientType::Radial || style.m_color.m_gradientType == GradientType::RadialCorner)
                {
                    // Radial, non rounded
                    if (style.m_textureHandle == 0)
                    {
                        GradientDrawBuffer& buf = Internal::g_rendererData.GetGradientBuffer(style.m_color, drawOrder);
                        Internal::FillRect_NoRound_RadialGra(&buf, rotateAngle, min, max, style.m_color.m_start, style.m_color.m_end, style, drawOrder);
                    }
                    else
                    {
                        TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                        Internal::FillRect_NoRound_RadialGra(&buf, rotateAngle, min, max, style.m_color.m_start, style.m_color.m_end, style, drawOrder);
                    }
                }
            }
        }
        else
        {
            if (Math::IsEqual(style.m_color.m_start, style.m_color.m_end))
            {
                // Rounded, single color.
                if (style.m_textureHandle == 0)
                {
                    Internal::FillRect_Round(&Internal::g_rendererData.GetDefaultBuffer(drawOrder), style.m_onlyRoundTheseCorners, rotateAngle, min, max, style.m_color.m_start, style.m_rounding, style, drawOrder);
                }
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                    Internal::FillRect_Round(&buf, style.m_onlyRoundTheseCorners, rotateAngle, min, max, style.m_color.m_start, style.m_rounding, style, drawOrder);
                }
            }
            else
            {
                if (style.m_textureHandle == 0)
                {
                    // Rounded, gradient.
                    GradientDrawBuffer& buf = Internal::g_rendererData.GetGradientBuffer(style.m_color, drawOrder);
                    Internal::FillRect_Round(&buf, style.m_onlyRoundTheseCorners, rotateAngle, min, max, style.m_color.m_start, style.m_rounding, style, drawOrder);
                }
                else
                {
                    // Rounded, gradient.
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                    Internal::FillRect_Round(&buf, style.m_onlyRoundTheseCorners, rotateAngle, min, max, style.m_color.m_start, style.m_rounding, style, drawOrder);
                }
            }
        }
    }

    void DrawNGon(const Vec2& center, float radius, int n, StyleOptions& style, float rotateAngle, int drawOrder)
    {
        if (Math::IsEqual(style.m_color.m_start, style.m_color.m_end))
        {
            if (style.m_textureHandle == 0)
                Internal::FillNGon_SC(&Internal::g_rendererData.GetDefaultBuffer(drawOrder), rotateAngle, center, radius, n, style.m_color.m_start, style, drawOrder);
            else
            {
                TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                Internal::FillNGon_SC(&buf, rotateAngle, center, radius, n, style.m_color.m_start, style, drawOrder);
            }
        }
        else
        {
            if (style.m_color.m_gradientType == GradientType::Horizontal)
            {
                // Horizontal, non rounded
                if (style.m_textureHandle == 0)
                    Internal::FillNGon_VerHorGra(&Internal::g_rendererData.GetDefaultBuffer(drawOrder), rotateAngle, center, radius, n, style.m_color.m_start, style.m_color.m_end, true, style, drawOrder);
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                    Internal::FillNGon_VerHorGra(&buf, rotateAngle, center, radius, n, style.m_color.m_start, style.m_color.m_end, true, style, drawOrder);
                }
            }
            else if (style.m_color.m_gradientType == GradientType::Vertical)
            {
                // Vertical, non rounded
                if (style.m_textureHandle == 0)
                    Internal::FillNGon_VerHorGra(&Internal::g_rendererData.GetDefaultBuffer(drawOrder), rotateAngle, center, radius, n, style.m_color.m_start, style.m_color.m_end, false, style, drawOrder);
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                    Internal::FillNGon_VerHorGra(&buf, rotateAngle, center, radius, n, style.m_color.m_start, style.m_color.m_end, false, style, drawOrder);
                }
            }
            else if (style.m_color.m_gradientType == GradientType::Radial || style.m_color.m_gradientType == GradientType::RadialCorner)
            {
                // // Radial, non rounded
                if (style.m_textureHandle == 0)
                {
                    GradientDrawBuffer& buf = Internal::g_rendererData.GetGradientBuffer(style.m_color, drawOrder);
                    Internal::FillNGon_RadialGra(&buf, rotateAngle, center, radius, n, style.m_color.m_start, style.m_color.m_end, style, drawOrder);
                }
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                    Internal::FillNGon_RadialGra(&buf, rotateAngle, center, radius, n, style.m_color.m_start, style.m_color.m_end, style, drawOrder);
                }
            }
        }
    }

    void DrawConvex(Vec2* points, int size, StyleOptions& style, float rotateAngle, int drawOrder)
    {
        if (size < 3)
        {
            Config.m_errorCallback("Can draw a convex shape that has less than 3 corners!", 0);
            return;
        }

        Vec2 avgCenter = Math::GetPolygonCentroidFast(points, size);

        if (Math::IsEqual(style.m_color.m_start, style.m_color.m_end))
        {
            if (style.m_textureHandle == 0)
                Internal::FillConvex_SC(&Internal::g_rendererData.GetDefaultBuffer(drawOrder), rotateAngle, points, size, avgCenter, style.m_color.m_start, style, drawOrder);
            else
            {
                TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                Internal::FillConvex_SC(&buf, rotateAngle, points, size, avgCenter, style.m_color.m_start, style, drawOrder);
            }
        }
        else
        {
            if (style.m_color.m_gradientType == GradientType::Horizontal)
            {
                // Horizontal, non rounded
                if (style.m_textureHandle == 0)
                    Internal::FillConvex_VerHorGra(&Internal::g_rendererData.GetDefaultBuffer(drawOrder), rotateAngle, points, size, avgCenter, style.m_color.m_start, style.m_color.m_end, true, style, drawOrder);
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                    Internal::FillConvex_VerHorGra(&buf, rotateAngle, points, size, avgCenter, style.m_color.m_start, style.m_color.m_end, true, style, drawOrder);
                }
            }
            else if (style.m_color.m_gradientType == GradientType::Vertical)
            {
                // Vertical, non rounded
                if (style.m_textureHandle == 0)
                    Internal::FillConvex_VerHorGra(&Internal::g_rendererData.GetDefaultBuffer(drawOrder), rotateAngle, points, size, avgCenter, style.m_color.m_start, style.m_color.m_end, false, style, drawOrder);
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                    Internal::FillConvex_VerHorGra(&buf, rotateAngle, points, size, avgCenter, style.m_color.m_start, style.m_color.m_end, false, style, drawOrder);
                }
            }
            else if (style.m_color.m_gradientType == GradientType::Radial || style.m_color.m_gradientType == GradientType::RadialCorner)
            {
                // // Radial, non rounded
                if (style.m_textureHandle == 0)
                {
                    GradientDrawBuffer& buf = Internal::g_rendererData.GetGradientBuffer(style.m_color, drawOrder);
                    Internal::FillConvex_RadialGra(&buf, rotateAngle, points, size, avgCenter, style.m_color.m_start, style.m_color.m_end, style, drawOrder);
                }
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                    Internal::FillConvex_RadialGra(&buf, rotateAngle, points, size, avgCenter, style.m_color.m_start, style.m_color.m_end, style, drawOrder);
                }
            }
        }
    }

    void DrawCircle(const Vec2& center, float radius, StyleOptions& style, int segments, float rotateAngle, float startAngle, float endAngle, int drawOrder)
    {
        if (startAngle == endAngle)
        {
            endAngle = startAngle + 360.0f;
        }
        if (Math::IsEqual(style.m_color.m_start, style.m_color.m_end))
        {
            if (style.m_textureHandle == 0)
                Internal::FillCircle_SC(&Internal::g_rendererData.GetDefaultBuffer(drawOrder), rotateAngle, center, radius, segments, style.m_color.m_start, startAngle, endAngle, style, drawOrder);
            else
            {
                TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                Internal::FillCircle_SC(&buf, rotateAngle, center, radius, segments, style.m_color.m_start, startAngle, endAngle, style, drawOrder);
            }
        }
        else
        {
            if (style.m_color.m_gradientType == GradientType::Horizontal)
            {
                // Horizontal, non rounded
                if (style.m_textureHandle == 0)
                    Internal::FillCircle_VerHorGra(&Internal::g_rendererData.GetDefaultBuffer(drawOrder), rotateAngle, center, radius, segments, style.m_color.m_start, style.m_color.m_end, true, startAngle, endAngle, style, drawOrder);
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                    Internal::FillCircle_VerHorGra(&buf, rotateAngle, center, radius, segments, style.m_color.m_start, style.m_color.m_end, true, startAngle, endAngle, style, drawOrder);
                }
            }
            else if (style.m_color.m_gradientType == GradientType::Vertical)
            {
                // Vertical, non rounded
                if (style.m_textureHandle == 0)
                    Internal::FillCircle_VerHorGra(&Internal::g_rendererData.GetDefaultBuffer(drawOrder), rotateAngle, center, radius, segments, style.m_color.m_start, style.m_color.m_end, false, startAngle, endAngle, style, drawOrder);
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                    Internal::FillCircle_VerHorGra(&buf, rotateAngle, center, radius, segments, style.m_color.m_start, style.m_color.m_end, false, startAngle, endAngle, style, drawOrder);
                }
            }
            else if (style.m_color.m_gradientType == GradientType::Radial || style.m_color.m_gradientType == GradientType::RadialCorner)
            {
                // // Radial, non rounded
                if (style.m_textureHandle == 0)
                {
                    GradientDrawBuffer& buf = Internal::g_rendererData.GetGradientBuffer(style.m_color, drawOrder);
                    Internal::FillCircle_RadialGra(&buf, rotateAngle, center, radius, segments, style.m_color.m_start, style.m_color.m_end, startAngle, endAngle, style, drawOrder);
                }
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.m_textureHandle, style.m_textureUVTiling, style.m_textureUVOffset, drawOrder);
                    Internal::FillCircle_RadialGra(&buf, rotateAngle, center, radius, segments, style.m_color.m_start, style.m_color.m_end, startAngle, endAngle, style, drawOrder);
                }
            }
        }
    }

    void ConvexFillVertices(int startIndex, int endIndex, Array<Index>& indices, bool skipLastTriangle)
    {
        // i = 0 is center.
        for (int i = startIndex + 1; i < endIndex; i++)
        {
            indices.push_back(startIndex);
            indices.push_back(i);
            indices.push_back(i + 1);
        }

        // Last fill.
        if (!skipLastTriangle)
        {
            indices.push_back(startIndex);
            indices.push_back(startIndex + 1);
            indices.push_back(endIndex);
        }
    }

    void ConvexExtrudeVertices(DrawBuffer* buf, const Vec2& center, int startIndex, int endIndex, float thickness, bool skipEndClosing)
    {
        const int totalSize = endIndex - startIndex + 1;
        thickness *= Config.m_framebufferScale.x;

        // Extrude vertices.
        for (int i = startIndex; i < startIndex + totalSize; i++)
        {
            // take two edges, this vertex to next and previous to this in order to calculate vertex normals.
            const int previous = i == startIndex ? endIndex : i - 1;
            const int next     = i == endIndex ? startIndex : i + 1;
            Vertex    v;
            v.m_col = buf->m_vertexBuffer[i].m_col;
            if (skipEndClosing && i == startIndex)
            {
                const Vec2 toNext  = Math::Normalized(Vec2(buf->m_vertexBuffer[next].m_pos.x - buf->m_vertexBuffer[i].m_pos.x, buf->m_vertexBuffer[next].m_pos.y - buf->m_vertexBuffer[i].m_pos.y));
                const Vec2 rotated = Math::Rotate90(toNext, true);
                v.m_pos            = Vec2(buf->m_vertexBuffer[i].m_pos.x + rotated.x * thickness, buf->m_vertexBuffer[i].m_pos.y + rotated.y * thickness);
            }
            else if (skipEndClosing && i == endIndex)
            {
                const Vec2 fromPrev = Math::Normalized(Vec2(buf->m_vertexBuffer[i].m_pos.x - buf->m_vertexBuffer[previous].m_pos.x, buf->m_vertexBuffer[i].m_pos.y - buf->m_vertexBuffer[previous].m_pos.y));
                const Vec2 rotated  = Math::Rotate90(fromPrev, true);
                v.m_pos             = Vec2(buf->m_vertexBuffer[i].m_pos.x + rotated.x * thickness, buf->m_vertexBuffer[i].m_pos.y + rotated.y * thickness);
            }
            else
            {
                const Vec2 vertexNormalAverage = Math::GetVertexNormal(buf->m_vertexBuffer[i].m_pos, buf->m_vertexBuffer[previous].m_pos, buf->m_vertexBuffer[next].m_pos);
                v.m_pos                        = Vec2(buf->m_vertexBuffer[i].m_pos.x + vertexNormalAverage.x * thickness, buf->m_vertexBuffer[i].m_pos.y + vertexNormalAverage.y * thickness);
            }

            buf->PushVertex(v);
        }

        Internal::CalculateVertexUVs(buf, startIndex, endIndex + totalSize);

        for (int i = startIndex; i < startIndex + totalSize; i++)
        {
            int next = i + 1;
            if (next >= startIndex + totalSize)
                next = startIndex;

            if (skipEndClosing && i == startIndex + totalSize - 1)
                return;

            buf->PushIndex(i);
            buf->PushIndex(next);
            buf->PushIndex(i + totalSize);
            buf->PushIndex(next);
            buf->PushIndex(next + totalSize);
            buf->PushIndex(i + totalSize);
        }
    }

    void RotateVertices(Array<Vertex>& vertices, const Vec2& center, int startIndex, int endIndex, float angle)
    {
        for (int i = startIndex; i < endIndex + 1; i++)
        {
            vertices[i].m_pos = Math::RotateAround(vertices[i].m_pos, center, angle);
        }
    }

    void RotatePoints(Vec2* points, int size, const Vec2& center, float angle)
    {
        for (int i = 0; i < size; i++)
        {
            points[i] = Math::RotateAround(points[i], center, angle);
        }
    }

    void GetArcPoints(Array<Vec2>& points, const Vec2& p1, const Vec2& p2, Vec2 directionHintPoint, float radius, float segments, bool flip, float angleOffset)
    {
        const float halfMag = Math::Mag(Vec2(p2.x - p1.x, p2.y - p1.y)) / 2.0f;
        const Vec2  center  = Vec2((p1.x + p2.x) / 2.0f, (p1.y + p2.y) / 2.0f);
        const Vec2  dir     = Vec2(p2.x - p1.x, p2.y - p1.y);

        // Determine flip if we have a hint point.
        if (!Math::IsEqual(directionHintPoint, Vec2(-1.0f, -1.0f)))
        {
            if (p1.x - p2.x == 0.0f)
            {
                // Flipped Y axis, if p1 is above.
                if (p1.y < p2.y)
                {
                    if (directionHintPoint.x < p1.x)
                        flip = true;
                }
                else
                {
                    // if p2 is above.
                    if (directionHintPoint.x > p1.x)
                        flip = true;
                }
            }
            else
            {
                const Vec2 centerToDirHint = Vec2(directionHintPoint.x - center.x, directionHintPoint.y - center.y);
                // p2 is on the right, p1 on the left
                if (p2.x > p1.x)
                {
                    if (centerToDirHint.y > 0.0f)
                        flip = true;
                    else if (centerToDirHint.y == 0.0f)
                    {
                        if (centerToDirHint.x < 0.0f)
                            flip = true;
                    }
                }
                else
                {
                    // p2 is on the left, p1 is on the right.
                    if (centerToDirHint.y < 0.0f)
                        flip = true;
                    else if (centerToDirHint.y == 0.0f)
                    {
                        if (centerToDirHint.x > 0.0f)
                            flip = true;
                    }
                }
            }
        }

        float angle1 = Math::GetAngleFromCenter(center, flip ? p2 : p1);
        float angle2 = Math::GetAngleFromCenter(center, flip ? p1 : p2);

        if (angleOffset == 0.0f)
            points.push_back(flip ? p2 : p1);

        if (angle2 < angle1)
            angle2 += 360.0f;

        const float midAngle      = (angle2 + angle1) / 2.0f;
        const float angleIncrease = (segments >= 180.0f || segments < 0.0f) ? 1.0f : 180.0f / (float)segments;

        for (float i = angle1 + angleIncrease + angleOffset; i < angle2 - angleOffset; i += angleIncrease)
        {
            Vec2 p = Vec2(0, 0);

            if (radius == 0.0f)
                p = Math::GetPointOnCircle(center, halfMag, i);
            else
            {
                const Vec2 out = Math::Normalized(Math::Rotate90(dir, !flip));
                p              = Math::SampleParabola(p1, p2, out, radius, Math::Remap(i, angle1, angle2, 0.0f, 1.0f));
            }

            points.push_back(p);
        }
    }

    void Internal::FillRect_NoRound_VerHorGra(DrawBuffer* buf, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& colorTL, const Vec4& colorTR, const Vec4& colorBR, const Vec4& colorBL, StyleOptions& opts, int drawOrder)
    {
        Vertex v[4];
        FillRectData(v, false, min, max);
        const int current = buf->m_vertexBuffer.m_size;
        v[0].m_col        = colorTL;
        v[1].m_col        = colorTR;
        v[2].m_col        = colorBR;
        v[3].m_col        = colorBL;

        for (int i = 0; i < 4; i++)
            buf->PushVertex(v[i]);

        const Vec2 center = Vec2((min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f);

        if (opts.m_isFilled)
        {
            buf->PushIndex(current);
            buf->PushIndex(current + 1);
            buf->PushIndex(current + 3);
            buf->PushIndex(current + 1);
            buf->PushIndex(current + 2);
            buf->PushIndex(current + 3);
        }
        else
            ConvexExtrudeVertices(buf, center, current, current + 3, opts.m_thickness.m_start);

        RotateVertices(buf->m_vertexBuffer, center, current, opts.m_isFilled ? current + 3 : current + 7, rotateAngle);

        if (opts.m_outlineOptions.m_thickness != 0.0f)
        {
            buf = DrawOutline(buf, opts, opts.m_isFilled ? 4 : 8, false, drawOrder);
        }
        else if (Config.m_enableAA)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.m_outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.m_isFilled ? 4 : 8, false, drawOrder, true);
        }
    }

    void Internal::FillRect_NoRound_SC(DrawBuffer* buf, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& color, StyleOptions& opts, int drawOrder)
    {
        Vertex v[4];
        FillRectData(v, false, min, max);
        v[0].m_col = color;
        v[1].m_col = color;
        v[2].m_col = color;
        v[3].m_col = color;

        const int current = buf->m_vertexBuffer.m_size;

        for (int i = 0; i < 4; i++)
            buf->PushVertex(v[i]);

        const Vec2 center = Vec2((min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f);

        if (opts.m_isFilled)
        {
            buf->PushIndex(current);
            buf->PushIndex(current + 1);
            buf->PushIndex(current + 3);
            buf->PushIndex(current + 1);
            buf->PushIndex(current + 2);
            buf->PushIndex(current + 3);
        }
        else
            ConvexExtrudeVertices(buf, center, current, current + 3, opts.m_thickness.m_start);

        RotateVertices(buf->m_vertexBuffer, center, current, opts.m_isFilled ? current + 3 : current + 7, rotateAngle);

        if (opts.m_outlineOptions.m_thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.m_isFilled ? 4 : 8, false, drawOrder);
        else if (Config.m_enableAA)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.m_outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.m_isFilled ? 4 : 8, false, drawOrder, true);
        }
    }

    void Internal::FillRect_NoRound_RadialGra(DrawBuffer* buf, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& startColor, const Vec4& endColor, StyleOptions& opts, int drawOrder)
    {
        Vertex v[5];
        FillRectData(v, true, min, max);
        int startIndex = buf->m_vertexBuffer.m_size;

        const int loopStart = opts.m_isFilled ? 0 : 1;
        for (int i = loopStart; i < 5; i++)
            buf->PushVertex(v[i]);

        const Vec2 center = Vec2((min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f);

        if (opts.m_isFilled)
            ConvexFillVertices(startIndex, startIndex + 4, buf->m_indexBuffer);
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + 3, opts.m_thickness.m_start);

        RotateVertices(buf->m_vertexBuffer, center, opts.m_isFilled ? startIndex + 1 : startIndex, opts.m_isFilled ? startIndex + 4 : startIndex + 7, rotateAngle);

        if (opts.m_outlineOptions.m_thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.m_isFilled ? 4 : 8, false, drawOrder);
        else if (Config.m_enableAA)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.m_outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.m_isFilled ? 4 : 8, false, drawOrder, true);
        }
    }

    void Internal::FillRect_Round(DrawBuffer* buf, Array<int>& roundedCorners, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& col, float rounding, StyleOptions& opts, int drawOrder)
    {
        rounding = Math::Clamp(rounding, 0.0f, 0.9f);

        Vertex v[4];
        FillRectData(v, false, min, max);
        v[0].m_col = col;
        v[1].m_col = col;
        v[2].m_col = col;
        v[3].m_col = col;

        // Calculate direction vectors
        const Vec2  center           = Vec2((min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f);
        Vec2        up               = Vec2(v[0].m_pos.x - v[3].m_pos.x, v[0].m_pos.y - v[3].m_pos.y);
        Vec2        right            = Vec2(v[1].m_pos.x - v[0].m_pos.x, v[1].m_pos.y - v[0].m_pos.y);
        const float verticalMag      = Math::Mag(up);
        const float horizontalMag    = Math::Mag(right);
        const float halfShortestSide = ((verticalMag > horizontalMag ? horizontalMag : verticalMag) / 2.0f); // 0.9f is internal offset just to make sure we never get to full
        up                           = Math::Normalized(up);
        right                        = Math::Normalized(right);

        // Max rounding.
        const float roundingMag = rounding * halfShortestSide;

        // For each corner vertices, first we inflate them towards the center by the magnitude.
        float       startAngle    = 180.0f;
        float       endAngle      = 270.0f;
        const float angleIncrease = GetAngleIncrease(rounding);
        const int   startIndex    = buf->m_vertexBuffer.m_size;
        int         vertexCount   = 0;

        // Eventually we will be filling a convex shape, so we need a center vertex, it's position is already "center",
        // now calculate it's vertex color & push it down the pipeline.
        if (opts.m_isFilled)
        {
            Vertex c;
            c.m_pos = center;
            c.m_col = col;
            c.m_uv  = Vec2(0.5f, 0.5f);
            buf->PushVertex(c);
        }

        for (int i = 0; i < 4; i++)
        {
            const int found = roundedCorners.find(i);
            if (roundedCorners.m_size != 0 && found == -1)
            {
                Vertex cornerVertex;
                cornerVertex.m_pos = v[i].m_pos;
                cornerVertex.m_col = col;
                cornerVertex.m_uv  = v[i].m_uv;
                buf->PushVertex(cornerVertex);
                vertexCount++;
                startAngle += 90.0f;
                endAngle += 90.0f;
                continue;
            }

            const Vec2 usedRight = (i == 0 || i == 3) ? right : Vec2(-right.x, -right.y);
            const Vec2 usedUp    = (i == 0 || i == 1) ? Vec2(-up.x, -up.y) : up;
            Vec2       inf0      = Vec2(v[i].m_pos.x + usedUp.x * roundingMag, v[i].m_pos.y + usedUp.y * roundingMag);
            Vec2       inf1      = Vec2(inf0.x + usedRight.x * roundingMag, inf0.y + usedRight.y * roundingMag);

            // After a vector is inflated, we use it as a center to draw an arc, arc range is based on which corner we are currently drawing.
            for (float k = startAngle; k < endAngle + 2.5f; k += angleIncrease)
            {
                const Vec2 p = Math::GetPointOnCircle(inf1, roundingMag, k);

                Vertex cornerVertex;
                cornerVertex.m_pos = p;
                cornerVertex.m_col = col;
                buf->PushVertex(cornerVertex);
                vertexCount++;
            }
            startAngle += 90.0f;
            endAngle += 90.0f;
        }

        if (opts.m_isFilled)
        {
            CalculateVertexUVs(buf, startIndex, opts.m_isFilled ? startIndex + vertexCount : startIndex + vertexCount - 1);
            ConvexFillVertices(startIndex, startIndex + vertexCount, buf->m_indexBuffer);
        }
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + vertexCount - 1, opts.m_thickness.m_start);

        RotateVertices(buf->m_vertexBuffer, center, opts.m_isFilled ? startIndex + 1 : startIndex, opts.m_isFilled ? startIndex + vertexCount : startIndex + (vertexCount * 2) - 1, rotateAngle);

        if (opts.m_outlineOptions.m_thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.m_isFilled ? vertexCount : vertexCount * 2, false, drawOrder);
        else if (Config.m_enableAA)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.m_outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.m_isFilled ? vertexCount : vertexCount * 2, false, drawOrder, true);
        }
    }

    void Internal::FillRectData(Vertex* v, bool hasCenter, const Vec2& min, const Vec2& max)
    {
        const int  i      = hasCenter ? 1 : 0;
        const Vec2 center = Vec2((max.x + min.x) / 2.0f, (max.y + min.y) / 2.0f);

        if (hasCenter)
        {
            v[0].m_pos = center;
            v[0].m_uv  = Vec2(0.5f, 0.5f);
        }

        if (!g_rectOverrideData.overrideRectPositions)
        {
            v[i].m_pos       = min;
            v[i + 1].m_pos.x = max.x;
            v[i + 1].m_pos.y = min.y;
            v[i + 2].m_pos.x = max.x;
            v[i + 2].m_pos.y = max.y;
            v[i + 3].m_pos.x = min.x;
            v[i + 3].m_pos.y = max.y;
        }
        else
        {
            v[i].m_pos     = g_rectOverrideData.m_p1;
            v[i + 1].m_pos = g_rectOverrideData.m_p2;
            v[i + 2].m_pos = g_rectOverrideData.m_p3;
            v[i + 3].m_pos = g_rectOverrideData.m_p4;
        }

        v[i].m_uv     = Vec2(0.0f, 0.0f);
        v[i + 1].m_uv = Vec2(1.0f, 0.0f);
        v[i + 2].m_uv = Vec2(1.0f, 1.0f);
        v[i + 3].m_uv = Vec2(0.0f, 1.0f);
    }

    void Internal::FillTri_NoRound_VerHorGra(DrawBuffer* buf, float rotateAngle, const Vec2& p3, const Vec2& p2, const Vec2& p1, const Vec4& colorLeft, const Vec4& colorRight, const Vec4& colorTop, StyleOptions& opts, int drawOrder)
    {
        Vertex v[3];
        FillTriData(v, false, true, p3, p2, p1);
        v[0].m_col = colorTop;
        v[1].m_col = colorRight;
        v[2].m_col = colorLeft;

        const int startIndex = buf->m_vertexBuffer.m_size;

        for (int i = 0; i < 3; i++)
            buf->PushVertex(v[i]);

        const Vec2 center = Vec2((p1.x + p2.x + p3.x) / 3.0f, (p1.y + p2.y + p3.y) / 3.0f);

        if (opts.m_isFilled)
        {
            buf->PushIndex(startIndex);
            buf->PushIndex(startIndex + 1);
            buf->PushIndex(startIndex + 2);
        }
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + 2, opts.m_thickness.m_start);

        RotateVertices(buf->m_vertexBuffer, center, startIndex, opts.m_isFilled ? startIndex + 2 : startIndex + 5, rotateAngle);

        if (opts.m_outlineOptions.m_thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.m_isFilled ? 3 : 6, false, drawOrder);
        else if (Config.m_enableAA)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.m_outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.m_isFilled ? 3 : 6, false, drawOrder, true);
        }
    }

    void Internal::FillTri_NoRound_SC(DrawBuffer* buf, float rotateAngle, const Vec2& p3, const Vec2& p2, const Vec2& p1, const Vec4& color, StyleOptions& opts, int drawOrder)
    {
        Vertex v[3];
        FillTriData(v, false, true, p3, p2, p1);
        v[0].m_col = color;
        v[1].m_col = color;
        v[2].m_col = color;

        const int startIndex = buf->m_vertexBuffer.m_size;

        for (int i = 0; i < 3; i++)
            buf->PushVertex(v[i]);

        const Vec2 center = Vec2((p1.x + p2.x + p3.x) / 3.0f, (p1.y + p2.y + p3.y) / 3.0f);

        if (opts.m_isFilled)
        {
            buf->PushIndex(startIndex);
            buf->PushIndex(startIndex + 1);
            buf->PushIndex(startIndex + 2);
        }
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + 2, opts.m_thickness.m_start);

        RotateVertices(buf->m_vertexBuffer, center, startIndex, opts.m_isFilled ? startIndex + 2 : startIndex + 5, rotateAngle);

        if (opts.m_outlineOptions.m_thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.m_isFilled ? 3 : 6, false, drawOrder);
        else if (Config.m_enableAA)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.m_outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.m_isFilled ? 3 : 6, false, drawOrder, true);
        }
    }

    void Internal::FillTri_NoRound_RadialGra(DrawBuffer* buf, float rotateAngle, const Vec2& p3, const Vec2& p2, const Vec2& p1, const Vec4& startcolor, const Vec4& endColor, StyleOptions& opts, int drawOrder)
    {
        Vertex v[4];
        Vec2   points[3] = {p1, p2, p3};
        FillTriData(v, true, true, p3, p2, p1);
        int startIndex = buf->m_vertexBuffer.m_size;

        const int loopStart = opts.m_isFilled ? 0 : 1;
        for (int i = loopStart; i < 4; i++)
            buf->PushVertex(v[i]);

        const Vec2 center = v[0].m_pos;

        if (opts.m_isFilled)
            ConvexFillVertices(startIndex, startIndex + 3, buf->m_indexBuffer);
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + 2, opts.m_thickness.m_start);

        RotateVertices(buf->m_vertexBuffer, center, opts.m_isFilled ? startIndex + 1 : startIndex, opts.m_isFilled ? startIndex + 3 : startIndex + 5, rotateAngle);

        if (opts.m_outlineOptions.m_thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.m_isFilled ? 3 : 6, false, drawOrder);
        else if (Config.m_enableAA)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.m_outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.m_isFilled ? 3 : 6, false, drawOrder, true);
        }
    }

    void Internal::FillTri_Round(DrawBuffer* buf, Array<int>& onlyRoundCorners, float rotateAngle, const Vec2& p3, const Vec2& p2, const Vec2& p1, const Vec4& col, float rounding, StyleOptions& opts, int drawOrder)
    {
        rounding = Math::Clamp(rounding, 0.0f, 1.0f);

        Vertex v[3];
        FillTriData(v, false, false, p3, p2, p1);
        v[0].m_col = col;
        v[1].m_col = col;
        v[2].m_col = col;

        Vec2 bbMin, bbMax;
        GetTriangleBoundingBox(p1, p2, p3, bbMin, bbMax);
        const Vec2  center          = Vec2((p1.x + p2.x + p3.x) / 3.0f, (p1.y + p2.y + p3.y) / 3.0f);
        const Vec2  v01Edge         = Vec2(v[0].m_pos.x - v[1].m_pos.x, v[0].m_pos.y - v[1].m_pos.y);
        const Vec2  v02Edge         = Vec2(v[0].m_pos.x - v[2].m_pos.x, v[0].m_pos.y - v[2].m_pos.y);
        const Vec2  v12Edge         = Vec2(v[1].m_pos.x - v[2].m_pos.x, v[1].m_pos.y - v[2].m_pos.y);
        const Vec2  v01Center       = Vec2((v[0].m_pos.x + v[1].m_pos.x) / 2.0f, (v[0].m_pos.y + v[1].m_pos.y) / 2.0f);
        const Vec2  v02Center       = Vec2((v[0].m_pos.x + v[2].m_pos.x) / 2.0f, (v[0].m_pos.y + v[2].m_pos.y) / 2.0f);
        const Vec2  v12Center       = Vec2((v[1].m_pos.x + v[2].m_pos.x) / 2.0f, (v[1].m_pos.y + v[2].m_pos.y) / 2.0f);
        const float mag01           = Math::Mag(v01Edge);
        const float mag02           = Math::Mag(v02Edge);
        const float mag12           = Math::Mag(v12Edge);
        const float ang0102         = Math::GetAngleBetween(v01Edge, v02Edge);
        const float ang0112         = Math::GetAngleBetween(v01Edge, v12Edge);
        const float ang0212         = Math::GetAngleBetween(v02Edge, v12Edge);
        const float maxAngle        = Math::Max(Math::Max(ang0102, ang0112), ang0212);
        const float shortestEdgeMag = Math::Min(Math::Min(mag01, mag02), mag12);
        const float roundingMag     = rounding * shortestEdgeMag / 2.0f;

        // For each corner vertices, first we inflate them towards the center by the magnitude.
        const int startIndex  = buf->m_vertexBuffer.m_size;
        int       vertexCount = 0;

        // Eventually we will be filling a convex shape, so we need a center vertex, it's position is already "center",
        // now calculate it's vertex color & push it down the pipeline.
        if (opts.m_isFilled)
        {
            Vertex c;
            c.m_pos  = center;
            c.m_col  = col;
            c.m_uv.x = Math::Remap(c.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            c.m_uv.y = Math::Remap(c.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            buf->PushVertex(c);
        }

        const float angleOffset = maxAngle > 90.0f ? maxAngle - 90.0f : 45.0f;

        for (int i = 0; i < 3; i++)
        {

            if (onlyRoundCorners.m_size != 0 && onlyRoundCorners.find(i) == -1)
            {
                Vertex cornerVertex;
                cornerVertex.m_col = col;
                cornerVertex.m_pos = v[i].m_pos;
                vertexCount++;
                buf->PushVertex(cornerVertex);
                continue;
            }

            if (i == 0)
            {
                const Vec2  toCenter01 = Math::Normalized(Vec2(v01Center.x - v[i].m_pos.x, v01Center.y - v[i].m_pos.y));
                const Vec2  toCenter02 = Math::Normalized(Vec2(v02Center.x - v[i].m_pos.x, v02Center.y - v[i].m_pos.y));
                const Vec2  inter1     = Vec2(v[i].m_pos.x + toCenter01.x * roundingMag, v[i].m_pos.y + toCenter01.y * roundingMag);
                const Vec2  inter2     = Vec2(v[i].m_pos.x + toCenter02.x * roundingMag, v[i].m_pos.y + toCenter02.y * roundingMag);
                Array<Vec2> arc;

                GetArcPoints(arc, inter1, inter2, v[i].m_pos, 0.0f, 36, false, angleOffset);

                for (int j = 0; j < arc.m_size; j++)
                {
                    Vertex cornerVertex;
                    cornerVertex.m_col = col;
                    cornerVertex.m_pos = arc[j];
                    buf->PushVertex(cornerVertex);
                    vertexCount++;
                }
            }
            else if (i == 1)
            {

                const Vec2  toCenter01 = Math::Normalized(Vec2(v01Center.x - v[i].m_pos.x, v01Center.y - v[i].m_pos.y));
                const Vec2  toCenter12 = Math::Normalized(Vec2(v12Center.x - v[i].m_pos.x, v12Center.y - v[i].m_pos.y));
                const Vec2  inter1     = Vec2(v[i].m_pos.x + toCenter01.x * roundingMag, v[i].m_pos.y + toCenter01.y * roundingMag);
                const Vec2  inter2     = Vec2(v[i].m_pos.x + toCenter12.x * roundingMag, v[i].m_pos.y + toCenter12.y * roundingMag);
                Array<Vec2> arc;
                GetArcPoints(arc, inter1, inter2, v[i].m_pos, 0.0f, 36, false, angleOffset);
                for (int j = 0; j < arc.m_size; j++)
                {

                    Vertex cornerVertex;
                    cornerVertex.m_col = col;
                    cornerVertex.m_pos = arc[j];
                    buf->PushVertex(cornerVertex);
                    vertexCount++;
                }
            }
            else if (i == 2)
            {

                const Vec2  toCenter12 = Math::Normalized(Vec2(v12Center.x - v[i].m_pos.x, v12Center.y - v[i].m_pos.y));
                const Vec2  toCenter02 = Math::Normalized(Vec2(v02Center.x - v[i].m_pos.x, v02Center.y - v[i].m_pos.y));
                const Vec2  inter1     = Vec2(v[i].m_pos.x + toCenter12.x * roundingMag, v[i].m_pos.y + toCenter12.y * roundingMag);
                const Vec2  inter2     = Vec2(v[i].m_pos.x + toCenter02.x * roundingMag, v[i].m_pos.y + toCenter02.y * roundingMag);
                Array<Vec2> arc;
                GetArcPoints(arc, inter1, inter2, v[i].m_pos, 0.0f, 36, false, angleOffset);
                for (int j = 0; j < arc.m_size; j++)
                {

                    Vertex cornerVertex;
                    cornerVertex.m_col = col;
                    cornerVertex.m_pos = arc[j];
                    buf->PushVertex(cornerVertex);
                    vertexCount++;
                }
            }
        }

        if (opts.m_isFilled)
        {
            CalculateVertexUVs(buf, startIndex, opts.m_isFilled ? startIndex + vertexCount : startIndex + vertexCount - 1);
            ConvexFillVertices(startIndex, startIndex + vertexCount, buf->m_indexBuffer);
        }
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + vertexCount - 1, opts.m_thickness.m_start);

        RotateVertices(buf->m_vertexBuffer, center, opts.m_isFilled ? startIndex + 1 : startIndex, opts.m_isFilled ? startIndex + vertexCount : startIndex + (vertexCount * 2) - 1, rotateAngle);

        if (opts.m_outlineOptions.m_thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.m_isFilled ? vertexCount : vertexCount * 2, false, drawOrder);
        else if (Config.m_enableAA)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.m_outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.m_isFilled ? vertexCount : vertexCount * 2, false, drawOrder, true);
        }
    }

    void Internal::FillTriData(Vertex* v, bool hasCenter, bool calculateUV, const Vec2& p3, const Vec2& p2, const Vec2& p1)
    {
        const int i = hasCenter ? 1 : 0;
        if (hasCenter)
        {
            Vec2 center = Vec2((p1.x + p2.x + p3.x) / 3.0f, (p1.y + p2.y + p3.y) / 3.0f);
            v[0].m_pos  = center;
        }

        v[i].m_pos     = p3;
        v[i + 1].m_pos = p2;
        v[i + 2].m_pos = p1;

        if (calculateUV)
        {
            Vec2 min, max;
            GetTriangleBoundingBox(p1, p2, p3, min, max);

            const int end = hasCenter ? 4 : 3;

            for (int i = 0; i < end; i++)
            {
                v[i].m_uv.x = Math::Remap(v[i].m_pos.x, min.x, max.x, 0.0f, 1.0f);
                v[i].m_uv.y = Math::Remap(v[i].m_pos.y, min.y, max.y, 0.0f, 1.0f);
            }
        }
    }

    void Internal::FillNGon_SC(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int n, const Vec4& color, StyleOptions& opts, int drawOrder)
    {
        Array<Vertex> v;
        FillNGonData(v, opts.m_isFilled, center, radius, n);

        const int startIndex = buf->m_vertexBuffer.m_size;

        for (int i = 0; i < v.m_size; i++)
        {
            v[i].m_col = color;
            buf->PushVertex(v[i]);
        }

        if (opts.m_isFilled)
            ConvexFillVertices(startIndex, startIndex + n, buf->m_indexBuffer);
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + n - 1, opts.m_thickness.m_start);

        RotateVertices(buf->m_vertexBuffer, center, opts.m_isFilled ? startIndex + 1 : startIndex, opts.m_isFilled ? startIndex + n : startIndex + (n * 2) - 1, rotateAngle);

        if (opts.m_outlineOptions.m_thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.m_isFilled ? n : n * 2, false, drawOrder);
        else if (Config.m_enableAA)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.m_outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.m_isFilled ? n : n * 2, false, drawOrder, true);
        }
    }

    void Internal::FillNGon_VerHorGra(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int n, const Vec4& colorStart, const Vec4& colorEnd, bool isHor, StyleOptions& opts, int drawOrder)
    {
        Array<Vertex> v;
        FillNGonData(v, opts.m_isFilled, center, radius, n);

        const int startIndex = buf->m_vertexBuffer.m_size;

        for (int i = 0; i < v.m_size; i++)
        {
            v[i].m_col = Math::Lerp(colorStart, colorEnd, isHor ? v[i].m_uv.x : v[i].m_uv.y);
            buf->PushVertex(v[i]);
        }

        if (opts.m_isFilled)
            ConvexFillVertices(startIndex, startIndex + n, buf->m_indexBuffer);
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + n - 1, opts.m_thickness.m_start);

        RotateVertices(buf->m_vertexBuffer, center, opts.m_isFilled ? startIndex + 1 : startIndex, opts.m_isFilled ? startIndex + n : startIndex + (n * 2) - 1, rotateAngle);

        if (opts.m_outlineOptions.m_thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.m_isFilled ? n : n * 2, false, drawOrder);
        else if (Config.m_enableAA)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.m_outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.m_isFilled ? n : n * 2, false, drawOrder, true);
        }
    }

    void Internal::FillNGon_RadialGra(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int n, const Vec4& colorStart, const Vec4& colorEnd, StyleOptions& opts, int drawOrder)
    {
        Array<Vertex> v;
        FillNGonData(v, opts.m_isFilled, center, radius, n);

        const int startIndex = buf->m_vertexBuffer.m_size;

        for (int i = 0; i < v.m_size; i++)
        {
            buf->PushVertex(v[i]);
        }

        if (opts.m_isFilled)
            ConvexFillVertices(startIndex, startIndex + n, buf->m_indexBuffer);
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + n - 1, opts.m_thickness.m_start);

        RotateVertices(buf->m_vertexBuffer, center, opts.m_isFilled ? startIndex + 1 : startIndex, opts.m_isFilled ? startIndex + n : startIndex + (n * 2) - 1, rotateAngle);

        if (opts.m_outlineOptions.m_thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.m_isFilled ? n : n * 2, false, drawOrder);
        else if (Config.m_enableAA)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.m_outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.m_isFilled ? n : n * 2, false, drawOrder, true);
        }
    }

    void Internal::FillNGonData(Array<Vertex>& vertArray, bool hasCenter, const Vec2& center, float radius, int n)
    {
        const float angleIncrease = 360.0f / (float)n;
        const Vec2  bbMin         = Vec2(center.x - radius, center.y - radius);
        const Vec2  bbMax         = Vec2(center.x + radius, center.y + radius);
        if (hasCenter)
        {
            Vertex v;
            v.m_pos  = center;
            v.m_uv.x = Math::Remap(v.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            v.m_uv.y = Math::Remap(v.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            vertArray.push_back(v);
        }

        int count = 0;
        for (float i = 0.0f; i < 360.0f; i += angleIncrease)
        {
            Vertex v;
            v.m_pos  = Math::GetPointOnCircle(center, radius, i);
            v.m_uv.x = Math::Remap(v.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            v.m_uv.y = Math::Remap(v.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            vertArray.push_back(v);
            count++;

            if (count == n)
                return;
        }
    }

    void Internal::FillCircle_SC(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int segments, const Vec4& color, float startAngle, float endAngle, StyleOptions& opts, int drawOrder)
    {
        Array<Vertex> v;
        FillCircleData(v, opts.m_isFilled, center, radius, segments, startAngle, endAngle);

        const int startIndex = buf->m_vertexBuffer.m_size;

        for (int i = 0; i < v.m_size; i++)
        {
            v[i].m_col = color;
            buf->PushVertex(v[i]);
        }

        const bool isFullCircle = Math::Abs(endAngle - startAngle) == 360.0f;
        const int  totalSize    = v.m_size - 1;

        if (opts.m_isFilled)
            ConvexFillVertices(startIndex, startIndex + totalSize, buf->m_indexBuffer, !isFullCircle);
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + totalSize, opts.m_thickness.m_start, !isFullCircle);

        RotateVertices(buf->m_vertexBuffer, center, opts.m_isFilled ? startIndex + 1 : startIndex, opts.m_isFilled ? startIndex + totalSize : startIndex + (totalSize * 2) + 1, rotateAngle);

        if (opts.m_outlineOptions.m_thickness != 0.0f)
        {
            if (isFullCircle)
                buf = DrawOutline(buf, opts, opts.m_isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder);
            else
            {
                if (opts.m_isFilled)
                {
                    Array<int> indices;

                    for (int i = 0; i < v.m_size; i++)
                        indices.push_back(startIndex + i);

                    buf = DrawOutlineAroundShape(buf, opts, &indices[0], v.m_size, opts.m_outlineOptions.m_thickness, true, drawOrder);
                }
                else if (opts.m_outlineOptions.m_drawDirection == OutlineDrawDirection::Both)
                {
                    Array<int> indices;

                    const int halfSize = v.m_size;
                    const int fullSize = halfSize * 2;
                    for (int i = 0; i < halfSize; i++)
                        indices.push_back(startIndex + i);

                    for (int i = fullSize - 1; i > halfSize - 1; i--)
                        indices.push_back(startIndex + i);

                    buf = DrawOutlineAroundShape(buf, opts, &indices[0], halfSize * 2, opts.m_outlineOptions.m_thickness, false, drawOrder);
                }
                else
                    buf = DrawOutline(buf, opts, opts.m_isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder);
            }
        }
        else if (Config.m_enableAA)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.m_outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);

            if (opts.m_isFilled)
            {
                Array<int> indices;

                for (int i = 0; i < v.m_size; i++)
                    indices.push_back(startIndex + i);

                buf = DrawOutlineAroundShape(buf, opts2, &indices[0], v.m_size, opts2.m_outlineOptions.m_thickness, true, drawOrder, true);
            }
            else if (opts.m_outlineOptions.m_drawDirection == OutlineDrawDirection::Both)
            {
                Array<int> indices;

                const int halfSize = v.m_size;
                const int fullSize = halfSize * 2;
                for (int i = 0; i < halfSize; i++)
                    indices.push_back(startIndex + i);

                for (int i = fullSize - 1; i > halfSize - 1; i--)
                    indices.push_back(startIndex + i);

                buf = DrawOutlineAroundShape(buf, opts2, &indices[0], halfSize * 2, opts2.m_outlineOptions.m_thickness, false, drawOrder, true);
            }
            else
                buf = DrawOutline(buf, opts2, opts2.m_isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder, true);
        }
    }

    void Internal::FillCircle_VerHorGra(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int segments, const Vec4& colorStart, const Vec4& colorEnd, bool isHor, float startAngle, float endAngle, StyleOptions& opts, int drawOrder)
    {
        Array<Vertex> v;
        FillCircleData(v, opts.m_isFilled, center, radius, segments, startAngle, endAngle);

        const int startIndex = buf->m_vertexBuffer.m_size;

        for (int i = 0; i < v.m_size; i++)
        {
            v[i].m_col = Math::Lerp(colorStart, colorEnd, isHor ? v[i].m_uv.x : v[i].m_uv.y);
            buf->PushVertex(v[i]);
        }

        const bool isFullCircle = Math::Abs(endAngle - startAngle) == 360.0f;
        const int  totalSize    = v.m_size - 1;

        if (opts.m_isFilled)
            ConvexFillVertices(startIndex, startIndex + totalSize, buf->m_indexBuffer, !isFullCircle);
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + totalSize, opts.m_thickness.m_start, !isFullCircle);

        RotateVertices(buf->m_vertexBuffer, center, opts.m_isFilled ? startIndex + 1 : startIndex, opts.m_isFilled ? startIndex + totalSize : startIndex + (totalSize * 2) + 1, rotateAngle);

        if (opts.m_outlineOptions.m_thickness != 0.0f)
        {
            if (isFullCircle)
                buf = DrawOutline(buf, opts, opts.m_isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder);
            else
            {
                if (opts.m_isFilled)
                {
                    Array<int> indices;

                    for (int i = 0; i < v.m_size; i++)
                        indices.push_back(startIndex + i);

                    buf = DrawOutlineAroundShape(buf, opts, &indices[0], v.m_size, opts.m_outlineOptions.m_thickness, true, drawOrder);
                }
                else if (opts.m_outlineOptions.m_drawDirection == OutlineDrawDirection::Both)
                {
                    Array<int> indices;

                    const int halfSize = v.m_size;
                    const int fullSize = halfSize * 2;
                    for (int i = 0; i < halfSize; i++)
                        indices.push_back(startIndex + i);

                    for (int i = fullSize - 1; i > halfSize - 1; i--)
                        indices.push_back(startIndex + i);

                    buf = DrawOutlineAroundShape(buf, opts, &indices[0], halfSize * 2, opts.m_outlineOptions.m_thickness, false, drawOrder);
                }
                else
                    buf = DrawOutline(buf, opts, opts.m_isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder);
            }
        }
        else if (Config.m_enableAA)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.m_outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);

            if (opts.m_isFilled)
            {
                Array<int> indices;

                for (int i = 0; i < v.m_size; i++)
                    indices.push_back(startIndex + i);

                buf = DrawOutlineAroundShape(buf, opts2, &indices[0], v.m_size, opts2.m_outlineOptions.m_thickness, true, drawOrder, true);
            }
            else if (opts.m_outlineOptions.m_drawDirection == OutlineDrawDirection::Both)
            {
                Array<int> indices;

                const int halfSize = v.m_size;
                const int fullSize = halfSize * 2;
                for (int i = 0; i < halfSize; i++)
                    indices.push_back(startIndex + i);

                for (int i = fullSize - 1; i > halfSize - 1; i--)
                    indices.push_back(startIndex + i);

                buf = DrawOutlineAroundShape(buf, opts2, &indices[0], halfSize * 2, opts2.m_outlineOptions.m_thickness, false, drawOrder, true);
            }
            else
                buf = DrawOutline(buf, opts2, opts2.m_isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, true);
        }
    }

    void Internal::FillCircle_RadialGra(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int segments, const Vec4& colorStart, const Vec4& colorEnd, float startAngle, float endAngle, StyleOptions& opts, int drawOrder)
    {
        Array<Vertex> v;
        FillCircleData(v, opts.m_isFilled, center, radius, segments, startAngle, endAngle);

        const int startIndex = buf->m_vertexBuffer.m_size;

        for (int i = 0; i < v.m_size; i++)
        {
            buf->PushVertex(v[i]);
        }

        const bool isFullCircle = Math::Abs(endAngle - startAngle) == 360.0f;
        const int  totalSize    = v.m_size - 1;

        if (opts.m_isFilled)
            ConvexFillVertices(startIndex, startIndex + totalSize, buf->m_indexBuffer, !isFullCircle);
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + totalSize, opts.m_thickness.m_start, !isFullCircle);

        RotateVertices(buf->m_vertexBuffer, center, opts.m_isFilled ? startIndex + 1 : startIndex, opts.m_isFilled ? startIndex + totalSize : startIndex + (totalSize * 2) + 1, rotateAngle);

        if (opts.m_outlineOptions.m_thickness != 0.0f)
        {
            if (isFullCircle)
                buf = DrawOutline(buf, opts, opts.m_isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder);
            else
            {
                if (opts.m_isFilled)
                {
                    Array<int> indices;

                    for (int i = v.m_size - 1; i > -1; i--)
                        indices.push_back(startIndex + i);

                    buf = DrawOutlineAroundShape(buf, opts, &indices[0], v.m_size, opts.m_outlineOptions.m_thickness, false, drawOrder);
                }
                else if (opts.m_outlineOptions.m_drawDirection == OutlineDrawDirection::Both)
                {
                    Array<int> indices;

                    const int halfSize = v.m_size;
                    const int fullSize = halfSize * 2;
                    for (int i = 0; i < halfSize; i++)
                        indices.push_back(startIndex + i);

                    for (int i = fullSize - 1; i > halfSize - 1; i--)
                        indices.push_back(startIndex + i);

                    buf = DrawOutlineAroundShape(buf, opts, &indices[0], halfSize * 2, opts.m_outlineOptions.m_thickness, false, drawOrder);
                }
                else
                    buf = DrawOutline(buf, opts, opts.m_isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder);
            }
        }
        else if (Config.m_enableAA)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.m_outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);

            if (opts.m_isFilled)
            {
                Array<int> indices;

                for (int i = 0; i < v.m_size; i++)
                    indices.push_back(startIndex + i);

                buf = DrawOutlineAroundShape(buf, opts2, &indices[0], v.m_size, opts2.m_outlineOptions.m_thickness, true, drawOrder, true);
            }
            else if (opts.m_outlineOptions.m_drawDirection == OutlineDrawDirection::Both)
            {
                Array<int> indices;

                const int halfSize = v.m_size;
                const int fullSize = halfSize * 2;
                for (int i = 0; i < halfSize; i++)
                    indices.push_back(startIndex + i);

                for (int i = fullSize - 1; i > halfSize - 1; i--)
                    indices.push_back(startIndex + i);

                buf = DrawOutlineAroundShape(buf, opts2, &indices[0], halfSize * 2, opts2.m_outlineOptions.m_thickness, false, drawOrder, true);
            }
            else
                buf = DrawOutline(buf, opts2, opts2.m_isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder, true);
        }
    }

    void Internal::FillCircleData(Array<Vertex>& vertices, bool hasCenter, const Vec2& center, float radius, int segments, float startAngle, float endAngle)
    {
        if (startAngle < 0.0f)
            startAngle += 360.0f;
        if (endAngle < 0.0f)
            endAngle += 360.0f;

        if (endAngle == startAngle)
        {
            endAngle   = 0.0f;
            startAngle = 360.0f;
        }

        segments                  = Math::Clamp(segments, 6, 180);
        const float angleIncrease = 360.0f / (float)segments;
        const Vec2  bbMin         = Vec2(center.x - radius, center.y - radius);
        const Vec2  bbMax         = Vec2(center.x + radius, center.y + radius);

        if (hasCenter)
        {
            Vertex c;
            c.m_pos  = center;
            c.m_uv.x = Math::Remap(c.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            c.m_uv.y = Math::Remap(c.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            vertices.push_back(c);
        }

        const float end       = Math::Abs(startAngle - endAngle) == 360.0f ? endAngle : endAngle + angleIncrease;
        Vec2        nextPoint = Vec2(-1.0f, -1.0f);
        Vec2        lastPoint = Vec2(-1.0f, -1.0f);
        for (float i = startAngle; i < end; i += angleIncrease)
        {
            Vertex v;
            v.m_pos  = Math::GetPointOnCircle(center, radius, i);
            v.m_uv.x = Math::Remap(v.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            v.m_uv.y = Math::Remap(v.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            vertices.push_back(v);
        }
    }

    void Internal::FillConvex_SC(DrawBuffer* buf, float rotateAngle, Vec2* points, int size, const Vec2& center, const Vec4& color, StyleOptions& opts, int drawOrder)
    {
        const int startIndex = buf->m_vertexBuffer.m_size;
        Vec2      bbMin, bbMax;
        GetConvexBoundingBox(points, size, bbMin, bbMax);

        if (opts.m_isFilled)
        {
            Vertex c;
            c.m_pos  = center;
            c.m_uv.x = Math::Remap(c.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            c.m_uv.y = Math::Remap(c.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            c.m_col  = color;
            buf->PushVertex(c);
        }

        for (int i = 0; i < size; i++)
        {
            Vertex v;
            v.m_pos             = points[i];
            const Vec2 toCenter = Math::Normalized(Vec2(center.x - v.m_pos.x, center.y - v.m_pos.y));
            v.m_uv.x            = Math::Remap(v.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            v.m_uv.y            = Math::Remap(v.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            v.m_col             = color;
            buf->PushVertex(v);
        }

        if (opts.m_isFilled)
            ConvexFillVertices(startIndex, startIndex + size, buf->m_indexBuffer);
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + size - 1, opts.m_thickness.m_start);

        RotateVertices(buf->m_vertexBuffer, center, opts.m_isFilled ? startIndex + 1 : startIndex, opts.m_isFilled ? startIndex + size : startIndex + (size * 2) - 1, rotateAngle);

        if (opts.m_outlineOptions.m_thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.m_isFilled ? size : size * 2, false, drawOrder);
        else if (Config.m_enableAA)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.m_outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.m_isFilled ? size : size * 2, false, drawOrder, true);
        }
    }

    void Internal::FillConvex_VerHorGra(DrawBuffer* buf, float rotateAngle, Vec2* points, int size, const Vec2& center, const Vec4& colorStart, const Vec4& colorEnd, bool isHor, StyleOptions& opts, int drawOrder)
    {
        const int startIndex = buf->m_vertexBuffer.m_size;
        Vec2      bbMin, bbMax;
        GetConvexBoundingBox(points, size, bbMin, bbMax);

        if (opts.m_isFilled)
        {
            Vertex c;
            c.m_pos  = center;
            c.m_col  = Math::Lerp(colorStart, colorEnd, 0.5f);
            c.m_uv.x = Math::Remap(c.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            c.m_uv.y = Math::Remap(c.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            buf->PushVertex(c);
        }

        for (int i = 0; i < size; i++)
        {
            Vertex v;
            v.m_pos             = points[i];
            const Vec2 toCenter = Math::Normalized(Vec2(center.x - v.m_pos.x, center.y - v.m_pos.y));
            v.m_uv.x            = Math::Remap(v.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            v.m_uv.y            = Math::Remap(v.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            v.m_col             = Math::Lerp(colorStart, colorEnd, isHor ? v.m_uv.x : v.m_uv.y);
            buf->PushVertex(v);
        }

        if (opts.m_isFilled)
            ConvexFillVertices(startIndex, startIndex + size, buf->m_indexBuffer);
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + size - 1, opts.m_thickness.m_start);

        RotateVertices(buf->m_vertexBuffer, center, opts.m_isFilled ? startIndex + 1 : startIndex, opts.m_isFilled ? startIndex + size : startIndex + (size * 2) - 1, rotateAngle);

        if (opts.m_outlineOptions.m_thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.m_isFilled ? size : size * 2, false, drawOrder);
        else if (Config.m_enableAA)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.m_outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.m_isFilled ? size : size * 2, false, drawOrder, true);
        }
    }

    void Internal::FillConvex_RadialGra(DrawBuffer* buf, float rotateAngle, Vec2* points, int size, const Vec2& center, const Vec4& colorStart, const Vec4& colorEnd, StyleOptions& opts, int drawOrder)
    {
        const int startIndex = buf->m_vertexBuffer.m_size;

        Vec2 bbMin, bbMax;
        GetConvexBoundingBox(points, size, bbMin, bbMax);

        if (opts.m_isFilled)
        {
            Vertex c;
            c.m_pos  = center;
            c.m_uv.x = Math::Remap(c.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            c.m_uv.y = Math::Remap(c.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            buf->PushVertex(c);
        }

        for (int i = 0; i < size; i++)
        {
            Vertex v;
            v.m_pos             = points[i];
            const Vec2 toCenter = Math::Normalized(Vec2(center.x - v.m_pos.x, center.y - v.m_pos.y));
            v.m_uv.x            = Math::Remap(v.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            v.m_uv.y            = Math::Remap(v.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            buf->PushVertex(v);
        }

        if (opts.m_isFilled)
            ConvexFillVertices(startIndex, startIndex + size, buf->m_indexBuffer);
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + size - 1, opts.m_thickness.m_start);

        RotateVertices(buf->m_vertexBuffer, center, opts.m_isFilled ? startIndex + 1 : startIndex, opts.m_isFilled ? startIndex + size : startIndex + (size * 2) - 1, rotateAngle);

        if (opts.m_outlineOptions.m_thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.m_isFilled ? size : size * 2, false, drawOrder);
        else if (Config.m_enableAA)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.m_outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.m_isFilled ? size : size * 2, false, drawOrder, true);
        }
    }

    void Internal::GetTriangleBoundingBox(const Vec2& p1, const Vec2& p2, const Vec2& p3, Vec2& outMin, Vec2& outMax)
    {
        outMin = Vec2(Math::Min(Math::Min(p1.x, p2.x), p3.x), Math::Min(Math::Min(p1.y, p2.y), p3.y));
        outMax = Vec2(Math::Max(Math::Max(p1.x, p2.x), p3.x), Math::Max(Math::Max(p1.y, p2.y), p3.y));
    }

    void Internal::GetConvexBoundingBox(Vec2* points, int size, Vec2& outMin, Vec2& outMax)
    {

        outMin = Vec2(99999, 99999);
        outMax = Vec2(-99999, -99999);

        for (int i = 0; i < size; i++)
        {
            if (points[i].x < outMin.x)
                outMin.x = points[i].x;
            else if (points[i].x > outMax.x)
                outMax.x = points[i].x;
            if (points[i].y < outMin.y)
                outMin.y = points[i].y;
            else if (points[i].y > outMax.y)
                outMax.y = points[i].y;
        }
    }

    void Internal::GetConvexBoundingBox(Vertex* points, int size, Vec2& outMin, Vec2& outMax)
    {
        outMin = Vec2(99999, 99999);
        outMax = Vec2(-99999, -99999);

        for (int i = 0; i < size; i++)
        {
            if (points[i].m_pos.x < outMin.x)
                outMin.x = points[i].m_pos.x;
            else if (points[i].m_pos.x > outMax.x)
                outMax.x = points[i].m_pos.x;
            if (points[i].m_pos.y < outMin.y)
                outMin.y = points[i].m_pos.y;
            else if (points[i].m_pos.y > outMax.y)
                outMax.y = points[i].m_pos.y;
        }
    }

    void Internal::GetConvexBoundingBox(DrawBuffer* buf, int startIndex, int endIndex, Vec2& outMin, Vec2& outMax)
    {
        outMin = Vec2(99999, 99999);
        outMax = Vec2(-99999, -99999);

        for (int i = startIndex; i < endIndex + 1; i++)
        {
            if (buf->m_vertexBuffer[i].m_pos.x < outMin.x)
                outMin.x = buf->m_vertexBuffer[i].m_pos.x;
            else if (buf->m_vertexBuffer[i].m_pos.x > outMax.x)
                outMax.x = buf->m_vertexBuffer[i].m_pos.x;
            if (buf->m_vertexBuffer[i].m_pos.y < outMin.y)
                outMin.y = buf->m_vertexBuffer[i].m_pos.y;
            else if (buf->m_vertexBuffer[i].m_pos.y > outMax.y)
                outMax.y = buf->m_vertexBuffer[i].m_pos.y;
        }
    }

    void Internal::CalculateVertexUVs(DrawBuffer* buf, int startIndex, int endIndex)
    {
        Vec2 bbMin, bbMax;
        GetConvexBoundingBox(&buf->m_vertexBuffer.m_data[startIndex], endIndex - startIndex + 1, bbMin, bbMax);

        // Recalculate UVs.
        for (int i = startIndex; i <= endIndex; i++)
        {
            buf->m_vertexBuffer[i].m_uv.x = Math::Remap(buf->m_vertexBuffer[i].m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            buf->m_vertexBuffer[i].m_uv.y = Math::Remap(buf->m_vertexBuffer[i].m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
        }
    }

    float Internal::GetAngleIncrease(float rounding)
    {
        if (rounding < 0.25f)
            return 20.0f;
        else if (rounding < 0.5f)
            return 15.0f;
        else if (rounding < 0.75f)
            return 10.0f;
        else
            return 5.0f;
    }

    /*void Internal::ConvexOutline(Array<Vertex>& srcVertices, int startIndex, int endIndex, const Vec2& convexCenter, OutlineOptions& options, bool skipEndClosing)
    {
        const int current   = DEF_VTX_BUF.m_size;
        const int totalSize = endIndex - startIndex + 1;

        for (size_t i = startIndex; i <= endIndex; i++)
        {
            // pushing a new one might invalidate the srcVertices[i] reference (due to growing and memcpy)
            // so check and grow if necessary first.
            DEF_VTX_BUF.checkGrow();
            Vertex* v = DEF_VTX_BUF.push_back(srcVertices[i]);
            v->m_col  = options.m_color;
        }

        // Iterate the latest added copy vertices.
        // Exflate each vertex towards outer direction from center.
        for (int i = 0; i < totalSize; i++)
        {
            Vertex&    v          = DEF_VTX_BUF[current + i];
            const Vec2 dir        = Math::Normalized(Vec2(v.m_pos.x - convexCenter.x, v.m_pos.y - convexCenter.y));
            Vec2       uniformDir = dir;

            //  if (Math::Abs(dir.x) > Math::Abs(dir.y))
            //  {
            //      uniformDir.x = dir.x;
            //      if (dir.y > 0.0)
            //          uniformDir.y = Math::Abs(dir.x);
            //      else
            //          uniformDir.y = -Math::Abs(dir.x);
            //  }
            //  else
            //  {
            //      uniformDir.y = dir.y;
            //      if (dir.x > 0.0)
            //          uniformDir.x = Math::Abs(dir.y);
            //      else
            //          uniformDir.x = -Math::Abs(dir.y);
            //  }

            Vertex exf;
            exf.m_pos = Vec2(v.m_pos.x + uniformDir.x * options.m_radius * Config.m_framebufferScale.x, v.m_pos.y + uniformDir.y * options.m_radius * Config.m_framebufferScale.y);
            exf.m_col = options.m_color;
            exf.m_uv  = v.m_uv;
            DEF_VTX_BUF.push_back(exf);
        }

        // Now add the indices.
        for (int i = current; i < current + totalSize; i++)
        {
            int next = i + 1;
            if (next >= current + totalSize)
                next = current;

            if (i == current + totalSize - 1 && skipEndClosing)
                return;

            DEF_INDEX_BUF.push_back(i);
            DEF_INDEX_BUF.push_back(i + totalSize);
            DEF_INDEX_BUF.push_back(next);
            DEF_INDEX_BUF.push_back(i + totalSize);
            DEF_INDEX_BUF.push_back(next);
            DEF_INDEX_BUF.push_back(next + totalSize);
        }
    }*/

    Vec2 Internal::GetArcDirection(const Vec2& center, float radius, float startAngle, float endAngle)
    {
        const Vec2 centerAnglePoint = Math::GetPointOnCircle(center, radius, (startAngle + endAngle) / 2.0f);
        return Vec2(centerAnglePoint.x - center.x, centerAnglePoint.y - center.y);
    }

    Line Internal::CalculateLine(const Vec2& p1, const Vec2& p2, StyleOptions& style)
    {
        const Vec2 dir = Vec2(p2.x - p1.x, p2.y - p1.y);
        const Vec2 up  = Math::Normalized(Math::Rotate90(dir, true));

        Line line;
        line.m_points[0] = Vec2(p1.x + up.x * style.m_thickness.m_start, p1.y + up.y * style.m_thickness.m_start);
        line.m_points[3] = Vec2(p1.x - up.x * style.m_thickness.m_start, p1.y - up.y * style.m_thickness.m_start);

        line.m_points[1] = Vec2(p2.x + up.x * style.m_thickness.m_start, p2.y + up.y * style.m_thickness.m_start);
        line.m_points[2] = Vec2(p2.x - up.x * style.m_thickness.m_start, p2.y - up.y * style.m_thickness.m_start);
        return line;
    }

    DrawBuffer* Internal::DrawOutlineAroundShape(DrawBuffer* sourceBuffer, StyleOptions& opts, int* indicesOrder, int vertexCount, float defThickness, bool ccw, int drawOrder, bool isAAOutline)
    {
        const bool useTextureBuffer = opts.m_outlineOptions.m_textureHandle != 0;
        const bool isGradient       = !Math::IsEqual(opts.m_outlineOptions.m_color.m_start, opts.m_outlineOptions.m_color.m_end) && !opts.m_outlineOptions.m_useVertexColors;
        const bool useGradBuffer    = !useTextureBuffer && isGradient;
        float      thickness        = isAAOutline ? Config.m_framebufferScale.x : (defThickness * Config.m_framebufferScale.x);

        // Determine which buffer to use.
        // Also correct the buffer pointer if getting a new buffer invalidated it.
        DrawBuffer* destBuf = nullptr;

        if (useTextureBuffer)
        {
            const int sourceIndex = Internal::g_rendererData.GetBufferIndexInTextureArray(sourceBuffer);
            destBuf               = &Internal::g_rendererData.GetTextureBuffer(opts.m_outlineOptions.m_textureHandle, opts.m_outlineOptions.m_textureUVTiling, opts.m_outlineOptions.m_textureUVOffset, drawOrder, isAAOutline);

            if (sourceIndex != -1)
                sourceBuffer = &Internal::g_rendererData.m_textureBuffers[sourceIndex];
        }
        else if (useGradBuffer)
        {
            const int sourceIndex = Internal::g_rendererData.GetBufferIndexInGradientArray(sourceBuffer);
            destBuf               = &Internal::g_rendererData.GetGradientBuffer(opts.m_outlineOptions.m_color, drawOrder, isAAOutline);

            if (sourceIndex != -1)
                sourceBuffer = &Internal::g_rendererData.m_gradientBuffers[sourceIndex];
        }
        else
        {
            const int sourceIndex = Internal::g_rendererData.GetBufferIndexInDefaultArray(sourceBuffer);
            destBuf = &Internal::g_rendererData.GetDefaultBuffer(drawOrder);

            if (sourceIndex != -1)
                sourceBuffer = &Internal::g_rendererData.m_defaultBuffers[sourceIndex];
        }

        const int destBufStart = destBuf->m_vertexBuffer.m_size;
        // First copy the given vertices, add them to the destination buffer.
        for (int i = 0; i < vertexCount; i++)
        {
            Vertex v;
            v.m_col = opts.m_outlineOptions.m_color.m_start;
            v.m_pos = sourceBuffer->m_vertexBuffer[indicesOrder[i]].m_pos;
            v.m_uv  = sourceBuffer->m_vertexBuffer[indicesOrder[i]].m_uv;

            if (isAAOutline)
                v.m_col.w = 1.0f;

            destBuf->PushVertex(v);
        }

        const int halfVC = vertexCount / 2;

        // only used if we are drawing AA.
        Array<int> extrudedVerticesOrder;

        // Now traverse the destination buffer from the point we started adding to it, extrude the border towards thickness.
        for (int i = 0; i < vertexCount; i++)
        {
            const int prev    = i == 0 ? destBufStart + vertexCount - 1 : destBufStart + i - 1;
            const int next    = i == vertexCount - 1 ? destBufStart : destBufStart + i + 1;
            const int current = destBufStart + i;
            Vertex    v;
            v.m_uv  = destBuf->m_vertexBuffer[current].m_uv;
            v.m_col = opts.m_outlineOptions.m_color.m_end;

            if (isAAOutline)
                v.m_col.w = 0.0f;

            const Vec2 prevP               = destBuf->m_vertexBuffer[prev].m_pos;
            const Vec2 nextP               = destBuf->m_vertexBuffer[next].m_pos;
            const Vec2 vertexNormalAverage = Math::GetVertexNormalFlatCheck(destBuf->m_vertexBuffer[current].m_pos, prevP, nextP, ccw);
            v.m_pos                        = Vec2(destBuf->m_vertexBuffer[current].m_pos.x + vertexNormalAverage.x * thickness, destBuf->m_vertexBuffer[current].m_pos.y + vertexNormalAverage.y * thickness);

            if (Config.m_enableAA && !isAAOutline)
                extrudedVerticesOrder.push_back(destBuf->m_vertexBuffer.m_size);

            destBuf->PushVertex(v);
        }

        if (!isAAOutline && (useTextureBuffer || useGradBuffer))
            CalculateVertexUVs(destBuf, destBufStart, (destBufStart + vertexCount * 2) - 1);

        for (int i = 0; i < vertexCount; i++)
        {
            const int current = destBufStart + i;
            int       next    = destBufStart + i + 1;
            if (i == vertexCount - 1)
                next = destBufStart;

            destBuf->PushIndex(current);
            destBuf->PushIndex(next);
            destBuf->PushIndex(current + vertexCount);
            destBuf->PushIndex(next);
            destBuf->PushIndex(next + vertexCount);
            destBuf->PushIndex(current + vertexCount);
        }

        if (Config.m_enableAA && !isAAOutline)
        {
            StyleOptions opts2 = StyleOptions(opts);
            DrawOutlineAroundShape(destBuf, opts2, &extrudedVerticesOrder[0], extrudedVerticesOrder.m_size, defThickness, ccw, true);
        }

        return sourceBuffer;
    }

    DrawBuffer* Internal::DrawOutline(DrawBuffer* sourceBuffer, StyleOptions& opts, int vertexCount, bool skipEnds, int drawOrder, bool isAAOutline, bool reverseDrawDir)
    {
        const bool useTextureBuffer = opts.m_outlineOptions.m_textureHandle != 0;
        const bool isGradient       = !Math::IsEqual(opts.m_outlineOptions.m_color.m_start, opts.m_outlineOptions.m_color.m_end) && !opts.m_outlineOptions.m_useVertexColors;
        const bool useGradBuffer    = !useTextureBuffer && isGradient;
        float      thickness        = isAAOutline ? Config.m_framebufferScale.x : (opts.m_outlineOptions.m_thickness * Config.m_framebufferScale.x);

        if (reverseDrawDir)
            thickness = -thickness;

        // Determine which buffer to use.
        // Also correct the buffer pointer if getting a new buffer invalidated it.
        DrawBuffer* destBuf = nullptr;

        if (useTextureBuffer)
        {
            const int sourceIndex = Internal::g_rendererData.GetBufferIndexInTextureArray(sourceBuffer);
            destBuf               = &Internal::g_rendererData.GetTextureBuffer(opts.m_outlineOptions.m_textureHandle, opts.m_outlineOptions.m_textureUVTiling, opts.m_outlineOptions.m_textureUVOffset, drawOrder, isAAOutline);

            if (sourceIndex != -1)
                sourceBuffer = &Internal::g_rendererData.m_textureBuffers[sourceIndex];
        }
        else if (useGradBuffer)
        {
            const int sourceIndex = Internal::g_rendererData.GetBufferIndexInGradientArray(sourceBuffer);
            destBuf               = &Internal::g_rendererData.GetGradientBuffer(opts.m_outlineOptions.m_color, drawOrder, isAAOutline);

            if (sourceIndex != -1)
                sourceBuffer = &Internal::g_rendererData.m_gradientBuffers[sourceIndex];
        }
        else
            destBuf = &Internal::g_rendererData.GetDefaultBuffer(drawOrder);

        int startIndex, endIndex;

        if (opts.m_isFilled)
        {
            endIndex   = sourceBuffer->m_vertexBuffer.m_size - 1;
            startIndex = sourceBuffer->m_vertexBuffer.m_size - vertexCount;
        }
        else
        {
            // Take the outer half.
            if (opts.m_outlineOptions.m_drawDirection == OutlineDrawDirection::Outwards)
            {
                endIndex   = sourceBuffer->m_vertexBuffer.m_size - 1;
                startIndex = sourceBuffer->m_vertexBuffer.m_size - vertexCount / 2;
            }
            else if (opts.m_outlineOptions.m_drawDirection == OutlineDrawDirection::Inwards)
            {
                endIndex   = sourceBuffer->m_vertexBuffer.m_size - vertexCount / 2 - 1;
                startIndex = sourceBuffer->m_vertexBuffer.m_size - vertexCount;
            }
            else
            {
                endIndex   = sourceBuffer->m_vertexBuffer.m_size - 1;
                startIndex = sourceBuffer->m_vertexBuffer.m_size - vertexCount;
            }
        }

        Vec2 bbMin, bbMax;
        GetConvexBoundingBox(sourceBuffer, startIndex, endIndex, bbMin, bbMax);
        const bool reCalcUVs    = useTextureBuffer || useGradBuffer;
        const int  destBufStart = destBuf->m_vertexBuffer.m_size;

        auto copyAndFill = [&](DrawBuffer* sourceBuffer, DrawBuffer* destBuf, int startIndex, int endIndex, float thickness, bool reCalcUVs) {
            const int destBufStart = destBuf->m_vertexBuffer.m_size;
            const int totalSize    = endIndex - startIndex + 1;

            // First copy vertices.
            for (int i = startIndex; i < endIndex + 1; i++)
            {
                Vertex v;
                v.m_col = opts.m_outlineOptions.m_color.m_start;
                v.m_pos = sourceBuffer->m_vertexBuffer[i].m_pos;
                v.m_uv  = sourceBuffer->m_vertexBuffer[i].m_uv;

                if (isAAOutline)
                    v.m_col.w = 1.0f;

                destBuf->PushVertex(v);
            }

            // Now extrude & re-add extruded.
            for (int i = startIndex; i < endIndex + 1; i++)
            {
                // take two edges, this vertex to next and previous to this in order to calculate vertex normals.
                const int previous = i == startIndex ? endIndex : i - 1;
                const int next     = i == endIndex ? startIndex : i + 1;
                Vertex    v;
                v.m_uv  = sourceBuffer->m_vertexBuffer[i].m_uv;
                v.m_col = opts.m_outlineOptions.m_color.m_end;

                if (isAAOutline)
                    v.m_col.w = 0.0f;

                if (skipEnds && i == startIndex)
                {
                    const Vec2 nextP               = sourceBuffer->m_vertexBuffer[next].m_pos;
                    const Vec2 vertexNormalAverage = Math::GetVertexNormal(sourceBuffer->m_vertexBuffer[i].m_pos, Vec2(-1, -1), nextP);
                    v.m_pos                        = Vec2(sourceBuffer->m_vertexBuffer[i].m_pos.x + vertexNormalAverage.x * thickness, sourceBuffer->m_vertexBuffer[i].m_pos.y + vertexNormalAverage.y * thickness);
                }
                else if (skipEnds && i == endIndex)
                {
                    const Vec2 prevP               = sourceBuffer->m_vertexBuffer[previous].m_pos;
                    const Vec2 vertexNormalAverage = Math::GetVertexNormal(sourceBuffer->m_vertexBuffer[i].m_pos, prevP, Vec2(-1, -1));
                    v.m_pos                        = Vec2(sourceBuffer->m_vertexBuffer[i].m_pos.x + vertexNormalAverage.x * thickness, sourceBuffer->m_vertexBuffer[i].m_pos.y + vertexNormalAverage.y * thickness);
                }
                else
                {
                    const Vec2 prevP               = sourceBuffer->m_vertexBuffer[previous].m_pos;
                    const Vec2 nextP               = sourceBuffer->m_vertexBuffer[next].m_pos;
                    const Vec2 vertexNormalAverage = Math::GetVertexNormal(sourceBuffer->m_vertexBuffer[i].m_pos, prevP, nextP);
                    v.m_pos                        = Vec2(sourceBuffer->m_vertexBuffer[i].m_pos.x + vertexNormalAverage.x * thickness, sourceBuffer->m_vertexBuffer[i].m_pos.y + vertexNormalAverage.y * thickness);
                }
                destBuf->PushVertex(v);
            }

            if (!isAAOutline && reCalcUVs)
                CalculateVertexUVs(destBuf, destBufStart, (destBufStart + totalSize * 2) - 1);

            const int halfIndex = (destBufStart + totalSize) / 2;
            for (int i = destBufStart; i < destBufStart + totalSize; i++)
            {
                int next = i + 1;
                if (next >= destBufStart + totalSize)
                    next = destBufStart;

                if (skipEnds && i == destBufStart + totalSize - 1)
                    return;

                destBuf->PushIndex(i);
                destBuf->PushIndex(next);
                destBuf->PushIndex(i + totalSize);
                destBuf->PushIndex(next);
                destBuf->PushIndex(next + totalSize);
                destBuf->PushIndex(i + totalSize);
            }
        };

        const bool recalcUvs = useTextureBuffer || useGradBuffer;

        if (opts.m_isFilled)
        {
            copyAndFill(sourceBuffer, destBuf, startIndex, endIndex, thickness, recalcUvs);

            if (Config.m_enableAA && !isAAOutline)
            {
                StyleOptions opts2                     = StyleOptions(opts);
                opts2.m_outlineOptions.m_drawDirection = OutlineDrawDirection::Outwards;
                DrawOutline(destBuf, opts2, vertexCount, skipEnds, true);
            }
        }
        else
        {
            if (opts.m_outlineOptions.m_drawDirection == OutlineDrawDirection::Outwards)
            {
                copyAndFill(sourceBuffer, destBuf, startIndex, endIndex, thickness, recalcUvs);
                if (Config.m_enableAA && !isAAOutline)
                {
                    // AA outline to the current outline we are drawing
                    StyleOptions opts2                     = StyleOptions(opts);
                    opts2.m_outlineOptions.m_drawDirection = OutlineDrawDirection::Outwards;
                    DrawOutline(destBuf, opts2, vertexCount, skipEnds, true);

                    // AA outline to the shape we are drawing
                    StyleOptions opts3     = StyleOptions(opts);
                    opts3.m_outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Inwards);
                    DrawOutline(sourceBuffer, opts3, vertexCount, skipEnds, true);
                }
            }
            else if (opts.m_outlineOptions.m_drawDirection == OutlineDrawDirection::Inwards)
            {
                copyAndFill(sourceBuffer, destBuf, startIndex, endIndex, -thickness, recalcUvs);

                if (Config.m_enableAA && !isAAOutline)
                {
                    // AA outline to the current outline we are drawing
                    StyleOptions opts2                     = StyleOptions(opts);
                    opts2.m_outlineOptions.m_drawDirection = OutlineDrawDirection::Outwards;
                    DrawOutline(destBuf, opts2, vertexCount, skipEnds, true, true);

                    // AA outline to the shape we are drawing
                    StyleOptions opts3     = StyleOptions(opts);
                    opts3.m_outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Outwards);
                    DrawOutline(sourceBuffer, opts3, vertexCount, skipEnds, true);
                }
            }
            else
            {

                copyAndFill(sourceBuffer, destBuf, startIndex, startIndex + vertexCount / 2 - 1, -thickness, recalcUvs);

                if (Config.m_enableAA && !isAAOutline)
                {
                    // AA outline to the current outline we are drawing
                    StyleOptions opts2                     = StyleOptions(opts);
                    opts2.m_outlineOptions.m_drawDirection = OutlineDrawDirection::Outwards;
                    DrawOutline(destBuf, opts2, vertexCount, skipEnds, true, true);

                    // AA outline to the shape we are drawing
                    StyleOptions opts3     = StyleOptions(opts);
                    opts3.m_outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Inwards);
                    DrawOutline(sourceBuffer, opts3, vertexCount, skipEnds, true);
                }

                copyAndFill(sourceBuffer, destBuf, startIndex + vertexCount / 2, endIndex, thickness, recalcUvs);

                if (Config.m_enableAA && !isAAOutline)
                {
                    // AA outline to the current outline we are drawing
                    StyleOptions opts2                     = StyleOptions(opts);
                    opts2.m_outlineOptions.m_drawDirection = OutlineDrawDirection::Outwards;
                    DrawOutline(destBuf, opts2, vertexCount, skipEnds, true);

                    // AA outline to the shape we are drawing
                    StyleOptions opts3     = StyleOptions(opts);
                    opts3.m_outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Outwards);
                    DrawOutline(sourceBuffer, opts3, vertexCount, skipEnds, true);
                }
            }
        }

        return sourceBuffer;
    }
} // namespace Lina2D
