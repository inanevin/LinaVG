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
#include <iostream>
#include <stdio.h>

namespace Lina2D
{
    void Drawer::DrawArc(const Vec2& p1, const Vec2& p2, const Vec4Grad& color, float radius, ThicknessGrad thickness, int segments, bool flip)
    {
        CheckStartLineJoint();

        const float   halfMag              = Math::Mag(Vec2(p2.x - p1.x, p2.y - p1.y)) / 2.0f;
        const Vec2 center               = Vec2((p1.x + p2.x) / 2.0f, (p1.y + p2.y) / 2.0f);
        const Vec2 dir                  = Vec2(p2.x - p1.x, p2.y - p1.y);
        float         angle1               = Math::GetAngleFromCenter(center, p1);
        float         angle2               = Math::GetAngleFromCenter(center, p2);
        const bool    useGradientColor     = !Math::IsEqual(color.m_start, color.m_end);
        const float   useGradientThickness = thickness.m_start != thickness.m_end;

        Vec2 lastPoint = p1;

        if (angle2 < angle1)
            angle2 += 360.0f;

        const float midAngle      = (angle2 + angle1) / 2.0f;
        const float angleIncrease = (segments >= 180.0f || segments < 0.0f) ? 1.0f : 180.0f / (float)segments;

        Vec4 startColor     = color.m_start;
        Vec4 endColor       = color.m_end;
        float   startThickness = thickness.m_start;
        float   endThickness   = thickness.m_end;
        for (float i = angle1 + angleIncrease; i < angle2; i += angleIncrease)
        {
            Vec2 p = Vec2(0, 0);

            if (radius == 0.0f)
                p = Math::GetPointOnSphere(center, halfMag, i);
            else
            {
                const Vec2 out = Math::Normalized(Math::Rotate90(dir, !flip));
                p                 = Math::SampleParabola(p1, p2, out, radius, Math::Remap(i, angle1, angle2, 0.0f, 1.0f));
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

        ResetLineJointData();
    }

    void Drawer::DrawBezier(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec4Grad& color, ThicknessGrad thickness, int segments)
    {
        CheckStartLineJoint();
        const bool  useGradientColor     = !Math::IsEqual(color.m_start, color.m_end);
        const float useGradientThickness = thickness.m_start != thickness.m_end;

        if (segments < 5)
            segments = 5;

        Vec2 lastPoint      = p0;
        Vec4 startColor     = color.m_start;
        Vec4 endColor       = color.m_end;
        float   startThickness = thickness.m_start;
        float   endThickness   = thickness.m_end;
        for (int i = 1; i < segments; i++)
        {
            float         t = Math::Remap((float)i, 0.0f, (float)segments, 0.0f, 1.0f);
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

        ResetLineJointData();
    }

    void Drawer::ResetLineJointData()
    {
        m_isJoiningLines = false;
        m_lastLineVtx1   = nullptr;
        m_lastLineVtx2   = nullptr;
    }

    void Drawer::SetLineJointType(JointType type)
    {
        Renderer::g_renderer->m_options.m_lineJointType = type;
    }

    void Drawer::SetFeatheringType(FeatherType type)
    {
        Renderer::g_renderer->m_options.m_featheringType = type;
    }

    void Drawer::SetFeatheringDistance(float distance)
    {
        Renderer::g_renderer->m_options.m_featheringDistance = distance;
    }

    void Drawer::CheckStartLineJoint()
    {
        if (Renderer::g_renderer->m_options.m_lineJointType != JointType::None)
        {
            m_lastLineVtx1   = nullptr;
            m_lastLineVtx2   = nullptr;
            m_isJoiningLines = true;
        }
    }

    void Drawer::DrawLine(const Vec2& p1, const Vec2& p2, const Vec4Grad& color, ThicknessGrad thickness)
    {
        thickness.m_start *= Renderer::g_renderer->m_options.m_framebufferScale.x;
        thickness.m_end *= Renderer::g_renderer->m_options.m_framebufferScale.x;
        const float   halfThickness1 = thickness.m_start / 2.0f;
        const float   halfThickness2 = thickness.m_end / 2.0f;
        const Vec2 dir            = Math::Normalized(Vec2(p2.x - p1.x, p2.y - p1.y));
        const Vec2 up             = Math::Rotate90(dir, false);
        const Vec2 down           = Math::Rotate90(dir, true);
        Vertex     p1U, p1D, p2U, p2D;
        p1U.m_pos = Vec2(p1.x + up.x * halfThickness1, p1.y + up.y * halfThickness1);
        p1D.m_pos = Vec2(p1.x + down.x * halfThickness1, p1.y + down.y * halfThickness1);
        p2U.m_pos = Vec2(p2.x + up.x * halfThickness2, p2.y + up.y * halfThickness2);
        p2D.m_pos = Vec2(p2.x + down.x * halfThickness2, p2.y + down.y * halfThickness2);
        p1U.m_col = color.m_start;
        p1D.m_col = color.m_start;
        p2U.m_col = color.m_end;
        p2D.m_col = color.m_end;

        if (m_isJoiningLines && m_lastLineVtx1 != nullptr)
        {
            if (Renderer::g_renderer->m_options.m_lineJointType == JointType::VertexAvg)
            {
                const Vec2 avgU    = Vec2((m_lastLineVtx1->m_pos.x + p1U.m_pos.x) / 2.0f, (m_lastLineVtx1->m_pos.y + p1U.m_pos.y) / 2.0f);
                const Vec2 avgD    = Vec2((m_lastLineVtx2->m_pos.x + p1D.m_pos.x) / 2.0f, (m_lastLineVtx2->m_pos.y + p1D.m_pos.y) / 2.0f);
                m_lastLineVtx1->m_pos = avgU;
                m_lastLineVtx2->m_pos = avgD;
                p1U.m_pos             = avgU;
                p1D.m_pos             = avgD;
            }
            else if (Renderer::g_renderer->m_options.m_lineJointType == JointType::Bevel)
            {
                const float angle = Math::GetAngleBetween(m_lastLineDir, down);

                if (angle < 90.0f)
                {
                    // going downwards, join down.
                    const Vec2 newPos = m_lastLineVtx1->m_pos;
                    const Vec2 diff   = Vec2(newPos.x - p1U.m_pos.x, newPos.y - p1U.m_pos.y);
                    p1U.m_pos            = newPos;
                    p1D.m_pos            = Vec2(p1D.m_pos.x + diff.x, p1D.m_pos.y + diff.y);
                    Renderer::g_renderer->m_drawData.m_indexBuffer.push_back(Renderer::g_renderer->m_drawData.m_indexCounter - 2);
                    Renderer::g_renderer->m_drawData.m_indexBuffer.push_back(Renderer::g_renderer->m_drawData.m_indexCounter - 3);
                    Renderer::g_renderer->m_drawData.m_indexBuffer.push_back(Renderer::g_renderer->m_drawData.m_indexCounter + 3);
                }
                else if (angle > 90.0f)
                {
                    const Vec2 newPos = m_lastLineVtx2->m_pos;
                    const Vec2 diff   = Vec2(newPos.x - p1D.m_pos.x, newPos.y - p1D.m_pos.y);
                    p1D.m_pos            = newPos;
                    p1U.m_pos            = Vec2(p1U.m_pos.x + diff.x, p1U.m_pos.y + diff.y);
                    Renderer::g_renderer->m_drawData.m_indexBuffer.push_back(Renderer::g_renderer->m_drawData.m_indexCounter - 2);
                    Renderer::g_renderer->m_drawData.m_indexBuffer.push_back(Renderer::g_renderer->m_drawData.m_indexCounter - 3);
                    Renderer::g_renderer->m_drawData.m_indexBuffer.push_back(Renderer::g_renderer->m_drawData.m_indexCounter);
                }
            }
        }

        m_lastLineDir = dir;

        Renderer::g_renderer->m_drawData.m_vertexBuffer.push_back(p1U);
        Renderer::g_renderer->m_drawData.m_vertexBuffer.push_back(p2U);
        Renderer::g_renderer->m_drawData.m_vertexBuffer.push_back(p2D);
        Renderer::g_renderer->m_drawData.m_vertexBuffer.push_back(p1D);
        m_lastLineVtx1 = &Renderer::g_renderer->m_drawData.m_vertexBuffer.m_data[Renderer::g_renderer->m_drawData.m_vertexBuffer.m_size - 3];
        m_lastLineVtx2 = &Renderer::g_renderer->m_drawData.m_vertexBuffer.m_data[Renderer::g_renderer->m_drawData.m_vertexBuffer.m_size - 2];

        const Index curr = Renderer::g_renderer->m_drawData.m_indexCounter;
        Renderer::g_renderer->m_drawData.m_indexBuffer.push_back(curr);
        Renderer::g_renderer->m_drawData.m_indexBuffer.push_back(curr + 1);
        Renderer::g_renderer->m_drawData.m_indexBuffer.push_back(curr + 3);
        Renderer::g_renderer->m_drawData.m_indexBuffer.push_back(curr + 1);
        Renderer::g_renderer->m_drawData.m_indexBuffer.push_back(curr + 2);
        Renderer::g_renderer->m_drawData.m_indexBuffer.push_back(curr + 3);
        Renderer::g_renderer->m_drawData.m_indexCounter += 4;

        if (Renderer::g_renderer->m_options.m_featheringType == FeatherType::Light)
        {
            const float    distance    = Renderer::g_renderer->m_options.m_featheringDistance;
            const Index featherCurr = Renderer::g_renderer->m_drawData.m_indexCounter;

            Vertex p1UU, p2UU, p1DD, p2DD;
            p1UU.m_pos.x = p1U.m_pos.x + up.x * distance;
            p1UU.m_pos.y = p1U.m_pos.y + up.y * distance;
            p2UU.m_pos.x = p2U.m_pos.x + up.x * distance;
            p2UU.m_pos.y = p2U.m_pos.y + up.y * distance;
            p1DD.m_pos.x = p1D.m_pos.x + down.x * distance;
            p1DD.m_pos.y = p1D.m_pos.y + down.y * distance;
            p2DD.m_pos.x = p2D.m_pos.x + down.x * distance;
            p2DD.m_pos.y = p2D.m_pos.y + down.y * distance;
            p1UU.m_col   = Vec4(p1U.m_col.x, p1U.m_col.y, p1U.m_col.z, 1.0f);
            p2UU.m_col   = Vec4(p2U.m_col.x, p2U.m_col.y, p2U.m_col.z, 1.0f);
            p1DD.m_col   = Vec4(p1D.m_col.x, p1D.m_col.y, p1D.m_col.z, 1.0f);
            p2DD.m_col   = Vec4(p2D.m_col.x, p2D.m_col.y, p2D.m_col.z, 1.0f);
           //Renderer::g_renderer->m_drawData.m_vertexBuffer.push_back(p1UU);
           //Renderer::g_renderer->m_drawData.m_vertexBuffer.push_back(p2UU);
           //Renderer::g_renderer->m_drawData.m_vertexBuffer.push_back(p2DD);
           //Renderer::g_renderer->m_drawData.m_vertexBuffer.push_back(p1DD);

           // Renderer::g_renderer->m_drawData.m_indexBuffer.push_back(featherCurr);
           // Renderer::g_renderer->m_drawData.m_indexBuffer.push_back(featherCurr + 1);
           // Renderer::g_renderer->m_drawData.m_indexBuffer.push_back(featherCurr - 4);
           // Renderer::g_renderer->m_drawData.m_indexBuffer.push_back(featherCurr + 1);
           // Renderer::g_renderer->m_drawData.m_indexBuffer.push_back(featherCurr - 3);
           // Renderer::g_renderer->m_drawData.m_indexBuffer.push_back(featherCurr - 4);
           // Renderer::g_renderer->m_drawData.m_indexCounter += 3;

            // g_drawData.m_indexBuffer.push_back(curr - 1);
            //  g_drawData.m_indexBuffer.push_back(curr - 2);
            //  g_drawData.m_indexBuffer.push_back(curr + 3);
            //  g_drawData.m_indexBuffer.push_back(curr - 2);
            //  g_drawData.m_indexBuffer.push_back(curr + 2);
            //  g_drawData.m_indexBuffer.push_back(curr - 1);
            //  g_drawData.m_indexCounter += 12;
        }
    }

} // namespace Lina2D::Drawer