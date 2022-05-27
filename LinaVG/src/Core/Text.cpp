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
#define MAX_WIDTH 1024

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
        if (FT_New_Memory_Face(Internal::g_textData.m_ftlib, static_cast<FT_Byte*>(data), dataSize, 0, &face))
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
            font->m_size = size;
            font->m_isSDF = loadAsSDF;

            auto& characterMap = font->m_characterGlyphs;
            int   maxHeight = 0;

            unsigned int roww = 0;
            unsigned int rowh = 0;

            unsigned int w = 0;
            unsigned int h = 0;
            FT_GlyphSlot slot = face->glyph;

            FT_Int32 loadFlags = loadAsSDF ? FT_LOAD_DEFAULT : FT_LOAD_RENDER;
            int      textureXAdvance = static_cast<int>(Config.m_framebufferScale.x + 2.0f);

            auto setSizes = [&](FT_ULong c) {
                if (FT_Load_Char(face, c, loadFlags))
                {
                    Config.m_errorCallback("LinaVG: Freetype Error -> Failed to load character!");
                    return false;
                }

                if (loadAsSDF)
                    FT_Render_Glyph(slot, FT_RENDER_MODE_SDF);

                const unsigned int glyphWidth = face->glyph->bitmap.width;
                const unsigned int glyphRows = face->glyph->bitmap.rows;

                if (roww + glyphWidth + textureXAdvance >= MAX_WIDTH)
                {
                    w = Math::Max(w, roww);
                    h += rowh;
                    roww = 0;
                    rowh = 0;
                }

                roww += glyphWidth + textureXAdvance;
                rowh = Math::Max(rowh, glyphRows);
                return true;
            };

            // First calculate the texture atlas size.
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
                int       index = 0;
                const int customRangeCount = customRangesSize / 2;
                for (int i = 0; i < customRangeCount; i++)
                {
                    if (customRanges[index] == customRanges[index + 1])
                        setSizes(customRanges[index]);
                    else
                    {
                        for (FT_ULong c = customRanges[index]; c < customRanges[index + 1]; c++)
                            setSizes(customRanges[index]);
                    }
                    index += 2;
                }
            }

            w = Math::Max(w, roww);
            h += rowh;

            // Generate atlas
            BackendHandle tex = Backend::CreateFontTexture(w, h);
            font->m_textureSize = Vec2(static_cast<float>(w), static_cast<float>(h));
            font->m_texture = tex;
            int offsetX = 0;
            int offsetY = 0;
            rowh = 0;

            auto generateTextures = [&](FT_ULong c) {
                if (FT_Load_Char(face, c, loadFlags))
                {
                    Config.m_errorCallback("LinaVG: Freetype Error -> Failed to load character!");
                    return false;
                }

                if (loadAsSDF)
                    FT_Render_Glyph(slot, FT_RENDER_MODE_SDF);

                unsigned int glyphWidth = face->glyph->bitmap.width;
                unsigned int glyphRows = face->glyph->bitmap.rows;

                if (offsetX + glyphWidth + Config.m_framebufferScale.x + 1.5f >= MAX_WIDTH)
                {

                    offsetY += rowh;
                    rowh = 0;
                    offsetX = 0;
                }

                Backend::BufferFontTextureAtlas(glyphWidth, glyphRows, offsetX, offsetY, static_cast<void*>(face->glyph->bitmap.buffer));
                characterMap[c] = {
                    Vec2(static_cast<float>(offsetX / (float)w), static_cast<float>(offsetY / (float)h)),
                    Vec2(static_cast<float>(glyphWidth), static_cast<float>(glyphRows)),
                    Vec2(static_cast<float>(face->glyph->bitmap_left), static_cast<float>(face->glyph->bitmap_top)),
                    Vec2(static_cast<float>(face->glyph->advance.x >> 6), static_cast<float>(face->glyph->advance.y >> 6)) };

                rowh = Math::Max(rowh, glyphRows);
                offsetX += glyphWidth + textureXAdvance;


                return true;
            };

            // Now atlas is generated, feed each character into it.
            for (FT_ULong c = 32; c < 128; c++)
                generateTextures(c);

            if (useCustomRanges)
            {
                int       index = 0;
                const int customRangeCount = customRangesSize / 2;
                for (int i = 0; i < customRangeCount; i++)
                {
                    if (customRanges[index] == customRanges[index + 1])
                        generateTextures(customRanges[index]);
                    else
                    {
                        for (FT_ULong c = customRanges[index]; c < customRanges[index + 1]; c++)
                            generateTextures(customRanges[index]);
                    }
                    index += 2;
                }
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
