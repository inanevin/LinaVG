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

namespace LinaVG
{
    namespace Internal
    {
        TextData   g_textData;
        FontHandle g_fontCounter = 0;
    } // namespace Internal

#define MAX_WIDTH 1024

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

    FontHandle LoadFont(const std::string& file, bool loadAsSDF, int size)
    {
        FT_Face face;
        if (FT_New_Face(Internal::g_textData.m_ftlib, file.c_str(), 0, &face))
        {
            Config.m_errorCallback("LinaVG: Freetype Error -> Failed to load the font!");
            return -1;
        }

        FT_Set_Pixel_Sizes(face, 0, size);
        // FT_Select_Charmap(face, FT_ENCODING_UNICODE);

        // Texture alignment changes might be necessary on some APIs such as OpenGL
        Backend::SaveAPIState();

        LinaVGFont* font = new LinaVGFont();
        font->m_size     = size;
        font->m_isSDF    = loadAsSDF;

        auto& characterMap = font->m_characterGlyphs;
        int   maxHeight    = 0;

        unsigned int roww = 0;
        unsigned int rowh = 0;

        unsigned int w    = 0;
        unsigned int h    = 0;
        FT_GlyphSlot slot = face->glyph;

        FT_Int32 loadFlags = loadAsSDF ? FT_LOAD_DEFAULT : FT_LOAD_RENDER;

        for (unsigned char c = 32; c < Config.m_maxGlyphCharSize; c++)
        {
            if (FT_Load_Char(face, c, loadFlags))
            {
                Config.m_errorCallback("LinaVG: Freetype Error -> Failed to load character!");
                continue;
            }

            if (loadAsSDF)
                FT_Render_Glyph(slot, FT_RENDER_MODE_SDF);

            const unsigned int glyphWidth = face->glyph->bitmap.width;
            const unsigned int glyphRows  = face->glyph->bitmap.rows;

            if (roww + glyphWidth + Config.m_framebufferScale.x + 1.5f >= MAX_WIDTH)
            {
                w = Math::Max(w, roww);
                h += rowh;
                roww = 0;
                rowh = 0;
            }

            roww += glyphWidth + Config.m_framebufferScale.x + 1.5f;
            rowh = Math::Max(rowh, glyphRows);
        }

        w = Math::Max(w, roww);
        h += rowh;

        BackendHandle tex   = Backend::CreateFontTexture(w, h);
        font->m_textureSize = Vec2(w, h);
        font->m_texture     = tex;
        int offsetX         = 0;
        int offsetY         = 0;
        rowh                = 0;

        for (unsigned char c = 32; c < Config.m_maxGlyphCharSize; c++)
        {
            if (FT_Load_Char(face, c, loadFlags))
            {
                Config.m_errorCallback("LinaVG: Freetype Error -> Failed to load character!");
                continue;
            }

            if (loadAsSDF)
                FT_Render_Glyph(slot, FT_RENDER_MODE_SDF);

            const unsigned int glyphWidth = face->glyph->bitmap.width;
            const unsigned int glyphRows  = face->glyph->bitmap.rows;

            if (offsetX + glyphWidth + Config.m_framebufferScale.x + 1.5f >= MAX_WIDTH)
            {

                offsetY += rowh;
                rowh    = 0;
                offsetX = 0;
            }

            Backend::BufferFontTextureAtlas(glyphWidth, glyphRows, offsetX, offsetY, static_cast<void*>(face->glyph->bitmap.buffer));
            // BackendHandle txt = Backend::GenerateFontTexture(glyphWidth, glyphRows, static_cast<void*>(face->glyph->bitmap.buffer));
            characterMap[c] = {
                Vec2(static_cast<float>(offsetX / (float)w), static_cast<float>(offsetY / (float)h)),
                Vec2(static_cast<float>(glyphWidth), static_cast<float>(glyphRows)),
                Vec2(static_cast<float>(face->glyph->bitmap_left), static_cast<float>(face->glyph->bitmap_top)),
                Vec2(static_cast<float>(face->glyph->advance.x >> 6), static_cast<float>(face->glyph->advance.y >> 6))};

            rowh = Math::Max(rowh, glyphRows);
            offsetX += glyphWidth + Config.m_framebufferScale.x + 1.5f;
        }

        FT_Done_Face(face);
        Backend::RestoreAPIState();

        Config.m_logCallback("LinaVG: Successfuly loaded font!");
        Internal::g_fontCounter++;
        Internal::g_textData.m_defaultFont = Internal::g_fontCounter;
        Internal::g_textData.m_loadedFonts.push_back(font);
        return Internal::g_fontCounter;
    }

    LINAVG_API void SetDefaultFont(FontHandle activeFont)
    {
        _ASSERT(activeFont < Internal::g_textData.m_loadedFonts.m_size && activeFont > -1);
        Internal::g_textData.m_defaultFont = activeFont;
    }

} // namespace LinaVG
