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

#include "Lina2D/Core/Common.hpp"
#include "Lina2D/Core/Math.hpp"

namespace Lina2D
{
    GradientDrawBuffer& RendererData::GetGradientBuffer(Vec4Grad& grad, int drawOrder, bool isAABuffer)
    {
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

        m_gradientBuffers.push_back(GradientDrawBuffer(grad, drawOrder, isAABuffer));
        return m_gradientBuffers.last_ref();
    }

    DrawBuffer& RendererData::GetDefaultBuffer(int drawOrder)
    {
        for (int i = 0; i < m_defaultBuffers.m_size; i++)
        {
            auto& buf = m_defaultBuffers[i];
            if (m_defaultBuffers[i].m_drawOrder == drawOrder)
                return m_defaultBuffers[i];
        }

        SetDrawOrderLimits(drawOrder);

        m_defaultBuffers.push_back(DrawBuffer(drawOrder));
        return m_defaultBuffers.last_ref();
    }

    TextureDrawBuffer& RendererData::GetTextureBuffer(BackendHandle textureHandle, const Vec2& tiling, const Vec2& uvOffset, int drawOrder, bool isAABuffer)
    {
        for (int i = 0; i < m_textureBuffers.m_size; i++)
        {
            auto& buf = m_textureBuffers[i];
            if (buf.m_drawOrder == drawOrder && buf.m_textureHandle == textureHandle && Math::IsEqual(buf.m_textureUVTiling, tiling) && Math::IsEqual(buf.m_textureUVOffset, uvOffset) && buf.m_isAABuffer == isAABuffer)
                return m_textureBuffers[i];
        }

        SetDrawOrderLimits(drawOrder);

        m_textureBuffers.push_back(TextureDrawBuffer(textureHandle, tiling, uvOffset, drawOrder, isAABuffer));
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

    OutlineOptions OutlineOptions::FromStyle(const StyleOptions& opts, OutlineDrawDirection drawDir)
    {
        OutlineOptions o;
        o.m_color           = opts.m_color;
        o.m_textureHandle   = opts.m_textureHandle;
        o.m_textureUVOffset = opts.m_textureUVOffset;
        o.m_textureUVTiling = opts.m_textureUVTiling;
        o.m_drawDirection   = drawDir;
        return o;
    }

} // namespace Lina2D