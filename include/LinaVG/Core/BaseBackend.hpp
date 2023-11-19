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

#include "../Core/Common.hpp"

namespace LinaVG
{
	namespace Internal
	{
		class ShaderData;
	}
} // namespace LinaVG

namespace LinaVG::Backend
{

	class BaseBackend
	{
	public:
		BaseBackend()		   = default;
		virtual ~BaseBackend() = default;

		static BaseBackend* Get()
		{
			return s_backend;
		}

		static void SetBackend(BaseBackend* ptr)
		{
			s_backend = ptr;
		}

		// Public API, if you want to implement your own backend, it needs to define these exact signatures.
		virtual bool		  Initialize()																				   = 0;
		virtual void		  Terminate()																				   = 0;
		virtual void		  StartFrame(int threadCount)																   = 0;
		virtual void		  DrawGradient(GradientDrawBuffer* buf, int thread)											   = 0;
		virtual void		  DrawTextured(TextureDrawBuffer* buf, int thread)											   = 0;
		virtual void		  DrawDefault(DrawBuffer* buf, int thread)													   = 0;
		virtual void		  DrawSimpleText(SimpleTextDrawBuffer* buf, int thread)										   = 0;
		virtual void		  DrawSDFText(SDFTextDrawBuffer* buf, int thread)											   = 0;
		virtual void		  EndFrame()																				   = 0;
		virtual void		  BufferFontTextureAtlas(int width, int height, int offsetX, int offsetY, unsigned char* data) = 0;
		virtual void		  BufferEnded()																				   = 0;
		virtual void		  BindFontTexture(BackendHandle texture)													   = 0;
		virtual void		  SaveAPIState()																			   = 0;
		virtual void		  RestoreAPIState()																			   = 0;
		virtual BackendHandle CreateFontTexture(int width, int height)													   = 0;

		static BaseBackend* s_backend;
	};

} // namespace LinaVG::Backend
