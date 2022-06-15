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

#define _CRTDBG_MAP_ALLOC

#include <iostream>
#include <crtdbg.h>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

#include "Core/Renderer.hpp"
#include "Core/Backend.hpp"
#include "Core/Drawer.hpp"
#include "Core/Math.hpp"
#include "Core/Text.hpp"
#include "Utility/Utility.hpp"
#include <math.h>

namespace LinaVG
{
    namespace Internal
    {
        RendererData    g_rendererData;
        LINAVG_API void ClearAllBuffers()
        {

            for (int i = 0; i < Internal::g_rendererData.m_gradientBuffers.m_size; i++)
                Internal::g_rendererData.m_gradientBuffers[i].Clear();

            Internal::g_rendererData.m_gradientBuffers.clear();

            for (int i = 0; i < Internal::g_rendererData.m_textureBuffers.m_size; i++)
                Internal::g_rendererData.m_textureBuffers[i].Clear();

            Internal::g_rendererData.m_textureBuffers.clear();

            for (int i = 0; i < Internal::g_rendererData.m_defaultBuffers.m_size; i++)
                Internal::g_rendererData.m_defaultBuffers[i].Clear();

            Internal::g_rendererData.m_defaultBuffers.clear();

            for (int i = 0; i < Internal::g_rendererData.m_simpleTextBuffers.m_size; i++)
                Internal::g_rendererData.m_simpleTextBuffers[i].Clear();

            Internal::g_rendererData.m_simpleTextBuffers.clear();

            for (int i = 0; i < Internal::g_rendererData.m_sdfTextBuffers.m_size; i++)
                Internal::g_rendererData.m_sdfTextBuffers[i].Clear();

            Internal::g_rendererData.m_sdfTextBuffers.clear();
        }
    } // namespace Internal

    bool Initialize()
    {
        Internal::g_rendererData.m_defaultBuffers.reserve(Config.defaultBufferReserve);
        Internal::g_rendererData.m_gradientBuffers.reserve(Config.gradientBufferReserve);
        Internal::g_rendererData.m_textureBuffers.reserve(Config.textureBufferReserve);
        Internal::g_rendererData.m_simpleTextBuffers.reserve(Config.textBuffersReserve);
        Internal::g_rendererData.m_sdfTextBuffers.reserve(Config.textBuffersReserve);

        if (!Backend::Initialize())
        {
            if (Config.errorCallback)
                Config.errorCallback("LinaVG: Could not initialize! Error initializing backend.");
            return false;
        }

        if (!Text::Initialize())
        {
            if (Config.errorCallback)
                Config.errorCallback("LinaVG: Could not initialize! Error initializing text API.");
            return false;
        }

        // TODO - error check
        if (Config.logCallback)
            Config.logCallback("LinaVG: Renderer and Backend initialized successfuly.");
        return true;
    }

    void Terminate()
    {
        Backend::Terminate();
        Text::Terminate();
        Internal::ClearAllBuffers();

        // TODO - error check
        if (Config.logCallback)
            Config.logCallback("LinaVG: Renderer and Backend terminated successfuly.");
    }

    void StartFrame()
    {
        if (Internal::g_rendererData.m_frameStarted)
        {
            if (Config.errorCallback)
                Config.errorCallback("LinaVG: StartFrame was called, but EndFrame was skipped! Make sure you always call EndFrame() after calling StartFrame() for the second time!");
            _ASSERT(false);
        }

        Backend::StartFrame();
        Internal::g_rendererData.m_frameStarted = true;
    }

