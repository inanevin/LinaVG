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

#include "Core/Text.hpp"
#include "Core/Renderer.hpp"
#include "Core/Backend.hpp"
#include "Core/Math.hpp"
#define MAX_WIDTH 600
#include <iostream>

namespace LinaVG
{
    namespace Text
    {
        bool Initialize()
        {
            if (FT_Init_FreeType(&Internal::g_textData.m_ftlib))
            {
                Config.m_logCallback("LinaVG: Error initializing FreeType Library");
                return false;
            }

            return true;
        }

        void Terminate()
        {
            FT_Done_FreeType(Internal::g_textData.m_ftlib);

            for (int i = 0; i < Internal::g_textData.m_loadedFonts.m_size; i++)
                delete Internal::g_textData.m_loadedFonts[i];

            Internal::g_textData.m_loadedFonts.clear();
        }
    } // namespace Text

    FontHandle LoadFont(const char* file, bool loadAsSDF, int size, GlyphEncoding* customRanges, int customRangesSize)
    {
        FT_Face face;
        if (FT_New_Face(Internal::g_textData.m_ftlib, file, 0, &face))
        {
            Config.m_errorCallback("LinaVG: Freetype Error -> Failed to load the font!");
            return -1;
        }

        return Internal::SetupFont(face, loadAsSDF, size, customRanges, customRangesSize);
    }

    LINAVG_API FontHandle LoadFontFromMemory(void* data, size_t dataSize, bool loadAsSDF, int size, GlyphEncoding* customRanges, int customRangesSize)
    {
        FT_Face face;
        if (FT_New_Memory_Face(Internal::g_textData.m_ftlib, static_cast<FT_Byte*>(data), static_cast<FT_Long>(dataSize), 0, &face))
        {
            Config.m_errorCallback("LinaVG: Freetype Error -> Failed to load the font!");
            return -1;
        }

        return Internal::SetupFont(face, loadAsSDF, size, customRanges, customRangesSize);
    }

    LINAVG_API void SetDefaultFont(FontHandle activeFont)
    {
        _ASSERT(activeFont < Internal::g_textData.m_loadedFonts.m_size && activeFont > -1);
        Internal::g_textData.m_defaultFont = activeFont;
    }

    namespace Internal
    {
        TextData   g_textData;
        FontHandle g_fontCounter = 0;

        int SetupFont(FT_Face& face, bool loadAsSDF, int size, GlyphEncoding* customRanges, int customRangesSize)
        {

            FT_Set_Pixel_Sizes(face, 0, size);
            FT_Select_Charmap(face, ft_encoding_unicode);

            // Texture alignment changes might be necessary on some APIs such as OpenGL
            Backend::SaveAPIState();
            
            LinaVGFont* font = new LinaVGFont();
            font->m_size     = size;
            font->m_isSDF    = loadAsSDF;

            auto&        characterMap    = font->m_characterGlyphs;
            int          maxHeight       = 0;
            unsigned int roww            = 0;
            unsigned int rowh            = 0;
            unsigned int w               = 3;
            unsigned int h               = 3;
            FT_GlyphSlot slot            = face->glyph;
            int          textureXAdvance = 3;

            auto setSizes = [&](FT_ULong c) {
                auto           i     = FT_Get_Char_Index(face, c);
                int            error = FT_Load_Glyph(face, i, FT_LOAD_DEFAULT);
                TextCharacter& ch    = characterMap[c];

                if (error)
                {
                    Config.m_errorCallback("LinaVG: Freetype Error -> Failed to load character!");
                    return false;
                }

                if (loadAsSDF)
                    error = FT_Render_Glyph(slot, FT_RENDER_MODE_SDF);
                else
                    error = FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);

                if (error)
                {
                    Config.m_errorCallback("LinaVG: Freetype Error -> Failed to render character!");
                    return false;
                }

                const unsigned int glyphWidth = slot->bitmap.width;
                const unsigned int glyphRows  = slot->bitmap.rows;

                if (roww + glyphWidth + textureXAdvance >= MAX_WIDTH)
                {
                    w = Math::Max(w, roww);
                    h += rowh + 3;
                    roww = 3;
                    rowh = 0;
                }

                const size_t bufSize = static_cast<size_t>(glyphWidth * glyphRows + 1);

                ch.m_buffer = (unsigned char*)LINAVG_MALLOC(bufSize);

                ch.m_size    = Vec2(static_cast<float>(glyphWidth), static_cast<float>(glyphRows));
                ch.m_bearing = Vec2(static_cast<float>(slot->bitmap_left), static_cast<float>(slot->bitmap_top));
                ch.m_advance = Vec2(static_cast<float>(slot->advance.x >> 6), static_cast<float>(slot->advance.y >> 6));

                if (ch.m_buffer != nullptr && slot->bitmap.buffer != nullptr)
                {
                    for (unsigned int row = 0; row < glyphRows; ++row)
                    {
                        for (unsigned int col = 0; col < glyphWidth; ++col)
                        {
                            const int index = row * glyphWidth + col;

                            if (index < bufSize)
                                ch.m_buffer[index] = slot->bitmap.buffer[index];
                            else
                            {
                                Config.m_errorCallback("LinaVG: Can't write to char buffer as index surpasses buffer size!");
                                continue;
                            }
                            // ch.m_buffer[row * glyphWidth + col] = slot->bitmap.buffer[row * slot->bitmap.pitch + col];
                        }
                    }
                }

                roww += glyphWidth + textureXAdvance;
                rowh = Math::Max(rowh, glyphRows);
                return true;
            };

