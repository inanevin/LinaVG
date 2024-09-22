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
#include "LinaVG/Core/BufferStore.hpp"
#include "LinaVG/Core/Math.hpp"
#include <iostream>

namespace LinaVG
{
    FT_Library g_ftLib;

	bool InitializeText()
	{
		if (FT_Init_FreeType(&g_ftLib))
		{
			if (Config.errorCallback)
				Config.errorCallback("LinaVG: Error initializing FreeType Library");
			return false;
		}

		return true;
	}

	void TerminateText()
	{
		FT_Done_FreeType(g_ftLib);
	}

    Text::~Text()
    {
        for(Atlas* atlas : m_atlases)
            delete atlas;
    }

    void Font::DestroyBuffers()
    {
        for(auto& [glyph, textChar] : m_characterGlyphs)
            LINAVG_FREE(textChar.m_buffer);
        m_characterGlyphs.clear();
        m_atlas->RemoveFont(this);
    }

    Atlas::Atlas(const Vec2ui& size, std::function<void(Atlas* atlas)> updateFunc)
    {
        m_updateFunc = updateFunc;
        m_size = size;
        m_data = new uint8_t[size.x * size.y];
        memset(m_data, 0, size.x * size.y);
        m_availableSlices.push_back(new Slice(0, size.y));
    }

    Atlas::~Atlas(){
        Destroy();
    };

    void Atlas::Destroy()
    {
        if(m_data != nullptr)
            delete[] m_data;
        m_data = nullptr;
        
        for(Slice* slice : m_availableSlices)
            delete slice;
        m_availableSlices.clear();
    }

    bool Atlas::AddFont(Font *font)
    {
        if(font->m_atlasRectHeight > m_size.y)
        {
            if (Config.errorCallback)
                Config.errorCallback("LinaVG: Font exceeds atlas size! Increase the max atlas size from config.");
            
            return;
        }
        
        font->m_atlas = this;

        for(Slice* slice : m_availableSlices)
        {
            if(slice->height <= font->m_atlasRectHeight)
                continue;
            
            font->m_atlasRectPos = slice->pos;

            unsigned int startX = 0;
            unsigned int startY = slice->pos;
            unsigned int maxHeight = 0;
            
            for(auto& [glyph, charData] : font->m_characterGlyphs)
            {
                const Vec2ui sz = Vec2ui(static_cast<unsigned int>(charData.m_size.x), static_cast<unsigned int>(charData.m_size.y));
 
                if(startX + sz.x >= m_size.x)
                {
                    startX = 0;
                    startY += maxHeight + 1;
                    maxHeight = 0;
                }
                
                unsigned int startOffset = startY * m_size.x + startX;
                                
                const Vec2 uv1 = Vec2(static_cast<float>(startX) / static_cast<float>(m_size.x), static_cast<float>(startY) / m_size.y);
                const Vec2 uv2 = Vec2(static_cast<float>(startX + sz.x) / static_cast<float>(m_size.x), static_cast<float>(startY) / m_size.y);
                const Vec2 uv3 = Vec2(static_cast<float>(startX + sz.x) / static_cast<float>(m_size.x), static_cast<float>(startY + sz.y) / m_size.y);
                const Vec2 uv4 = Vec2(static_cast<float>(startX) / static_cast<float>(m_size.x), static_cast<float>(startY + sz.y) / m_size.y);
                charData.m_uv12 = Vec4(uv1.x, uv1.y, uv2.x, uv2.y);
                charData.m_uv34 = Vec4(uv3.x, uv3.y, uv4.x, uv4.y);
                
                const size_t width = static_cast<size_t>(charData.m_size.x);
    
                for(unsigned int row = 0; row < sz.y; row++)
                {
                    LINAVG_MEMCPY(m_data + startOffset, &charData.m_buffer[width * row], width);
                    startOffset += m_size.x;
                }
                
                maxHeight = Math::Max(maxHeight, static_cast<unsigned int>(charData.m_size.y));
                startX += static_cast<unsigned int>(charData.m_size.x) + 1;
            
            }
            
            Slice* newSlice = new Slice(slice->pos + font->m_atlasRectHeight, slice->height - font->m_atlasRectHeight);
            m_availableSlices.push_back(newSlice);
            auto it = std::find_if(m_availableSlices.begin(), m_availableSlices.end(), [slice](Slice* s) -> bool { return s == slice; });
            m_availableSlices.erase(it);
            delete slice;
            
            m_updateFunc(this);
            return true;
        }
        
        return false;
    }