    void Render()
    {

        auto& renderBuffs = [](int drawOrder, DrawBufferShapeType shapeType) {
            for (int i = 0; i < Internal::g_rendererData.m_defaultBuffers.m_size; i++)
            {
                DrawBuffer& buf = Internal::g_rendererData.m_defaultBuffers[i];

                if (buf.m_drawOrder == drawOrder && buf.m_shapeType == shapeType)
                    Backend::DrawDefault(&(buf));
            }

            for (int i = 0; i < Internal::g_rendererData.m_gradientBuffers.m_size; i++)
            {
                GradientDrawBuffer& buf = Internal::g_rendererData.m_gradientBuffers[i];

                if (buf.m_drawOrder == drawOrder && buf.m_shapeType == shapeType)
                    Backend::DrawGradient(&buf);
            }

            for (int i = 0; i < Internal::g_rendererData.m_textureBuffers.m_size; i++)
            {
                TextureDrawBuffer& buf = Internal::g_rendererData.m_textureBuffers[i];

                if (buf.m_drawOrder == drawOrder && buf.m_shapeType == shapeType)
                    Backend::DrawTextured(&buf);
            }

            for (int i = 0; i < Internal::g_rendererData.m_simpleTextBuffers.m_size; i++)
            {
                SimpleTextDrawBuffer& buf = Internal::g_rendererData.m_simpleTextBuffers[i];

                if (buf.m_drawOrder == drawOrder && buf.m_shapeType == shapeType)
                    Backend::DrawSimpleText(&buf);
            }

            for (int i = 0; i < Internal::g_rendererData.m_sdfTextBuffers.m_size; i++)
            {
                SDFTextDrawBuffer& buf = Internal::g_rendererData.m_sdfTextBuffers[i];

                if (buf.m_drawOrder == drawOrder && buf.m_shapeType == shapeType)
                    Backend::DrawSDFText(&buf);
            }
        };

        auto& arr = Internal::g_rendererData.m_drawOrders;

        for (int i = 0; i < arr.m_size; i++)
        {
            const int drawOrder = arr[i];
            renderBuffs(drawOrder, DrawBufferShapeType::DropShadow);
            renderBuffs(drawOrder, DrawBufferShapeType::Shape);
            renderBuffs(drawOrder, DrawBufferShapeType::Outline);
            renderBuffs(drawOrder, DrawBufferShapeType::AA);
        }
    }

    void EndFrame()
    {
        Internal::g_rendererData.m_frameStarted = false;

        Backend::EndFrame();

        Internal::g_rendererData.m_gcFrameCounter++;

        if (Internal::g_rendererData.m_gcFrameCounter > Config.gcCollectInterval)
        {
            Internal::g_rendererData.m_gcFrameCounter = 0;
            Internal::ClearAllBuffers();
            Internal::g_rendererData.m_drawOrders.clear();
        }
        else
        {
            for (int i = 0; i < Internal::g_rendererData.m_gradientBuffers.m_size; i++)
                Internal::g_rendererData.m_gradientBuffers[i].ShrinkZero();

            for (int i = 0; i < Internal::g_rendererData.m_textureBuffers.m_size; i++)
                Internal::g_rendererData.m_textureBuffers[i].ShrinkZero();

            for (int i = 0; i < Internal::g_rendererData.m_defaultBuffers.m_size; i++)
                Internal::g_rendererData.m_defaultBuffers[i].ShrinkZero();

            for (int i = 0; i < Internal::g_rendererData.m_simpleTextBuffers.m_size; i++)
                Internal::g_rendererData.m_simpleTextBuffers[i].ShrinkZero();

            for (int i = 0; i < Internal::g_rendererData.m_sdfTextBuffers.m_size; i++)
                Internal::g_rendererData.m_sdfTextBuffers[i].ShrinkZero();
        }
    }

