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

#include "Lina2D/Core/Renderer.hpp"
#include "Lina2D/Core/GLBackend.hpp"
#include "Lina2D/Core/Drawer.hpp"
#include "Lina2D/Core/Internal.hpp"
#include <math.h>

namespace Lina2D
{
    Configuration Config;

    void Initialize()
    {
        Backend::Initialize();
    }

    void Terminate()
    {
        Backend::Terminate();
    }

    void StartFrame()
    {
        Backend::StartFrame();
    }

    void Render()
    {
        const int  diff  = Internal::g_rendererData.m_maxDrawOrder - Internal::g_rendererData.m_minDrawOrder;
        const bool valid = !(diff == 0 || diff < 0);

        const int start = valid ? Internal::g_rendererData.m_maxDrawOrder  : 0;
        const int end   = valid ? Internal::g_rendererData.m_minDrawOrder - 1 : -1;

        for (int k = start; k > end; k--)
        {
            for (int i = 0; i < Internal::g_rendererData.m_defaultBuffers.m_size; i++)
            {
                DrawBuffer& buf = Internal::g_rendererData.m_defaultBuffers[i];

                if (buf.m_drawOrder == k)
                    Backend::DrawDefault(&(buf));
            }

            for (int i = 0; i < Internal::g_rendererData.m_gradientBuffers.m_size; i++)
            {
                GradientDrawBuffer& buf = Internal::g_rendererData.m_gradientBuffers[i];

                if (buf.m_drawOrder == k)
                    Backend::DrawGradient(&buf);
            }

            for (int i = 0; i < Internal::g_rendererData.m_textureBuffers.m_size; i++)
            {
                TextureDrawBuffer& buf = Internal::g_rendererData.m_textureBuffers[i];

                if (buf.m_drawOrder == k)
                    Backend::DrawTextured(&buf);
            }
        }
    }

    void EndFrame()
    {
        Backend::EndFrame();

        Internal::g_rendererData.m_gcFrameCounter++;
        Internal::g_rendererData.m_minDrawOrder = -1;
        Internal::g_rendererData.m_maxDrawOrder = -1;

        for (int i = 0; i < Internal::g_rendererData.m_gradientBuffers.m_size; i++)
            Internal::g_rendererData.m_gradientBuffers[i].Clear();

        Internal::g_rendererData.m_gradientBuffers.clear();

        for (int i = 0; i < Internal::g_rendererData.m_textureBuffers.m_size; i++)
            Internal::g_rendererData.m_textureBuffers[i].Clear();

        Internal::g_rendererData.m_textureBuffers.clear();

        for (int i = 0; i < Internal::g_rendererData.m_defaultBuffers.m_size; i++)
            Internal::g_rendererData.m_defaultBuffers[i].Clear();

        Internal::g_rendererData.m_defaultBuffers.clear();

        if (Internal::g_rendererData.m_gcFrameCounter > Config.m_gcCollectInterval)
        {
            // Internal::g_rendererData.m_defaultBuffer.Clear();

            // Internal::g_rendererData.m_gcFrameCounter = 0;
        }
        else
        {
            // Internal::g_rendererData.m_defaultBuffer.Clear();
            // m_defaultBuffer.ResizeZero();

            // for (int i = 0; i < Internal::g_rendererData.m_gradientBuffers.m_size; i++)
            // {
            //     DrawBufferGradient& buf = Internal::g_rendererData.m_gradientBuffers[i];
            //     buf.Clear();
            //     // buf.ResizeZero();
            // }
        }
    }

} // namespace Lina2D