    void Atlas::RemoveFont(Font *font)
    {
        Slice* slice = new Slice(font->m_atlasRectPos, font->m_atlasRectHeight);
        m_availableSlices.push_back(slice);
    }

	Font* Text::LoadFont(const char* file, bool loadAsSDF, int size, GlyphEncoding* customRanges, int customRangesSize, bool useKerningIfAvailable)
	{
		FT_Face face;
		if (FT_New_Face(g_ftLib, file, 0, &face))
		{
			if (Config.errorCallback)
				Config.errorCallback("LinaVG: Freetype Error -> Failed to load the font!");
			return nullptr;
		}

		return SetupFont(face, loadAsSDF, size, customRanges, customRangesSize, useKerningIfAvailable);
	}

	Font* Text::LoadFontFromMemory(void* data, size_t dataSize, bool loadAsSDF, int size, GlyphEncoding* customRanges, int customRangesSize, bool useKerningIfAvailable)
	{
		FT_Face face;
		if (FT_New_Memory_Face(g_ftLib, static_cast<FT_Byte*>(data), static_cast<FT_Long>(dataSize), 0, &face))
		{
			if (Config.errorCallback)
				Config.errorCallback("LinaVG: Freetype Error -> Failed to load the font!");
			return nullptr;
		}

		return SetupFont(face, loadAsSDF, size, customRanges, customRangesSize, useKerningIfAvailable);
	}

