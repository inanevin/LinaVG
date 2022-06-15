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

#include "Core/Drawer.hpp"
#include "Core/Math.hpp"
#include "Core/Renderer.hpp"
#include "Core/Backend.hpp"
#include "Core/Text.hpp"
#include "Utility/Utility.hpp"
#include <codecvt> // for std::codecvt_utf8

namespace LinaVG
{
    RectOverrideData g_rectOverrideData;
    UVOverrideData   g_uvOverride;

    void DrawBezier(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, StyleOptions& style, LineCapDirection cap, LineJointType jointType, int drawOrder, int segments)
    {
        float       acc      = (float)Math::Clamp(segments, 0, 100);
        const float increase = Math::Remap(acc, 0.0f, 100.0f, 0.15f, 0.01f);
        Array<Vec2> points;

        bool addLast = true;
        for (float t = 0.0f; t < 1.0f; t += increase)
        {
            points.push_back(Math::SampleBezier(p0, p1, p2, p3, t));

            if (Math::IsEqualMarg(t, 1.0f, 0.001f))
                addLast = false;
        }

        if (addLast)
            points.push_back(Math::SampleBezier(p0, p1, p2, p3, 1.0f));

        DrawLines(&points[0], points.m_size, style, cap, jointType, drawOrder);

        points.clear();
    }

    void DrawPoint(const Vec2& p1, const Vec4& col)
    {
        StyleOptions style;
        style.color        = col;
        style.isFilled     = true;
        const float distance = Config.framebufferScale.x / 2.0f;
        DrawRect(Vec2(p1.x - distance, p1.y - distance), Vec2(p1.x + distance, p1.y + distance), style);
    }

    void DrawLine(const Vec2& p1, const Vec2& p2, StyleOptions& style, LineCapDirection cap, float rotateAngle, int drawOrder)
    {
        SimpleLine   l = Internal::CalculateSimpleLine(p1, p2, style);
        StyleOptions s = StyleOptions(style);
        s.isFilled   = true;

        if (cap == LineCapDirection::Left || cap == LineCapDirection::Both)
        {
            s.onlyRoundTheseCorners.push_back(0);
            s.onlyRoundTheseCorners.push_back(3);
            s.rounding = 1.0f;
        }

        if (cap == LineCapDirection::Right || cap == LineCapDirection::Both)
        {
            s.onlyRoundTheseCorners.push_back(1);
            s.onlyRoundTheseCorners.push_back(2);
            s.rounding = 1.0f;
        }

        Internal::DrawSimpleLine(l, s, rotateAngle, drawOrder);
    }

    void DrawLines(Vec2* points, int count, StyleOptions& opts, LineCapDirection cap, LineJointType jointType, int drawOrder)
    {
        if (count < 3)
        {
            if (Config.errorCallback)
                Config.errorCallback("LinaVG: Can't draw lines as the point array count is smaller than 3!");
            return;
        }

        // Generate line structs between each points.
        // Each line struct will contain -> line vertices, upper & below vertices.
        StyleOptions style = StyleOptions(opts);
        style.isFilled   = true;

        const bool useTextureBuffer = style.textureHandle != 0;
        const bool isGradient       = !Math::IsEqual(style.color.start, style.color.end);
        const bool useGradBuffer    = !useTextureBuffer && isGradient;

        // Determine which buffer to use.
        // Also correct the buffer pointer if getting a new buffer invalidated it.
        DrawBuffer* destBuf = nullptr;

        if (useTextureBuffer)
            destBuf = &Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
        else if (useGradBuffer)
            destBuf = &Internal::g_rendererData.GetGradientBuffer(style.color, drawOrder, DrawBufferShapeType::Shape);
        else
            destBuf = &Internal::g_rendererData.GetDefaultBuffer(drawOrder, DrawBufferShapeType::Shape);

        // Calculate the line points.
        Array<Line*>     lines;
        LineCapDirection usedCapDir = LineCapDirection::None;

        for (int i = 0; i < count - 1; i++)
        {
            if (i == 0 && (cap == LineCapDirection::Left || cap == LineCapDirection::Both))
                usedCapDir = LineCapDirection::Left;
            else if (i == count - 2 && (cap == LineCapDirection::Right || cap == LineCapDirection::Both))
                usedCapDir = LineCapDirection::Right;
            else
                usedCapDir = LineCapDirection::None;

            const float t             = static_cast<float>(i) / static_cast<float>(count - 1);
            const float t2            = static_cast<float>(i + 1) / static_cast<float>(count - 1);
            style.thickness.start = Math::Lerp(opts.thickness.start, opts.thickness.end, t);
            style.thickness.end   = Math::Lerp(opts.thickness.start, opts.thickness.end, t2);

            Line* line = new Line();
            Internal::CalculateLine(*line, points[i], points[i + 1], style, usedCapDir);
            lines.push_back(line);
        }

        // Calculate line joints.
        for (int i = 0; i < lines.m_size - 1; i++)
        {
            Line* curr = lines[i];
            Line* next = lines[i + 1];

            const Vec2 currDir = Math::Normalized(Vec2(curr->m_vertices[2].pos.x - curr->m_vertices[3].pos.x, curr->m_vertices[2].pos.y - curr->m_vertices[3].pos.y));
            const Vec2 nextDir = Math::Normalized(Vec2(next->m_vertices[2].pos.x - next->m_vertices[3].pos.x, next->m_vertices[2].pos.y - next->m_vertices[3].pos.y));

            if (!Math::AreLinesParallel(curr->m_vertices[3].pos, curr->m_vertices[2].pos, next->m_vertices[3].pos, next->m_vertices[2].pos))
            {
                // If next line is going below current one, angle is positive, and we merge lower vertices while joining upper.
                // Vice versa if angle is negative
                const float angle = Math::GetAngleBetweenDirs(currDir, nextDir);

                LineJointType usedJointType = jointType;

                if (jointType != LineJointType::VtxAverage)
                {
                    if (Math::Abs(angle) < 15.0f)
                        usedJointType = LineJointType::VtxAverage;
                    else
                    {
                        // Joint type fallbacks.
                        if (jointType == LineJointType::Miter && Math::Abs(angle) > Config.miterLimit)
                            usedJointType = LineJointType::BevelRound;

                        if (jointType == LineJointType::BevelRound && style.rounding == 0.0f)
                            usedJointType = LineJointType::Bevel;
                    }
                }

                Internal::JoinLines(*curr, *next, style, usedJointType, angle < 0.0f);
            }
            else
            {
                next->m_upperIndices.erase(next->m_upperIndices.findAddr(0));
                next->m_lowerIndices.erase(next->m_lowerIndices.findAddr(3));
            }
        }

        // Calculate line UVs
        Array<Vertex> vertices;

        for (int i = 0; i < lines.m_size; i++)
        {
            for (int j = 0; j < lines[i]->m_vertices.m_size; j++)
                vertices.push_back(lines[i]->m_vertices[j]);
        }

        Vec2 bbMin, bbMax;
        Internal::GetConvexBoundingBox(&vertices[0], vertices.m_size, bbMin, bbMax);

        // Recalculate UVs.
        for (int i = 0; i < lines.m_size; i++)
        {
            for (int j = 0; j < lines[i]->m_vertices.m_size; j++)
            {
                lines[i]->m_vertices[j].uv.x = Math::Remap(lines[i]->m_vertices[j].pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
                lines[i]->m_vertices[j].uv.y = Math::Remap(lines[i]->m_vertices[j].pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            }
        }

        int drawBufferStartBeforeLines = destBuf->m_vertexBuffer.m_size;
        // Actually draw the lines after all calculations are done & corrected.
        for (int i = 0; i < lines.m_size; i++)
        {
            int destBufStart = destBuf->m_vertexBuffer.m_size;
            for (int j = 0; j < lines[i]->m_vertices.m_size; j++)
            {
                destBuf->PushVertex(lines[i]->m_vertices[j]);
            }

            for (int j = 0; j < lines[i]->m_tris.m_size; j++)
            {
                destBuf->PushIndex(destBufStart + lines[i]->m_tris[j].m_indices[0]);
                destBuf->PushIndex(destBufStart + lines[i]->m_tris[j].m_indices[1]);
                destBuf->PushIndex(destBufStart + lines[i]->m_tris[j].m_indices[2]);
            }
        }

        if (style.outlineOptions.thickness == 0.0f && !Config.aaEnabled)
        {
            for (int i = 0; i < lines.m_size; i++)
                delete lines[i];

            lines.clear();
            return;
        }

        int drawBufferStartForOutlines = drawBufferStartBeforeLines;

        Array<int> totalUpperIndices;
        Array<int> totalLowerIndices;

        for (int i = 0; i < lines.m_size; i++)
        {
            for (int j = 0; j < lines[i]->m_upperIndices.m_size; j++)
                totalUpperIndices.push_back(drawBufferStartForOutlines + lines[i]->m_upperIndices[j]);

            for (int j = 0; j < lines[i]->m_lowerIndices.m_size; j++)
                totalLowerIndices.push_back(drawBufferStartForOutlines + lines[i]->m_lowerIndices[j]);

            drawBufferStartForOutlines += lines[i]->m_vertices.m_size;
        }

        if (style.outlineOptions.thickness != 0.0f)
        {
            Array<int> indicesOrder;
            for (int i = 0; i < totalLowerIndices.m_size; i++)
                indicesOrder.push_back(totalLowerIndices[i]);

            for (int i = totalUpperIndices.m_size - 1; i > -1; i--)
                indicesOrder.push_back(totalUpperIndices[i]);

            Internal::DrawOutlineAroundShape(destBuf, style, &indicesOrder[0], indicesOrder.m_size, style.outlineOptions.thickness, false, drawOrder, Internal::OutlineCallType::Normal);
        }
        else if (Config.aaEnabled)
        {
            StyleOptions opts2     = StyleOptions(style);
            opts2.outlineOptions = OutlineOptions::FromStyle(style, OutlineDrawDirection::Both);

            Array<int> indicesOrder;
            for (int i = 0; i < totalLowerIndices.m_size; i++)
                indicesOrder.push_back(totalLowerIndices[i]);

            for (int i = totalUpperIndices.m_size - 1; i > -1; i--)
                indicesOrder.push_back(totalUpperIndices[i]);
             
            for (int i = 0; i < indicesOrder.m_size; i++)
            {
                Vertex& v = destBuf->m_vertexBuffer[indicesOrder[i]];
                Vec4 col = v.col;
                int a = 5;
             }
            Internal::DrawOutlineAroundShape(destBuf, opts2, &indicesOrder[0], indicesOrder.m_size, opts2.outlineOptions.thickness, false, drawOrder, Internal::OutlineCallType::AA);
        }

        for (int i = 0; i < lines.m_size; i++)
            delete lines[i];

        lines.clear();
    }

    void DrawImage(BackendHandle textureHandle, const Vec2& pos, const Vec2& size, float rotateAngle, int drawOrder, Vec2 uvTiling, Vec2 uvOffset, Vec2 uvTL, Vec2 uvBR)
    {
        StyleOptions style;
        style.isFilled        = true;
        style.textureHandle   = textureHandle;
        style.textureUVOffset = uvOffset;
        style.textureUVTiling = uvTiling;
        const Vec2 min          = Vec2(pos.x - size.x / 2.0f, pos.y - size.y / 2.0f);
        const Vec2 max          = Vec2(pos.x + size.x / 2.0f, pos.y + size.y / 2.0f);

        g_uvOverride.m_override = true;
        g_uvOverride.m_uvTL     = uvTL;
        g_uvOverride.m_uvBR     = uvBR;
        const bool currAA       = Config.aaEnabled;
        Config.aaEnabled      = false;
        DrawRect(min, max, style, rotateAngle, drawOrder);
        Config.aaEnabled      = currAA;
        g_uvOverride.m_override = false;
        g_uvOverride.m_uvTL     = Vec2(0, 0);
        g_uvOverride.m_uvBR     = Vec2(1, 1);
    }

    void DrawTriangle(const Vec2& top, const Vec2& right, const Vec2& left, StyleOptions& style, float rotateAngle, int drawOrder)
    {
        // NR - SC - def buf
        // NR - SC - text
        // NR - VH - DEF
        // NR - VH - text

        if (style.rounding == 0.0f)
        {
            if (Math::IsEqual(style.color.start, style.color.end))
            {
                if (style.textureHandle == 0)
                    Internal::FillTri_NoRound_SC(&Internal::g_rendererData.GetDefaultBuffer(drawOrder, DrawBufferShapeType::Shape), rotateAngle, top, right, left, style.color.start, style, drawOrder);
                else
                {

                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                    Internal::FillTri_NoRound_SC(&buf, rotateAngle, top, right, left, style.color.start, style, drawOrder);
                }
            }
            else
            {
                if (style.color.gradientType == GradientType::Horizontal)
                {
                    // Horizontal, non rounded
                    if (style.textureHandle == 0)
                        Internal::FillTri_NoRound_VerHorGra(&Internal::g_rendererData.GetDefaultBuffer(drawOrder, DrawBufferShapeType::Shape), rotateAngle, top, right, left, style.color.start, style.color.end, style.color.end, style, drawOrder);
                    else
                    {
                        TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                        Internal::FillTri_NoRound_VerHorGra(&buf, rotateAngle, top, right, left, style.color.start, style.color.end, style.color.end, style, drawOrder);
                    }
                }
                else if (style.color.gradientType == GradientType::Vertical)
                {
                    // Vertical, non rounded
                    if (style.textureHandle == 0)
                        Internal::FillTri_NoRound_VerHorGra(&Internal::g_rendererData.GetDefaultBuffer(drawOrder, DrawBufferShapeType::Shape), rotateAngle, top, right, left, style.color.end, style.color.end, style.color.start, style, drawOrder);
                    else
                    {
                        TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                        Internal::FillTri_NoRound_VerHorGra(&buf, rotateAngle, top, right, left, style.color.end, style.color.end, style.color.start, style, drawOrder);
                    }
                }
                else if (style.color.gradientType == GradientType::Radial || style.color.gradientType == GradientType::RadialCorner)
                {
                    // Radial, non rounded
                    if (style.textureHandle == 0)
                    {
                        GradientDrawBuffer& buf = Internal::g_rendererData.GetGradientBuffer(style.color, drawOrder, DrawBufferShapeType::Shape);
                        Internal::FillTri_NoRound_RadialGra(&buf, rotateAngle, top, right, left, style.color.start, style.color.end, style, drawOrder);
                    }
                    else
                    {
                        TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                        Internal::FillTri_NoRound_RadialGra(&buf, rotateAngle, top, right, left, style.color.start, style.color.end, style, drawOrder);
                    }
                }
            }
        }
        else
        {
            if (Math::IsEqual(style.color.start, style.color.end))
            {
                // Rounded, single m_color.
                if (style.textureHandle == 0)
                    Internal::FillTri_Round(&Internal::g_rendererData.GetDefaultBuffer(drawOrder, DrawBufferShapeType::Shape), style.onlyRoundTheseCorners, rotateAngle, top, right, left, style.color.start, style.rounding, style, drawOrder);
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                    Internal::FillTri_Round(&buf, style.onlyRoundTheseCorners, rotateAngle, top, right, left, style.color.start, style.rounding, style, drawOrder);
                }
            }
            else
            {
                // Rounded, gradient.
                if (style.textureHandle == 0)
                {
                    GradientDrawBuffer& buf = Internal::g_rendererData.GetGradientBuffer(style.color, drawOrder, DrawBufferShapeType::Shape);
                    Internal::FillTri_Round(&buf, style.onlyRoundTheseCorners, rotateAngle, top, right, left, style.color.start, style.rounding, style, drawOrder);
                }
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                    Internal::FillTri_Round(&buf, style.onlyRoundTheseCorners, rotateAngle, top, right, left, style.color.start, style.rounding, style, drawOrder);
                }
            }
        }
    }