            for (FT_ULong c = 32; c < 128; c++)
                setSizes(c);

            bool useCustomRanges = customRangesSize != 0;
            if (customRangesSize % 2 == 1)
            {
                useCustomRanges = false;
                Config.m_errorCallback("LinaVG: Custom ranges given to font loading must have a size multiple of 2!");
            }

            if (useCustomRanges)
            {
                int       index            = 0;
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

            // Generate atlas
            BackendHandle tex   = Backend::CreateFontTexture(w, h);
            font->m_textureSize = Vec2(static_cast<float>(w), static_cast<float>(h));
            font->m_texture     = tex;
            int offsetX         = 3;
            int offsetY         = 3;
            rowh                = 0;

            for (auto& ch : characterMap)
            {
                const unsigned int glyphWidth = static_cast<unsigned int>(ch.second.m_size.x);
                const unsigned int glyphRows  = static_cast<unsigned int>(ch.second.m_size.y);

                if (offsetX + glyphWidth + textureXAdvance >= MAX_WIDTH)
                {
                    offsetY += rowh + 3;
                    rowh    = 0;
                    offsetX = 3;
                }

                const Vec2  size       = Vec2(static_cast<float>(glyphWidth), static_cast<float>(glyphRows));
                const float fontWidth  = static_cast<float>(w);
                const float fontHeight = static_cast<float>(h);
                const float xx         = (float)offsetX / fontWidth;
                const float yy         = (float)offsetY / fontHeight;
                Vec2        uv1        = Vec2(xx, yy);
                Vec2        uv2        = Vec2(xx + size.x / fontWidth, yy);
                Vec2        uv3        = Vec2(xx + size.x / fontWidth, yy + size.y / fontHeight);
                Vec2        uv4        = Vec2(xx, yy + size.y / fontHeight);

                 Vec2       points[] = {uv1, uv2, uv3, uv4};
                 const Vec2 avg      = Math::GetPolygonCentroidFast(points, 4);
                 uv1                 = Math::ScalePoint(points[0], avg, 1.1f);
                 uv2                 = Math::ScalePoint(points[1], avg, 1.1f);
                 uv3                 = Math::ScalePoint(points[2], avg, 1.1f);
                 uv4                 = Math::ScalePoint(points[3], avg, 1.1f);

                const Vec4 uv12 = Vec4(uv1.x, uv1.y, uv2.x, uv2.y);
                const Vec4 uv34 = Vec4(uv3.x, uv3.y, uv4.x, uv4.y);

                Backend::BufferFontTextureAtlas(glyphWidth, glyphRows, offsetX, offsetY, static_cast<void*>(ch.second.m_buffer));

                LINAVG_FREE(ch.second.m_buffer);
                ch.second.m_buffer = nullptr;
                ch.second.m_uv12   = uv12;
                ch.second.m_uv34   = uv34;

                rowh = Math::Max(rowh, glyphRows);
                offsetX += glyphWidth + textureXAdvance;
            }

            font->m_spaceAdvance = characterMap[' '].m_advance.x;
            FT_Done_Face(face);
            Backend::RestoreAPIState();

            Config.m_logCallback("LinaVG: Successfuly loaded font!");
            Internal::g_fontCounter++;
            Internal::g_textData.m_defaultFont = Internal::g_fontCounter;
            Internal::g_textData.m_loadedFonts.push_back(font);
            return Internal::g_fontCounter;
        }
    } // namespace Internal

} // namespace LinaVG
