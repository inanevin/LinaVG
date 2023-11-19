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

#pragma once

#include "Common.hpp"

namespace LinaVG
{

	struct TextCache
	{
		TextOptions	  opts;
		Array<Vertex> vtxBuffer;
		Array<Index>  indxBuffer;
	};

	struct SDFTextCache
	{
		SDFTextOptions opts;
		Array<Vertex>  vtxBuffer;
		Array<Index>   indxBuffer;
	};

	struct RectOverrideData
	{
		bool overrideRectPositions = false;
		Vec2 m_p1				   = Vec2(0, 0);
		Vec2 m_p2				   = Vec2(0, 0);
		Vec2 m_p3				   = Vec2(0, 0);
		Vec2 m_p4				   = Vec2(0, 0);
	};

	struct UVOverrideData
	{
		bool m_override = false;
		Vec2 m_uvTL		= Vec2(0, 0);
		Vec2 m_uvBR		= Vec2(1, 1);
	};

	/// <summary>
	/// Management for draw buffers.
	/// </summary>
	struct RendererData
	{
		Array<DrawBuffer>				   m_defaultBuffers;
		Array<GradientDrawBuffer>		   m_gradientBuffers;
		Array<TextureDrawBuffer>		   m_textureBuffers;
		Array<SimpleTextDrawBuffer>		   m_simpleTextBuffers;
		Array<SDFTextDrawBuffer>		   m_sdfTextBuffers;
		Array<int>						   m_drawOrders;
		LINAVG_MAP<uint32_t, TextCache>	   m_textCache;
		LINAVG_MAP<uint32_t, SDFTextCache> m_sdfTextCache;
		int								   m_gcFrameCounter		   = 0;
		int								   m_textCacheFrameCounter = 0;
		RectOverrideData				   m_rectOverrideData;
		UVOverrideData					   m_uvOverride;
		BackendHandle					   m_clipPosX  = 0;
		BackendHandle					   m_clipPosY  = 0;
		BackendHandle					   m_clipSizeX = 0;
		BackendHandle					   m_clipSizeY = 0;

		void				  SetDrawOrderLimits(int drawOrder);
		int					  GetBufferIndexInGradientArray(DrawBuffer* buf);
		int					  GetBufferIndexInTextureArray(DrawBuffer* buf);
		int					  GetBufferIndexInDefaultArray(DrawBuffer* buf);
		int					  GetBufferIndexInCharArray(DrawBuffer* buf);
		DrawBuffer&			  GetDefaultBuffer(int drawOrder, DrawBufferShapeType shapeType);
		GradientDrawBuffer&	  GetGradientBuffer(Vec4Grad& grad, int drawOrder, DrawBufferShapeType shapeType);
		TextureDrawBuffer&	  GetTextureBuffer(BackendHandle textureHandle, const Vec2& tiling, const Vec2& uvOffset, const Vec4& tint, int drawOrder, DrawBufferShapeType shapeType);
		SimpleTextDrawBuffer& GetSimpleTextBuffer(BackendHandle glyphHandle, int drawOrder, bool isDropShadow);
		SDFTextDrawBuffer&	  GetSDFTextBuffer(BackendHandle glyphHandle, int drawOrder, const SDFTextOptions& opts, bool isDropShadow);
		void				  AddTextCache(uint32_t sid, const TextOptions& opts, DrawBuffer* buf, int vtxStart, int indexStart);
		void				  AddSDFTextCache(uint32_t sid, const SDFTextOptions& opts, DrawBuffer* buf, int vtxStart, int indexStart);
		TextCache*			  CheckTextCache(uint32_t sid, const TextOptions& opts, DrawBuffer* buf);
		SDFTextCache*		  CheckSDFTextCache(uint32_t sid, const SDFTextOptions& opts, DrawBuffer* buf);
	};

	namespace Internal
	{
		extern LINAVG_API LINAVG_VEC<RendererData> g_rendererData;

		/// <summary>
		/// Erases all vertex & index data on all buffers.
		/// </summary>
		LINAVG_API void ClearAllBuffers();

		LINAVG_API void InitThreadedData();
	} // namespace Internal

	/// <summary>
	/// Initializes LinaVG renderer. Must be called before any other calls to LinaVG API!
	/// </summary>
	/// <returns></returns>
	LINAVG_API bool Initialize();

	/// <summary>
	/// Terminates LinaVG. Call once your main application loop is complete.
	/// </summary>
	/// <returns></returns>
	LINAVG_API void Terminate();

	/// <summary>
	/// Any Draw commands via LinaVG must take place between StartFrame and EndFrame.
	/// You may need to Clear your color buffer bits before calling StartFrame. LinaVG doesn't do any clearing.
	/// </summary>
	/// <returns></returns>
	LINAVG_API void StartFrame(int threadCount = 1);

	/// <summary>
	/// Call after you submit your draw requests to LinaVG, before EndFrame;
	/// </summary>
	/// <returns></returns>
	LINAVG_API void Render(int thread = 0);

	/// <summary>
	/// Sets the scissors/clipping data. Only use after calling LinaVG::StartFrame().
	/// </summary>
	/// <returns></returns>
	LINAVG_API void SetClipPosX(BackendHandle posX, int thread = 0);
	LINAVG_API void SetClipPosY(BackendHandle posY, int thread = 0);
	LINAVG_API void SetClipSizeX(BackendHandle sizeX, int thread = 0);
	LINAVG_API void SetClipSizeY(BackendHandle sizeY, int thread = 0);

	/// <summary>
	/// Any Draw commands via LinaVG must take place between StartFrame and EndFrame.
	/// </summary>
	/// <returns></returns>
	LINAVG_API void EndFrame();

}; // namespace LinaVG
