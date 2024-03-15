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

#ifndef LINAVG_DISABLE_TEXT_SUPPORT

#include "LinaVG/Core/Text.hpp"
#include "LinaVG/Core/Renderer.hpp"
#include "LinaVG/Core/BaseBackend.hpp"
#include "LinaVG/Core/Math.hpp"
#include <iostream>

namespace LinaVG
{
	namespace Text
	{
		bool Initialize()
		{
			if (FT_Init_FreeType(&Internal::g_textData.m_ftlib))
			{
				if (Config.errorCallback)
					Config.errorCallback("LinaVG: Error initializing FreeType Library");
				return false;
			}

			return true;
		}

		void Terminate()
		{
			FT_Done_FreeType(Internal::g_textData.m_ftlib);
			Internal::g_textData.m_createdAtlases.clear();
		}
	} // namespace Text

	LinaVGFont* LoadFont(const char* file, bool loadAsSDF, int size, GlyphEncoding* customRanges, int customRangesSize, bool useKerningIfAvailable)
	{
		FT_Face face;
		if (FT_New_Face(Internal::g_textData.m_ftlib, file, 0, &face))
		{
			if (Config.errorCallback)
				Config.errorCallback("LinaVG: Freetype Error -> Failed to load the font!");
			return nullptr;
		}

		return Internal::SetupFont(face, loadAsSDF, size, customRanges, customRangesSize, useKerningIfAvailable);
	}

	LinaVGFont* LoadFontFromMemory(void* data, size_t dataSize, bool loadAsSDF, int size, GlyphEncoding* customRanges, int customRangesSize, bool useKerningIfAvailable)
	{
		FT_Face face;
		if (FT_New_Memory_Face(Internal::g_textData.m_ftlib, static_cast<FT_Byte*>(data), static_cast<FT_Long>(dataSize), 0, &face))
		{
			if (Config.errorCallback)
				Config.errorCallback("LinaVG: Freetype Error -> Failed to load the font!");
			return nullptr;
		}

		return Internal::SetupFont(face, loadAsSDF, size, customRanges, customRangesSize, useKerningIfAvailable);
	}

	namespace Internal
	{
		TextData g_textData;

		LinaVGFont* SetupFont(FT_Face& face, bool loadAsSDF, int size, GlyphEncoding* customRanges, int customRangesSize, bool useKerningIfAvailable)
		{

			FT_Error err = FT_Set_Pixel_Sizes(face, 0, size);

			if (err)
				Config.errorCallback("LinaVG: Error on FT_Set_Pixel_Sizes!");

			err = FT_Select_Charmap(face, ft_encoding_unicode);

			if (err)
				Config.errorCallback("LinaVG: Error on FT_Select_Charmap!");

			// Texture alignment changes might be necessary on some APIs such as OpenGL
			Backend::BaseBackend::Get()->SaveAPIState();

			LinaVGFont* font		= new LinaVGFont();
			font->m_supportsUnicode = customRanges != nullptr;
			font->m_size			= size;
			font->m_isSDF			= loadAsSDF;
			font->m_newLineHeight	= static_cast<float>(face->size->metrics.height) / 64.0f;
			font->m_supportsKerning = useKerningIfAvailable && FT_HAS_KERNING(face) != 0;

			// int		 maxHeight		   = 0;
			auto&		 characterMap	   = font->m_characterGlyphs;
			unsigned int roww			   = 0;
			unsigned int rowh			   = 0;
			int			 bufferCharSpacing = 5;
			unsigned int w				   = bufferCharSpacing;
			unsigned int h				   = bufferCharSpacing;
			FT_GlyphSlot slot			   = face->glyph;

			auto setSizes = [&](FT_ULong c) {
				auto i			  = FT_Get_Char_Index(face, c);
                
                // not found.
                if(i == 0)
                {
                    return true;
                }
                
				err				  = FT_Load_Glyph(face, i, FT_LOAD_DEFAULT);
				TextCharacter& ch = characterMap[c];

                
				if (err)
				{
					if (Config.errorCallback)
						Config.errorCallback("LinaVG: Freetype Error -> Failed to load character!");
					return false;
				}

				if (loadAsSDF)
					err = FT_Render_Glyph(slot, FT_RENDER_MODE_SDF);
				else
					err = FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);

				if (err)
				{
					if (Config.errorCallback)
						Config.errorCallback("LinaVG: Freetype Error -> Failed to render character!");
					return false;
				}

				const unsigned int glyphWidth = slot->bitmap.width;
				const unsigned int glyphRows  = slot->bitmap.rows;

				if (roww + glyphWidth + bufferCharSpacing >= Config.maxFontAtlasSize)
				{
					w = Math::Max(w, roww);
					h += rowh + bufferCharSpacing;
					roww = bufferCharSpacing;
					rowh = 0;
				}

				const size_t bufSize = static_cast<size_t>(glyphWidth * glyphRows);

				if (slot->bitmap.buffer != nullptr)
				{
					ch.m_buffer = (unsigned char*)LINAVG_MALLOC(bufSize);

					if (ch.m_buffer != 0) 
						LINAVG_MEMCPY(ch.m_buffer, slot->bitmap.buffer, bufSize);
				}
                
				ch.m_size	 = Vec2(static_cast<float>(glyphWidth), static_cast<float>(glyphRows));
				ch.m_bearing = Vec2(static_cast<float>(slot->bitmap_left), static_cast<float>(slot->bitmap_top));
				ch.m_advance = Vec2(static_cast<float>(slot->advance.x >> 6), static_cast<float>(slot->advance.y >> 6));

				roww += glyphWidth + bufferCharSpacing;
				rowh = Math::Max(rowh, glyphRows);
				return true;
			};

