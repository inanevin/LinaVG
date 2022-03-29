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
    void DrawArc(const Vec2& p1, const Vec2& p2, const Vec4Grad& color, float radius, ThicknessGrad thickness, int segments, bool flip)
    {

        const float halfMag              = Math::Mag(Vec2(p2.x - p1.x, p2.y - p1.y)) / 2.0f;
        const Vec2  center               = Vec2((p1.x + p2.x) / 2.0f, (p1.y + p2.y) / 2.0f);
        const Vec2  dir                  = Vec2(p2.x - p1.x, p2.y - p1.y);
        float       angle1               = Math::GetAngleFromCenter(center, p1);
        float       angle2               = Math::GetAngleFromCenter(center, p2);
        const bool  useGradientColor     = !Math::IsEqual(color.m_start, color.m_end);
        const float useGradientThickness = thickness.m_start != thickness.m_end;

        Vec2 lastPoint = p1;

        if (angle2 < angle1)
            angle2 += 360.0f;

        const float midAngle      = (angle2 + angle1) / 2.0f;
        const float angleIncrease = (segments >= 180.0f || segments < 0.0f) ? 1.0f : 180.0f / (float)segments;

        Vec4  startColor     = color.m_start;
        Vec4  endColor       = color.m_end;
        float startThickness = thickness.m_start;
        float endThickness   = thickness.m_end;
        for (float i = angle1 + angleIncrease; i < angle2; i += angleIncrease)
        {
            Vec2 p = Vec2(0, 0);

            if (radius == 0.0f)
                p = Math::GetPointOnCircle(center, halfMag, i);
            else
            {
                const Vec2 out = Math::Normalized(Math::Rotate90(dir, !flip));
                p              = Math::SampleParabola(p1, p2, out, radius, Math::Remap(i, angle1, angle2, 0.0f, 1.0f));
            }

            const float remapped = Math::Remap(i, angle1, angle2, 0.0f, 1.0f);

            if (useGradientColor)
                endColor = Math::Lerp(color.m_start, color.m_end, remapped);

            if (useGradientThickness)
                endThickness = Math::Lerp(thickness.m_start, thickness.m_end, remapped);

            DrawLine(lastPoint, p, {startColor, endColor}, {startThickness, endThickness});

            if (useGradientColor)
                startColor = endColor;

            if (useGradientThickness)
                startThickness = endThickness;

            lastPoint = p;
        }
    }

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

            DrawLine(lastPoint, p, {startColor, endColor}, {startThickness, endThickness});

            if (useGradientColor)
                startColor = endColor;

            if (useGradientThickness)
                startThickness = endThickness;
            lastPoint = p;
        }
    }

    void DrawLine(const Vec2& p1, const Vec2& p2, const Vec4Grad& color, ThicknessGrad thickness)
    {
        //  thickness.m_start *= Renderer::Internal::g_renderer->m_options.m_framebufferScale.x;
        //  thickness.m_end *= Renderer::Internal::g_renderer->m_options.m_framebufferScale.x;
        //  const float halfThickness1 = thickness.m_start / 2.0f;
        //  const float halfThickness2 = thickness.m_end / 2.0f;
        //  const Vec2  dir            = Math::Normalized(Vec2(p2.x - p1.x, p2.y - p1.y));
        //  const Vec2  up             = Math::Rotate90(dir, false);
        //  const Vec2  down           = Math::Rotate90(dir, true);
        //  Vertex      p1U, p1D, p2U, p2D;
        //  p1U.m_pos = Vec2(p1.x + up.x * halfThickness1, p1.y + up.y * halfThickness1);
        //  p1D.m_pos = Vec2(p1.x + down.x * halfThickness1, p1.y + down.y * halfThickness1);
        //  p2U.m_pos = Vec2(p2.x + up.x * halfThickness2, p2.y + up.y * halfThickness2);
        //  p2D.m_pos = Vec2(p2.x + down.x * halfThickness2, p2.y + down.y * halfThickness2);
        //  p1U.m_col = color.m_start;
        //  p1D.m_col = color.m_start;
        //  p2U.m_col = color.m_end;
        //  p2D.m_col = color.m_end;
        //
        //  Vertex p1UU, p2UU, p1DD, p2DD;
        //
        //  // If we use CPU feathering, create 2 more additional "top" & "down" quads.
        //  if (Renderer::Internal::g_renderer->m_options.m_featheringType == AAType::VertexAA)
        //  {
        //      const float distance = Renderer::Internal::g_renderer->m_options.m_featheringDistance * 2;
        //      p1UU.m_pos.x         = p1U.m_pos.x + up.x * distance;
        //      p1UU.m_pos.y         = p1U.m_pos.y + up.y * distance;
        //      p2UU.m_pos.x         = p2U.m_pos.x + up.x * distance;
        //      p2UU.m_pos.y         = p2U.m_pos.y + up.y * distance;
        //      p1DD.m_pos.x         = p1D.m_pos.x + down.x * distance;
        //      p1DD.m_pos.y         = p1D.m_pos.y + down.y * distance;
        //      p2DD.m_pos.x         = p2D.m_pos.x + down.x * distance;
        //      p2DD.m_pos.y         = p2D.m_pos.y + down.y * distance;
        //      p1UU.m_col           = Vec4(p1U.m_col.x, p1U.m_col.y, p1U.m_col.z, 0.0f);
        //      p2UU.m_col           = Vec4(p2U.m_col.x, p2U.m_col.y, p2U.m_col.z, 0.0f);
        //      p1DD.m_col           = Vec4(p1D.m_col.x, p1D.m_col.y, p1D.m_col.z, 0.0f);
        //      p2DD.m_col           = Vec4(p2D.m_col.x, p2D.m_col.y, p2D.m_col.z, 0.0f);
        //  }
        //
        //  if (m_isJoiningLines && m_lastLineVtx1 != -1)
        //  {
        //      if (Renderer::Internal::g_renderer->m_options.m_lineJointType == JointType::VertexAvg)
        //      {
        //          if (Renderer::Internal::g_renderer->m_options.m_featheringType == AAType::VertexAA)
        //          {
        //              Vertex&    vtx1  = Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer[m_lastLineVtx1];
        //              Vertex&    vtx2  = Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer[m_lastLineVtx2];
        //              Vertex&    vtx3  = Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer[m_lastLineVtx3];
        //              Vertex&    vtx4  = Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer[m_lastLineVtx4];
        //              const Vec2 avgUU = Vec2((vtx1.m_pos.x + p1UU.m_pos.x) / 2.0f, (vtx1.m_pos.y + p1UU.m_pos.y) / 2.0f);
        //              const Vec2 avgU  = Vec2((vtx2.m_pos.x + p1U.m_pos.x) / 2.0f, (vtx2.m_pos.y + p1U.m_pos.y) / 2.0f);
        //              const Vec2 avgD  = Vec2((vtx3.m_pos.x + p1D.m_pos.x) / 2.0f, (vtx3.m_pos.y + p1D.m_pos.y) / 2.0f);
        //              const Vec2 avgDD = Vec2((vtx4.m_pos.x + p1DD.m_pos.x) / 2.0f, (vtx4.m_pos.y + p1DD.m_pos.y) / 2.0f);
        //              vtx1.m_pos       = avgUU;
        //              vtx2.m_pos       = avgU;
        //              vtx3.m_pos       = avgD;
        //              vtx4.m_pos       = avgDD;
        //              p1UU.m_pos       = avgUU;
        //              p1U.m_pos        = avgU;
        //              p1D.m_pos        = avgD;
        //              p1DD.m_pos       = avgDD;
        //          }
        //          else
        //          {
        //              Vertex&    vtx1 = Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer[m_lastLineVtx1];
        //              Vertex&    vtx2 = Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer[m_lastLineVtx2];
        //              const Vec2 avgU = Vec2((vtx1.m_pos.x + p1U.m_pos.x) / 2.0f, (vtx1.m_pos.y + p1U.m_pos.y) / 2.0f);
        //              const Vec2 avgD = Vec2((vtx2.m_pos.x + p1D.m_pos.x) / 2.0f, (vtx2.m_pos.y + p1D.m_pos.y) / 2.0f);
        //              vtx1.m_pos      = avgU;
        //              vtx2.m_pos      = avgD;
        //              p1U.m_pos       = avgU;
        //              p1D.m_pos       = avgD;
        //          }
        //      }
        //      else if (Renderer::Internal::g_renderer->m_options.m_lineJointType == JointType::Bevel)
        //      {
        //          if (Renderer::Internal::g_renderer->m_options.m_featheringType == AAType::VertexAA)
        //          {
        //              const float angle = Math::GetAngleBetween(m_lastLineDir, down);
        //
        //              if (angle < 90.0f)
        //              {
        //                  Vertex& vtx1 = Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer[m_lastLineVtx1];
        //
        //                  // going downwards, join down.
        //                  const Vec2 newPos = vtx1.m_pos;
        //                  const Vec2 diff   = Vec2(newPos.x - p1UU.m_pos.x, newPos.y - p1UU.m_pos.y);
        //                  p1UU.m_pos        = newPos;
        //                  p1U.m_pos         = Vec2(p1U.m_pos.x + diff.x, p1U.m_pos.y + diff.y);
        //                  p1D.m_pos         = Vec2(p1D.m_pos.x + diff.x, p1D.m_pos.y + diff.y);
        //                  p1DD.m_pos        = Vec2(p1DD.m_pos.x + diff.x, p1DD.m_pos.y + diff.y);
        //                  p2UU.m_pos        = Vec2(p2UU.m_pos.x + diff.x, p2UU.m_pos.y + diff.y);
        //                  p2U.m_pos         = Vec2(p2U.m_pos.x + diff.x, p2U.m_pos.y + diff.y);
        //                  p2D.m_pos         = Vec2(p2D.m_pos.x + diff.x, p2D.m_pos.y + diff.y);
        //                  p2DD.m_pos        = Vec2(p2DD.m_pos.x + diff.x, p2DD.m_pos.y + diff.y);
        //                  // Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(Renderer::Internal::g_renderer->m_drawData.m_indexCounter - 2);
        //                  // Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(Renderer::Internal::g_renderer->m_drawData.m_indexCounter - 3);
        //                  // Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(Renderer::Internal::g_renderer->m_drawData.m_indexCounter + 3);
        //              }
        //              else if (angle > 90.0f)
        //              {
        //                  Vertex& vtx4 = Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer[m_lastLineVtx4];
        //
        //                  const Vec2 newPos = vtx4.m_pos;
        //                  const Vec2 diff   = Vec2(newPos.x - p1D.m_pos.x, newPos.y - p1D.m_pos.y);
        //                  p1D.m_pos         = newPos;
        //                  p1UU.m_pos        = Vec2(p1UU.m_pos.x + diff.x, p1UU.m_pos.y + diff.y);
        //                  p1U.m_pos         = Vec2(p1U.m_pos.x + diff.x, p1U.m_pos.y + diff.y);
        //                  p1DD.m_pos        = Vec2(p1DD.m_pos.x + diff.x, p1DD.m_pos.y + diff.y);
        //                  p2UU.m_pos        = Vec2(p2UU.m_pos.x + diff.x, p2UU.m_pos.y + diff.y);
        //                  p2U.m_pos         = Vec2(p2U.m_pos.x + diff.x, p2U.m_pos.y + diff.y);
        //                  p2D.m_pos         = Vec2(p2D.m_pos.x + diff.x, p2D.m_pos.y + diff.y);
        //                  p2DD.m_pos        = Vec2(p2DD.m_pos.x + diff.x, p2DD.m_pos.y + diff.y);
        //                  // Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(Renderer::Internal::g_renderer->m_drawData.m_indexCounter - 2);
        //                  // Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(Renderer::Internal::g_renderer->m_drawData.m_indexCounter - 3);
        //                  // Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(Renderer::Internal::g_renderer->m_drawData.m_indexCounter);
        //              }
        //          }
        //          else
        //          {
        //              const float angle = Math::GetAngleBetween(m_lastLineDir, down);
        //
        //              if (angle < 90.0f)
        //              {
        //                  Vertex& vtx1 = Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer[m_lastLineVtx1];
        //
        //                  // going downwards, join down.
        //                  const Vec2 newPos = vtx1.m_pos;
        //                  const Vec2 diff   = Vec2(newPos.x - p1U.m_pos.x, newPos.y - p1U.m_pos.y);
        //                  p1U.m_pos         = newPos;
        //                  p1D.m_pos         = Vec2(p1D.m_pos.x + diff.x, p1D.m_pos.y + diff.y);
        //                  p2U.m_pos         = Vec2(p2U.m_pos.x + diff.x, p2U.m_pos.y + diff.y);
        //                  p2D.m_pos         = Vec2(p2D.m_pos.x + diff.x, p2D.m_pos.y + diff.y);
        //                  Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(Renderer::Internal::g_renderer->m_drawData.m_indexCounter - 2);
        //                  Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(Renderer::Internal::g_renderer->m_drawData.m_indexCounter - 3);
        //                  Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(Renderer::Internal::g_renderer->m_drawData.m_indexCounter + 3);
        //              }
        //              else if (angle > 90.0f)
        //              {
        //                  Vertex& vtx2 = Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer[m_lastLineVtx2];
        //
        //                  const Vec2 newPos = vtx2.m_pos;
        //                  const Vec2 diff   = Vec2(newPos.x - p1D.m_pos.x, newPos.y - p1D.m_pos.y);
        //                  p1D.m_pos         = newPos;
        //                  p1U.m_pos         = Vec2(p1U.m_pos.x + diff.x, p1U.m_pos.y + diff.y);
        //                  p2U.m_pos         = Vec2(p2U.m_pos.x + diff.x, p2U.m_pos.y + diff.y);
        //                  p2D.m_pos         = Vec2(p2D.m_pos.x + diff.x, p2D.m_pos.y + diff.y);
        //                  Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(Renderer::Internal::g_renderer->m_drawData.m_indexCounter - 2);
        //                  Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(Renderer::Internal::g_renderer->m_drawData.m_indexCounter - 3);
        //                  Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(Renderer::Internal::g_renderer->m_drawData.m_indexCounter);
        //              }
        //          }
        //      }
        //  }
        //
        //  m_lastLineDir = dir;
        //
        //  if (Renderer::Internal::g_renderer->m_options.m_featheringType == AAType::VertexAA)
        //  {
        //      Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer.push_back(p1U);
        //      Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer.push_back(p2U);
        //      Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer.push_back(p2D);
        //      Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer.push_back(p1D);
        //      m_lastLineVtx2 = Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer.m_size - 3;
        //      m_lastLineVtx3 = Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer.m_size - 2;
        //
        //      const Index curr = Renderer::Internal::g_renderer->m_drawData.m_indexCounter;
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(curr);
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(curr + 1);
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(curr + 3);
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(curr + 1);
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(curr + 2);
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(curr + 3);
        //      Renderer::Internal::g_renderer->m_drawData.m_indexCounter += 4;
        //
        //      // DrawPoint(p1UU.m_pos, Vec4(1,0,0,1));
        //      // DrawPoint(p2UU.m_pos, Vec4(1,0,0,1));
        //      const Index featherCurr = Renderer::Internal::g_renderer->m_drawData.m_indexCounter;
        //
        //      Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer.push_back(p1UU);
        //      Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer.push_back(p2UU);
        //      Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer.push_back(p2DD);
        //      Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer.push_back(p1DD);
        //      m_lastLineVtx1 = Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer.m_size - 3;
        //      m_lastLineVtx4 = Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer.m_size - 2;
        //
        //      // Up (inverted)
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(featherCurr);
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(featherCurr + 1);
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(featherCurr - 4);
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(featherCurr + 1);
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(featherCurr - 3);
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(featherCurr - 4);
        //
        //      // Down (inverted)
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(featherCurr + 2);
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(featherCurr + 3);
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(featherCurr - 1);
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(featherCurr + 2);
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(featherCurr - 2);
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(featherCurr - 1);
        //
        //      Renderer::Internal::g_renderer->m_drawData.m_indexCounter += 4;
        //  }
        //  else
        //  {
        //
        //      Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer.push_back(p1U);
        //      Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer.push_back(p2U);
        //      Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer.push_back(p2D);
        //      Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer.push_back(p1D);
        //      m_lastLineVtx1 = Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer.m_size - 3;
        //      m_lastLineVtx2 = Renderer::Internal::g_renderer->m_drawData.m_vertexBuffer.m_size - 2;
        //
        //      const Index curr = Renderer::Internal::g_renderer->m_drawData.m_indexCounter;
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(curr);
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(curr + 1);
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(curr + 3);
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(curr + 1);
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(curr + 2);
        //      Renderer::Internal::g_renderer->m_drawData.m_indexBuffer.push_back(curr + 3);
        //      Renderer::Internal::g_renderer->m_drawData.m_indexCounter += 4;
        //  }
    }

    void DrawPoint(const Vec2& p1, const Vec4& col)
    {
        StyleOptions style;
        style.m_color        = col;
        const float distance = Config.m_framebufferScale.x / 2.0f;
        DrawRectFilled(Vec2(p1.x - distance, p1.y - distance), Vec2(p1.x + distance, p1.y + distance), style);
    }

    void DrawTriangleFilled(const Vec2& left, const Vec2& right, const Vec2& top, StyleOptions& style, float rotateAngle)
    {
        if (style.m_rounding == 0.0f)
        {
            if (Math::IsEqual(style.m_color.m_start, style.m_color.m_end))
            {
                if (style.m_textureHandle == 0)
                    Internal::FillTri_NoRound_SC(Internal::g_rendererData.m_defaultBuffer.m_vertexBuffer, Internal::g_rendererData.m_defaultBuffer.m_indexBuffer, rotateAngle, left, right, top, style.m_color.m_start);
                else
                {
                    Array<Vertex> vertices;
                    Array<Index>  indices;
                    Internal::FillTri_NoRound_SC(vertices, indices, rotateAngle, left, right, top, style.m_color.m_start);
                    Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
                }
            }
            else
            {
                if (style.m_color.m_gradientType == GradientType::Horizontal)
                {
                    // Horizontal, non rounded
                    if (style.m_textureHandle == 0)
                        Internal::FillTri_NoRound_VerHorGra(Internal::g_rendererData.m_defaultBuffer.m_vertexBuffer, Internal::g_rendererData.m_defaultBuffer.m_indexBuffer, rotateAngle, left, right, top, style.m_color.m_start, style.m_color.m_end, style.m_color.m_end);
                    else
                    {
                        Array<Vertex> vertices;
                        Array<Index>  indices;
                        Internal::FillTri_NoRound_VerHorGra(vertices, indices, rotateAngle, left, right, top, style.m_color.m_start, style.m_color.m_end, style.m_color.m_end);
                        Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
                    }
                }
                else if (style.m_color.m_gradientType == GradientType::Vertical)
                {
                    // Vertical, non rounded
                    if (style.m_textureHandle == 0)
                        Internal::FillTri_NoRound_VerHorGra(Internal::g_rendererData.m_defaultBuffer.m_vertexBuffer, Internal::g_rendererData.m_defaultBuffer.m_indexBuffer, rotateAngle, left, right, top, style.m_color.m_end, style.m_color.m_end, style.m_color.m_start);
                    else
                    {
                        Array<Vertex> vertices;
                        Array<Index>  indices;
                        Internal::FillTri_NoRound_VerHorGra(vertices, indices, rotateAngle, left, right, top, style.m_color.m_end, style.m_color.m_end, style.m_color.m_start);
                        Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
                    }
                }
                else if (style.m_color.m_gradientType == GradientType::Radial || style.m_color.m_gradientType == GradientType::RadialCorner)
                {
                    // Radial, non rounded
                    if (style.m_textureHandle == 0)
                    {
                        Array<Vertex> vertices;
                        Array<Index>  indices;
                        Internal::FillTri_NoRound_RadialGra(vertices, indices, rotateAngle, left, right, top, style.m_color.m_start, style.m_color.m_end);
                        Backend::DrawGradient(vertices, indices, style.m_color.m_start, style.m_color.m_end, style.m_color.m_gradientType, style.m_color.m_radialSize);
                    }
                    else
                    {
                        Array<Vertex> vertices;
                        Array<Index>  indices;
                        Internal::FillTri_NoRound_RadialGra(vertices, indices, rotateAngle, left, right, top, style.m_color.m_start, style.m_color.m_end);
                        Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
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
                    Internal::FillTri_Round(Internal::g_rendererData.m_defaultBuffer.m_vertexBuffer, Internal::g_rendererData.m_defaultBuffer.m_indexBuffer, style.m_onlyRoundTheseCorners, rotateAngle, left, right, top, style.m_color.m_start, style.m_rounding);
                else
                {
                    Array<Vertex> vertices;
                    Array<Index>  indices;
                    Internal::FillTri_Round(vertices, indices, style.m_onlyRoundTheseCorners, rotateAngle, left, right, top, style.m_color.m_start, style.m_rounding);
                    Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
                }
            }
            else
            {
                // Rounded, gradient.
                if (style.m_textureHandle == 0)
                {
                    Array<Vertex> vertices;
                    Array<Index>  indices;
                    Internal::FillTri_Round(vertices, indices, style.m_onlyRoundTheseCorners, rotateAngle, left, right, top, style.m_color.m_start, style.m_rounding);
                    Backend::DrawGradient(vertices, indices, style.m_color.m_start, style.m_color.m_end, style.m_color.m_gradientType, style.m_color.m_radialSize);
                }
                else
                {
                    Array<Vertex> vertices;
                    Array<Index>  indices;
                    Internal::FillTri_Round(vertices, indices, style.m_onlyRoundTheseCorners, rotateAngle, left, right, top, style.m_color.m_start, style.m_rounding);
                    Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
                }
            }
        }
    }

    void DrawRectFilled(const Vec2& min, const Vec2& max, StyleOptions& style, float rotateAngle)
    {
        if (style.m_rounding == 0.0f)
        {
            // Single color
            if (Math::IsEqual(style.m_color.m_start, style.m_color.m_end))
            {
                if (style.m_textureHandle == 0)
                    Internal::FillRect_NoRound_SC(Internal::g_rendererData.m_defaultBuffer.m_vertexBuffer, Internal::g_rendererData.m_defaultBuffer.m_indexBuffer, rotateAngle, min, max, style.m_color.m_start);
                else
                {
                    Array<Vertex> vertices;
                    Array<Index>  indices;
                    Internal::FillRect_NoRound_SC(vertices, indices, rotateAngle, min, max, style.m_color.m_start);
                    Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
                }
            }
            else
            {
                if (style.m_color.m_gradientType == GradientType::Horizontal)
                {
                    // Horizontal, non rounded
                    if (style.m_textureHandle == 0)
                    {
                        Internal::FillRect_NoRound_VerHorGra(Internal::g_rendererData.m_defaultBuffer.m_vertexBuffer, Internal::g_rendererData.m_defaultBuffer.m_indexBuffer, rotateAngle, min, max,
                                                             style.m_color.m_start, style.m_color.m_end, style.m_color.m_end, style.m_color.m_start);
                    }
                    else
                    {
                        Array<Vertex> vertices;
                        Array<Index>  indices;
                        Internal::FillRect_NoRound_VerHorGra(vertices, indices, rotateAngle, min, max,
                                                             style.m_color.m_start, style.m_color.m_end, style.m_color.m_end, style.m_color.m_start);
                        Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
                    }
                }
                else if (style.m_color.m_gradientType == GradientType::Vertical)
                {
                    // Vertical, non rounded
                    if (style.m_textureHandle == 0)
                    {
                        Internal::FillRect_NoRound_VerHorGra(Internal::g_rendererData.m_defaultBuffer.m_vertexBuffer, Internal::g_rendererData.m_defaultBuffer.m_indexBuffer, rotateAngle, min, max,
                                                             style.m_color.m_start, style.m_color.m_start, style.m_color.m_end, style.m_color.m_end);
                    }
                    else
                    {
                        Array<Vertex> vertices;
                        Array<Index>  indices;
                        Internal::FillRect_NoRound_VerHorGra(vertices, indices, rotateAngle, min, max,
                                                             style.m_color.m_start, style.m_color.m_start, style.m_color.m_end, style.m_color.m_end);
                        Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
                    }
                }
                else if (style.m_color.m_gradientType == GradientType::Radial || style.m_color.m_gradientType == GradientType::RadialCorner)
                {
                    // Radial, non rounded
                    if (style.m_textureHandle == 0)
                    {
                        Array<Vertex> vertices;
                        Array<Index>  indices;
                        Internal::FillRect_NoRound_RadialGra(vertices, indices, rotateAngle, min, max, style.m_color.m_start, style.m_color.m_end);
                        Backend::DrawGradient(vertices, indices, style.m_color.m_start, style.m_color.m_end, style.m_color.m_gradientType, style.m_color.m_radialSize);
                    }
                    else
                    {
                        Array<Vertex> vertices;
                        Array<Index>  indices;
                        Internal::FillRect_NoRound_RadialGra(vertices, indices, rotateAngle, min, max, style.m_color.m_start, style.m_color.m_end);
                        Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
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
                    Internal::FillRect_Round(Internal::g_rendererData.m_defaultBuffer.m_vertexBuffer, Internal::g_rendererData.m_defaultBuffer.m_indexBuffer, style.m_onlyRoundTheseCorners, rotateAngle, min, max, style.m_color.m_start, style.m_rounding);
                }
                else
                {
                    Array<Vertex> vertices;
                    Array<Index>  indices;
                    Internal::FillRect_Round(vertices, indices, style.m_onlyRoundTheseCorners, rotateAngle, min, max, style.m_color.m_start, style.m_rounding);
                    Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
                }
            }
            else
            {
                if (style.m_textureHandle == 0)
                {
                    // Rounded, gradient.
                    Array<Vertex> vertices;
                    Array<Index>  indices;
                    Internal::FillRect_Round(vertices, indices, style.m_onlyRoundTheseCorners, rotateAngle, min, max, style.m_color.m_start, style.m_rounding);
                    Backend::DrawGradient(vertices, indices, style.m_color.m_start, style.m_color.m_end, style.m_color.m_gradientType, style.m_color.m_radialSize);
                }
                else
                {
                    // Rounded, gradient.
                    Array<Vertex> vertices;
                    Array<Index>  indices;
                    Internal::FillRect_Round(vertices, indices, style.m_onlyRoundTheseCorners, rotateAngle, min, max, style.m_color.m_start, style.m_rounding);
                    Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
                }
            }
        }
    }

    void DrawNGonFilled(const Vec2& center, float radius, int n, StyleOptions& style, float rotateAngle)
    {
        if (Math::IsEqual(style.m_color.m_start, style.m_color.m_end))
        {
            if (style.m_textureHandle == 0)
                Internal::FillNGon_SC(Internal::g_rendererData.m_defaultBuffer.m_vertexBuffer, Internal::g_rendererData.m_defaultBuffer.m_indexBuffer, rotateAngle, center, radius, n, style.m_color.m_start);
            else
            {
                Array<Vertex> vertices;
                Array<Index>  indices;
                Internal::FillNGon_SC(vertices, indices, rotateAngle, center, radius, n, style.m_color.m_start);
                Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
            }
        }
        else
        {
            if (style.m_color.m_gradientType == GradientType::Horizontal)
            {
                // Horizontal, non rounded
                if (style.m_textureHandle == 0)
                    Internal::FillNGon_VerHorGra(Internal::g_rendererData.m_defaultBuffer.m_vertexBuffer, Internal::g_rendererData.m_defaultBuffer.m_indexBuffer, rotateAngle, center, radius, n, style.m_color.m_start, style.m_color.m_end, true);
                else
                {
                    Array<Vertex> vertices;
                    Array<Index>  indices;
                    Internal::FillNGon_VerHorGra(vertices, indices, rotateAngle, center, radius, n, style.m_color.m_start, style.m_color.m_end, true);
                    Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
                }
            }
            else if (style.m_color.m_gradientType == GradientType::Vertical)
            {
                // Vertical, non rounded
                if (style.m_textureHandle == 0)
                    Internal::FillNGon_VerHorGra(Internal::g_rendererData.m_defaultBuffer.m_vertexBuffer, Internal::g_rendererData.m_defaultBuffer.m_indexBuffer, rotateAngle, center, radius, n, style.m_color.m_start, style.m_color.m_end, false);
                else
                {
                    Array<Vertex> vertices;
                    Array<Index>  indices;
                    Internal::FillNGon_VerHorGra(vertices, indices, rotateAngle, center, radius, n, style.m_color.m_start, style.m_color.m_end, false);
                    Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
                }
            }
            else if (style.m_color.m_gradientType == GradientType::Radial || style.m_color.m_gradientType == GradientType::RadialCorner)
            {
                // // Radial, non rounded
                if (style.m_textureHandle == 0)
                {
                    Array<Vertex> vertices;
                    Array<Index>  indices;
                    Internal::FillNGon_RadialGra(vertices, indices, rotateAngle, center, radius, n, style.m_color.m_start, style.m_color.m_end);
                    Backend::DrawGradient(vertices, indices, style.m_color.m_start, style.m_color.m_end, style.m_color.m_gradientType, style.m_color.m_radialSize);
                }
                else
                {
                    Array<Vertex> vertices;
                    Array<Index>  indices;
                    Internal::FillNGon_RadialGra(vertices, indices, rotateAngle, center, radius, n, style.m_color.m_start, style.m_color.m_end);
                    Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
                }
            }
        }
    }

    void DrawConvexFilled(Vec2* points, int size, StyleOptions& style, float rotateAngle)
    {
        if (size < 3)
        {
            Config.m_errorCallback("Can draw a convex shape that has less than 3 corners!", 0);
            return;
        }

        Vec2 avgCenter = Vec2(0.0f, 0.0f);

        for (int i = 0; i < size; i++)
            avgCenter = Vec2(avgCenter.x + points[i].x, avgCenter.y + points[i].y);

        avgCenter = Vec2(avgCenter.x / (float)size, avgCenter.y / (float)size);

        if (Math::IsEqual(style.m_color.m_start, style.m_color.m_end))
        {
            if (style.m_textureHandle == 0)
                Internal::FillConvex_SC(Internal::g_rendererData.m_defaultBuffer.m_vertexBuffer, Internal::g_rendererData.m_defaultBuffer.m_indexBuffer, rotateAngle, points, size, avgCenter, style.m_color.m_start);
            else
            {
                Array<Vertex> vertices;
                Array<Index>  indices;
                Internal::FillConvex_SC(vertices, indices, rotateAngle, points, size, avgCenter, style.m_color.m_start);
                Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
            }
        }
        else
        {
            if (style.m_color.m_gradientType == GradientType::Horizontal)
            {
                // Horizontal, non rounded
                if (style.m_textureHandle == 0)
                    Internal::FillConvex_VerHorGra(Internal::g_rendererData.m_defaultBuffer.m_vertexBuffer, Internal::g_rendererData.m_defaultBuffer.m_indexBuffer, rotateAngle, points, size, avgCenter, style.m_color.m_start, style.m_color.m_end, true);
                else
                {
                    Array<Vertex> vertices;
                    Array<Index>  indices;
                    Internal::FillConvex_VerHorGra(vertices, indices, rotateAngle, points, size, avgCenter, style.m_color.m_start, style.m_color.m_end, true);
                    Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
                }
            }
            else if (style.m_color.m_gradientType == GradientType::Vertical)
            {
                // Vertical, non rounded
                if (style.m_textureHandle == 0)
                    Internal::FillConvex_VerHorGra(Internal::g_rendererData.m_defaultBuffer.m_vertexBuffer, Internal::g_rendererData.m_defaultBuffer.m_indexBuffer, rotateAngle, points, size, avgCenter, style.m_color.m_start, style.m_color.m_end, false);
                else
                {
                    Array<Vertex> vertices;
                    Array<Index>  indices;
                    Internal::FillConvex_VerHorGra(vertices, indices, rotateAngle, points, size, avgCenter, style.m_color.m_start, style.m_color.m_end, false);
                    Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
                }
            }
            else if (style.m_color.m_gradientType == GradientType::Radial || style.m_color.m_gradientType == GradientType::RadialCorner)
            {
                // // Radial, non rounded
                if (style.m_textureHandle == 0)
                {
                    Array<Vertex> vertices;
                    Array<Index>  indices;
                    Internal::FillConvex_RadialGra(vertices, indices, rotateAngle, points, size, avgCenter, style.m_color.m_start, style.m_color.m_end);
                    Backend::DrawGradient(vertices, indices, style.m_color.m_start, style.m_color.m_end, style.m_color.m_gradientType, style.m_color.m_radialSize);
                }
                else
                {
                    Array<Vertex> vertices;
                    Array<Index>  indices;
                    Internal::FillConvex_RadialGra(vertices, indices, rotateAngle, points, size, avgCenter, style.m_color.m_start, style.m_color.m_end);
                    Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
                }
            }
        }
    }

    void DrawCircleFilled(const Vec2& center, float radius, StyleOptions& style, int segments, float rotateAngle, float startAngle, float endAngle)
    {
        if (Math::IsEqual(style.m_color.m_start, style.m_color.m_end))
        {
            if (style.m_textureHandle == 0)
                Internal::FillCircle_SC(Internal::g_rendererData.m_defaultBuffer.m_vertexBuffer, Internal::g_rendererData.m_defaultBuffer.m_indexBuffer, rotateAngle, center, radius, segments, style.m_color.m_start, startAngle, endAngle);
            else
            {
                Array<Vertex> vertices;
                Array<Index>  indices;
                Internal::FillCircle_SC(vertices, indices, rotateAngle, center, radius, segments, style.m_color.m_start, startAngle, endAngle);
                Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
            }
        }
        else
        {
            if (style.m_color.m_gradientType == GradientType::Horizontal)
            {
                // Horizontal, non rounded
                if (style.m_textureHandle == 0)
                    Internal::FillCircle_VerHorGra(Internal::g_rendererData.m_defaultBuffer.m_vertexBuffer, Internal::g_rendererData.m_defaultBuffer.m_indexBuffer, rotateAngle, center, radius, segments, style.m_color.m_start, style.m_color.m_end, true, startAngle, endAngle);
                else
                {
                    Array<Vertex> vertices;
                    Array<Index>  indices;
                    Internal::FillCircle_VerHorGra(vertices, indices, rotateAngle, center, radius, segments, style.m_color.m_start, style.m_color.m_end, true, startAngle, endAngle);
                    Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
                }
            }
            else if (style.m_color.m_gradientType == GradientType::Vertical)
            {
                // Vertical, non rounded
                if (style.m_textureHandle == 0)
                    Internal::FillCircle_VerHorGra(Internal::g_rendererData.m_defaultBuffer.m_vertexBuffer, Internal::g_rendererData.m_defaultBuffer.m_indexBuffer, rotateAngle, center, radius, segments, style.m_color.m_start, style.m_color.m_end, false, startAngle, endAngle);
                else
                {
                    Array<Vertex> vertices;
                    Array<Index>  indices;
                    Internal::FillCircle_VerHorGra(vertices, indices, rotateAngle, center, radius, segments, style.m_color.m_start, style.m_color.m_end, false, startAngle, endAngle);
                    Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
                }
            }
            else if (style.m_color.m_gradientType == GradientType::Radial || style.m_color.m_gradientType == GradientType::RadialCorner)
            {
                // // Radial, non rounded
                if (style.m_textureHandle == 0)
                {
                    Array<Vertex> vertices;
                    Array<Index>  indices;
                    Internal::FillCircle_RadialGra(vertices, indices, rotateAngle, center, radius, segments, style.m_color.m_start, style.m_color.m_end, startAngle, endAngle);
                    Backend::DrawGradient(vertices, indices, style.m_color.m_start, style.m_color.m_end, style.m_color.m_gradientType, style.m_color.m_radialSize);
                }
                else
                {
                    Array<Vertex> vertices;
                    Array<Index>  indices;
                    Internal::FillCircle_RadialGra(vertices, indices, rotateAngle, center, radius, segments, style.m_color.m_start, style.m_color.m_end, startAngle, endAngle);
                    Backend::DrawTextured(vertices, indices, style.m_textureHandle, style.m_textureUVOffset, style.m_textureUVTiling);
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

    void GenerateLine(const Vec2& p1, const Vec2& p2, const Vec4Grad& color, ThicknessGrad thickness)
    {
        thickness.m_start *= Config.m_framebufferScale.x;
        thickness.m_end *= Config.m_framebufferScale.x;
        const float halfThickness1 = thickness.m_start / 2.0f;
        const float halfThickness2 = thickness.m_end / 2.0f;
        const Vec2  dir            = Math::Normalized(Vec2(p2.x - p1.x, p2.y - p1.y));
        const Vec2  up             = Math::Rotate90(dir, false);
        const Vec2  down           = Math::Rotate90(dir, true);
        // m_lineBuffer.push_back(Line());

        // Line* line                = m_lineBuffer.last();
        // line->m_vertices[0].m_pos = Vec2(p1.x + up.x * halfThickness1, p1.y + up.y * halfThickness1);
        // line->m_vertices[3].m_pos = Vec2(p1.x + down.x * halfThickness1, p1.y + down.y * halfThickness1);
        // line->m_vertices[1].m_pos = Vec2(p2.x + up.x * halfThickness2, p2.y + up.y * halfThickness2);
        // line->m_vertices[2].m_pos = Vec2(p2.x + down.x * halfThickness2, p2.y + down.y * halfThickness2);
        // line->m_vertices[0].m_col = color.m_start;
        // line->m_vertices[3].m_col = color.m_start;
        // line->m_vertices[1].m_col = color.m_end;
        // line->m_vertices[2].m_col = color.m_end;
        // line->m_center            = Vec2((p1.x + p2.x) / 2.0f, (p1.y + p2.y) / 2.0f);
    }

    void RotateVertices(Array<Vertex>& vertices, const Vec2& center, int startIndex, int endIndex, float angle)
    {
        for (int i = startIndex; i < endIndex + 1; i++)
        {
            vertices[i].m_pos = Math::RotateAround(vertices[i].m_pos, center, angle);
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

    void Internal::FillRect_NoRound_VerHorGra(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& colorTL, const Vec4& colorTR, const Vec4& colorBR, const Vec4& colorBL)
    {
        Vertex v[4];
        FillRectData(v, false, min, max);
        const int current = vertices.m_size;
        v[0].m_col        = colorTL;
        v[1].m_col        = colorTR;
        v[2].m_col        = colorBR;
        v[3].m_col        = colorBL;

        for (int i = 0; i < 4; i++)
            vertices.push_back(v[i]);

        RotateVertices(vertices, Vec2((min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f), current, current + 3, rotateAngle);

        indices.push_back(current);
        indices.push_back(current + 1);
        indices.push_back(current + 3);
        indices.push_back(current + 1);
        indices.push_back(current + 2);
        indices.push_back(current + 3);
    }

    void Internal::FillRect_NoRound_SC(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& color)
    {
        Vertex v[4];
        FillRectData(v, false, min, max);
        v[0].m_col = color;
        v[1].m_col = color;
        v[2].m_col = color;
        v[3].m_col = color;

        const int current = vertices.m_size;

        for (int i = 0; i < 4; i++)
            vertices.push_back(v[i]);

        RotateVertices(vertices, Vec2((min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f), current, current + 3, rotateAngle);

        indices.push_back(current);
        indices.push_back(current + 1);
        indices.push_back(current + 3);
        indices.push_back(current + 1);
        indices.push_back(current + 2);
        indices.push_back(current + 3);
    }

    void Internal::FillRect_NoRound_RadialGra(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& startColor, const Vec4& endColor)
    {
        Vertex v[5];
        FillRectData(v, true, min, max);
        int startIndex = vertices.m_size;

        for (int i = 0; i < 5; i++)
            vertices.push_back(v[i]);

        RotateVertices(vertices, Vec2((min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f), startIndex + 1, startIndex + 4, rotateAngle);
        ConvexFillVertices(startIndex, startIndex + 4, indices);
    }

    void Internal::FillRect_Round(Array<Vertex>& vertices, Array<Index>& indices, Array<int>& roundedCorners, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& col, float rounding)
    {
        rounding = Math::Clamp(rounding, 0.0f, 1.0f);

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
        const int   startIndex    = vertices.m_size;
        int         vertexCount   = 0;

        // Eventually we will be filling a convex shape, so we need a center vertex, it's position is already "center",
        // now calculate it's vertex color & push it down the pipeline.
        Vertex c;
        c.m_pos = center;
        c.m_col = col;
        c.m_uv  = Vec2(0.5f, 0.5f);
        vertices.push_back(c);

        for (int i = 0; i < 4; i++)
        {
            const int found = roundedCorners.find(i);
            if (roundedCorners.m_size != 0 && found == -1)
            {
                Vertex cornerVertex;
                cornerVertex.m_pos = v[i].m_pos;
                cornerVertex.m_col = col;
                cornerVertex.m_uv  = v[i].m_uv;
                vertices.push_back(cornerVertex);
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
                cornerVertex.m_pos  = p;
                cornerVertex.m_col  = col;
                cornerVertex.m_uv.x = Math::Remap(cornerVertex.m_pos.x, min.x, max.x, 0.0f, 1.0f);
                cornerVertex.m_uv.y = Math::Remap(cornerVertex.m_pos.y, min.y, max.y, 0.0f, 1.0f);
                vertices.push_back(cornerVertex);
                vertexCount++;
            }
            startAngle += 90.0f;
            endAngle += 90.0f;
        }

        // Fill vertices, don't forget to add the center too.
        RotateVertices(vertices, center, startIndex + 1, startIndex + vertexCount, rotateAngle);
        ConvexFillVertices(startIndex, startIndex + vertexCount, indices);
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

        v[i].m_pos       = min;
        v[i + 1].m_pos.x = max.x;
        v[i + 1].m_pos.y = min.y;
        v[i + 2].m_pos.x = max.x;
        v[i + 2].m_pos.y = max.y;
        v[i + 3].m_pos.x = min.x;
        v[i + 3].m_pos.y = max.y;
        v[i].m_uv        = Vec2(0.0f, 0.0f);
        v[i + 1].m_uv    = Vec2(1.0f, 0.0f);
        v[i + 2].m_uv    = Vec2(1.0f, 1.0f);
        v[i + 3].m_uv    = Vec2(0.0f, 1.0f);
    }

    void Internal::FillTri_NoRound_VerHorGra(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec4& colorLeft, const Vec4& colorRight, const Vec4& colorTop)
    {
        Vertex v[3];
        FillTriData(v, false, true, p1, p2, p3);
        v[0].m_col = colorLeft;
        v[1].m_col = colorRight;
        v[2].m_col = colorTop;

        const int start = vertices.m_size;

        for (int i = 0; i < 3; i++)
            vertices.push_back(v[i]);

        RotateVertices(vertices, Vec2((p1.x + p2.x + p3.x) / 3.0f, (p1.y + p2.y + p3.y) / 3.0f), start, start + 2, rotateAngle);

        indices.push_back(start);
        indices.push_back(start + 1);
        indices.push_back(start + 2);
    }

    void Internal::FillTri_NoRound_SC(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec4& color)
    {
        Vertex v[3];
        FillTriData(v, false, true, p1, p2, p3);
        v[0].m_col = color;
        v[1].m_col = color;
        v[2].m_col = color;

        const int start = vertices.m_size;

        for (int i = 0; i < 3; i++)
            vertices.push_back(v[i]);

        RotateVertices(vertices, Vec2((p1.x + p2.x + p3.x) / 3.0f, (p1.y + p2.y + p3.y) / 3.0f), start, start + 2, rotateAngle);

        indices.push_back(start);
        indices.push_back(start + 1);
        indices.push_back(start + 2);
    }

    void Internal::FillTri_NoRound_RadialGra(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec4& startcolor, const Vec4& endColor)
    {
        Vertex v[4];
        Vec2   points[3] = {p1, p2, p3};
        FillTriData(v, true, true, p1, p2, p3);
        int startIndex = vertices.m_size;

        for (int i = 0; i < 4; i++)
            vertices.push_back(v[i]);

        RotateVertices(vertices, Vec2((p1.x + p2.x + p3.x) / 3.0f, (p1.y + p2.y + p3.y) / 3.0f), startIndex + 1, startIndex + 3, rotateAngle);
        ConvexFillVertices(startIndex, startIndex + 3, indices);
    }

    void Internal::FillTri_Round(Array<Vertex>& vertices, Array<Index>& indices, Array<int>& onlyRoundCorners, float rotateAngle, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec4& col, float rounding)
    {
        rounding = Math::Clamp(rounding, 0.0f, 1.0f);

        Vertex v[3];
        FillTriData(v, false, false, p1, p2, p3);
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
        const int startIndex  = vertices.m_size;
        int       vertexCount = 0;

        // Eventually we will be filling a convex shape, so we need a center vertex, it's position is already "center",
        // now calculate it's vertex color & push it down the pipeline.
        Vertex c;
        c.m_pos  = center;
        c.m_col  = col;
        c.m_uv.x = Math::Remap(c.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
        c.m_uv.y = Math::Remap(c.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
        vertices.push_back(c);
        const float angleOffset = maxAngle > 90.0f ? maxAngle - 90.0f : 45.0f;

        for (int i = 2; i > -1; i--)
        {

            if (onlyRoundCorners.m_size != 0 && onlyRoundCorners.find(i) == -1)
            {
                Vertex cornerVertex;
                cornerVertex.m_col = col;
                cornerVertex.m_pos = v[i].m_pos;
                vertexCount++;
                cornerVertex.m_uv.x = Math::Remap(cornerVertex.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
                cornerVertex.m_uv.y = Math::Remap(cornerVertex.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
                vertices.push_back(cornerVertex);
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

                    const Vec2 toCenter       = Math::Abs(Vec2(center.x - cornerVertex.m_pos.x, center.y - cornerVertex.m_pos.y));
                    const Vec2 cornerToCenter = Math::Abs(Vec2(center.x - v[i].m_pos.x, center.y - v[i].m_pos.y));
                    cornerVertex.m_uv.x       = Math::Remap(cornerVertex.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
                    cornerVertex.m_uv.y       = Math::Remap(cornerVertex.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
                    vertices.push_back(cornerVertex);
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
                    cornerVertex.m_col        = col;
                    cornerVertex.m_pos        = arc[j];
                    const Vec2 toCenter       = Math::Abs(Vec2(center.x - cornerVertex.m_pos.x, center.y - cornerVertex.m_pos.y));
                    const Vec2 cornerToCenter = Math::Abs(Vec2(center.x - v[i].m_pos.x, center.y - v[i].m_pos.y));
                    cornerVertex.m_uv.x       = Math::Remap(cornerVertex.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
                    cornerVertex.m_uv.y       = Math::Remap(cornerVertex.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);

                    vertices.push_back(cornerVertex);
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
                    cornerVertex.m_col        = col;
                    cornerVertex.m_pos        = arc[j];
                    Vec2       toCenter       = Vec2(center.x - cornerVertex.m_pos.x, center.y - cornerVertex.m_pos.y);
                    const bool xPositive      = toCenter.x > 0.0f;
                    toCenter                  = Math::Abs(toCenter);
                    const Vec2 cornerToCenter = Math::Abs(Vec2(center.x - v[i].m_pos.x, center.y - v[i].m_pos.y));
                    cornerVertex.m_uv.x       = Math::Remap(cornerVertex.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
                    cornerVertex.m_uv.y       = Math::Remap(cornerVertex.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);

                    vertices.push_back(cornerVertex);
                    vertexCount++;
                }
            }
        }

        // Fill vertices
        RotateVertices(vertices, center, startIndex + 1, startIndex + vertexCount, rotateAngle);
        ConvexFillVertices(startIndex, startIndex + vertexCount, indices);
    }

    void Internal::FillTriData(Vertex* v, bool hasCenter, bool calculateUV, const Vec2& p1, const Vec2& p2, const Vec2& p3)
    {
        const int i         = hasCenter ? 1 : 0;
        Vec2      points[3] = {p1, p2, p3};
        if (hasCenter)
        {
            Vec2 center = Vec2((p1.x + p2.x + p3.x) / 3.0f, (p1.y + p2.y + p3.y) / 3.0f);
            v[0].m_pos  = center;

            const Vec2  otherEdge   = Vec2((p2.x + p1.x) / 2.0f, (p2.y + p1.y) / 2.0f);
            const float h           = Math::Mag(Vec2(p3.x - otherEdge.x, p3.y - otherEdge.y));
            const float centerToTop = Math::Mag(Vec2(p3.x - center.x, p3.y - center.y));
            const float uvY         = Math::Remap(centerToTop, 0.0f, h, 0.0f, 1.0f);
        }

        v[i].m_pos     = p1;
        v[i + 1].m_pos = p2;
        v[i + 2].m_pos = p3;

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

    void Internal::FillNGon_SC(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& center, float radius, int n, const Vec4& color)
    {
        Array<Vertex> v;
        FillNGonData(v, true, center, radius, n);

        const int start = vertices.m_size;

        for (int i = 0; i < v.m_size; i++)
        {
            v[i].m_col = color;
            vertices.push_back(v[i]);
        }

        RotateVertices(vertices, center, start + 1, start + n, rotateAngle);
        ConvexFillVertices(start, start + n, indices);
    }

    void Internal::FillNGon_VerHorGra(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& center, float radius, int n, const Vec4& colorStart, const Vec4& colorEnd, bool isHor)
    {
        Array<Vertex> v;
        FillNGonData(v, true, center, radius, n);

        const int start = vertices.m_size;

        for (int i = 0; i < v.m_size; i++)
        {
            v[i].m_col = Math::Lerp(colorStart, colorEnd, isHor ? v[i].m_uv.x : v[i].m_uv.y);
            vertices.push_back(v[i]);
        }

        RotateVertices(vertices, center, start + 1, start + n, rotateAngle);
        ConvexFillVertices(start, start + n, indices);
    }

    void Internal::FillNGon_RadialGra(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& center, float radius, int n, const Vec4& colorStart, const Vec4& colorEnd)
    {
        Array<Vertex> v;
        FillNGonData(v, true, center, radius, n);

        const int start = vertices.m_size;

        for (int i = 0; i < v.m_size; i++)
        {
            vertices.push_back(v[i]);
        }

        RotateVertices(vertices, center, start + 1, start + n, rotateAngle);
        ConvexFillVertices(start, start + n, indices);
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

        for (float i = 0.0f; i < 360.0f; i += angleIncrease)
        {
            Vertex v;
            v.m_pos  = Math::GetPointOnCircle(center, radius, i);
            v.m_uv.x = Math::Remap(v.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            v.m_uv.y = Math::Remap(v.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            vertArray.push_back(v);
        }
    }

    void Internal::FillCircle_SC(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& center, float radius, int segments, const Vec4& color, float startAngle, float endAngle)
    {
        Array<Vertex> v;
        FillCircleData(v, true, center, radius, segments, startAngle, endAngle);

        const int start = vertices.m_size;

        for (int i = 0; i < v.m_size; i++)
        {
            v[i].m_col = color;
            vertices.push_back(v[i]);
        }

        RotateVertices(vertices, center, start + 1, start + v.m_size - 1, rotateAngle);
        ConvexFillVertices(start, start + v.m_size - 1, indices, Math::Abs(endAngle - startAngle) != 360.0f);
    }

    void Internal::FillCircle_VerHorGra(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& center, float radius, int segments, const Vec4& colorStart, const Vec4& colorEnd, bool isHor, float startAngle, float endAngle)
    {
        Array<Vertex> v;
        FillCircleData(v, true, center, radius, segments, startAngle, endAngle);

        const int start = vertices.m_size;

        for (int i = 0; i < v.m_size; i++)
        {
            v[i].m_col = Math::Lerp(colorStart, colorEnd, isHor ? v[i].m_uv.x : v[i].m_uv.y);
            vertices.push_back(v[i]);
        }

        RotateVertices(vertices, center, start + 1, start + v.m_size - 1, rotateAngle);
        ConvexFillVertices(start, start + v.m_size - 1, indices, Math::Abs(endAngle - startAngle) != 360.0f);
    }

    void Internal::FillCircle_RadialGra(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, const Vec2& center, float radius, int segments, const Vec4& colorStart, const Vec4& colorEnd, float startAngle, float endAngle)
    {
        Array<Vertex> v;
        FillCircleData(v, true, center, radius, segments, startAngle, endAngle);

        const int start = vertices.m_size;

        for (int i = 0; i < v.m_size; i++)
        {
            vertices.push_back(v[i]);
        }

        RotateVertices(vertices, center, start + 1, start + v.m_size - 1, rotateAngle);
        ConvexFillVertices(start, start + v.m_size - 1, indices, Math::Abs(endAngle - startAngle) != 360.0f);
    }

    void Internal::FillCircleData(Array<Vertex>& vertices, bool hasCenter, const Vec2& center, float radius, int segments, float startAngle, float endAngle)
    {
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
        for (float i = startAngle; i <= endAngle; i += angleIncrease)
        {
            Vertex v;
            v.m_pos  = Math::GetPointOnCircle(center, radius, i);
            v.m_uv.x = Math::Remap(v.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            v.m_uv.y = Math::Remap(v.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            vertices.push_back(v);
        }
    }

    void Internal::FillConvex_SC(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, Vec2* points, int size, const Vec2& center, const Vec4& color)
    {
        const int start = vertices.m_size;
        Vec2      bbMin, bbMax;
        GetConvexBoundingBox(points, size, bbMin, bbMax);

        Vertex c;
        c.m_pos  = center;
        c.m_uv.x = Math::Remap(c.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
        c.m_uv.y = Math::Remap(c.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
        c.m_col  = color;
        vertices.push_back(c);

        for (int i = 0; i < size; i++)
        {
            Vertex v;
            v.m_pos             = points[i];
            const Vec2 toCenter = Math::Normalized(Vec2(center.x - v.m_pos.x, center.y - v.m_pos.y));
            v.m_uv.x            = Math::Remap(v.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            v.m_uv.y            = Math::Remap(v.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            v.m_col             = color;
            vertices.push_back(v);
        }

        RotateVertices(vertices, center, start + 1, start + size, rotateAngle);
        ConvexFillVertices(start, start + size, indices);
    }

    void Internal::FillConvex_VerHorGra(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, Vec2* points, int size, const Vec2& center, const Vec4& colorStart, const Vec4& colorEnd, bool isHor)
    {
        const int start = vertices.m_size;
        Vec2      bbMin, bbMax;
        GetConvexBoundingBox(points, size, bbMin, bbMax);

        Vertex c;
        c.m_pos  = center;
        c.m_col  = Math::Lerp(colorStart, colorEnd, 0.5f);
        c.m_uv.x = Math::Remap(c.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
        c.m_uv.y = Math::Remap(c.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
        vertices.push_back(c);

        for (int i = 0; i < size; i++)
        {
            Vertex v;
            v.m_pos             = points[i];
            const Vec2 toCenter = Math::Normalized(Vec2(center.x - v.m_pos.x, center.y - v.m_pos.y));
            v.m_uv.x            = Math::Remap(v.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            v.m_uv.y            = Math::Remap(v.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            v.m_col             = Math::Lerp(colorStart, colorEnd, isHor ? v.m_uv.x : v.m_uv.y);
            vertices.push_back(v);
        }

        RotateVertices(vertices, center, start + 1, start + size, rotateAngle);
        ConvexFillVertices(start, start + size, indices);
    }

    void Internal::FillConvex_RadialGra(Array<Vertex>& vertices, Array<Index>& indices, float rotateAngle, Vec2* points, int size, const Vec2& center, const Vec4& colorStart, const Vec4& colorEnd)
    {
        const int start = vertices.m_size;

        Vec2 bbMin, bbMax;
        GetConvexBoundingBox(points, size, bbMin, bbMax);

        Vertex c;
        c.m_pos  = center;
        c.m_uv.x = Math::Remap(c.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
        c.m_uv.y = Math::Remap(c.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
        vertices.push_back(c);

        for (int i = 0; i < size; i++)
        {
            Vertex v;
            v.m_pos             = points[i];
            const Vec2 toCenter = Math::Normalized(Vec2(center.x - v.m_pos.x, center.y - v.m_pos.y));
            v.m_uv.x            = Math::Remap(v.m_pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
            v.m_uv.y            = Math::Remap(v.m_pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
            vertices.push_back(v);
        }

        RotateVertices(vertices, center, start + 1, start + size, rotateAngle);
        ConvexFillVertices(start, start + size, indices);
    }

    Vec2 Internal::GetPolygonCentroid(Vec2* vertices, int vertexCount)
    {
        Vec2   centroid   = {0, 0};
        double signedArea = 0.0;
        double x0         = 0.0; // Current vertex X
        double y0         = 0.0; // Current vertex Y
        double x1         = 0.0; // Next vertex X
        double y1         = 0.0; // Next vertex Y
        double a          = 0.0; // Partial signed area

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

        signedArea *= 0.5;
        centroid.x /= (6.0 * signedArea);
        centroid.y /= (6.0 * signedArea);

        return centroid;
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

} // namespace Lina2D