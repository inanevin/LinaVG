/* 
This file is a part of: LinaVG
https://github.com/inanevin/LinaVG

Author: Inan Evin
http://www.inanevin.com

The 2-Clause BSD License

Copyright (c) [2022-] Inan Evin

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <iostream>

#include "LinaVG/Core/Renderer.hpp"
#include "LinaVG/Core/BaseBackend.hpp"
#include "LinaVG/Core/Drawer.hpp"
#include "LinaVG/Core/Math.hpp"
#include "LinaVG/Core/Text.hpp"
#include "LinaVG/Utility/Utility.hpp"
#include <math.h>
#include <cassert>

namespace LinaVG
{
	Backend::BaseBackend* Backend::BaseBackend::s_backend = nullptr;

void Renderer::ClearAllBuffers()
{
    m_data.m_gcFrameCounter = 0;

    for (int i = 0; i < m_data.m_gradientBuffers.m_size; i++)
        m_data.m_gradientBuffers[i].Clear();

    m_data.m_gradientBuffers.clear();

    for (int i = 0; i < m_data.m_textureBuffers.m_size; i++)
        m_data.m_textureBuffers[i].Clear();

    m_data.m_textureBuffers.clear();

    for (int i = 0; i < m_data.m_defaultBuffers.m_size; i++)
        m_data.m_defaultBuffers[i].Clear();

    m_data.m_defaultBuffers.clear();

    for (int i = 0; i < m_data.m_simpleTextBuffers.m_size; i++)
        m_data.m_simpleTextBuffers[i].Clear();

    m_data.m_simpleTextBuffers.clear();

    for (int i = 0; i < m_data.m_sdfTextBuffers.m_size; i++)
        m_data.m_sdfTextBuffers[i].Clear();

    m_data.m_sdfTextBuffers.clear();
    m_data.m_drawOrders.clear();
}

void Renderer::InitThreadedData()
{
    m_data.m_defaultBuffers.reserve(Config.defaultBufferReserve);
    m_data.m_gradientBuffers.reserve(Config.gradientBufferReserve);
    m_data.m_textureBuffers.reserve(Config.textureBufferReserve);
    m_data.m_simpleTextBuffers.reserve(Config.textBuffersReserve);
    m_data.m_sdfTextBuffers.reserve(Config.textBuffersReserve);

    if (Config.textCachingEnabled)
        m_data.m_textCache.reserve(Config.textCacheReserve);

    if (Config.textCachingSDFEnabled)
        m_data.m_sdfTextCache.reserve(Config.textCacheSDFReserve);
}

    Renderer::~Renderer()
    {
        ClearAllBuffers();
    }

	void Renderer::StartFrame()
	{
        if(m_data.m_defaultBuffers.m_size == 0)
            InitThreadedData();
        
        m_data.m_gcFrameCounter++;

        if (m_data.m_gcFrameCounter > Config.gcCollectInterval)
        {
            ClearAllBuffers();
        }
        else
        {
            for (int i = 0; i < m_data.m_gradientBuffers.m_size; i++)
                m_data.m_gradientBuffers[i].ShrinkZero();

            for (int i = 0; i < m_data.m_textureBuffers.m_size; i++)
                m_data.m_textureBuffers[i].ShrinkZero();

            for (int i = 0; i < m_data.m_defaultBuffers.m_size; i++)
                m_data.m_defaultBuffers[i].ShrinkZero();

            for (int i = 0; i < m_data.m_simpleTextBuffers.m_size; i++)
                m_data.m_simpleTextBuffers[i].ShrinkZero();

            for (int i = 0; i < m_data.m_sdfTextBuffers.m_size; i++)
                m_data.m_sdfTextBuffers[i].ShrinkZero();
        }

        // SDF
        if (Config.textCachingEnabled || Config.textCachingSDFEnabled)
            m_data.m_textCacheFrameCounter++;

        if (m_data.m_textCacheFrameCounter > Config.textCacheExpireInterval)
        {
            m_data.m_textCacheFrameCounter = 0;
            m_data.m_textCache.clear();
            m_data.m_sdfTextCache.clear();
        }
	}

	void Renderer::Render()
	{
        int thread = 0;
		auto renderBuffs = [this, thread](int drawOrder, DrawBufferShapeType shapeType) {
			for (int i = 0; i < m_data.m_defaultBuffers.m_size; i++)
			{
				DrawBuffer& buf = m_data.m_defaultBuffers[i];

				if (buf.m_drawOrder == drawOrder && buf.m_shapeType == shapeType && buf.m_vertexBuffer.m_size != 0 && buf.m_indexBuffer.m_size != 0)
					Backend::BaseBackend::Get()->DrawDefault(&(buf), thread);
			}

			for (int i = 0; i < m_data.m_gradientBuffers.m_size; i++)
			{
				GradientDrawBuffer& buf = m_data.m_gradientBuffers[i];

				if (buf.m_drawOrder == drawOrder && buf.m_shapeType == shapeType && buf.m_vertexBuffer.m_size != 0 && buf.m_indexBuffer.m_size != 0)
					Backend::BaseBackend::Get()->DrawGradient(&buf, thread);
			}

			for (int i = 0; i < m_data.m_textureBuffers.m_size; i++)
			{
				TextureDrawBuffer& buf = m_data.m_textureBuffers[i];

				if (buf.m_drawOrder == drawOrder && buf.m_shapeType == shapeType && buf.m_vertexBuffer.m_size != 0 && buf.m_indexBuffer.m_size != 0)
					Backend::BaseBackend::Get()->DrawTextured(&buf, thread);
			}

			for (int i = 0; i < m_data.m_simpleTextBuffers.m_size; i++)
			{
				SimpleTextDrawBuffer& buf = m_data.m_simpleTextBuffers[i];

				if (buf.m_drawOrder == drawOrder && buf.m_shapeType == shapeType && buf.m_vertexBuffer.m_size != 0 && buf.m_indexBuffer.m_size != 0)
					Backend::BaseBackend::Get()->DrawSimpleText(&buf, thread);
			}

			for (int i = 0; i < m_data.m_sdfTextBuffers.m_size; i++)
			{
				SDFTextDrawBuffer& buf = m_data.m_sdfTextBuffers[i];

				if (buf.m_drawOrder == drawOrder && buf.m_shapeType == shapeType && buf.m_vertexBuffer.m_size != 0 && buf.m_indexBuffer.m_size != 0)
					Backend::BaseBackend::Get()->DrawSDFText(&buf, thread);
			}
		};

		auto& arr = m_data.m_drawOrders;

		for (int i = 0; i < arr.m_size; i++)
		{
			const int drawOrder = arr[i];
			renderBuffs(drawOrder, DrawBufferShapeType::DropShadow);
			renderBuffs(drawOrder, DrawBufferShapeType::Shape);
			renderBuffs(drawOrder, DrawBufferShapeType::Outline);
			renderBuffs(drawOrder, DrawBufferShapeType::AA);
		}
	}

	LINAVG_API void Renderer::SetClipPosX(BackendHandle posX, int thread)
	{
		m_data.m_clipPosX = posX;
	}

	LINAVG_API void Renderer::SetClipPosY(BackendHandle posY, int thread)
	{
		m_data.m_clipPosY = posY;
	}

	LINAVG_API void Renderer::SetClipSizeX(BackendHandle sizeX, int thread)
	{
		m_data.m_clipSizeX = sizeX;
	}

	LINAVG_API void Renderer::SetClipSizeY(BackendHandle sizeY, int thread)
	{
		m_data.m_clipSizeY = sizeY;
	}

	GradientDrawBuffer& RendererData::GetGradientBuffer(Vec4Grad& grad, int drawOrder, DrawBufferShapeType shapeType)
	{
		const bool isAABuffer = shapeType == DrawBufferShapeType::AA;

		for (int i = 0; i < m_gradientBuffers.m_size; i++)
		{
			auto& buf = m_gradientBuffers[i];
			if (buf.m_shapeType == shapeType && buf.m_drawOrder == drawOrder && Math::IsEqual(buf.m_color.start, grad.start) && Math::IsEqual(buf.m_color.end, grad.end) && buf.m_color.gradientType == grad.gradientType && !buf.IsClipDifferent(m_clipPosX, m_clipPosY, m_clipSizeX, m_clipSizeY))
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

		m_gradientBuffers.push_back(GradientDrawBuffer(grad, drawOrder, shapeType, m_clipPosX, m_clipPosY, m_clipSizeX, m_clipSizeY));
		return m_gradientBuffers.last_ref();
	}

	DrawBuffer& RendererData::GetDefaultBuffer(int drawOrder, DrawBufferShapeType shapeType)
	{
		for (int i = 0; i < m_defaultBuffers.m_size; i++)
		{
			auto& buf = m_defaultBuffers[i];
			if (m_defaultBuffers[i].m_drawOrder == drawOrder && buf.m_shapeType == shapeType && !buf.IsClipDifferent(m_clipPosX, m_clipPosY, m_clipSizeX, m_clipSizeY))
				return m_defaultBuffers[i];
		}

		SetDrawOrderLimits(drawOrder);

		m_defaultBuffers.push_back(DrawBuffer(drawOrder, DrawBufferType::Default, shapeType, m_clipPosX, m_clipPosY, m_clipSizeX, m_clipSizeY));
		return m_defaultBuffers.last_ref();
	}

	TextureDrawBuffer& RendererData::GetTextureBuffer(BackendHandle textureHandle, const Vec2& tiling, const Vec2& uvOffset, const Vec4& tint, int drawOrder, DrawBufferShapeType shapeType)
	{
		const bool isAABuffer = shapeType == DrawBufferShapeType::AA;
		for (int i = 0; i < m_textureBuffers.m_size; i++)
		{
			auto& buf = m_textureBuffers[i];
			if (buf.m_shapeType == shapeType && buf.m_drawOrder == drawOrder && buf.m_textureHandle == textureHandle && Math::IsEqual(buf.m_tint, tint) && Math::IsEqual(buf.m_textureUVTiling, tiling) && Math::IsEqual(buf.m_textureUVOffset, uvOffset) && buf.m_isAABuffer == isAABuffer && !buf.IsClipDifferent(m_clipPosX, m_clipPosY, m_clipSizeX, m_clipSizeY))
				return m_textureBuffers[i];
		}

		SetDrawOrderLimits(drawOrder);

		m_textureBuffers.push_back(TextureDrawBuffer(textureHandle, tiling, uvOffset, tint, drawOrder, shapeType, m_clipPosX, m_clipPosY, m_clipSizeX, m_clipSizeY));
		return m_textureBuffers.last_ref();
	}

	SimpleTextDrawBuffer& RendererData::GetSimpleTextBuffer(BackendHandle textureHandle, int drawOrder, bool isDropShadow)
	{
		for (int i = 0; i < m_simpleTextBuffers.m_size; i++)
		{
			auto& buf = m_simpleTextBuffers[i];
			if (buf.m_isDropShadow == isDropShadow && buf.m_drawOrder == drawOrder && buf.m_textureHandle == textureHandle && !buf.IsClipDifferent(m_clipPosX, m_clipPosY, m_clipSizeX, m_clipSizeY))
				return m_simpleTextBuffers[i];
		}

		SetDrawOrderLimits(drawOrder);

		m_simpleTextBuffers.push_back(SimpleTextDrawBuffer(textureHandle, drawOrder, isDropShadow, m_clipPosX, m_clipPosY, m_clipSizeX, m_clipSizeY));
		return m_simpleTextBuffers.last_ref();
	}

	SDFTextDrawBuffer& RendererData::GetSDFTextBuffer(BackendHandle textureHandle, int drawOrder, const SDFTextOptions& opts, bool isDropShadow)
	{
		for (int i = 0; i < m_sdfTextBuffers.m_size; i++)
		{
			auto& buf = m_sdfTextBuffers[i];
			if (buf.m_isDropShadow == isDropShadow && buf.m_textureHandle == textureHandle && buf.m_drawOrder == drawOrder && Math::IsEqualMarg(buf.m_thickness, opts.sdfThickness) && Math::IsEqualMarg(buf.m_softness, opts.sdfSoftness) &&
                Math::IsEqualMarg(buf.m_outlineThickness, opts.sdfOutlineThickness) && buf.m_flipAlpha == opts.flipAlpha && Math::IsEqual(buf.m_outlineColor, opts.sdfOutlineColor) && !buf.IsClipDifferent(m_clipPosX, m_clipPosY, m_clipSizeX, m_clipSizeY))
				return m_sdfTextBuffers[i];
		}

		SetDrawOrderLimits(drawOrder);

		m_sdfTextBuffers.push_back(SDFTextDrawBuffer(textureHandle, drawOrder, opts, isDropShadow, m_clipPosX, m_clipPosY, m_clipSizeX, m_clipSizeY));
		return m_sdfTextBuffers.last_ref();
	}

	void RendererData::AddTextCache(uint32_t sid, const TextOptions& opts, DrawBuffer* buf, int vtxStart, int indexStart)
	{
		TextCache& newCache = m_textCache[sid];
		newCache.opts		= opts;
		newCache.indxBuffer.clear();
		newCache.vtxBuffer.clear();

		for (int i = vtxStart; i < buf->m_vertexBuffer.m_size; i++)
			newCache.vtxBuffer.push_back(buf->m_vertexBuffer[i]);

		for (int i = indexStart; i < buf->m_indexBuffer.m_size; i++)
			newCache.indxBuffer.push_back(buf->m_indexBuffer[i] - vtxStart);
	}

	void RendererData::AddSDFTextCache(uint32_t sid, const SDFTextOptions& opts, DrawBuffer* buf, int vtxStart, int indexStart)
	{
		SDFTextCache& newCache = m_sdfTextCache[sid];
		newCache.opts		   = opts;
		newCache.indxBuffer.clear();
		newCache.vtxBuffer.clear();

		for (int i = vtxStart; i < buf->m_vertexBuffer.m_size; i++)
			newCache.vtxBuffer.push_back(buf->m_vertexBuffer[i]);

		for (int i = indexStart; i < buf->m_indexBuffer.m_size; i++)
			newCache.indxBuffer.push_back(buf->m_indexBuffer[i] - vtxStart);
	}

	TextCache* RendererData::CheckTextCache(uint32_t sid, const TextOptions& opts, DrawBuffer* buf)
	{
		const auto& it = m_textCache.find(sid);

		if (it == m_textCache.end())
			return nullptr;

		if (!it->second.opts.IsSame(opts))
			return nullptr;

		const int vtxStart = buf->m_vertexBuffer.m_size;

		for (auto& b : it->second.vtxBuffer)
			buf->PushVertex(b);

		for (auto& i : it->second.indxBuffer)
			buf->PushIndex(i + vtxStart);

		return &it->second;
	}

	SDFTextCache* RendererData::CheckSDFTextCache(uint32_t sid, const SDFTextOptions& opts, DrawBuffer* buf)
	{
		const auto& it = m_sdfTextCache.find(sid);

		if (it == m_sdfTextCache.end())
			return nullptr;

		if (!it->second.opts.IsSame(opts))
			return nullptr;

		const int vtxStart = buf->m_vertexBuffer.m_size;

		for (auto& b : it->second.vtxBuffer)
			buf->PushVertex(b);

		for (auto& i : it->second.indxBuffer)
			buf->PushIndex(i + vtxStart);

		return &it->second;
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
