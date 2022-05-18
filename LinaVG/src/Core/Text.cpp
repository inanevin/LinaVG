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

namespace LinaVG
{
    namespace Internal
    {
        TextData g_textData;
    }

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
        }
    } // namespace Text

    bool LoadFont(const std::string& file, int height)
    {
        FT_Face face;
        if (FT_New_Face(Internal::g_textData.m_ftlib, file.c_str(), 0, &face))
        {
            Config.m_errorCallback("LinaVG: Freetype Error -> Failed to load the font!");
            return false;
        }

        FT_Set_Pixel_Sizes(face, 0, height);

        auto& characterMap = Internal::g_textData.m_loadedFonts[file];
        for (unsigned char c = 0; c < Config.m_maxGlyphCharSize; c++)
        {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                Config.m_errorCallback("LinaVG: Freetype Error -> Failed to load character!");
                continue;
            }

            BackendHandle generatedTexture = Backend::GenerateFontTexture(face->glyph->bitmap.width, face->glyph->bitmap.rows, static_cast<void*>(face->glyph->bitmap.buffer));

            characterMap[c] = {
                generatedTexture,
                Vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                Vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x};
        }

        FT_Done_Face(face);
        Config.m_logCallback("LinaVG: Successfuly loaded font!");
    }

} // namespace LinaVG