			auto storeKerning = [&](FT_ULong first, FT_ULong second) {
				auto firstIndex	 = FT_Get_Char_Index(face, first);
				auto secondIndex = FT_Get_Char_Index(face, second);

				FT_Vector delta;
				err = FT_Get_Kerning(face, firstIndex, secondIndex, FT_KERNING_DEFAULT, &delta);

				if (err)
					Config.errorCallback("LinaVG: Error on FT_Get_Kerning!");

				font->m_kerningTable[first].xAdvances[second] = delta.x;
			};

			for (FT_ULong c = 32; c < 128; c++)
			{
				setSizes(c);

				if (font->m_supportsKerning)
				{
					for (FT_ULong a = 32; a < c; a++)
						storeKerning(a, c);
				}
			}

			bool useCustomRanges = customRangesSize != 0;
			if (customRangesSize % 2 == 1)
			{
				useCustomRanges = false;
				if (Config.errorCallback)
					Config.errorCallback("LinaVG: Custom ranges given to font loading must have a size multiple of 2!");
			}

			if (useCustomRanges)
			{
				int		  index			   = 0;
				const int customRangeCount = customRangesSize / 2;
				for (int i = 0; i < customRangeCount; i++)
				{
					if (customRanges[index] == customRanges[index + 1])
						setSizes(customRanges[index]);
					else
					{
						for (FT_ULong c = customRanges[index]; c < customRanges[index + 1]; c++)
							setSizes(c);
					}
					index += 2;
				}
			}

			w = Math::Max(w, roww);
			h += rowh;

			int availableAtlasIndex = -1;
			for (int i = 0; i < g_textData.m_createdAtlases.m_size; i++)
			{
				FontAtlas& atlas = g_textData.m_createdAtlases[i];
				// const int  totalSize = Config.maxFontAtlasSize - atlas.m_currentOffsetX + Config.maxFontAtlasSize - atlas.m_currentOffsetY;
				if (Config.maxFontAtlasSize - atlas.m_currentOffsetY > h)
				{
					availableAtlasIndex = i;
					Backend::BaseBackend::Get()->BindFontTexture(atlas.m_texture);
					break;
				}
			}

			int offsetX = bufferCharSpacing;
			int offsetY = bufferCharSpacing;

			int usedAtlasIndex = -1;
			w = h = Config.maxFontAtlasSize;

			unsigned int availableAtlasRowSizeY = 0;