	Font* Text::SetupFont(FT_Face& face, bool loadAsSDF, int size, GlyphEncoding* customRanges, int customRangesSize, bool useKerningIfAvailable)
	{
		FT_Error err = FT_Set_Pixel_Sizes(face, 0, size);

		if (err)
			Config.errorCallback("LinaVG: Error on FT_Set_Pixel_Sizes!");

		err = FT_Select_Charmap(face, ft_encoding_unicode);

		if (err)
			Config.errorCallback("LinaVG: Error on FT_Select_Charmap!");

		Font* font		= new Font();
		font->m_supportsUnicode = customRanges != nullptr;
		font->m_size			= size;
		font->m_isSDF			= loadAsSDF;
		font->m_newLineHeight	= static_cast<float>(face->size->metrics.height) / 64.0f;
		font->m_supportsKerning = useKerningIfAvailable && FT_HAS_KERNING(face) != 0;

		// int		 maxHeight		   = 0;
		auto&		 characterMap	   = font->m_characterGlyphs;
		FT_GlyphSlot slot			   = face->glyph;

        unsigned int sizeCtrX = 0;
        unsigned int sizeCtrY = 0;
        
		auto setSizes = [&](FT_ULong c) {
			auto i = FT_Get_Char_Index(face, c);

			// not found.
			if (i == 0)
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

            sizeCtrY = Math::Max(sizeCtrY, glyphRows);
            
            if(sizeCtrX + glyphWidth >= Config.maxFontAtlasSize)
            {
                sizeCtrX = 0;
                font->m_atlasRectHeight += sizeCtrY + 1;
            }
            
            sizeCtrX += glyphWidth + 1;
            
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

        font->m_atlasRectHeight += sizeCtrY + 1;
        
        font->m_spaceAdvance = characterMap[' '].m_advance.x;
      
        Atlas* foundAtlas = nullptr;
        
        for(Atlas* atlas : m_atlases)
        {
            if(atlas->AddFont(font))
            {
                foundAtlas = atlas;
                break;
            }
        }
        
        if(foundAtlas == nullptr)
        {
            Atlas* newAtlas = new Atlas(Vec2ui(Config.maxFontAtlasSize, Config.maxFontAtlasSize), m_callbacks.atlasNeedsUpdate);
            
            if(!newAtlas->AddFont(font))
            {
                if (Config.errorCallback)
                    Config.errorCallback("LinaVG: Could not fit font!");
            }
            
            m_atlases.push_back(newAtlas);
        }

        /*
		w = Math::Max(w, roww);
		h += rowh;

		int availableAtlasIndex = -1;
		for (int i = 0; i < m_textData.m_createdAtlases.m_size; i++)
		{
			FontAtlas& atlas = m_textData.m_createdAtlases[i];
			// const int  totalSize = Config.maxFontAtlasSize - atlas.m_currentOffsetX + Config.maxFontAtlasSize - atlas.m_currentOffsetY;
			if (Config.maxFontAtlasSize - atlas.m_currentOffsetY > h)
			{
				availableAtlasIndex = i;

				if (m_callbacks.fontTextureBind)
					m_callbacks.fontTextureBind(atlas.m_texture);
				else
				{
					if (Config.errorCallback)
						Config.errorCallback("LinaVG: Callback FontTextureBind is not set!");
				}
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
			offsetX				   = m_textData.m_createdAtlases[availableAtlasIndex].m_currentOffsetX;
			offsetY				   = m_textData.m_createdAtlases[availableAtlasIndex].m_currentOffsetY;
			availableAtlasRowSizeY = m_textData.m_createdAtlases[availableAtlasIndex].m_rowSizeY;
			font->m_texture		   = m_textData.m_createdAtlases[availableAtlasIndex].m_texture;
			usedAtlasIndex		   = availableAtlasIndex;
		}
		else
		{
			FontAtlas atlas;

			if (m_callbacks.fontTextureCreate)
				atlas.m_texture = m_callbacks.fontTextureCreate(w, h);
			else
			{
				if (Config.errorCallback)
					Config.errorCallback("LinaVG: Callback FontTextureCreate is not set!");
			}
			font->m_texture = atlas.m_texture;
			usedAtlasIndex	= m_textData.m_createdAtlases.m_size;
			m_textData.m_createdAtlases.push_back(atlas);
		}

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

			const Vec4 uv12 = Vec4(uv1.x, uv1.y, uv2.x, uv2.y);
			const Vec4 uv34 = Vec4(uv3.x, uv3.y, uv4.x, uv4.y);

			if (ch.second.m_buffer != nullptr)
			{
				if (m_callbacks.fontTextureBufferData)
					m_callbacks.fontTextureBufferData(glyphWidth, glyphRows, offsetX, offsetY, ch.second.m_buffer);
				else
				{
					if (Config.errorCallback)
						Config.errorCallback("LinaVG: Callback FontTextureBufferData is not set!");
				}
				buffered = true;
				LINAVG_FREE(ch.second.m_buffer);
			}

			ch.second.m_buffer = nullptr;
			ch.second.m_uv12   = uv12;
			ch.second.m_uv34   = uv34;

			rowh												   = Math::Max(rowh, glyphRows);
			m_textData.m_createdAtlases[usedAtlasIndex].m_rowSizeY = rowh;
			offsetX += glyphWidth + bufferCharSpacing;
		}

		if (buffered)
		{
			if (m_callbacks.fontTextureBufferData)
				m_callbacks.fontTextureBufferEnd();
			else
			{
				if (Config.errorCallback)
					Config.errorCallback("LinaVG: Callback FontTextureBufferEnd is not set!");
			}
		}

		if (usedAtlasIndex != -1)
		{
			m_textData.m_createdAtlases[usedAtlasIndex].m_currentOffsetX = offsetX;
			m_textData.m_createdAtlases[usedAtlasIndex].m_currentOffsetY = offsetY;
		}
*/
        
		err					 = FT_Done_Face(face);
		if (err)
			Config.errorCallback("LinaVG: Error on FT_Done_Face!");
		Config.logCallback("LinaVG: Successfuly loaded font!");
		return font;
	}
} // namespace LinaVG::Text
#endif
