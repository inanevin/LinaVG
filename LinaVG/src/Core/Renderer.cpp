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

#include "Core/Renderer.hpp"
#include "Core/Backend.hpp"
#include "Core/Drawer.hpp"
#include "Core/Math.hpp"
#include <math.h>

namespace LinaVG
{
    namespace Internal
    {
        RendererData g_rendererData;
    }

    Configuration Config;

    void Initialize()
    {
        Internal::g_rendererData.m_defaultBuffers.reserve(Config.m_defaultBufferReserve);
        Internal::g_rendererData.m_gradientBuffers.reserve(Config.m_gradientBufferReserve);
        Internal::g_rendererData.m_textureBuffers.reserve(Config.m_textureBufferReserve);
        Backend::Initialize();

        // TODO - error check
        Config.m_logCallback("LinaVG: Renderer and Backend initialized successfuly.");
    }

    void Terminate()
    {
        Backend::Terminate();

        // TODO - error check
        Config.m_logCallback("LinaVG: Renderer and Backend terminated successfuly.");
    }

    void StartFrame()
    {
        Backend::StartFrame();
    }

    void Render()
    {
        const int  diff  = Internal::g_rendererData.m_maxDrawOrder - Internal::g_rendererData.m_minDrawOrder;
        const bool valid = !(diff == 0 || diff < 0);

        const int start = valid ? Internal::g_rendererData.m_maxDrawOrder : 0;
        const int end   = valid ? Internal::g_rendererData.m_minDrawOrder - 1 : -1;

        auto drawBuffers = [valid, start, end](DrawBufferShapeType shapeType) {
            for (int k = start; k > end; k--)
            {
                for (int i = 0; i < Internal::g_rendererData.m_defaultBuffers.m_size; i++)
                {
                    DrawBuffer& buf = Internal::g_rendererData.m_defaultBuffers[i];

                    if (buf.m_shapeType == shapeType && buf.m_drawOrder == k)
                        Backend::DrawDefault(&(buf));
                }

                for (int i = 0; i < Internal::g_rendererData.m_gradientBuffers.m_size; i++)
                {
                    GradientDrawBuffer& buf = Internal::g_rendererData.m_gradientBuffers[i];

                    if (buf.m_shapeType == shapeType && buf.m_drawOrder == k)
                        Backend::DrawGradient(&buf);
                }

                for (int i = 0; i < Internal::g_rendererData.m_textureBuffers.m_size; i++)
                {
                    TextureDrawBuffer& buf = Internal::g_rendererData.m_textureBuffers[i];

                    if (buf.m_shapeType == shapeType && buf.m_drawOrder == k)
                        Backend::DrawTextured(&buf);
                }
            }
        };

        drawBuffers(DrawBufferShapeType::Shape);
        drawBuffers(DrawBufferShapeType::Outline);
        drawBuffers(DrawBufferShapeType::AA);
    }

    void EndFrame()
    {
        Backend::EndFrame();

        Internal::g_rendererData.m_gcFrameCounter++;
        Internal::g_rendererData.m_minDrawOrder = -1;
        Internal::g_rendererData.m_maxDrawOrder = -1;

        if (Internal::g_rendererData.m_gcFrameCounter > Config.m_gcCollectInterval)
        {
            Internal::g_rendererData.m_gcFrameCounter = 0;
            for (int i = 0; i < Internal::g_rendererData.m_gradientBuffers.m_size; i++)
                Internal::g_rendererData.m_gradientBuffers[i].Clear();

            Internal::g_rendererData.m_gradientBuffers.clear();

            for (int i = 0; i < Internal::g_rendererData.m_textureBuffers.m_size; i++)
                Internal::g_rendererData.m_textureBuffers[i].Clear();

            Internal::g_rendererData.m_textureBuffers.clear();

            for (int i = 0; i < Internal::g_rendererData.m_defaultBuffers.m_size; i++)
                Internal::g_rendererData.m_defaultBuffers[i].Clear();

            Internal::g_rendererData.m_defaultBuffers.clear();
        }
        else
        {
            for (int i = 0; i < Internal::g_rendererData.m_gradientBuffers.m_size; i++)
                Internal::g_rendererData.m_gradientBuffers[i].ResizeZero();

            Internal::g_rendererData.m_gradientBuffers.resize(0);

            for (int i = 0; i < Internal::g_rendererData.m_textureBuffers.m_size; i++)
                Internal::g_rendererData.m_textureBuffers[i].ResizeZero();

            Internal::g_rendererData.m_textureBuffers.resize(0);

            for (int i = 0; i < Internal::g_rendererData.m_defaultBuffers.m_size; i++)
                Internal::g_rendererData.m_defaultBuffers[i].ResizeZero();

            Internal::g_rendererData.m_defaultBuffers.resize(0);
        }
    }

