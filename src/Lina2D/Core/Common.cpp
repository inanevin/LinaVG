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
    GradientDrawBuffer& RendererData::GetGradientBuffer(Vec4Grad& grad)
    {
        for (int i = 0; i < m_gradientBuffers.m_size; i++)
        {
            auto& buf = m_gradientBuffers[i];
            if (Math::IsEqual(buf.m_color.m_start, grad.m_start) && Math::IsEqual(buf.m_color.m_end, grad.m_end) && buf.m_color.m_gradientType == grad.m_gradientType)
            {
                if (grad.m_gradientType == GradientType::Radial || grad.m_gradientType == GradientType::RadialCorner)
                {
                    if (buf.m_color.m_radialSize == grad.m_radialSize)
                        return m_gradientBuffers[i];
                }
                else
                    return m_gradientBuffers[i];
            }
        }

        std::cout << "added" << std::endl;
        m_gradientBuffers.push_back(GradientDrawBuffer(grad));
        return m_gradientBuffers.last_ref();
    }

    TextureDrawBuffer& RendererData::GetTextureBuffer(BackendHandle textureHandle, const Vec2& tiling, const Vec2& uvOffset)
    {
        for (int i = 0; i < m_textureBuffers.m_size; i++)
        {
            auto& buf = m_textureBuffers[i];
            if (buf.m_textureHandle == textureHandle && Math::IsEqual(buf.m_textureUVTiling, tiling) && Math::IsEqual(buf.m_textureUVOffset, uvOffset))
                return m_textureBuffers[i];
        }

        m_textureBuffers.push_back(TextureDrawBuffer(textureHandle, tiling, uvOffset));
        return m_textureBuffers.last_ref();
    }

} // namespace Lina2D