    void DrawRect(const Vec2& min, const Vec2& max, StyleOptions& style, float rotateAngle, int drawOrder)
    {

        if (style.rounding == 0.0f)
        {
            // Single m_color
            if (Math::IsEqual(style.color.start, style.color.end))
            {
                if (style.textureHandle == 0)
                    Internal::FillRect_NoRound_SC(&Internal::g_rendererData.GetDefaultBuffer(drawOrder, DrawBufferShapeType::Shape), rotateAngle, min, max, style.color.start, style, drawOrder);
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                    Internal::FillRect_NoRound_SC(&buf, rotateAngle, min, max, style.color.start, style, drawOrder);
                }
            }
            else
            {
                if (style.color.gradientType == GradientType::Horizontal)
                {
                    // Horizontal, non rounded
                    if (style.textureHandle == 0)
                    {
                        Internal::FillRect_NoRound_VerHorGra(&Internal::g_rendererData.GetDefaultBuffer(drawOrder, DrawBufferShapeType::Shape), rotateAngle, min, max,
                                                             style.color.start, style.color.end, style.color.end, style.color.start, style, drawOrder);
                    }
                    else
                    {
                        TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                        Internal::FillRect_NoRound_VerHorGra(&buf, rotateAngle, min, max,
                                                             style.color.start, style.color.end, style.color.end, style.color.start, style, drawOrder);
                    }
                }
                else if (style.color.gradientType == GradientType::Vertical)
                {
                    // Vertical, non rounded
                    if (style.textureHandle == 0)
                    {
                        Internal::FillRect_NoRound_VerHorGra(&Internal::g_rendererData.GetDefaultBuffer(drawOrder, DrawBufferShapeType::Shape), rotateAngle, min, max,
                                                             style.color.start, style.color.start, style.color.end, style.color.end, style, drawOrder);
                    }
                    else
                    {
                        TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                        Internal::FillRect_NoRound_VerHorGra(&buf, rotateAngle, min, max,
                                                             style.color.start, style.color.start, style.color.end, style.color.end, style, drawOrder);
                    }
                }
                else if (style.color.gradientType == GradientType::Radial || style.color.gradientType == GradientType::RadialCorner)
                {
                    // Radial, non rounded
                    if (style.textureHandle == 0)
                    {
                        GradientDrawBuffer& buf = Internal::g_rendererData.GetGradientBuffer(style.color, drawOrder, DrawBufferShapeType::Shape);
                        Internal::FillRect_NoRound_RadialGra(&buf, rotateAngle, min, max, style.color.start, style.color.end, style, drawOrder);
                    }
                    else
                    {
                        TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                        Internal::FillRect_NoRound_RadialGra(&buf, rotateAngle, min, max, style.color.start, style.color.end, style, drawOrder);
                    }
                }
            }
        }
        else
        {
            if (Math::IsEqual(style.color.start, style.color.end))
            {
                // Rounded, single m_color.
                if (style.textureHandle == 0)
                {
                    Internal::FillRect_Round(&Internal::g_rendererData.GetDefaultBuffer(drawOrder, DrawBufferShapeType::Shape), style.onlyRoundTheseCorners, rotateAngle, min, max, style.color.start, style.rounding, style, drawOrder);
                }
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                    Internal::FillRect_Round(&buf, style.onlyRoundTheseCorners, rotateAngle, min, max, style.color.start, style.rounding, style, drawOrder);
                }
            }
            else
            {
                if (style.textureHandle == 0)
                {
                    // Rounded, gradient.
                    GradientDrawBuffer& buf = Internal::g_rendererData.GetGradientBuffer(style.color, drawOrder, DrawBufferShapeType::Shape);
                    Internal::FillRect_Round(&buf, style.onlyRoundTheseCorners, rotateAngle, min, max, style.color.start, style.rounding, style, drawOrder);
                }
                else
                {
                    // Rounded, gradient.
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                    Internal::FillRect_Round(&buf, style.onlyRoundTheseCorners, rotateAngle, min, max, style.color.start, style.rounding, style, drawOrder);
                }
            }
        }
    }

    void DrawNGon(const Vec2& center, float radius, int n, StyleOptions& style, float rotateAngle, int drawOrder)
    {
        if (Math::IsEqual(style.color.start, style.color.end))
        {
            if (style.textureHandle == 0)
                Internal::FillNGon_SC(&Internal::g_rendererData.GetDefaultBuffer(drawOrder, DrawBufferShapeType::Shape), rotateAngle, center, radius, n, style.color.start, style, drawOrder);
            else
            {
                TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                Internal::FillNGon_SC(&buf, rotateAngle, center, radius, n, style.color.start, style, drawOrder);
            }
        }
        else
        {
            if (style.color.gradientType == GradientType::Horizontal)
            {
                // Horizontal, non rounded
                if (style.textureHandle == 0)
                    Internal::FillNGon_VerHorGra(&Internal::g_rendererData.GetDefaultBuffer(drawOrder, DrawBufferShapeType::Shape), rotateAngle, center, radius, n, style.color.start, style.color.end, true, style, drawOrder);
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                    Internal::FillNGon_VerHorGra(&buf, rotateAngle, center, radius, n, style.color.start, style.color.end, true, style, drawOrder);
                }
            }
            else if (style.color.gradientType == GradientType::Vertical)
            {
                // Vertical, non rounded
                if (style.textureHandle == 0)
                    Internal::FillNGon_VerHorGra(&Internal::g_rendererData.GetDefaultBuffer(drawOrder, DrawBufferShapeType::Shape), rotateAngle, center, radius, n, style.color.start, style.color.end, false, style, drawOrder);
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                    Internal::FillNGon_VerHorGra(&buf, rotateAngle, center, radius, n, style.color.start, style.color.end, false, style, drawOrder);
                }
            }
            else if (style.color.gradientType == GradientType::Radial || style.color.gradientType == GradientType::RadialCorner)
            {
                // // Radial, non rounded
                if (style.textureHandle == 0)
                {
                    GradientDrawBuffer& buf = Internal::g_rendererData.GetGradientBuffer(style.color, drawOrder, DrawBufferShapeType::Shape);
                    Internal::FillNGon_RadialGra(&buf, rotateAngle, center, radius, n, style.color.start, style.color.end, style, drawOrder);
                }
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                    Internal::FillNGon_RadialGra(&buf, rotateAngle, center, radius, n, style.color.start, style.color.end, style, drawOrder);
                }
            }
        }
    }

    void DrawConvex(Vec2* points, int size, StyleOptions& style, float rotateAngle, int drawOrder)
    {
        if (size < 3)
        {
            if (Config.errorCallback)
                Config.errorCallback("LinaVG: Can draw a convex shape that has less than 3 corners!");
            return;
        }

        Vec2 avgCenter = Math::GetPolygonCentroidFast(points, size);

        if (Math::IsEqual(style.color.start, style.color.end))
        {
            if (style.textureHandle == 0)
                Internal::FillConvex_SC(&Internal::g_rendererData.GetDefaultBuffer(drawOrder, DrawBufferShapeType::Shape), rotateAngle, points, size, avgCenter, style.color.start, style, drawOrder);
            else
            {
                TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                Internal::FillConvex_SC(&buf, rotateAngle, points, size, avgCenter, style.color.start, style, drawOrder);
            }
        }
        else
        {
            if (style.color.gradientType == GradientType::Horizontal)
            {
                // Horizontal, non rounded
                if (style.textureHandle == 0)
                    Internal::FillConvex_VerHorGra(&Internal::g_rendererData.GetDefaultBuffer(drawOrder, DrawBufferShapeType::Shape), rotateAngle, points, size, avgCenter, style.color.start, style.color.end, true, style, drawOrder);
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                    Internal::FillConvex_VerHorGra(&buf, rotateAngle, points, size, avgCenter, style.color.start, style.color.end, true, style, drawOrder);
                }
            }
            else if (style.color.gradientType == GradientType::Vertical)
            {
                // Vertical, non rounded
                if (style.textureHandle == 0)
                    Internal::FillConvex_VerHorGra(&Internal::g_rendererData.GetDefaultBuffer(drawOrder, DrawBufferShapeType::Shape), rotateAngle, points, size, avgCenter, style.color.start, style.color.end, false, style, drawOrder);
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                    Internal::FillConvex_VerHorGra(&buf, rotateAngle, points, size, avgCenter, style.color.start, style.color.end, false, style, drawOrder);
                }
            }
            else if (style.color.gradientType == GradientType::Radial || style.color.gradientType == GradientType::RadialCorner)
            {
                // // Radial, non rounded
                if (style.textureHandle == 0)
                {
                    GradientDrawBuffer& buf = Internal::g_rendererData.GetGradientBuffer(style.color, drawOrder, DrawBufferShapeType::Shape);
                    Internal::FillConvex_RadialGra(&buf, rotateAngle, points, size, avgCenter, style.color.start, style.color.end, style, drawOrder);
                }
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                    Internal::FillConvex_RadialGra(&buf, rotateAngle, points, size, avgCenter, style.color.start, style.color.end, style, drawOrder);
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
        if (Math::IsEqual(style.color.start, style.color.end))
        {
            if (style.textureHandle == 0)
                Internal::FillCircle_SC(&Internal::g_rendererData.GetDefaultBuffer(drawOrder, DrawBufferShapeType::Shape), rotateAngle, center, radius, segments, style.color.start, startAngle, endAngle, style, drawOrder);
            else
            {
                TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                Internal::FillCircle_SC(&buf, rotateAngle, center, radius, segments, style.color.start, startAngle, endAngle, style, drawOrder);
            }
        }
        else
        {
            if (style.color.gradientType == GradientType::Horizontal)
            {
                // Horizontal, non rounded
                if (style.textureHandle == 0)
                    Internal::FillCircle_VerHorGra(&Internal::g_rendererData.GetDefaultBuffer(drawOrder, DrawBufferShapeType::Shape), rotateAngle, center, radius, segments, style.color.start, style.color.end, true, startAngle, endAngle, style, drawOrder);
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                    Internal::FillCircle_VerHorGra(&buf, rotateAngle, center, radius, segments, style.color.start, style.color.end, true, startAngle, endAngle, style, drawOrder);
                }
            }
            else if (style.color.gradientType == GradientType::Vertical)
            {
                // Vertical, non rounded
                if (style.textureHandle == 0)
                    Internal::FillCircle_VerHorGra(&Internal::g_rendererData.GetDefaultBuffer(drawOrder, DrawBufferShapeType::Shape), rotateAngle, center, radius, segments, style.color.start, style.color.end, false, startAngle, endAngle, style, drawOrder);
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                    Internal::FillCircle_VerHorGra(&buf, rotateAngle, center, radius, segments, style.color.start, style.color.end, false, startAngle, endAngle, style, drawOrder);
                }
            }
            else if (style.color.gradientType == GradientType::Radial || style.color.gradientType == GradientType::RadialCorner)
            {
                // // Radial, non rounded
                if (style.textureHandle == 0)
                {
                    GradientDrawBuffer& buf = Internal::g_rendererData.GetGradientBuffer(style.color, drawOrder, DrawBufferShapeType::Shape);
                    Internal::FillCircle_RadialGra(&buf, rotateAngle, center, radius, segments, style.color.start, style.color.end, startAngle, endAngle, style, drawOrder);
                }
                else
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.GetTextureBuffer(style.textureHandle, style.textureUVTiling, style.textureUVOffset, drawOrder, DrawBufferShapeType::Shape);
                    Internal::FillCircle_RadialGra(&buf, rotateAngle, center, radius, segments, style.color.start, style.color.end, startAngle, endAngle, style, drawOrder);
                }
            }
        }
    }

    LINAVG_API void DrawTextSDF(const LINAVG_STRING& text, const Vec2& position, const SDFTextOptions& opts, float rotateAngle, int drawOrder)
    {
        if (text.compare("") == 0)
            return;

        FontHandle  fontHandle = opts.font > 0 && Internal::g_textData.m_loadedFonts.m_size > opts.font - 1 ? opts.font : Internal::g_textData.m_defaultFont;
        LinaVGFont* font       = Internal::g_textData.m_loadedFonts[static_cast<int>(fontHandle) - 1];

        if (!font->m_isSDF)
        {
            if (Config.errorCallback)
                Config.errorCallback("LinaVG: The font you are trying to draw is not loaded as an SDF font, but DrawTextSDF was called! Please use DrawTextNormal().");
            return;
        }

        const float scale      = opts.textScale;
        DrawBuffer* buf        = &Internal::g_rendererData.GetSDFTextBuffer(font->m_texture, drawOrder, opts, false);
        const bool  isGradient = !Math::IsEqual(opts.color.start, opts.color.end);
        Internal::ProcessText(buf, font, text, position, Vec2(0.0f, 0.0f), opts.color, opts.spacing, isGradient, scale, opts.wrapWidth, rotateAngle, opts.alignment, opts.newLineSpacing, opts.sdfThickness);

        if (opts.dropShadowOffset.x != 0.0f || opts.dropShadowOffset.y != 0.0f)
        {
            SDFTextOptions usedOpts = SDFTextOptions(opts);
            usedOpts.sdfThickness = opts.sdfDropShadowThickness;
            usedOpts.sdfSoftness  = opts.sdfDropShadowSoftness;
            DrawBuffer* dsBuf       = &Internal::g_rendererData.GetSDFTextBuffer(font->m_texture, drawOrder, usedOpts, true);
            const int   dsStart     = buf->m_vertexBuffer.m_size;
            Internal::ProcessText(dsBuf, font, text, position, Vec2(opts.dropShadowOffset.x * Config.framebufferScale.x, opts.dropShadowOffset.y * Config.framebufferScale.y), opts.dropShadowColor, opts.spacing, false, scale, opts.wrapWidth, rotateAngle, opts.alignment, opts.newLineSpacing, opts.sdfThickness);
        }
    }

    void DrawTextNormal(const LINAVG_STRING& text, const Vec2& position, const TextOptions& opts, float rotateAngle, int drawOrder)
    {
        if (text.compare("") == 0)
            return;

        FontHandle  fontHandle = opts.font > 0 && Internal::g_textData.m_loadedFonts.m_size > opts.font - 1 ? opts.font : Internal::g_textData.m_defaultFont;
        LinaVGFont* font       = Internal::g_textData.m_loadedFonts[static_cast<int>(fontHandle) - 1];

        if (font->m_isSDF)
        {
            if (Config.errorCallback)
                Config.errorCallback("LinaVG: The font you are trying to draw with is loaded SDF font, but DrawTextNormal was called! Please use DrawTextSDF().");
            return;
        }

        const float scale      = opts.textScale;
        DrawBuffer* buf        = &Internal::g_rendererData.GetSimpleTextBuffer(font->m_texture, drawOrder, false);
        const bool  isGradient = !Math::IsEqual(opts.color.start, opts.color.end);
        Internal::ProcessText(buf, font, text, position, Vec2(0.0f, 0.0f), opts.color, opts.spacing, isGradient, scale, opts.wrapWidth, rotateAngle, opts.alignment, opts.newLineSpacing, 0.0f);
        if (opts.dropShadowOffset.x != 0.0f || opts.dropShadowOffset.y != 0.0f)
        {
            DrawBuffer* dsBuf = &Internal::g_rendererData.GetSimpleTextBuffer(font->m_texture, drawOrder, true);
            Internal::ProcessText(dsBuf, font, text, position, Vec2(opts.dropShadowOffset.x * Config.framebufferScale.x, opts.dropShadowOffset.y * Config.framebufferScale.y), opts.dropShadowColor, opts.spacing, false, scale, opts.wrapWidth, rotateAngle, opts.alignment, opts.newLineSpacing, 0.0f);
        }
    }

    LINAVG_API Vec2 CalculateTextSize(const LINAVG_STRING& text, TextOptions& opts)
    {
        FontHandle  fontHandle = opts.font > 0 && Internal::g_textData.m_loadedFonts.m_size > opts.font - 1 ? opts.font : Internal::g_textData.m_defaultFont;
        LinaVGFont* font       = Internal::g_textData.m_loadedFonts[static_cast<int>(fontHandle) - 1];
        const float scale      = opts.textScale;

        if (opts.wrapWidth == 0.0f)
            return Internal::CalcTextSize(text.c_str(), font, scale, opts.spacing, 0.0f);
        else
            return Internal::CalcTextSizeWrapped(text, font, opts.newLineSpacing, opts.wrapWidth, scale, opts.spacing, 0.0f);
    }

    LINAVG_API Vec2 CalculateTextSize(const LINAVG_STRING& text, SDFTextOptions& opts)
    {
        FontHandle  fontHandle = opts.font > 0 && Internal::g_textData.m_loadedFonts.m_size > opts.font - 1 ? opts.font : Internal::g_textData.m_defaultFont;
        LinaVGFont* font       = Internal::g_textData.m_loadedFonts[static_cast<int>(fontHandle) - 1];
        const float scale      = opts.textScale;

        if (opts.wrapWidth == 0.0f)
            return Internal::CalcTextSize(text.c_str(), font, scale, opts.spacing, opts.sdfThickness);
        else
            return Internal::CalcTextSizeWrapped(text, font, opts.newLineSpacing, opts.wrapWidth, scale, opts.spacing, opts.sdfThickness);
    }

    void Internal::FillRect_NoRound_VerHorGra(DrawBuffer* buf, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& colorTL, const Vec4& colorTR, const Vec4& colorBR, const Vec4& colorBL, StyleOptions& opts, int drawOrder)
    {
        Vertex v[4];
        FillRectData(v, false, min, max);
        const int current = buf->m_vertexBuffer.m_size;
        v[0].col        = colorTL;
        v[1].col        = colorTR;
        v[2].col        = colorBR;
        v[3].col        = colorBL;

        for (int i = 0; i < 4; i++)
            buf->PushVertex(v[i]);

        const Vec2 center = Vec2((min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f);

        if (opts.isFilled)
        {
            buf->PushIndex(current);
            buf->PushIndex(current + 1);
            buf->PushIndex(current + 3);
            buf->PushIndex(current + 1);
            buf->PushIndex(current + 2);
            buf->PushIndex(current + 3);
        }
        else
            ConvexExtrudeVertices(buf, center, current, current + 3, opts.thickness.start);

        RotateVertices(buf->m_vertexBuffer, center, current, opts.isFilled ? current + 3 : current + 7, rotateAngle);

        if (opts.outlineOptions.thickness != 0.0f)
        {
            buf = DrawOutline(buf, opts, opts.isFilled ? 4 : 8, false, drawOrder);
        }
        else if (Config.aaEnabled)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.isFilled ? 4 : 8, false, drawOrder, Internal::OutlineCallType::AA);
        }
    }

    void Internal::FillRect_NoRound_SC(DrawBuffer* buf, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& color, StyleOptions& opts, int drawOrder)
    {
        Vertex v[4];
        FillRectData(v, false, min, max);
        v[0].col = color;
        v[1].col = color;
        v[2].col = color;
        v[3].col = color;

        const int current = buf->m_vertexBuffer.m_size;

        for (int i = 0; i < 4; i++)
            buf->PushVertex(v[i]);

        const Vec2 center = Vec2((min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f);

        if (opts.isFilled)
        {
            buf->PushIndex(current);
            buf->PushIndex(current + 1);
            buf->PushIndex(current + 3);
            buf->PushIndex(current + 1);
            buf->PushIndex(current + 2);
            buf->PushIndex(current + 3);
        }
        else
            ConvexExtrudeVertices(buf, center, current, current + 3, opts.thickness.start);

        RotateVertices(buf->m_vertexBuffer, center, current, opts.isFilled ? current + 3 : current + 7, rotateAngle);

        if (opts.outlineOptions.thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.isFilled ? 4 : 8, false, drawOrder);
        else if (Config.aaEnabled)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.isFilled ? 4 : 8, false, drawOrder, Internal::OutlineCallType::AA);
        }
    }

    void Internal::FillRect_NoRound_RadialGra(DrawBuffer* buf, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& startColor, const Vec4& endColor, StyleOptions& opts, int drawOrder)
    {
        Vertex v[5];
        FillRectData(v, true, min, max);
        int startIndex = buf->m_vertexBuffer.m_size;

        const int loopStart = opts.isFilled ? 0 : 1;
        for (int i = loopStart; i < 5; i++)
            buf->PushVertex(v[i]);

        const Vec2 center = Vec2((min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f);

        if (opts.isFilled)
            ConvexFillVertices(startIndex, startIndex + 4, buf->m_indexBuffer);
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + 3, opts.thickness.start);

        RotateVertices(buf->m_vertexBuffer, center, opts.isFilled ? startIndex + 1 : startIndex, opts.isFilled ? startIndex + 4 : startIndex + 7, rotateAngle);

        if (opts.outlineOptions.thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.isFilled ? 4 : 8, false, drawOrder);
        else if (Config.aaEnabled)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.isFilled ? 4 : 8, false, drawOrder, Internal::OutlineCallType::AA);
        }
    }

    void Internal::FillRect_Round(DrawBuffer* buf, Array<int>& roundedCorners, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& col, float rounding, StyleOptions& opts, int drawOrder)
    {
        rounding = Math::Clamp(rounding, 0.0f, 0.9f);

        Vertex v[4];
        FillRectData(v, false, min, max);
        v[0].col = col;
        v[1].col = col;
        v[2].col = col;
        v[3].col = col;

        // Calculate direction vectors
        const Vec2  center           = Vec2((min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f);
        Vec2        up               = Vec2(v[0].pos.x - v[3].pos.x, v[0].pos.y - v[3].pos.y);
        Vec2        right            = Vec2(v[1].pos.x - v[0].pos.x, v[1].pos.y - v[0].pos.y);
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
        // now calculate it's vertex m_color & push it down the pipeline.
        if (opts.isFilled)
        {
            Vertex c;
            c.pos = center;
            c.col = col;
            c.uv  = Vec2(0.5f, 0.5f);
            buf->PushVertex(c);
        }

        for (int i = 0; i < 4; i++)
        {
            const int found = roundedCorners.findIndex(i);
            if (roundedCorners.m_size != 0 && found == -1)
            {
                Vertex cornerVertex;
                cornerVertex.pos = v[i].pos;
                cornerVertex.col = col;
                cornerVertex.uv  = v[i].uv;
                buf->PushVertex(cornerVertex);
                vertexCount++;
                startAngle += 90.0f;
                endAngle += 90.0f;
                continue;
            }

            const Vec2 usedRight = (i == 0 || i == 3) ? right : Vec2(-right.x, -right.y);
            const Vec2 usedUp    = (i == 0 || i == 1) ? Vec2(-up.x, -up.y) : up;
            Vec2       inf0      = Vec2(v[i].pos.x + usedUp.x * roundingMag, v[i].pos.y + usedUp.y * roundingMag);
            Vec2       inf1      = Vec2(inf0.x + usedRight.x * roundingMag, inf0.y + usedRight.y * roundingMag);

            // After a vector is inflated, we use it as a center to draw an arc, arc range is based on which corner we are currently drawing.
            for (float k = startAngle; k < endAngle + 2.5f; k += angleIncrease)
            {
                const Vec2 p = Math::GetPointOnCircle(inf1, roundingMag, k);

                Vertex cornerVertex;
                cornerVertex.pos = p;
                cornerVertex.col = col;
                buf->PushVertex(cornerVertex);
                vertexCount++;
            }
            startAngle += 90.0f;
            endAngle += 90.0f;
        }

        if (opts.isFilled)
        {
            CalculateVertexUVs(buf, startIndex, opts.isFilled ? startIndex + vertexCount : startIndex + vertexCount - 1);
            ConvexFillVertices(startIndex, startIndex + vertexCount, buf->m_indexBuffer);
        }
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + vertexCount - 1, opts.thickness.start);

        RotateVertices(buf->m_vertexBuffer, center, opts.isFilled ? startIndex + 1 : startIndex, opts.isFilled ? startIndex + vertexCount : startIndex + (vertexCount * 2) - 1, rotateAngle);

        if (opts.outlineOptions.thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.isFilled ? vertexCount : vertexCount * 2, false, drawOrder);
        else if (Config.aaEnabled)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.isFilled ? vertexCount : vertexCount * 2, false, drawOrder, Internal::OutlineCallType::AA);
        }
    }

    void Internal::FillRectData(Vertex* v, bool hasCenter, const Vec2& min, const Vec2& max)
    {
        const int  i      = hasCenter ? 1 : 0;
        const Vec2 center = Vec2((max.x + min.x) / 2.0f, (max.y + min.y) / 2.0f);

        if (hasCenter)
        {
            v[0].pos = center;
            v[0].uv  = Vec2((g_uvOverride.m_uvTL.x + g_uvOverride.m_uvBR.x) / 2.0f, (g_uvOverride.m_uvTL.y + g_uvOverride.m_uvBR.y) / 2.0f);
        }

        if (!g_rectOverrideData.overrideRectPositions)
        {
            v[i].pos       = min;
            v[i + 1].pos.x = max.x;
            v[i + 1].pos.y = min.y;
            v[i + 2].pos.x = max.x;
            v[i + 2].pos.y = max.y;
            v[i + 3].pos.x = min.x;
            v[i + 3].pos.y = max.y;
        }
        else
        {
            v[i].pos     = g_rectOverrideData.m_p1;
            v[i + 1].pos = g_rectOverrideData.m_p2;
            v[i + 2].pos = g_rectOverrideData.m_p3;
            v[i + 3].pos = g_rectOverrideData.m_p4;
        }

        v[i].uv     = g_uvOverride.m_uvTL;
        v[i + 1].uv = Vec2(g_uvOverride.m_uvBR.x, g_uvOverride.m_uvTL.y);
        v[i + 2].uv = g_uvOverride.m_uvBR;
        v[i + 3].uv = Vec2(g_uvOverride.m_uvTL.x, g_uvOverride.m_uvBR.y);
    }

    void Internal::FillTri_NoRound_VerHorGra(DrawBuffer* buf, float rotateAngle, const Vec2& p3, const Vec2& p2, const Vec2& p1, const Vec4& colorLeft, const Vec4& colorRight, const Vec4& colorTop, StyleOptions& opts, int drawOrder)
    {
        Vertex v[3];
        FillTriData(v, false, true, p3, p2, p1);
        v[0].col = colorTop;
        v[1].col = colorRight;
        v[2].col = colorLeft;

        const int startIndex = buf->m_vertexBuffer.m_size;

        for (int i = 0; i < 3; i++)
            buf->PushVertex(v[i]);

        const Vec2 center = Vec2((p1.x + p2.x + p3.x) / 3.0f, (p1.y + p2.y + p3.y) / 3.0f);

        if (opts.isFilled)
        {
            buf->PushIndex(startIndex);
            buf->PushIndex(startIndex + 1);
            buf->PushIndex(startIndex + 2);
        }
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + 2, opts.thickness.start);

        RotateVertices(buf->m_vertexBuffer, center, startIndex, opts.isFilled ? startIndex + 2 : startIndex + 5, rotateAngle);

        if (opts.outlineOptions.thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.isFilled ? 3 : 6, false, drawOrder);
        else if (Config.aaEnabled)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.isFilled ? 3 : 6, false, drawOrder, Internal::OutlineCallType::AA);
        }
    }

    void Internal::FillTri_NoRound_SC(DrawBuffer* buf, float rotateAngle, const Vec2& p3, const Vec2& p2, const Vec2& p1, const Vec4& color, StyleOptions& opts, int drawOrder)
    {
        Vertex v[3];
        FillTriData(v, false, true, p3, p2, p1);
        v[0].col = color;
        v[1].col = color;
        v[2].col = color;

        const int startIndex = buf->m_vertexBuffer.m_size;

        for (int i = 0; i < 3; i++)
            buf->PushVertex(v[i]);

        const Vec2 center = Vec2((p1.x + p2.x + p3.x) / 3.0f, (p1.y + p2.y + p3.y) / 3.0f);

        if (opts.isFilled)
        {
            buf->PushIndex(startIndex);
            buf->PushIndex(startIndex + 1);
            buf->PushIndex(startIndex + 2);
        }
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + 2, opts.thickness.start);

        RotateVertices(buf->m_vertexBuffer, center, startIndex, opts.isFilled ? startIndex + 2 : startIndex + 5, rotateAngle);

        if (opts.outlineOptions.thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.isFilled ? 3 : 6, false, drawOrder);
        else if (Config.aaEnabled)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.isFilled ? 3 : 6, false, drawOrder, Internal::OutlineCallType::AA);
        }
    }

    void Internal::FillTri_NoRound_RadialGra(DrawBuffer* buf, float rotateAngle, const Vec2& p3, const Vec2& p2, const Vec2& p1, const Vec4& startcolor, const Vec4& endColor, StyleOptions& opts, int drawOrder)
    {
        Vertex v[4];
        Vec2   points[3] = {p1, p2, p3};
        FillTriData(v, true, true, p3, p2, p1);
        int startIndex = buf->m_vertexBuffer.m_size;

        const int loopStart = opts.isFilled ? 0 : 1;
        for (int i = loopStart; i < 4; i++)
            buf->PushVertex(v[i]);

        const Vec2 center = v[0].pos;

        if (opts.isFilled)
            ConvexFillVertices(startIndex, startIndex + 3, buf->m_indexBuffer);
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + 2, opts.thickness.start);

        RotateVertices(buf->m_vertexBuffer, center, opts.isFilled ? startIndex + 1 : startIndex, opts.isFilled ? startIndex + 3 : startIndex + 5, rotateAngle);

        if (opts.outlineOptions.thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.isFilled ? 3 : 6, false, drawOrder);
        else if (Config.aaEnabled)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.isFilled ? 3 : 6, false, drawOrder, Internal::OutlineCallType::AA);
        }
    }

    void Internal::FillTri_Round(DrawBuffer* buf, Array<int>& onlyRoundCorners, float rotateAngle, const Vec2& p3, const Vec2& p2, const Vec2& p1, const Vec4& col, float rounding, StyleOptions& opts, int drawOrder)
    {
        rounding = Math::Clamp(rounding, 0.0f, 1.0f);

        Vertex v[3];
        FillTriData(v, false, false, p3, p2, p1);
        v[0].col = col;
        v[1].col = col;
        v[2].col = col;

        Vec2 bbMin, bbMax;
        GetTriangleBoundingBox(p1, p2, p3, bbMin, bbMax);
        const Vec2  center          = Vec2((p1.x + p2.x + p3.x) / 3.0f, (p1.y + p2.y + p3.y) / 3.0f);
        const Vec2  v01Edge         = Vec2(v[0].pos.x - v[1].pos.x, v[0].pos.y - v[1].pos.y);
        const Vec2  v02Edge         = Vec2(v[0].pos.x - v[2].pos.x, v[0].pos.y - v[2].pos.y);
        const Vec2  v12Edge         = Vec2(v[1].pos.x - v[2].pos.x, v[1].pos.y - v[2].pos.y);
        const Vec2  v01Center       = Vec2((v[0].pos.x + v[1].pos.x) / 2.0f, (v[0].pos.y + v[1].pos.y) / 2.0f);
        const Vec2  v02Center       = Vec2((v[0].pos.x + v[2].pos.x) / 2.0f, (v[0].pos.y + v[2].pos.y) / 2.0f);
        const Vec2  v12Center       = Vec2((v[1].pos.x + v[2].pos.x) / 2.0f, (v[1].pos.y + v[2].pos.y) / 2.0f);
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
        // now calculate it's vertex m_color & push it down the pipeline.
        if (opts.isFilled)
        {
            Vertex c;
            c.pos  = center;
            c.col  = col;
            c.uv.x = Math::Remap(c.pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            c.uv.y = Math::Remap(c.pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            buf->PushVertex(c);
        }

        const float angleOffset = maxAngle > 90.0f ? maxAngle - 90.0f : 45.0f;

        for (int i = 0; i < 3; i++)
        {

            if (onlyRoundCorners.m_size != 0 && onlyRoundCorners.findIndex(i) == -1)
            {
                Vertex cornerVertex;
                cornerVertex.col = col;
                cornerVertex.pos = v[i].pos;
                vertexCount++;
                buf->PushVertex(cornerVertex);
                continue;
            }

            if (i == 0)
            {
                const Vec2  toCenter01 = Math::Normalized(Vec2(v01Center.x - v[i].pos.x, v01Center.y - v[i].pos.y));
                const Vec2  toCenter02 = Math::Normalized(Vec2(v02Center.x - v[i].pos.x, v02Center.y - v[i].pos.y));
                const Vec2  inter1     = Vec2(v[i].pos.x + toCenter01.x * roundingMag, v[i].pos.y + toCenter01.y * roundingMag);
                const Vec2  inter2     = Vec2(v[i].pos.x + toCenter02.x * roundingMag, v[i].pos.y + toCenter02.y * roundingMag);
                Array<Vec2> arc;

                GetArcPoints(arc, inter1, inter2, v[i].pos, 0.0f, 36, false, angleOffset);

                for (int j = 0; j < arc.m_size; j++)
                {
                    Vertex cornerVertex;
                    cornerVertex.col = col;
                    cornerVertex.pos = arc[j];
                    buf->PushVertex(cornerVertex);
                    vertexCount++;
                }
            }
            else if (i == 1)
            {

                const Vec2  toCenter01 = Math::Normalized(Vec2(v01Center.x - v[i].pos.x, v01Center.y - v[i].pos.y));
                const Vec2  toCenter12 = Math::Normalized(Vec2(v12Center.x - v[i].pos.x, v12Center.y - v[i].pos.y));
                const Vec2  inter1     = Vec2(v[i].pos.x + toCenter01.x * roundingMag, v[i].pos.y + toCenter01.y * roundingMag);
                const Vec2  inter2     = Vec2(v[i].pos.x + toCenter12.x * roundingMag, v[i].pos.y + toCenter12.y * roundingMag);
                Array<Vec2> arc;
                GetArcPoints(arc, inter1, inter2, v[i].pos, 0.0f, 36, false, angleOffset);
                for (int j = 0; j < arc.m_size; j++)
                {

                    Vertex cornerVertex;
                    cornerVertex.col = col;
                    cornerVertex.pos = arc[j];
                    buf->PushVertex(cornerVertex);
                    vertexCount++;
                }
            }
            else if (i == 2)
            {

                const Vec2  toCenter12 = Math::Normalized(Vec2(v12Center.x - v[i].pos.x, v12Center.y - v[i].pos.y));
                const Vec2  toCenter02 = Math::Normalized(Vec2(v02Center.x - v[i].pos.x, v02Center.y - v[i].pos.y));
                const Vec2  inter1     = Vec2(v[i].pos.x + toCenter12.x * roundingMag, v[i].pos.y + toCenter12.y * roundingMag);
                const Vec2  inter2     = Vec2(v[i].pos.x + toCenter02.x * roundingMag, v[i].pos.y + toCenter02.y * roundingMag);
                Array<Vec2> arc;
                GetArcPoints(arc, inter1, inter2, v[i].pos, 0.0f, 36, false, angleOffset);
                for (int j = 0; j < arc.m_size; j++)
                {

                    Vertex cornerVertex;
                    cornerVertex.col = col;
                    cornerVertex.pos = arc[j];
                    buf->PushVertex(cornerVertex);
                    vertexCount++;
                }
            }
        }

        if (opts.isFilled)
        {
            CalculateVertexUVs(buf, startIndex, opts.isFilled ? startIndex + vertexCount : startIndex + vertexCount - 1);
            ConvexFillVertices(startIndex, startIndex + vertexCount, buf->m_indexBuffer);
        }
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + vertexCount - 1, opts.thickness.start);

        RotateVertices(buf->m_vertexBuffer, center, opts.isFilled ? startIndex + 1 : startIndex, opts.isFilled ? startIndex + vertexCount : startIndex + (vertexCount * 2) - 1, rotateAngle);

        if (opts.outlineOptions.thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.isFilled ? vertexCount : vertexCount * 2, false, drawOrder);
        else if (Config.aaEnabled)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.isFilled ? vertexCount : vertexCount * 2, false, drawOrder, Internal::OutlineCallType::AA);
        }
    }

    void Internal::FillTriData(Vertex* v, bool hasCenter, bool calculateUV, const Vec2& p3, const Vec2& p2, const Vec2& p1)
    {
        const int i = hasCenter ? 1 : 0;
        if (hasCenter)
        {
            Vec2 center = Vec2((p1.x + p2.x + p3.x) / 3.0f, (p1.y + p2.y + p3.y) / 3.0f);
            v[0].pos  = center;
        }

        v[i].pos     = p3;
        v[i + 1].pos = p2;
        v[i + 2].pos = p1;

        if (calculateUV)
        {
            Vec2 min, max;
            GetTriangleBoundingBox(p1, p2, p3, min, max);

            const int end = hasCenter ? 4 : 3;

            for (int i = 0; i < end; i++)
            {
                v[i].uv.x = Math::Remap(v[i].pos.x, min.x, max.x, 0.0f, 1.0f);
                v[i].uv.y = Math::Remap(v[i].pos.y, min.y, max.y, 0.0f, 1.0f);
            }
        }
    }

    void Internal::FillNGon_SC(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int n, const Vec4& color, StyleOptions& opts, int drawOrder)
    {
        Array<Vertex> v;
        FillNGonData(v, opts.isFilled, center, radius, n);

        const int startIndex = buf->m_vertexBuffer.m_size;

        for (int i = 0; i < v.m_size; i++)
        {
            v[i].col = color;
            buf->PushVertex(v[i]);
        }

        if (opts.isFilled)
            ConvexFillVertices(startIndex, startIndex + n, buf->m_indexBuffer);
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + n - 1, opts.thickness.start);

        RotateVertices(buf->m_vertexBuffer, center, opts.isFilled ? startIndex + 1 : startIndex, opts.isFilled ? startIndex + n : startIndex + (n * 2) - 1, rotateAngle);

        if (opts.outlineOptions.thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.isFilled ? n : n * 2, false, drawOrder);
        else if (Config.aaEnabled)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.isFilled ? n : n * 2, false, drawOrder, Internal::OutlineCallType::AA);
        }
    }

    void Internal::FillNGon_VerHorGra(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int n, const Vec4& colorStart, const Vec4& colorEnd, bool isHor, StyleOptions& opts, int drawOrder)
    {
        Array<Vertex> v;
        FillNGonData(v, opts.isFilled, center, radius, n);

        const int startIndex = buf->m_vertexBuffer.m_size;

        for (int i = 0; i < v.m_size; i++)
        {
            v[i].col = Math::Lerp(colorStart, colorEnd, isHor ? v[i].uv.x : v[i].uv.y);
            buf->PushVertex(v[i]);
        }

        if (opts.isFilled)
            ConvexFillVertices(startIndex, startIndex + n, buf->m_indexBuffer);
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + n - 1, opts.thickness.start);

        RotateVertices(buf->m_vertexBuffer, center, opts.isFilled ? startIndex + 1 : startIndex, opts.isFilled ? startIndex + n : startIndex + (n * 2) - 1, rotateAngle);

        if (opts.outlineOptions.thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.isFilled ? n : n * 2, false, drawOrder);
        else if (Config.aaEnabled)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.isFilled ? n : n * 2, false, drawOrder, Internal::OutlineCallType::AA);
        }
    }

    void Internal::FillNGon_RadialGra(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int n, const Vec4& colorStart, const Vec4& colorEnd, StyleOptions& opts, int drawOrder)
    {
        Array<Vertex> v;
        FillNGonData(v, opts.isFilled, center, radius, n);

        const int startIndex = buf->m_vertexBuffer.m_size;

        for (int i = 0; i < v.m_size; i++)
        {
            buf->PushVertex(v[i]);
        }

        if (opts.isFilled)
            ConvexFillVertices(startIndex, startIndex + n, buf->m_indexBuffer);
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + n - 1, opts.thickness.start);

        RotateVertices(buf->m_vertexBuffer, center, opts.isFilled ? startIndex + 1 : startIndex, opts.isFilled ? startIndex + n : startIndex + (n * 2) - 1, rotateAngle);

        if (opts.outlineOptions.thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.isFilled ? n : n * 2, false, drawOrder);
        else if (Config.aaEnabled)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.isFilled ? n : n * 2, false, drawOrder, Internal::OutlineCallType::AA);
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
            v.pos  = center;
            v.uv.x = Math::Remap(v.pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            v.uv.y = Math::Remap(v.pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            vertArray.push_back(v);
        }

        int count = 0;
        for (float i = 0.0f; i < 360.0f; i += angleIncrease)
        {
            Vertex v;
            v.pos  = Math::GetPointOnCircle(center, radius, i);
            v.uv.x = Math::Remap(v.pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            v.uv.y = Math::Remap(v.pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            vertArray.push_back(v);
            count++;

            if (count == n)
                return;
        }
    }

    void Internal::FillCircle_SC(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int segments, const Vec4& color, float startAngle, float endAngle, StyleOptions& opts, int drawOrder)
    {
        Array<Vertex> v;
        FillCircleData(v, opts.isFilled, center, radius, segments, startAngle, endAngle);

        const int startIndex = buf->m_vertexBuffer.m_size;

        for (int i = 0; i < v.m_size; i++)
        {
            v[i].col = color;
            buf->PushVertex(v[i]);
        }

        const bool isFullCircle = Math::Abs(endAngle - startAngle) == 360.0f;
        const int  totalSize    = v.m_size - 1;

        if (opts.isFilled)
            ConvexFillVertices(startIndex, startIndex + totalSize, buf->m_indexBuffer, !isFullCircle);
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + totalSize, opts.thickness.start, !isFullCircle);

        RotateVertices(buf->m_vertexBuffer, center, opts.isFilled ? startIndex + 1 : startIndex, opts.isFilled ? startIndex + totalSize : startIndex + (totalSize * 2) + 1, rotateAngle);

        if (opts.outlineOptions.thickness != 0.0f)
        {
            if (isFullCircle)
                buf = DrawOutline(buf, opts, opts.isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder);
            else
            {
                if (opts.isFilled)
                {
                    Array<int> indices;

                    for (int i = 0; i < v.m_size; i++)
                        indices.push_back(startIndex + i);

                    buf = DrawOutlineAroundShape(buf, opts, &indices[0], v.m_size, opts.outlineOptions.thickness, true, drawOrder);
                }
                else if (opts.outlineOptions.drawDirection == OutlineDrawDirection::Both)
                {
                    Array<int> indices;

                    const int halfSize = v.m_size;
                    const int fullSize = halfSize * 2;
                    for (int i = 0; i < halfSize; i++)
                        indices.push_back(startIndex + i);

                    for (int i = fullSize - 1; i > halfSize - 1; i--)
                        indices.push_back(startIndex + i);

                    buf = DrawOutlineAroundShape(buf, opts, &indices[0], halfSize * 2, opts.outlineOptions.thickness, false, drawOrder);
                }
                else
                    buf = DrawOutline(buf, opts, opts.isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder);
            }
        }
        else if (Config.aaEnabled)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);

            if (opts.isFilled)
            {
                if (isFullCircle)
                {
                    buf = DrawOutline(buf, opts, opts.isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder, Internal::OutlineCallType::AA);
                }
                else
                {
                    Array<int> indices;

                    for (int i = 0; i < v.m_size; i++)
                        indices.push_back(startIndex + i);

                    buf = DrawOutlineAroundShape(buf, opts2, &indices[0], v.m_size, opts2.outlineOptions.thickness, true, drawOrder, Internal::OutlineCallType::AA);
                }
            }
            else if (opts.outlineOptions.drawDirection == OutlineDrawDirection::Both)
            {
                Array<int> indices;

                const int halfSize = v.m_size;
                const int fullSize = halfSize * 2;
                for (int i = 0; i < halfSize; i++)
                    indices.push_back(startIndex + i);

                for (int i = fullSize - 1; i > halfSize - 1; i--)
                    indices.push_back(startIndex + i);

                buf = DrawOutlineAroundShape(buf, opts2, &indices[0], halfSize * 2, opts2.outlineOptions.thickness, false, drawOrder, Internal::OutlineCallType::AA);
            }
            else
                buf = DrawOutline(buf, opts2, opts2.isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder, Internal::OutlineCallType::AA);
        }
    }

    void Internal::FillCircle_VerHorGra(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int segments, const Vec4& colorStart, const Vec4& colorEnd, bool isHor, float startAngle, float endAngle, StyleOptions& opts, int drawOrder)
    {
        Array<Vertex> v;
        FillCircleData(v, opts.isFilled, center, radius, segments, startAngle, endAngle);

        const int startIndex = buf->m_vertexBuffer.m_size;

        for (int i = 0; i < v.m_size; i++)
        {
            v[i].col = Math::Lerp(colorStart, colorEnd, isHor ? v[i].uv.x : v[i].uv.y);
            buf->PushVertex(v[i]);
        }

        const bool isFullCircle = Math::Abs(endAngle - startAngle) == 360.0f;
        const int  totalSize    = v.m_size - 1;

        if (opts.isFilled)
            ConvexFillVertices(startIndex, startIndex + totalSize, buf->m_indexBuffer, !isFullCircle);
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + totalSize, opts.thickness.start, !isFullCircle);

        RotateVertices(buf->m_vertexBuffer, center, opts.isFilled ? startIndex + 1 : startIndex, opts.isFilled ? startIndex + totalSize : startIndex + (totalSize * 2) + 1, rotateAngle);

        if (opts.outlineOptions.thickness != 0.0f)
        {
            if (isFullCircle)
                buf = DrawOutline(buf, opts, opts.isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder);
            else
            {
                if (opts.isFilled)
                {
                    Array<int> indices;

                    for (int i = 0; i < v.m_size; i++)
                        indices.push_back(startIndex + i);

                    buf = DrawOutlineAroundShape(buf, opts, &indices[0], v.m_size, opts.outlineOptions.thickness, true, drawOrder);
                }
                else if (opts.outlineOptions.drawDirection == OutlineDrawDirection::Both)
                {
                    Array<int> indices;

                    const int halfSize = v.m_size;
                    const int fullSize = halfSize * 2;
                    for (int i = 0; i < halfSize; i++)
                        indices.push_back(startIndex + i);

                    for (int i = fullSize - 1; i > halfSize - 1; i--)
                        indices.push_back(startIndex + i);

                    buf = DrawOutlineAroundShape(buf, opts, &indices[0], halfSize * 2, opts.outlineOptions.thickness, false, drawOrder);
                }
                else
                    buf = DrawOutline(buf, opts, opts.isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder);
            }
        }
        else if (Config.aaEnabled)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);

            if (opts.isFilled)
            {
                if (isFullCircle)
                    buf = DrawOutline(buf, opts, opts.isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder, Internal::OutlineCallType::AA);
                else
                {
                    Array<int> indices;

                    for (int i = 0; i < v.m_size; i++)
                        indices.push_back(startIndex + i);

                    buf = DrawOutlineAroundShape(buf, opts2, &indices[0], v.m_size, opts2.outlineOptions.thickness, true, drawOrder, Internal::OutlineCallType::AA);
                }
            }
            else if (opts.outlineOptions.drawDirection == OutlineDrawDirection::Both)
            {
                Array<int> indices;

                const int halfSize = v.m_size;
                const int fullSize = halfSize * 2;
                for (int i = 0; i < halfSize; i++)
                    indices.push_back(startIndex + i);

                for (int i = fullSize - 1; i > halfSize - 1; i--)
                    indices.push_back(startIndex + i);

                buf = DrawOutlineAroundShape(buf, opts2, &indices[0], halfSize * 2, opts2.outlineOptions.thickness, false, drawOrder, Internal::OutlineCallType::AA);
            }
            else
                buf = DrawOutline(buf, opts2, opts2.isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder, Internal::OutlineCallType::AA);
        }
    }

    void Internal::FillCircle_RadialGra(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int segments, const Vec4& colorStart, const Vec4& colorEnd, float startAngle, float endAngle, StyleOptions& opts, int drawOrder)
    {
        Array<Vertex> v;
        FillCircleData(v, opts.isFilled, center, radius, segments, startAngle, endAngle);

        const int startIndex = buf->m_vertexBuffer.m_size;

        for (int i = 0; i < v.m_size; i++)
        {
            if (i == 0)
                v[i].col = opts.color.start;
            else
                v[i].col = opts.color.end;
            buf->PushVertex(v[i]);
        }

        const bool isFullCircle = Math::Abs(endAngle - startAngle) == 360.0f;
        const int  totalSize    = v.m_size - 1;

        if (opts.isFilled)
            ConvexFillVertices(startIndex, startIndex + totalSize, buf->m_indexBuffer, !isFullCircle);
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + totalSize, opts.thickness.start, !isFullCircle);

        RotateVertices(buf->m_vertexBuffer, center, opts.isFilled ? startIndex + 1 : startIndex, opts.isFilled ? startIndex + totalSize : startIndex + (totalSize * 2) + 1, rotateAngle);

        if (opts.outlineOptions.thickness != 0.0f)
        {
            if (isFullCircle)
                buf = DrawOutline(buf, opts, opts.isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder);
            else
            {
                if (opts.isFilled)
                {
                    Array<int> indices;

                    for (int i = v.m_size - 1; i > -1; i--)
                        indices.push_back(startIndex + i);

                    buf = DrawOutlineAroundShape(buf, opts, &indices[0], v.m_size, opts.outlineOptions.thickness, false, drawOrder);
                }
                else if (opts.outlineOptions.drawDirection == OutlineDrawDirection::Both)
                {
                    Array<int> indices;

                    const int halfSize = v.m_size;
                    const int fullSize = halfSize * 2;
                    for (int i = 0; i < halfSize; i++)
                        indices.push_back(startIndex + i);

                    for (int i = fullSize - 1; i > halfSize - 1; i--)
                        indices.push_back(startIndex + i);

                    buf = DrawOutlineAroundShape(buf, opts, &indices[0], halfSize * 2, opts.outlineOptions.thickness, false, drawOrder);
                }
                else
                    buf = DrawOutline(buf, opts, opts.isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder);
            }
        }
        else if (Config.aaEnabled)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);

            if (opts.isFilled)
            {
                if (isFullCircle)
                    buf = DrawOutline(buf, opts, opts.isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder, Internal::OutlineCallType::AA);
                else
                {
                    Array<int> indices;

                    for (int i = 0; i < v.m_size; i++)
                        indices.push_back(startIndex + i);

                    buf = DrawOutlineAroundShape(buf, opts2, &indices[0], v.m_size, opts2.outlineOptions.thickness, true, drawOrder, Internal::OutlineCallType::AA);
                }
            }
            else if (opts.outlineOptions.drawDirection == OutlineDrawDirection::Both)
            {
                Array<int> indices;

                const int halfSize = v.m_size;
                const int fullSize = halfSize * 2;
                for (int i = 0; i < halfSize; i++)
                    indices.push_back(startIndex + i);

                for (int i = fullSize - 1; i > halfSize - 1; i--)
                    indices.push_back(startIndex + i);

                buf = DrawOutlineAroundShape(buf, opts2, &indices[0], halfSize * 2, opts2.outlineOptions.thickness, false, drawOrder, Internal::OutlineCallType::AA);
            }
            else
                buf = DrawOutline(buf, opts2, opts2.isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder, Internal::OutlineCallType::AA);
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
            c.pos  = center;
            c.uv.x = Math::Remap(c.pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            c.uv.y = Math::Remap(c.pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            vertices.push_back(c);
        }

        const float end       = Math::Abs(startAngle - endAngle) == 360.0f ? endAngle : endAngle + angleIncrease;
        Vec2        nextPoint = Vec2(-1.0f, -1.0f);
        Vec2        lastPoint = Vec2(-1.0f, -1.0f);
        for (float i = startAngle; i < end; i += angleIncrease)
        {
            Vertex v;
            v.pos   = Math::GetPointOnCircle(center, radius, i);
            v.uv.x  = Math::Remap(v.pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            v.uv.y  = Math::Remap(v.pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            v.col.w = 1.0f;
            vertices.push_back(v);
        }
    }

    void Internal::FillConvex_SC(DrawBuffer* buf, float rotateAngle, Vec2* points, int size, const Vec2& center, const Vec4& color, StyleOptions& opts, int drawOrder)
    {
        const int startIndex = buf->m_vertexBuffer.m_size;
        Vec2      bbMin, bbMax;
        GetConvexBoundingBox(points, size, bbMin, bbMax);

        if (opts.isFilled)
        {
            Vertex c;
            c.pos  = center;
            c.uv.x = Math::Remap(c.pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            c.uv.y = Math::Remap(c.pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            c.col  = color;
            buf->PushVertex(c);
        }

        for (int i = 0; i < size; i++)
        {
            Vertex v;
            v.pos             = points[i];
            const Vec2 toCenter = Math::Normalized(Vec2(center.x - v.pos.x, center.y - v.pos.y));
            v.uv.x            = Math::Remap(v.pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            v.uv.y            = Math::Remap(v.pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            v.col             = color;
            buf->PushVertex(v);
        }

        if (opts.isFilled)
            ConvexFillVertices(startIndex, startIndex + size, buf->m_indexBuffer);
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + size - 1, opts.thickness.start);

        RotateVertices(buf->m_vertexBuffer, center, opts.isFilled ? startIndex + 1 : startIndex, opts.isFilled ? startIndex + size : startIndex + (size * 2) - 1, rotateAngle);

        if (opts.outlineOptions.thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.isFilled ? size : size * 2, false, drawOrder);
        else if (Config.aaEnabled)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.isFilled ? size : size * 2, false, drawOrder, Internal::OutlineCallType::AA);
        }
    }

    void Internal::FillConvex_VerHorGra(DrawBuffer* buf, float rotateAngle, Vec2* points, int size, const Vec2& center, const Vec4& colorStart, const Vec4& colorEnd, bool isHor, StyleOptions& opts, int drawOrder)
    {
        const int startIndex = buf->m_vertexBuffer.m_size;
        Vec2      bbMin, bbMax;
        GetConvexBoundingBox(points, size, bbMin, bbMax);

        if (opts.isFilled)
        {
            Vertex c;
            c.pos  = center;
            c.col  = Math::Lerp(colorStart, colorEnd, 0.5f);
            c.uv.x = Math::Remap(c.pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            c.uv.y = Math::Remap(c.pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            buf->PushVertex(c);
        }

        for (int i = 0; i < size; i++)
        {
            Vertex v;
            v.pos             = points[i];
            const Vec2 toCenter = Math::Normalized(Vec2(center.x - v.pos.x, center.y - v.pos.y));
            v.uv.x            = Math::Remap(v.pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            v.uv.y            = Math::Remap(v.pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            v.col             = Math::Lerp(colorStart, colorEnd, isHor ? v.uv.x : v.uv.y);
            buf->PushVertex(v);
        }

        if (opts.isFilled)
            ConvexFillVertices(startIndex, startIndex + size, buf->m_indexBuffer);
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + size - 1, opts.thickness.start);

        RotateVertices(buf->m_vertexBuffer, center, opts.isFilled ? startIndex + 1 : startIndex, opts.isFilled ? startIndex + size : startIndex + (size * 2) - 1, rotateAngle);

        if (opts.outlineOptions.thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.isFilled ? size : size * 2, false, drawOrder);
        else if (Config.aaEnabled)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.isFilled ? size : size * 2, false, drawOrder, Internal::OutlineCallType::AA);
        }
    }

    void Internal::FillConvex_RadialGra(DrawBuffer* buf, float rotateAngle, Vec2* points, int size, const Vec2& center, const Vec4& colorStart, const Vec4& colorEnd, StyleOptions& opts, int drawOrder)
    {
        const int startIndex = buf->m_vertexBuffer.m_size;

        Vec2 bbMin, bbMax;
        GetConvexBoundingBox(points, size, bbMin, bbMax);

        if (opts.isFilled)
        {
            Vertex c;
            c.pos  = center;
            c.uv.x = Math::Remap(c.pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            c.uv.y = Math::Remap(c.pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            buf->PushVertex(c);
        }

        for (int i = 0; i < size; i++)
        {
            Vertex v;
            v.pos             = points[i];
            const Vec2 toCenter = Math::Normalized(Vec2(center.x - v.pos.x, center.y - v.pos.y));
            v.uv.x            = Math::Remap(v.pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            v.uv.y            = Math::Remap(v.pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            buf->PushVertex(v);
        }

        if (opts.isFilled)
            ConvexFillVertices(startIndex, startIndex + size, buf->m_indexBuffer);
        else
            ConvexExtrudeVertices(buf, center, startIndex, startIndex + size - 1, opts.thickness.start);

        RotateVertices(buf->m_vertexBuffer, center, opts.isFilled ? startIndex + 1 : startIndex, opts.isFilled ? startIndex + size : startIndex + (size * 2) - 1, rotateAngle);

        if (opts.outlineOptions.thickness != 0.0f)
            buf = DrawOutline(buf, opts, opts.isFilled ? size : size * 2, false, drawOrder);
        else if (Config.aaEnabled)
        {
            StyleOptions opts2     = StyleOptions(opts);
            opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
            buf                    = DrawOutline(buf, opts2, opts2.isFilled ? size : size * 2, false, drawOrder, Internal::OutlineCallType::AA);
        }
    }

    void Internal::ConvexFillVertices(int startIndex, int endIndex, Array<Index>& indices, bool skipLastTriangle)
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

    void Internal::ConvexExtrudeVertices(DrawBuffer* buf, const Vec2& center, int startIndex, int endIndex, float thickness, bool skipEndClosing)
    {
        const int totalSize = endIndex - startIndex + 1;
        thickness *= Config.framebufferScale.x;

        // Extrude vertices.
        for (int i = startIndex; i < startIndex + totalSize; i++)
        {
            // take two edges, this vertex to next and previous to this in order to calculate vertex normals.
            const int previous = i == startIndex ? endIndex : i - 1;
            const int next     = i == endIndex ? startIndex : i + 1;
            Vertex    v;
            v.col = buf->m_vertexBuffer[i].col;
            if (skipEndClosing && i == startIndex)
            {
                const Vec2 toNext  = Math::Normalized(Vec2(buf->m_vertexBuffer[next].pos.x - buf->m_vertexBuffer[i].pos.x, buf->m_vertexBuffer[next].pos.y - buf->m_vertexBuffer[i].pos.y));
                const Vec2 rotated = Math::Rotate90(toNext, true);
                v.pos            = Vec2(buf->m_vertexBuffer[i].pos.x + rotated.x * thickness, buf->m_vertexBuffer[i].pos.y + rotated.y * thickness);
            }
            else if (skipEndClosing && i == endIndex)
            {
                const Vec2 fromPrev = Math::Normalized(Vec2(buf->m_vertexBuffer[i].pos.x - buf->m_vertexBuffer[previous].pos.x, buf->m_vertexBuffer[i].pos.y - buf->m_vertexBuffer[previous].pos.y));
                const Vec2 rotated  = Math::Rotate90(fromPrev, true);
                v.pos             = Vec2(buf->m_vertexBuffer[i].pos.x + rotated.x * thickness, buf->m_vertexBuffer[i].pos.y + rotated.y * thickness);
            }
            else
            {
                const Vec2 vertexNormalAverage = Math::GetVertexNormal(buf->m_vertexBuffer[i].pos, buf->m_vertexBuffer[previous].pos, buf->m_vertexBuffer[next].pos);
                v.pos                        = Vec2(buf->m_vertexBuffer[i].pos.x + vertexNormalAverage.x * thickness, buf->m_vertexBuffer[i].pos.y + vertexNormalAverage.y * thickness);
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

    void Internal::RotateVertices(Array<Vertex>& vertices, const Vec2& center, int startIndex, int endIndex, float angle)
    {
        for (int i = startIndex; i < endIndex + 1; i++)
        {
            vertices[i].pos = Math::RotateAround(vertices[i].pos, center, angle);
        }
    }

    void Internal::RotatePoints(Vec2* points, int size, const Vec2& center, float angle)
    {
        for (int i = 0; i < size; i++)
        {
            points[i] = Math::RotateAround(points[i], center, angle);
        }
    }

    void Internal::GetArcPoints(Array<Vec2>& points, const Vec2& p1, const Vec2& p2, Vec2 directionHintPoint, float radius, float segments, bool flip, float angleOffset)
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
            if (points[i].pos.x < outMin.x)
                outMin.x = points[i].pos.x;
            else if (points[i].pos.x > outMax.x)
                outMax.x = points[i].pos.x;
            if (points[i].pos.y < outMin.y)
                outMin.y = points[i].pos.y;
            else if (points[i].pos.y > outMax.y)
                outMax.y = points[i].pos.y;
        }
    }

    void Internal::GetConvexBoundingBox(DrawBuffer* buf, int startIndex, int endIndex, Vec2& outMin, Vec2& outMax)
    {
        outMin = Vec2(99999, 99999);
        outMax = Vec2(-99999, -99999);

        for (int i = startIndex; i < endIndex + 1; i++)
        {
            if (buf->m_vertexBuffer[i].pos.x < outMin.x)
                outMin.x = buf->m_vertexBuffer[i].pos.x;
            else if (buf->m_vertexBuffer[i].pos.x > outMax.x)
                outMax.x = buf->m_vertexBuffer[i].pos.x;
            if (buf->m_vertexBuffer[i].pos.y < outMin.y)
                outMin.y = buf->m_vertexBuffer[i].pos.y;
            else if (buf->m_vertexBuffer[i].pos.y > outMax.y)
                outMax.y = buf->m_vertexBuffer[i].pos.y;
        }
    }

    void Internal::CalculateVertexUVs(DrawBuffer* buf, int startIndex, int endIndex)
    {
        Vec2 bbMin, bbMax;
        GetConvexBoundingBox(&buf->m_vertexBuffer.m_data[startIndex], endIndex - startIndex + 1, bbMin, bbMax);

        // Recalculate UVs.
        for (int i = startIndex; i <= endIndex; i++)
        {
            buf->m_vertexBuffer[i].uv.x = Math::Remap(buf->m_vertexBuffer[i].pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            buf->m_vertexBuffer[i].uv.y = Math::Remap(buf->m_vertexBuffer[i].pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
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

    Vec2 Internal::GetArcDirection(const Vec2& center, float radius, float startAngle, float endAngle)
    {
        const Vec2 centerAnglePoint = Math::GetPointOnCircle(center, radius, (startAngle + endAngle) / 2.0f);
        return Vec2(centerAnglePoint.x - center.x, centerAnglePoint.y - center.y);
    }

    Vec2 Internal::GetVerticesCenter(DrawBuffer* buf, int startIndex, int endIndex)
    {
        Vec2 total = Vec2(0.0f, 0.0f);

        for (int i = startIndex; i <= endIndex; i++)
        {
            total.x += buf->m_vertexBuffer[i].pos.x;
            total.y += buf->m_vertexBuffer[i].pos.y;
        }

        const int count = endIndex - startIndex + 1;
        total.x /= static_cast<float>(count);
        total.y /= static_cast<float>(count);
        return total;
    }

    void Internal::CalculateLine(Line& line, const Vec2& p1, const Vec2& p2, StyleOptions& style, LineCapDirection lineCapToAdd)
    {
        const Vec2 up = Math::Normalized(Math::Rotate90(Vec2(p2.x - p1.x, p2.y - p1.y), true));
        Vertex     v0, v1, v2, v3;

        v0.pos = Vec2(p1.x + up.x * style.thickness.start / 2.0f, p1.y + up.y * style.thickness.start / 2.0f);
        v3.pos = Vec2(p1.x - up.x * style.thickness.start / 2.0f, p1.y - up.y * style.thickness.start / 2.0f);
        v1.pos = Vec2(p2.x + up.x * style.thickness.end / 2.0f, p2.y + up.y * style.thickness.end / 2.0f);
        v2.pos = Vec2(p2.x - up.x * style.thickness.end / 2.0f, p2.y - up.y * style.thickness.end / 2.0f);
        v0.col = v3.col = style.color.start;
        v1.col = v2.col = style.color.end;
        line.m_vertices.push_back(v0);
        line.m_vertices.push_back(v1);
        line.m_vertices.push_back(v2);
        line.m_vertices.push_back(v3);

        const Vec2 upRaw          = Vec2(v0.pos.x - v3.pos.x, v0.pos.y - v3.pos.y);
        const bool willAddLineCap = lineCapToAdd == LineCapDirection::Left || lineCapToAdd == LineCapDirection::Right;

        if (willAddLineCap)
        {
            Vertex vmLeft, vmRight;
            vmLeft.pos  = Math::Lerp(v0.pos, v3.pos, 0.5f);
            vmRight.pos = Math::Lerp(v1.pos, v2.pos, 0.5f);
            vmLeft.col  = style.color.start;
            vmRight.col = style.color.end;
            line.m_vertices.push_back(vmLeft);
            line.m_vertices.push_back(vmRight);
            line.m_hasMidpoints = true;
        }

        if (willAddLineCap)
        {
            const Vertex* upVtx   = lineCapToAdd == LineCapDirection::Left ? &v0 : &v1;
            const Vertex* downVtx = lineCapToAdd == LineCapDirection::Left ? &v3 : &v2;

            const float increase = Math::Remap(style.rounding, 0.0f, 1.0f, 0.4f, 0.1f);
            const float radius   = (Math::Mag(upRaw) / 2.0f) * 0.6f;
            const Vec2  dir      = Math::Rotate90(up, lineCapToAdd == LineCapDirection::Left);

            Array<int> upperParabolaPoints;
            Array<int> lowerParabolaPoints;

            for (float k = 0.0f + increase; k < 1.0f; k += increase)
            {
                const Vec2 p = Math::SampleParabola(upVtx->pos, downVtx->pos, dir, radius, k);
                Vertex     v;
                v.col = lineCapToAdd == LineCapDirection::Left ? style.color.start : style.color.end;
                v.pos = p;

                line.m_vertices.push_back(v);
                line.m_lineCapVertexCount++;

                const float distToUp   = Math::Mag(Vec2(upVtx->pos.x - p.x, upVtx->pos.y - p.y));
                const float distToDown = Math::Mag(Vec2(downVtx->pos.x - p.x, downVtx->pos.y - p.y));

                if (distToUp < distToDown)
                    upperParabolaPoints.push_back(line.m_vertices.m_size - 1);
                else
                    lowerParabolaPoints.push_back(line.m_vertices.m_size - 1);
            }

            if (lineCapToAdd == LineCapDirection::Left)
            {

                for (int i = upperParabolaPoints.m_size - 1; i > -1; i--)
                    line.m_upperIndices.push_back(upperParabolaPoints[i]);

                line.m_upperIndices.push_back(0);
                line.m_upperIndices.push_back(1);

                for (int i = 0; i < lowerParabolaPoints.m_size; i++)
                    line.m_lowerIndices.push_back(lowerParabolaPoints[i]);

                line.m_lowerIndices.push_back(3);
                line.m_lowerIndices.push_back(2);
            }
            else
            {
                line.m_upperIndices.push_back(0);
                line.m_upperIndices.push_back(1);

                for (int i = 0; i < upperParabolaPoints.m_size; i++)
                    line.m_upperIndices.push_back(upperParabolaPoints[i]);

                line.m_lowerIndices.push_back(3);
                line.m_lowerIndices.push_back(2);

                for (int i = lowerParabolaPoints.m_size - 1; i > -1; i--)
                    line.m_lowerIndices.push_back(lowerParabolaPoints[i]);
            }
        }
        else
        {
            line.m_upperIndices.push_back(0);
            line.m_upperIndices.push_back(1);
            line.m_lowerIndices.push_back(3);
            line.m_lowerIndices.push_back(2);
        }

        // Draw 4 triangles if there are middle vertices.
        // Draw 2 if no middle vertices.
        if (willAddLineCap)
        {
            LineTriangle tri1, tri2, tri3, tri4;
            tri1.m_indices[0] = 0;
            tri1.m_indices[1] = 1;
            tri1.m_indices[2] = 4;
            tri2.m_indices[0] = 1;
            tri2.m_indices[1] = 4;
            tri2.m_indices[2] = 5;
            tri3.m_indices[0] = 4;
            tri3.m_indices[1] = 5;
            tri3.m_indices[2] = 3;
            tri4.m_indices[0] = 5;
            tri4.m_indices[1] = 2;
            tri4.m_indices[2] = 3;
            line.m_tris.push_back(tri1);
            line.m_tris.push_back(tri2);
            line.m_tris.push_back(tri3);
            line.m_tris.push_back(tri4);
        }
        else
        {
            LineTriangle tri1, tri2;
            tri1.m_indices[0] = 0;
            tri1.m_indices[1] = 1;
            tri1.m_indices[2] = 3;
            tri2.m_indices[0] = 1;
            tri2.m_indices[1] = 2;
            tri2.m_indices[2] = 3;
            line.m_tris.push_back(tri1);
            line.m_tris.push_back(tri2);
        }

        // Triangles for cap vertices.
        if (willAddLineCap)
        {
            const int    middleIndex = lineCapToAdd == LineCapDirection::Left ? 4 : 5;
            const int    upperIndex  = lineCapToAdd == LineCapDirection::Left ? 0 : 1;
            const int    lowerIndex  = lineCapToAdd == LineCapDirection::Left ? 3 : 2;
            LineTriangle tri1, tri2; // start / end.
            tri1.m_indices[0] = upperIndex;
            tri1.m_indices[1] = 6; // start of cap rounding.
            tri1.m_indices[2] = middleIndex;
            tri2.m_indices[0] = lowerIndex;
            tri2.m_indices[1] = line.m_vertices.m_size - 1; // end of cap rounding.
            tri2.m_indices[2] = middleIndex;
            line.m_tris.push_back(tri1);
            line.m_tris.push_back(tri2);

            for (int i = 6; i < line.m_vertices.m_size - 1; i++)
            {
                LineTriangle tri;
                tri.m_indices[0] = i;
                tri.m_indices[1] = i + 1;
                tri.m_indices[2] = middleIndex;
                line.m_tris.push_back(tri);
            }
        }
    }

    SimpleLine Internal::CalculateSimpleLine(const Vec2& p1, const Vec2& p2, StyleOptions& style)
    {
        const Vec2 up = Math::Normalized(Math::Rotate90(Vec2(p2.x - p1.x, p2.y - p1.y), true));
        SimpleLine line;
        line.m_points[0] = Vec2(p1.x + up.x * style.thickness.start / 2.0f, p1.y + up.y * style.thickness.start / 2.0f);
        line.m_points[3] = Vec2(p1.x - up.x * style.thickness.start / 2.0f, p1.y - up.y * style.thickness.start / 2.0f);
        line.m_points[1] = Vec2(p2.x + up.x * style.thickness.end / 2.0f, p2.y + up.y * style.thickness.end / 2.0f);
        line.m_points[2] = Vec2(p2.x - up.x * style.thickness.end / 2.0f, p2.y - up.y * style.thickness.end / 2.0f);

        return line;
    }

    void Internal::JoinLines(Line& line1, Line& line2, StyleOptions& opts, LineJointType jointType, bool mergeUpperVertices)
    {
        const bool addUpperLowerIndices = Config.aaEnabled || opts.outlineOptions.thickness != 0.0;

        if (jointType == LineJointType::VtxAverage)
        {
            const Vec2 upperAvg = Vec2((line1.m_vertices[1].pos.x + line2.m_vertices[0].pos.x) / 2.0f, (line1.m_vertices[1].pos.y + line2.m_vertices[0].pos.y) / 2.0f);
            const Vec2 lowerAvg = Vec2((line1.m_vertices[2].pos.x + line2.m_vertices[3].pos.x) / 2.0f, (line1.m_vertices[2].pos.y + line2.m_vertices[3].pos.y) / 2.0f);

            line1.m_vertices[1].pos = line2.m_vertices[0].pos = upperAvg;
            line1.m_vertices[2].pos = line2.m_vertices[3].pos = lowerAvg;

            if (addUpperLowerIndices)
            {
                line2.m_upperIndices.erase(line2.m_upperIndices.findAddr(0));
                line2.m_lowerIndices.erase(line2.m_lowerIndices.findAddr(3));
            }
        }
        else if (jointType == LineJointType::Miter)
        {
            const Vec2 upperIntersection = Math::LineIntersection(line1.m_vertices[0].pos, line1.m_vertices[1].pos, line2.m_vertices[0].pos, line2.m_vertices[1].pos);
            const Vec2 lowerIntersection = Math::LineIntersection(line1.m_vertices[3].pos, line1.m_vertices[2].pos, line2.m_vertices[3].pos, line2.m_vertices[2].pos);
            line1.m_vertices[1].pos = line2.m_vertices[0].pos = upperIntersection;
            line1.m_vertices[2].pos = line2.m_vertices[3].pos = lowerIntersection;
            if (addUpperLowerIndices)
            {
                line2.m_upperIndices.erase(line2.m_upperIndices.findAddr(0));
                line2.m_lowerIndices.erase(line2.m_lowerIndices.findAddr(3));
            }
        }
        else if (jointType == LineJointType::Bevel)
        {
            const int intersection0 = mergeUpperVertices ? 0 : 3;
            const int intersection1 = mergeUpperVertices ? 1 : 2;
            const int intersection2 = mergeUpperVertices ? 2 : 1;
            const int intersection3 = mergeUpperVertices ? 3 : 0;

            if (addUpperLowerIndices)
            {
                if (mergeUpperVertices)
                    line2.m_upperIndices.erase(line2.m_upperIndices.findAddr(0));
                else
                    line2.m_lowerIndices.erase(line2.m_lowerIndices.findAddr(3));
            }

            const Vec2 intersection               = Math::LineIntersection(line1.m_vertices[intersection0].pos, line1.m_vertices[intersection1].pos, line2.m_vertices[intersection0].pos, line2.m_vertices[intersection1].pos);
            line1.m_vertices[intersection1].pos = line2.m_vertices[intersection0].pos = intersection;

            const int vLowIndex = line1.m_vertices.m_size;
            Vertex    vLow;
            vLow.col = opts.color.start;
            vLow.pos = line2.m_vertices[intersection3].pos;
            line1.m_vertices.push_back(vLow);

            LineTriangle tri1;
            tri1.m_indices[0] = intersection1;
            tri1.m_indices[1] = intersection2;
            tri1.m_indices[2] = vLowIndex;
            line1.m_tris.push_back(tri1);
        }
        else if (jointType == LineJointType::BevelRound)
        {
            const int   intersection0      = mergeUpperVertices ? 0 : 3;
            const int   intersection1      = mergeUpperVertices ? 1 : 2;
            const int   intersection2      = mergeUpperVertices ? 2 : 1;
            const int   intersection3      = mergeUpperVertices ? 3 : 0;
            const Vec2  upperIntersection  = Math::LineIntersection(line1.m_vertices[intersection0].pos, line1.m_vertices[intersection1].pos, line2.m_vertices[intersection0].pos, line2.m_vertices[intersection1].pos);
            const Vec2  lowerIntersection  = Math::LineIntersection(line1.m_vertices[intersection3].pos, line1.m_vertices[intersection2].pos, line2.m_vertices[intersection3].pos, line2.m_vertices[intersection2].pos);
            const Vec2  intersectionCenter = Vec2((upperIntersection.x + lowerIntersection.x) / 2.0f, (upperIntersection.y + lowerIntersection.y) / 2.0f);
            const float ang2               = Math::GetAngleFromCenter(intersectionCenter, line1.m_vertices[intersection2].pos);
            const float ang1               = Math::GetAngleFromCenter(intersectionCenter, line2.m_vertices[intersection3].pos);
            const float startAngle         = ang2 > ang1 ? ang1 : ang2;
            const float endAngle           = ang2 > ang1 ? ang2 : ang1;
            const float arcRad             = Math::Mag(Vec2(line1.m_vertices[intersection2].pos.x - intersectionCenter.x, line1.m_vertices[intersection2].pos.y - intersectionCenter.y));

            // Merge
            line1.m_vertices[intersection1].pos = line2.m_vertices[intersection0].pos = upperIntersection;

            if (addUpperLowerIndices)
            {
                if (mergeUpperVertices)
                    line2.m_upperIndices.erase(line2.m_upperIndices.findAddr(0));
                else
                    line2.m_lowerIndices.erase(line2.m_lowerIndices.findAddr(3));
            }

            const int vLowIndex = line1.m_vertices.m_size;
            Vertex    vLow;
            vLow.col = opts.color.start;
            vLow.pos = line2.m_vertices[intersection3].pos;
            line1.m_vertices.push_back(vLow);

            const float increase      = Math::Remap(opts.rounding, 0.0f, 1.0f, 45.0f, 6.0f);
            const int   parabolaStart = line1.m_vertices.m_size;

            Array<int> lowerIndicesToAdd;
            Array<int> upperIndicesToAdd;

            for (float k = startAngle + increase; k < endAngle; k += increase)
            {
                const Vec2 p = Math::GetPointOnCircle(intersectionCenter, arcRad, k);
                Vertex     v;
                v.col = opts.color.start;
                v.pos = p;

                if (addUpperLowerIndices)
                {
                    if (mergeUpperVertices)
                        lowerIndicesToAdd.push_back(line1.m_vertices.m_size);
                    else
                        upperIndicesToAdd.push_back(line1.m_vertices.m_size);
                }

                line1.m_vertices.push_back(v);
            }

            if (addUpperLowerIndices)
            {
                if (mergeUpperVertices)
                {
                    if (ang1 > ang2)
                    {
                        for (int i = 0; i < lowerIndicesToAdd.m_size; i++)
                            line1.m_lowerIndices.push_back(lowerIndicesToAdd[i]);
                    }
                    else
                    {
                        for (int i = lowerIndicesToAdd.m_size - 1; i > -1; i--)
                            line1.m_lowerIndices.push_back(lowerIndicesToAdd[i]);
                    }
                }
                else
                {
                    if (ang1 > ang2)
                    {
                        for (int i = 0; i < upperIndicesToAdd.m_size; i++)
                            line1.m_upperIndices.push_back(upperIndicesToAdd[i]);
                    }
                    else
                    {
                        for (int i = upperIndicesToAdd.m_size - 1; i > -1; i--)
                            line1.m_upperIndices.push_back(upperIndicesToAdd[i]);
                    }
                }
            }

            LineTriangle tri1, tri2;
            tri1.m_indices[0] = intersection1;
            tri1.m_indices[1] = intersection2;
            tri1.m_indices[2] = ang1 > ang2 ? parabolaStart : line1.m_vertices.m_size - 1;
            tri2.m_indices[0] = intersection1;
            tri2.m_indices[1] = vLowIndex;
            tri2.m_indices[2] = ang1 > ang2 ? line1.m_vertices.m_size - 1 : parabolaStart;
            line1.m_tris.push_back(tri1);
            line1.m_tris.push_back(tri2);

            for (int i = parabolaStart; i < line1.m_vertices.m_size - 1; i++)
            {
                LineTriangle tri;
                tri.m_indices[0] = intersection1;
                tri.m_indices[1] = i;
                tri.m_indices[2] = i + 1;
                line1.m_tris.push_back(tri);
            }
        }
    }

    void Internal::DrawSimpleLine(SimpleLine& line, StyleOptions& opts, float rotateAngle, int drawOrder)
    {
        g_rectOverrideData.m_p1                  = line.m_points[0];
        g_rectOverrideData.m_p4                  = line.m_points[3];
        g_rectOverrideData.m_p2                  = line.m_points[1];
        g_rectOverrideData.m_p3                  = line.m_points[2];
        g_rectOverrideData.overrideRectPositions = true;
        DrawRect(g_rectOverrideData.m_p1, g_rectOverrideData.m_p3, opts, rotateAngle, drawOrder);
        g_rectOverrideData.overrideRectPositions = false;
    }

    void Internal::CalculateLineUVs(Line& line)
    {
        Vec2 bbMin, bbMax;
        GetConvexBoundingBox(&line.m_vertices[0], line.m_vertices.m_size, bbMin, bbMax);

        // Recalculate UVs.
        for (int i = 0; i < line.m_vertices.m_size; i++)
        {
            line.m_vertices[i].uv.x = Math::Remap(line.m_vertices[i].pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            line.m_vertices[i].uv.y = Math::Remap(line.m_vertices[i].pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
        }
    }

    DrawBuffer* Internal::DrawOutlineAroundShape(DrawBuffer* sourceBuffer, StyleOptions& opts, int* indicesOrder, int vertexCount, float defThickness, bool ccw, int drawOrder, OutlineCallType outlineType)
    {
        float      thickness   = outlineType != OutlineCallType::Normal ? Config.framebufferScale.x * Config.aaMultiplier : (defThickness * Config.framebufferScale.x);
        const bool isAAOutline = outlineType != OutlineCallType::Normal;

        bool isGradient = false;
        if (isAAOutline)
            isGradient = sourceBuffer->m_drawBufferType == DrawBufferType::Gradient;
        else
            isGradient = !Math::IsEqual(opts.outlineOptions.color.start, opts.outlineOptions.color.end);

        bool       useTextureBuffer = outlineType == OutlineCallType::AA ? (opts.textureHandle != 0) : (opts.outlineOptions.textureHandle != 0);
        const bool useGradBuffer    = !useTextureBuffer && isGradient;

        // Determine which buffer to use.
        // Also correct the buffer pointer if getting a new buffer invalidated it.
        DrawBuffer* destBuf = nullptr;

        if (useTextureBuffer)
        {
            const int           sourceIndex = Internal::g_rendererData.GetBufferIndexInTextureArray(sourceBuffer);
            const BackendHandle handle      = outlineType == OutlineCallType::AA ? opts.textureHandle : opts.outlineOptions.textureHandle;
            const Vec2          uvOffset    = outlineType == OutlineCallType::AA ? opts.textureUVOffset : opts.outlineOptions.textureUVOffset;
            const Vec2          uvTiling    = outlineType == OutlineCallType::AA ? opts.textureUVTiling : opts.outlineOptions.textureUVTiling;
            destBuf                         = &Internal::g_rendererData.GetTextureBuffer(handle, uvTiling, uvOffset, drawOrder, isAAOutline ? DrawBufferShapeType::AA : DrawBufferShapeType::Shape);

            if (sourceIndex != -1)
                sourceBuffer = &Internal::g_rendererData.m_textureBuffers[sourceIndex];
        }
        else if (useGradBuffer)
        {
            const int sourceIndex = Internal::g_rendererData.GetBufferIndexInGradientArray(sourceBuffer);
            Vec4Grad  col         = outlineType == OutlineCallType::AA ? opts.color : opts.outlineOptions.color;
            destBuf               = &Internal::g_rendererData.GetGradientBuffer(col, drawOrder, isAAOutline ? DrawBufferShapeType::AA : DrawBufferShapeType::Shape);

            if (sourceIndex != -1)
                sourceBuffer = &Internal::g_rendererData.m_gradientBuffers[sourceIndex];
        }
        else
        {
            const int sourceIndex = Internal::g_rendererData.GetBufferIndexInDefaultArray(sourceBuffer);
            destBuf               = &Internal::g_rendererData.GetDefaultBuffer(drawOrder, isAAOutline ? DrawBufferShapeType::AA : DrawBufferShapeType::Shape);

            if (sourceIndex != -1)
                sourceBuffer = &Internal::g_rendererData.m_defaultBuffers[sourceIndex];
        }

        // only used if we are drawing AA.
        Array<int> copiedVerticesOrder;

        const int destBufStart = destBuf->m_vertexBuffer.m_size;
        // First copy the given vertices, add them to the destination buffer.
        for (int i = 0; i < vertexCount; i++)
        {
            Vertex v;
            v.col = isAAOutline ? sourceBuffer->m_vertexBuffer[indicesOrder[i]].col : opts.outlineOptions.color.start;
            v.pos = sourceBuffer->m_vertexBuffer[indicesOrder[i]].pos;
            v.uv  = sourceBuffer->m_vertexBuffer[indicesOrder[i]].uv;

            // if (isAAOutline)
            //     v.col.w = 1.0f;

            if (Config.aaEnabled && !isAAOutline)
                copiedVerticesOrder.push_back(destBuf->m_vertexBuffer.m_size);

            destBuf->PushVertex(v);
        }

        const int halfVC = vertexCount / 2;

        // only used if we are drawing AA.
        Array<int> extrudedVerticesOrder;

        // Now traverse the destination buffer from the point we started adding to it, extrude the border towards m_thickness.
        for (int i = 0; i < vertexCount; i++)
        {
            const int prev    = i == 0 ? destBufStart + vertexCount - 1 : destBufStart + i - 1;
            const int next    = i == vertexCount - 1 ? destBufStart : destBufStart + i + 1;
            const int current = destBufStart + i;
            Vertex    v;
            v.uv  = destBuf->m_vertexBuffer[current].uv;
            v.col = isAAOutline ? sourceBuffer->m_vertexBuffer[indicesOrder[i]].col : opts.outlineOptions.color.end;

            if (isAAOutline)
                v.col.w = 0.0f;

            const Vec2 prevP               = destBuf->m_vertexBuffer[prev].pos;
            const Vec2 nextP               = destBuf->m_vertexBuffer[next].pos;
            const Vec2 vertexNormalAverage = Math::GetVertexNormalFlatCheck(destBuf->m_vertexBuffer[current].pos, prevP, nextP, ccw);
            v.pos                        = Vec2(destBuf->m_vertexBuffer[current].pos.x + vertexNormalAverage.x * thickness, destBuf->m_vertexBuffer[current].pos.y + vertexNormalAverage.y * thickness);

            if (Config.aaEnabled && !isAAOutline)
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

        if (Config.aaEnabled && !isAAOutline)
        {
            StyleOptions opts2 = StyleOptions(opts);
            destBuf            = DrawOutlineAroundShape(destBuf, opts2, &extrudedVerticesOrder[0], extrudedVerticesOrder.m_size, defThickness, ccw, drawOrder, OutlineCallType::OutlineAA);
            DrawOutlineAroundShape(destBuf, opts2, &copiedVerticesOrder[0], copiedVerticesOrder.m_size, -defThickness, !ccw, drawOrder, OutlineCallType::OutlineAA);
        }

        return sourceBuffer;
    }

    DrawBuffer* Internal::DrawOutline(DrawBuffer* sourceBuffer, StyleOptions& opts, int vertexCount, bool skipEnds, int drawOrder, OutlineCallType outlineType, bool reverseDrawDir)
    {
        const bool isAAOutline = outlineType != OutlineCallType::Normal;
        float      thickness   = isAAOutline ? Config.framebufferScale.x * Config.aaMultiplier : (opts.outlineOptions.thickness * Config.framebufferScale.x);

        bool isGradient = false;

        if (isAAOutline)
            isGradient = sourceBuffer->m_drawBufferType == DrawBufferType::Gradient;
        else
            isGradient = !Math::IsEqual(opts.outlineOptions.color.start, opts.outlineOptions.color.end);

        bool       useTextureBuffer = outlineType == OutlineCallType::AA ? (opts.textureHandle != 0) : (opts.outlineOptions.textureHandle != 0);
        const bool useGradBuffer    = !useTextureBuffer && isGradient;

        if (reverseDrawDir)
            thickness = -thickness;

        // Determine which buffer to use.
        // Also correct the buffer pointer if getting a new buffer invalidated it.
        DrawBuffer* destBuf = nullptr;

        if (useTextureBuffer)
        {
            const int           sourceIndex = Internal::g_rendererData.GetBufferIndexInTextureArray(sourceBuffer);
            const BackendHandle handle      = outlineType == OutlineCallType::AA ? opts.textureHandle : opts.outlineOptions.textureHandle;
            const Vec2          uvOffset    = outlineType == OutlineCallType::AA ? opts.textureUVOffset : opts.outlineOptions.textureUVOffset;
            const Vec2          uvTiling    = outlineType == OutlineCallType::AA ? opts.textureUVTiling : opts.outlineOptions.textureUVTiling;
            destBuf                         = &Internal::g_rendererData.GetTextureBuffer(handle, uvTiling, uvOffset, drawOrder, isAAOutline ? DrawBufferShapeType::AA : DrawBufferShapeType::Shape);

            if (sourceIndex != -1)
                sourceBuffer = &Internal::g_rendererData.m_textureBuffers[sourceIndex];
        }
        else if (useGradBuffer)
        {
            const int sourceIndex = Internal::g_rendererData.GetBufferIndexInGradientArray(sourceBuffer);
            Vec4Grad  col         = outlineType == OutlineCallType::AA ? opts.color : opts.outlineOptions.color;
            destBuf               = &Internal::g_rendererData.GetGradientBuffer(col, drawOrder, isAAOutline ? DrawBufferShapeType::AA : DrawBufferShapeType::Shape);

            if (sourceIndex != -1)
                sourceBuffer = &Internal::g_rendererData.m_gradientBuffers[sourceIndex];
        }
        else
        {
            const int sourceIndex = Internal::g_rendererData.GetBufferIndexInDefaultArray(sourceBuffer);

            destBuf = &Internal::g_rendererData.GetDefaultBuffer(drawOrder, isAAOutline ? DrawBufferShapeType::AA : DrawBufferShapeType::Shape);

            if (sourceIndex != -1)
                sourceBuffer = &Internal::g_rendererData.m_defaultBuffers[sourceIndex];
        }

        int startIndex, endIndex;

        if (opts.isFilled)
        {
            endIndex   = sourceBuffer->m_vertexBuffer.m_size - 1;
            startIndex = sourceBuffer->m_vertexBuffer.m_size - vertexCount;
        }
        else
        {
            // Take the outer half.
            if (opts.outlineOptions.drawDirection == OutlineDrawDirection::Outwards)
            {
                endIndex   = sourceBuffer->m_vertexBuffer.m_size - 1;
                startIndex = sourceBuffer->m_vertexBuffer.m_size - vertexCount / 2;
            }
            else if (opts.outlineOptions.drawDirection == OutlineDrawDirection::Inwards)
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
                v.pos = sourceBuffer->m_vertexBuffer[i].pos;
                v.uv  = sourceBuffer->m_vertexBuffer[i].uv;

                if (isAAOutline)
                {
                    v.col = sourceBuffer->m_vertexBuffer[i].col;
                    // v.col.w = 1.0f;
                }
                else
                    v.col = opts.outlineOptions.color.start;

                destBuf->PushVertex(v);
            }

            // Now extrude & re-add extruded.
            for (int i = startIndex; i < endIndex + 1; i++)
            {
                // take two edges, this vertex to next and previous to this in order to calculate vertex normals.
                const int previous = i == startIndex ? endIndex : i - 1;
                const int next     = i == endIndex ? startIndex : i + 1;
                Vertex    v;
                v.uv = sourceBuffer->m_vertexBuffer[i].uv;

                if (isAAOutline)
                {
                    v.col   = sourceBuffer->m_vertexBuffer[i].col;
                    v.col.w = 0.0f;
                }
                else
                    v.col = opts.outlineOptions.color.end;

                if (skipEnds && i == startIndex)
                {
                    const Vec2 nextP               = sourceBuffer->m_vertexBuffer[next].pos;
                    const Vec2 vertexNormalAverage = Math::GetVertexNormal(sourceBuffer->m_vertexBuffer[i].pos, Vec2(-1, -1), nextP);
                    v.pos                        = Vec2(sourceBuffer->m_vertexBuffer[i].pos.x + vertexNormalAverage.x * thickness, sourceBuffer->m_vertexBuffer[i].pos.y + vertexNormalAverage.y * thickness);
                }
                else if (skipEnds && i == endIndex)
                {
                    const Vec2 prevP               = sourceBuffer->m_vertexBuffer[previous].pos;
                    const Vec2 vertexNormalAverage = Math::GetVertexNormal(sourceBuffer->m_vertexBuffer[i].pos, prevP, Vec2(-1, -1));
                    v.pos                        = Vec2(sourceBuffer->m_vertexBuffer[i].pos.x + vertexNormalAverage.x * thickness, sourceBuffer->m_vertexBuffer[i].pos.y + vertexNormalAverage.y * thickness);
                }
                else
                {
                    const Vec2 prevP               = sourceBuffer->m_vertexBuffer[previous].pos;
                    const Vec2 nextP               = sourceBuffer->m_vertexBuffer[next].pos;
                    const Vec2 vertexNormalAverage = Math::GetVertexNormal(sourceBuffer->m_vertexBuffer[i].pos, prevP, nextP);
                    v.pos                        = Vec2(sourceBuffer->m_vertexBuffer[i].pos.x + vertexNormalAverage.x * thickness, sourceBuffer->m_vertexBuffer[i].pos.y + vertexNormalAverage.y * thickness);
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
        const bool useAA     = Config.aaEnabled && !isAAOutline;

        if (opts.isFilled)
        {
            copyAndFill(sourceBuffer, destBuf, startIndex, endIndex, thickness, recalcUvs);

            if (useAA)
            {
                StyleOptions opts2                     = StyleOptions(opts);
                opts2.isFilled                       = false;
                opts2.outlineOptions.drawDirection = OutlineDrawDirection::Outwards;
                destBuf                                = DrawOutline(destBuf, opts2, vertexCount * 2, skipEnds, drawOrder, OutlineCallType::OutlineAA);

                opts2.outlineOptions.drawDirection = OutlineDrawDirection::Inwards;
                DrawOutline(destBuf, opts2, vertexCount * 2, skipEnds, drawOrder, OutlineCallType::OutlineAA);
            }
        }
        else
        {

            if (opts.outlineOptions.drawDirection == OutlineDrawDirection::Outwards)
            {
                if (useAA)
                {
                    // AA outline to the shape we are drawing
                    StyleOptions opts3     = StyleOptions(opts);
                    opts3.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Inwards);
                    DrawOutline(sourceBuffer, opts3, vertexCount, skipEnds, drawOrder, OutlineCallType::OutlineAA);
                }

                copyAndFill(sourceBuffer, destBuf, startIndex, endIndex, thickness, recalcUvs);

                if (useAA)
                {
                    // AA outline to the current outline we are drawing
                    StyleOptions opts2                     = StyleOptions(opts);
                    opts2.outlineOptions.drawDirection = OutlineDrawDirection::Outwards;
                    destBuf                                = DrawOutline(destBuf, opts2, vertexCount, skipEnds, drawOrder, OutlineCallType::OutlineAA);

                    opts2.outlineOptions.drawDirection = OutlineDrawDirection::Inwards;
                    DrawOutline(destBuf, opts2, vertexCount, skipEnds, drawOrder, OutlineCallType::OutlineAA);
                }
            }
            else if (opts.outlineOptions.drawDirection == OutlineDrawDirection::Inwards)
            {
                if (useAA)
                {
                    // // AA outline to the shape we are drawing
                    StyleOptions opts3     = StyleOptions(opts);
                    opts3.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Outwards);
                    DrawOutline(sourceBuffer, opts3, vertexCount, skipEnds, drawOrder, OutlineCallType::OutlineAA);
                }

                copyAndFill(sourceBuffer, destBuf, startIndex, endIndex, -thickness, recalcUvs);

                if (useAA)
                {
                    // AA outline to the current outline we are drawing
                    StyleOptions opts2                     = StyleOptions(opts);
                    opts2.outlineOptions.drawDirection = OutlineDrawDirection::Outwards;
                    destBuf                                = DrawOutline(destBuf, opts2, vertexCount, skipEnds, drawOrder, OutlineCallType::OutlineAA, true);

                    opts2.outlineOptions.drawDirection = OutlineDrawDirection::Inwards;
                    DrawOutline(destBuf, opts2, vertexCount, skipEnds, drawOrder, OutlineCallType::OutlineAA, true);
                }
            }
            else
            {
                copyAndFill(sourceBuffer, destBuf, startIndex, startIndex + vertexCount / 2 - 1, -thickness, recalcUvs);

                if (useAA)
                {
                    // AA outline to the current outline we are drawing
                    StyleOptions opts2                     = StyleOptions(opts);
                    opts2.outlineOptions.drawDirection = OutlineDrawDirection::Outwards;
                    destBuf                                = DrawOutline(destBuf, opts2, vertexCount, skipEnds, drawOrder, OutlineCallType::OutlineAA, true);

                    opts2.outlineOptions.drawDirection = OutlineDrawDirection::Inwards;
                    DrawOutline(destBuf, opts2, vertexCount, skipEnds, drawOrder, OutlineCallType::OutlineAA, true);
                }

                copyAndFill(sourceBuffer, destBuf, startIndex + vertexCount / 2, endIndex, thickness, recalcUvs);

                if (useAA)
                {
                    // AA outline to the current outline we are drawing
                    StyleOptions opts2                     = StyleOptions(opts);
                    opts2.outlineOptions.drawDirection = OutlineDrawDirection::Outwards;
                    destBuf                                = DrawOutline(destBuf, opts2, vertexCount, skipEnds, drawOrder, OutlineCallType::OutlineAA);

                    opts2.outlineOptions.drawDirection = OutlineDrawDirection::Inwards;
                    DrawOutline(destBuf, opts2, vertexCount, skipEnds, drawOrder, OutlineCallType::OutlineAA);
                }
            }
        }

        return sourceBuffer;
    }

    void Internal::ParseTextIntoWords(Array<TextPart*>& arr, const LINAVG_STRING& text, LinaVGFont* font, float scale, float spacing)
    {
        bool          added = false;
        Vec2          size  = Vec2(0.0f, 0.0f);
        LINAVG_STRING word  = "";
        for (auto x : text)
        {
            if (x == ' ')
            {
                if (added)
                {
                    TextPart* w = new TextPart();
                    w->m_size   = size;
                    w->m_str    = word;
                    arr.push_back(w);
                }
                added = false;
                size  = Vec2(0.0f, 0.0f);
                word  = "";
            }
            else
            {
                auto& ch = font->m_characterGlyphs[x];
                size.y   = Math::Max(size.y, ch.m_size.y * scale);
                size.x += ch.m_advance.x * scale + spacing;
                word  = word + x;
                added = true;
            }
        }

        TextPart* w = new TextPart();
        w->m_size   = size;
        w->m_str    = word;
        arr.push_back(w);
    }

    void Internal::ParseWordsIntoLines(Array<TextPart*>& lines, const Array<TextPart*>& words, LinaVGFont* font, float scale, float spacing, float wrapWidth, float sdfThickness)
    {
        const float   spaceAdvance = font->m_spaceAdvance * scale + spacing;
        float         maxHeight    = 0.0f;
        float         totalWidth   = 0.0f;
        LINAVG_STRING append       = "";
        float         remap        = font->m_isSDF ? Math::Remap(sdfThickness, 0.5f, 1.0f, 2.0f, 0.0f) : 0.0f;
        const Vec2    offset       = Internal::CalcMaxCharOffset(words[0]->m_str.c_str(), font, scale);

        for (int i = 0; i < words.m_size; i++)
        {
            totalWidth += words[i]->m_size.x;
            maxHeight = Math::Max(words[i]->m_size.y - offset.y * remap, maxHeight);

            if (totalWidth > wrapWidth)
            {
                // Can't even render the first word.
                if (i == 0)
                    break;

                TextPart* newLine = new TextPart();
                newLine->m_size.x = totalWidth - words[i]->m_size.x - spaceAdvance;
                newLine->m_size.y = maxHeight;
                newLine->m_str    = append;
                lines.push_back(newLine);
                append     = words[i]->m_str + " ";
                totalWidth = words[i]->m_size.x + spaceAdvance;
                maxHeight  = words[i]->m_size.y - offset.y * remap;
            }
            else
            {
                totalWidth += spaceAdvance;
                append += words[i]->m_str + " ";
            }
        }

        TextPart* newLine = new TextPart();
        newLine->m_size.x = totalWidth - spaceAdvance;
        newLine->m_size.y = maxHeight;
        newLine->m_str    = append;
        lines.push_back(newLine);
    }

    void Internal::ProcessText(DrawBuffer* buf, LinaVGFont* font, const LINAVG_STRING& text, const Vec2& pos, const Vec2& offset, const Vec4Grad& color, float spacing, bool isGradient, float scale, float wrapWidth, float rotateAngle, TextAlignment alignment, float newLineSpacing, float sdfThickness)
    {
        const int  bufStart = buf->m_vertexBuffer.m_size;
        const Vec2 size     = Internal::CalcTextSize(text.c_str(), font, scale, spacing, sdfThickness);
        Vec2       usedPos  = pos;
        float      remap    = font->m_isSDF ? Math::Remap(sdfThickness, 0.5f, 1.0f, 0.0f, 1.0f) : 0.0f;
        remap               = Math::Clamp(remap, 0.0f, 1.0f);

        const Vec2 off = Internal::CalcMaxCharOffset(text.c_str(), font, scale);
        usedPos.y -= off.y * remap;
        usedPos.x += Math::Abs(off.x) * remap;
        usedPos.y += font->m_ascent + font->m_descent;

        if (wrapWidth == 0.0f || size.x < wrapWidth)
        {
            if (alignment == TextAlignment::Center)
                usedPos.x -= size.x / 2.0f;
            else if (alignment == TextAlignment::Right)
                usedPos.x -= size.x;
            Internal::DrawText(buf, font, text.c_str(), usedPos, offset, color, spacing, isGradient, scale);
        }
        else
        {
            Array<TextPart*> arr;
            Array<TextPart*> lines;
            arr.reserve(10);
            lines.reserve(5);

            ParseTextIntoWords(arr, text, font, scale, spacing);
            ParseWordsIntoLines(lines, arr, font, scale, spacing, wrapWidth, sdfThickness);

            for (int i = 0; i < lines.m_size; i++)
            {

                if (alignment == TextAlignment::Center)
                    usedPos.x = pos.x - lines[i]->m_size.x / 2.0f;
                else if (alignment == TextAlignment::Right)
                    usedPos.x = pos.x - lines[i]->m_size.x;

                Internal::DrawText(buf, font, lines[i]->m_str.c_str(), usedPos, offset, color, spacing, isGradient, scale);
                usedPos.y += font->m_newLineHeight + newLineSpacing;
                delete lines[i];
            }

            for (int i = 0; i < arr.m_size; i++)
                delete arr[i];

            arr.clear();
            lines.clear();
        }

        if (rotateAngle != 0.0f)
        {
            const Vec2 center = Internal::GetVerticesCenter(buf, bufStart, buf->m_vertexBuffer.m_size - 1);
            Internal::RotateVertices(buf->m_vertexBuffer, center, bufStart, buf->m_vertexBuffer.m_size - 1, rotateAngle);
        }
    }

    Vec2 Internal::CalcMaxCharOffset(const char* text, LinaVGFont* font, float scale)
    {
        const uint8_t* c;
        Vec2           offset  = Vec2(0.0f, 0.0f);
        int            counter = 0;

        // Iterate through the whole text and determine max width & height
        // As well as line breaks based on wrapping.
        for (c = (const uint8_t*)text; *c; c++)
        {
            auto& ch = font->m_characterGlyphs[*c];
            float x  = ch.m_advance.x * scale;
            float y  = ch.m_size.y * scale;

            if (counter == 0)
                offset.x = ch.m_bearing.x < 0.0f ? ch.m_bearing.x * scale : 0.0f;
            offset.y = Math::Max(offset.y, (ch.m_size.y - ch.m_bearing.y) * scale);
            counter++;
        }

        return offset;
    }

    void Internal::DrawDebugFontAtlas(const Vec2& pos, FontHandle handle)
    {
        FontHandle  fontHandle = handle > 0 && Internal::g_textData.m_loadedFonts.m_size > handle - 1 ? handle : Internal::g_textData.m_defaultFont;
        LinaVGFont* font       = Internal::g_textData.m_loadedFonts[static_cast<int>(fontHandle) - 1];
        DrawBuffer* buf        = &Internal::g_rendererData.GetSimpleTextBuffer(font->m_texture, 999, false);
        const int   startIndex = buf->m_vertexBuffer.m_size;
        Vertex      v0, v1, v2, v3;
        v0.pos = Vec2(pos.x, pos.y);
        v1.pos = Vec2(pos.x + static_cast<float>(Config.maxFontAtlasSize), pos.y);
        v2.pos = Vec2(pos.x + static_cast<float>(Config.maxFontAtlasSize), pos.y + static_cast<float>(Config.maxFontAtlasSize));
        v3.pos = Vec2(pos.x, pos.y + static_cast<float>(Config.maxFontAtlasSize));
        v0.uv  = Vec2(0.0f, 0.0f);
        v1.uv  = Vec2(1.0f, 0.0f);
        v2.uv  = Vec2(1.0f, 1.0f);
        v3.uv  = Vec2(0.0f, 1.0f);

        buf->PushVertex(v0);
        buf->PushVertex(v1);
        buf->PushVertex(v2);
        buf->PushVertex(v3);

        buf->PushIndex(startIndex);
        buf->PushIndex(startIndex + 1);
        buf->PushIndex(startIndex + 3);
        buf->PushIndex(startIndex + 1);
        buf->PushIndex(startIndex + 2);
        buf->PushIndex(startIndex + 3);
    }

    void Internal::DrawText(DrawBuffer* buf, LinaVGFont* font, const char* text, const Vec2& position, const Vec2& offset, const Vec4Grad& color, float spacing, bool isGradient, float scale)
    {
        const uint8_t* c;
        const int      totalCharacterCount = Utility::GetTextCharacterSize(text);
        const int      bufStart            = buf->m_vertexBuffer.m_size;
        Vec4           lastMinGrad         = color.start;
        Vec2           pos                 = position;
        int            characterCount      = 0;

        auto drawChar = [&](TextCharacter& ch) {
            const int startIndex = buf->m_vertexBuffer.m_size;

            float x2 = pos.x + ch.m_bearing.x * scale;
            float y2 = pos.y - ch.m_bearing.y * scale;
            float w  = ch.m_size.x * scale;
            float h  = ch.m_size.y * scale;

            pos.x += ch.m_advance.x * scale + spacing;
            pos.y += ch.m_advance.y * scale;

            if (w == 0.0f || h == 0.0f)
                return;

            Vertex v0, v1, v2, v3;

            if (isGradient)
            {
                if (color.gradientType == GradientType::Horizontal)
                {
                    const float maxT       = static_cast<float>(characterCount + 1) / static_cast<float>(totalCharacterCount);
                    const Vec4  currentMin = lastMinGrad;
                    const Vec4  currentMax = Math::Lerp(color.start, color.end, maxT);
                    lastMinGrad            = currentMax;

                    v0.col = currentMin;
                    v1.col = currentMax;
                    v2.col = currentMax;
                    v3.col = currentMin;
                }
                else // fallback is vertical since radial gradients are not supported.
                {
                    v0.col = color.start;
                    v1.col = color.start;
                    v2.col = color.end;
                    v3.col = color.end;
                }
            }
            else
                v0.col = v1.col = v2.col = v3.col = color.start;

            v0.pos = Vec2(x2 + offset.x, y2 + offset.y);
            v1.pos = Vec2(x2 + offset.x + w, y2 + offset.y);
            v2.pos = Vec2(x2 + offset.x + w, y2 + h + offset.y);
            v3.pos = Vec2(x2 + offset.x, y2 + h + offset.y);

            v0.uv = Vec2(ch.m_uv12.x, ch.m_uv12.y);
            v1.uv = Vec2(ch.m_uv12.z, ch.m_uv12.w);
            v2.uv = Vec2(ch.m_uv34.x, ch.m_uv34.y);
            v3.uv = Vec2(ch.m_uv34.z, ch.m_uv34.w);

            buf->PushVertex(v0);
            buf->PushVertex(v1);
            buf->PushVertex(v2);
            buf->PushVertex(v3);

            buf->PushIndex(startIndex);
            buf->PushIndex(startIndex + 1);
            buf->PushIndex(startIndex + 3);
            buf->PushIndex(startIndex + 1);
            buf->PushIndex(startIndex + 2);
            buf->PushIndex(startIndex + 3);
            characterCount++;
        };

        if (Config.useUnicodeEncoding)
        {
// _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#pragma warning(disable : 4996)

            std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cv;
            auto                                                        str32 = cv.from_bytes(text);
            std::u32string::iterator                                    it;
            int                                                         counter = 0;
            for (it = str32.begin(); it < str32.end(); it++)
            {
                auto& ch = font->m_characterGlyphs[*it];
                drawChar(ch);
                counter++;
            }
        }
        else
        {
            for (c = (uint8_t*)text; *c; c++)
            {
                auto& ch = font->m_characterGlyphs[*c];
                drawChar(ch);
            }
        }
    }

    Vec2 Internal::CalcTextSize(const char* text, LinaVGFont* font, float scale, float spacing, float sdfThickness)
    {
        float          maxCharacterHeight = 0.0f;
        float          totalWidth         = 0.0f;
        const uint8_t* c;

        // Iterate through the whole text and determine max width & height
        // As well as line breaks based on wrapping.
        for (c = (const uint8_t*)text; *c; c++)
        {
            auto& ch = font->m_characterGlyphs[*c];
            float x  = ch.m_advance.x * scale;
            float y  = ch.m_size.y * scale;
            totalWidth += x + spacing;
            maxCharacterHeight = Math::Max(maxCharacterHeight, y);
        }

        if (font->m_isSDF)
        {
            float remapY   = Math::Remap(sdfThickness, 0.5f, 1.0f, 2.0f, 0.0f);
            float remapX   = Math::Remap(sdfThickness, 0.5f, 1.0f, 0.0f, 1.0f);
            remapX         = Math::Clamp(remapX, 0.0f, 1.0f);
            remapY         = Math::Clamp(remapY, 0.0f, 2.0f);
            const Vec2 off = Internal::CalcMaxCharOffset(text, font, scale);
            maxCharacterHeight -= off.y * remapY;
            totalWidth += Math::Abs(off.x) * remapX;
        }

        return Vec2(totalWidth, maxCharacterHeight);
    }

    Vec2 Internal::CalcTextSizeWrapped(const LINAVG_STRING& text, LinaVGFont* font, float newLineSpacing, float wrapWidth, float scale, float spacing, float sdfThickness)
    {
        Array<TextPart*> arr;
        Array<TextPart*> lines;
        arr.reserve(10);
        lines.reserve(5);
        ParseTextIntoWords(arr, text, font, scale, spacing);
        ParseWordsIntoLines(lines, arr, font, scale, spacing, wrapWidth, sdfThickness);

        if (lines.m_size == 1)
        {
            const Vec2 finalSize = Internal::CalcTextSize(lines[0]->m_str.c_str(), font, scale, spacing, sdfThickness);

            for (int i = 0; i < lines.m_size; i++)
                delete lines[i];

            for (int i = 0; i < arr.m_size; i++)
                delete arr[i];

            arr.clear();
            lines.clear();
            return finalSize;
        }

        Vec2 size = Vec2(0.0f, 0.0f);

        for (int i = 0; i < lines.m_size; i++)
        {
            const Vec2 calcSize = lines[i]->m_size;
            size.x              = Math::Max(calcSize.x, size.x);
            size.y += calcSize.y;

            if (i < lines.m_size - 1)
                size.y += newLineSpacing + font->m_newLineHeight;

            delete lines[i];
        }

        for (int i = 0; i < arr.m_size; i++)
            delete arr[i];

        arr.clear();
        lines.clear();
        // size.y -= offset.y * remap;
        return size;
    }

} // namespace LinaVG