    GradientDrawBuffer& RendererData::GetGradientBuffer(Vec4Grad& grad, int drawOrder, DrawBufferShapeType shapeType)
    {
        const bool isAABuffer = shapeType == DrawBufferShapeType::AA;

        for (int i = 0; i < m_gradientBuffers.m_size; i++)
        {
            auto& buf = m_gradientBuffers[i];
            if (buf.m_drawOrder == drawOrder && Math::IsEqual(buf.m_color.m_start, grad.m_start) && Math::IsEqual(buf.m_color.m_end, grad.m_end) && buf.m_color.m_gradientType == grad.m_gradientType)
            {
                if (grad.m_gradientType == GradientType::Radial || grad.m_gradientType == GradientType::RadialCorner)
                {
                    if (buf.m_color.m_radialSize == grad.m_radialSize && buf.m_isAABuffer == isAABuffer)
                        return m_gradientBuffers[i];
                }
                else
                {
                    if (buf.m_isAABuffer == isAABuffer)
                        return m_gradientBuffers[i];
                }
            }
        }

        SetDrawOrderLimits(drawOrder);

        m_gradientBuffers.push_back(GradientDrawBuffer(grad, drawOrder, shapeType));
        return m_gradientBuffers.last_ref();
    }

    DrawBuffer& RendererData::GetDefaultBuffer(int drawOrder, DrawBufferShapeType shapeType)
    {
        for (int i = 0; i < m_defaultBuffers.m_size; i++)
        {
            auto& buf = m_defaultBuffers[i];
            if (m_defaultBuffers[i].m_drawOrder == drawOrder)
                return m_defaultBuffers[i];
        }

        SetDrawOrderLimits(drawOrder);

        m_defaultBuffers.push_back(DrawBuffer(drawOrder, DrawBufferType::Default, shapeType));
        return m_defaultBuffers.last_ref();
    }

    TextureDrawBuffer& RendererData::GetTextureBuffer(BackendHandle textureHandle, const Vec2& tiling, const Vec2& uvOffset, int drawOrder, DrawBufferShapeType shapeType)
    {
        const bool isAABuffer = shapeType == DrawBufferShapeType::AA;
        for (int i = 0; i < m_textureBuffers.m_size; i++)
        {
            auto& buf = m_textureBuffers[i];
            if (buf.m_drawOrder == drawOrder && buf.m_textureHandle == textureHandle && Math::IsEqual(buf.m_textureUVTiling, tiling) && Math::IsEqual(buf.m_textureUVOffset, uvOffset) && buf.m_isAABuffer == isAABuffer)
                return m_textureBuffers[i];
        }

        SetDrawOrderLimits(drawOrder);

        m_textureBuffers.push_back(TextureDrawBuffer(textureHandle, tiling, uvOffset, drawOrder, shapeType));
        return m_textureBuffers.last_ref();
    }

    int RendererData::GetBufferIndexInDefaultArray(DrawBuffer* buf)
    {
        for (int i = 0; i < m_defaultBuffers.m_size; i++)
        {
            if (buf == &m_defaultBuffers[i])
                return i;
        }
        return -1;
    }

    int RendererData::GetBufferIndexInGradientArray(DrawBuffer* buf)
    {
        for (int i = 0; i < m_gradientBuffers.m_size; i++)
        {
            if (buf == &m_gradientBuffers[i])
                return i;
        }
        return -1;
    }

    int RendererData::GetBufferIndexInTextureArray(DrawBuffer* buf)
    {
        for (int i = 0; i < m_textureBuffers.m_size; i++)
        {
            if (buf == &m_textureBuffers[i])
                return i;
        }
        return -1;
    }

    void RendererData::SetDrawOrderLimits(int drawOrder)
    {
        if (m_minDrawOrder == -1 || drawOrder < m_minDrawOrder)
            m_minDrawOrder = drawOrder;

        if (m_maxDrawOrder == -1 || drawOrder > m_maxDrawOrder)
            m_maxDrawOrder = drawOrder;
    }

} // namespace LinaVG