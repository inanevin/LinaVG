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

/*
Class: Text

Timestamp: 12/29/2018 10:43:46 PM
*/

#pragma once
#ifndef LinaVGText_HPP
#define LinaVGText_HPP

#include "Common.hpp"
#include <unordered_map>
#include <functional>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace LinaVG
{
    typedef FT_ULong GlyphEncoding;

    struct TextCharacter
    {
        Vec4  m_uv12    = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
        Vec4  m_uv34    = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
        Vec2  m_size    = Vec2(0.0f, 0.0f);
        Vec2  m_bearing = Vec2(0.0f, 0.0f);
        Vec2  m_advance = Vec2(0.0f, 0.0f);
        float m_ascent  = 0.0f;
        float m_descent = 0.0f;

        /// Cleaned after load.
        unsigned char* m_buffer = nullptr;
    };

    class LinaVGFont
    {
    public:
        BackendHandle                            m_texture       = 0;
        int                                      m_size          = 0;
        float                                    m_newLineHeight = 0.0f;
        float                                    m_ascent        = 0.0f;
        float                                    m_descent       = 0.0f;
        float                                    m_spaceAdvance  = 0.0f;
        bool                                     m_isSDF         = false;
        LINAVG_MAP<GlyphEncoding, TextCharacter> m_characterGlyphs;
    };

    struct FontAtlas
    {
        BackendHandle m_texture        = 0;
        unsigned int  m_currentOffsetX = 0;
        unsigned int  m_currentOffsetY = 0;
    };
    /// <summary>
    /// Management for text rendering.
    /// </summary>
    struct TextData
    {
        FT_Library m_ftlib       = nullptr;
        int        m_defaultFont = 0;

        /// <summary>
        /// !OFFSETTED BY 1! always access m_loadedFonts[myFontHandle - 1];
        /// </summary>
        Array<LinaVGFont*> m_loadedFonts;

        Array<FontAtlas> m_createdAtlases;
    };

    namespace Text
    {
        LINAVG_API bool Initialize();
        LINAVG_API void Terminate();
    } // namespace Text

    /// <summary>
    /// You can also pass a custom glyph range, currently 32-bit encoding (unsigned long) to load additional glyphs for non-ASCII characters.
    /// </summary>
    /// <returns>Font handle, store this handle if you like to use multiple fonts. You can pass the handle inside TextOptions to draw with a specific font. </returns>

    /// <summary>
    /// Loads the given font & generates textures based on given size.
    /// You can load the same font with different sizes to achieve varying text scales.
    /// Alternatively, you can use the scale modifier in TextOptions but it's not recommended to upscale.
    /// Best quality would be achieved by loading fonts with bigger sizes and scaling them down using TextOptions.
    /// </summary>
    /// <param name="file">TTF or OTF file.</param>
    /// <param name="loadAsSDF">Creates an SDF font.</param>
    /// <param name="size">Font height, width will automatically adjust.</param>
    /// <param name="customRanges">Send custom ranges in UTF32 encoding, e.g. 0x1F028, to load specific characters or sets.</param>
    /// <param name="customRangesSize">Size of the range array, each 2 pair in the array is treated as a range. Needs to be power of 2! </param>
    /// <returns></returns>
    LINAVG_API FontHandle LoadFont(const char* file, bool loadAsSDF, int size = 48, GlyphEncoding* customRanges = nullptr, int customRangesSize = 0);

    /// <summary>
    /// Loads the given font & generates textures based on given size.
    /// You can load the same font with different sizes to achieve varying text scales.
    /// Alternatively, you can use the scale modifier in TextOptions but it's not recommended to upscale.
    /// Best quality would be achieved by loading fonts with bigger sizes and scaling them down using TextOptions.
    /// </summary>
    /// <param name="data">Binary font data.</param>
    /// <param name="size">Binary font data size.</param>
    /// <param name="loadAsSDF">Creates an SDF font.</param>
    /// <param name="size">Font height, width will automatically adjust.</param>
    /// <param name="customRanges">Send custom ranges in UTF32 encoding, e.g. 0x1F028, to load specific characters or sets.</param>
    /// <param name="customRangesSize">Size of the range array, each 2 pair in the array is treated as a range. Needs to be power of 2! </param>
    /// <returns></returns>
    LINAVG_API FontHandle LoadFontFromMemory(void* data, size_t dataSize, bool loadAsSDF, int size = 48, GlyphEncoding* customRanges = nullptr, int customRangesSize = 0);

    /// <summary>
    /// While drawing texts, the system will try to use the font passed inside TextOptions.
    /// If its 0 or doesn't exists, it will fall-back to the default font.
    /// Set the default font handle using this method.
    /// !NOTE!: When you load a font, it's always set as Default font. So only use this method after you are done loading all your fonts.
    /// </summary>
    /// <returns></returns>
    LINAVG_API void SetDefaultFont(FontHandle font);

    namespace Internal
    {
        extern LINAVG_API TextData g_textData;

        /// <summary>
        /// !Internal! Do not modify.
        /// </summary>
        extern LINAVG_API FontHandle g_fontCounter;

        /// <summary>
        /// Uses loaded face (from file or mem) to setup rest of the font data.
        /// </summary>
        int SetupFont(FT_Face& face, bool loadAsSDF, int size, GlyphEncoding* customRanges, int customRangesSize);
    } // namespace Internal

}; // namespace LinaVG

#endif