			if (availableAtlasIndex != -1)
			{
				offsetX				   = g_textData.m_createdAtlases[availableAtlasIndex].m_currentOffsetX;
				offsetY				   = g_textData.m_createdAtlases[availableAtlasIndex].m_currentOffsetY;
				availableAtlasRowSizeY = g_textData.m_createdAtlases[availableAtlasIndex].m_rowSizeY;
				font->m_texture		   = g_textData.m_createdAtlases[availableAtlasIndex].m_texture;
				usedAtlasIndex		   = availableAtlasIndex;
			}
			else
			{
				FontAtlas atlas;
				atlas.m_texture = Backend::BaseBackend::Get()->CreateFontTexture(w, h);
				font->m_texture = atlas.m_texture;
				usedAtlasIndex	= g_textData.m_createdAtlases.m_size;
				g_textData.m_createdAtlases.push_back(atlas);
			}

			// TODO: figure out width as height issue
			rowh = 0;

			bool firstRow = true;
			bool buffered = false;

			for (auto& ch : characterMap)
			{
				const unsigned int glyphWidth = static_cast<unsigned int>(ch.second.m_size.x);
				const unsigned int glyphRows  = static_cast<unsigned int>(ch.second.m_size.y);

				if (offsetX + glyphWidth + bufferCharSpacing >= Config.maxFontAtlasSize)
				{
					if (availableAtlasIndex != -1 && firstRow)
					{
						offsetY += bufferCharSpacing + Math::Max(rowh, availableAtlasRowSizeY);
						firstRow = false;
					}
					else
						offsetY += rowh + bufferCharSpacing;

					rowh	= 0;
					offsetX = bufferCharSpacing;
				}

				const Vec2	size	   = Vec2(static_cast<float>(glyphWidth), static_cast<float>(glyphRows));
				const float fontWidth  = static_cast<float>(w);
				const float fontHeight = static_cast<float>(h);
				const float xx		   = (float)offsetX / fontWidth;
				const float yy		   = (float)offsetY / fontHeight;
				Vec2		uv1		   = Vec2(xx, yy);
				Vec2		uv2		   = Vec2(xx + size.x / fontWidth, yy);
				Vec2		uv3		   = Vec2(xx + size.x / fontWidth, yy + size.y / fontHeight);
				Vec2		uv4		   = Vec2(xx, yy + size.y / fontHeight);

				// Vec2       points[] = {uv1, uv2, uv3, uv4};
				// const Vec2 avg      = Math::GetPolygonCentroidFast(points, 4);
				// uv1                 = Math::ScalePoint(points[0], avg, 1.1f);
				// uv2                 = Math::ScalePoint(points[1], avg, 1.1f);
				// uv3                 = Math::ScalePoint(points[2], avg, 1.1f);
				// uv4                 = Math::ScalePoint(points[3], avg, 1.1f);

				const Vec4 uv12 = Vec4(uv1.x, uv1.y, uv2.x, uv2.y);
				const Vec4 uv34 = Vec4(uv3.x, uv3.y, uv4.x, uv4.y);

				if (ch.second.m_buffer != nullptr)
				{
					Backend::BaseBackend::Get()->BufferFontTextureAtlas(glyphWidth, glyphRows, offsetX, offsetY, ch.second.m_buffer);
					buffered = true;
					LINAVG_FREE(ch.second.m_buffer);
				}

				ch.second.m_buffer = nullptr;
				ch.second.m_uv12   = uv12;
				ch.second.m_uv34   = uv34;

				rowh												   = Math::Max(rowh, glyphRows);
				g_textData.m_createdAtlases[usedAtlasIndex].m_rowSizeY = rowh;
				offsetX += glyphWidth + bufferCharSpacing;
			}

			if (buffered)
				Backend::BaseBackend::Get()->BufferEnded();

			if (usedAtlasIndex != -1)
			{
				g_textData.m_createdAtlases[usedAtlasIndex].m_currentOffsetX = offsetX;
				g_textData.m_createdAtlases[usedAtlasIndex].m_currentOffsetY = offsetY;
			}

			font->m_spaceAdvance = characterMap[' '].m_advance.x;
			err					 = FT_Done_Face(face);

			if (err)
				Config.errorCallback("LinaVG: Error on FT_Done_Face!");

			Backend::BaseBackend::Get()->RestoreAPIState();
			Config.logCallback("LinaVG: Successfuly loaded font!");
			return font;
		}
	} // namespace Internal

} // namespace LinaVG

#endif