    GradientDrawBuffer& RendererData::GetGradientBuffer(Vec4Grad& grad, int drawOrder, DrawBufferShapeType shapeType)
    {
        const bool isAABuffer = shapeType == DrawBufferShapeType::AA;

        for (int i = 0; i < m_gradientBuffers.m_size; i++)
        {
            auto& buf = m_gradientBuffers[i];
            if (buf.m_shapeType == shapeType && buf.m_drawOrder == drawOrder && Math::IsEqual(buf.m_color.start, grad.start) && Math::IsEqual(buf.m_color.end, grad.end) && buf.m_color.gradientType == grad.gradientType && !buf.IsClipDifferent(Config.clipPosX, Config.clipPosY, Config.clipSizeX, Config.clipSizeY))
            {
                if (grad.gradientType == GradientType::Radial || grad.gradientType == GradientType::RadialCorner)
                {
                    if (buf.m_color.radialSize == grad.radialSize && buf.m_isAABuffer == isAABuffer)
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
            if (m_defaultBuffers[i].m_drawOrder == drawOrder && buf.m_shapeType == shapeType && !buf.IsClipDifferent(Config.clipPosX, Config.clipPosY, Config.clipSizeX, Config.clipSizeY))
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
            if (buf.m_shapeType == shapeType && buf.m_drawOrder == drawOrder && buf.m_textureHandle == textureHandle && Math::IsEqual(buf.m_textureUVTiling, tiling) && Math::IsEqual(buf.m_textureUVOffset, uvOffset) && buf.m_isAABuffer == isAABuffer && !buf.IsClipDifferent(Config.clipPosX, Config.clipPosY, Config.clipSizeX, Config.clipSizeY))
                return m_textureBuffers[i];
        }

        SetDrawOrderLimits(drawOrder);

        m_textureBuffers.push_back(TextureDrawBuffer(textureHandle, tiling, uvOffset, drawOrder, shapeType));
        return m_textureBuffers.last_ref();
    }

    SimpleTextDrawBuffer& RendererData::GetSimpleTextBuffer(BackendHandle textureHandle, int drawOrder, bool isDropShadow)
    {
        for (int i = 0; i < m_simpleTextBuffers.m_size; i++)
        {
            auto& buf = m_simpleTextBuffers[i];
            if (buf.m_isDropShadow == isDropShadow && buf.m_drawOrder == drawOrder && buf.m_textureHandle == textureHandle && !buf.IsClipDifferent(Config.clipPosX, Config.clipPosY, Config.clipSizeX, Config.clipSizeY))
                return m_simpleTextBuffers[i];
        }

        SetDrawOrderLimits(drawOrder);

        m_simpleTextBuffers.push_back(SimpleTextDrawBuffer(textureHandle, drawOrder, isDropShadow));
        return m_simpleTextBuffers.last_ref();
    }

    SDFTextDrawBuffer& RendererData::GetSDFTextBuffer(BackendHandle textureHandle, int drawOrder, const SDFTextOptions& opts, bool isDropShadow)
    {
        for (int i = 0; i < m_sdfTextBuffers.m_size; i++)
        {
            auto& buf = m_sdfTextBuffers[i];
            if (buf.m_isDropShadow == isDropShadow && buf.m_textureHandle == textureHandle && buf.m_drawOrder == drawOrder && buf.m_thickness == opts.sdfThickness && buf.m_softness == opts.sdfSoftness &&
                buf.m_outlineThickness == opts.sdfOutlineThickness && buf.m_flipAlpha == opts.flipAlpha && Math::IsEqual(buf.m_outlineColor, opts.sdfOutlineColor) && !buf.IsClipDifferent(Config.clipPosX, Config.clipPosY, Config.clipSizeX, Config.clipSizeY))
                return m_sdfTextBuffers[i];
        }

        SetDrawOrderLimits(drawOrder);

        m_sdfTextBuffers.push_back(SDFTextDrawBuffer(textureHandle, drawOrder, opts, isDropShadow));
        return m_sdfTextBuffers.last_ref();
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

    int RendererData::GetBufferIndexInCharArray(DrawBuffer* buf)
    {
        for (int i = 0; i < m_simpleTextBuffers.m_size; i++)
        {
            if (buf == &m_simpleTextBuffers[i])
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
        bool found = false;
        for (int i = 0; i < m_drawOrders.m_size; i++)
        {
            if (m_drawOrders[i] == drawOrder)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            m_drawOrders.push_back(drawOrder);
            Utility::QuickSortArray<int>(m_drawOrders, 0, m_drawOrders.m_size - 1);
        }
    }

} // namespace LinaVG