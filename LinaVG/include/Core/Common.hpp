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
Class: Common



Timestamp: 3/26/2022 10:36:46 AM
*/

#pragma once

#ifndef LinaVGCommon_HPP
#define LinaVGCommon_HPP

// Headers here.
#include <iostream>
#include <sstream>
#include <functional>
#include <unordered_map>

namespace LinaVG
{
#define LVG_RAD2DEG 57.2957f
#define LVG_DEG2RAD 0.0174533f
#define LINAVG_API  // TODO

    typedef unsigned int Index;
    typedef unsigned int BackendHandle;
    typedef uint_fast8_t FontHandle;

    /// <summary>
    /// Color vector, range 0.0f - 1.0f
    /// </summary>
    LINAVG_API struct Vec4
    {
        Vec4(){};
        Vec4(float x, float y, float z, float w)
            : x(x), y(y), z(z), w(w){};
        Vec4(const Vec4& v)
        {
            this->x = v.x;
            this->y = v.y;
            this->z = v.z;
            this->w = v.w;
        }

        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float w = 0.0f;
    };

    LINAVG_API enum class GradientType
    {
        Horizontal   = 0,
        Vertical     = 1,
        Radial       = 2,
        RadialCorner = 3
    };

    LINAVG_API struct Vec4Grad
    {
        Vec4Grad(){};
        Vec4Grad(const Vec4& c1)
            : m_start(c1), m_end(c1){};

        Vec4Grad(const Vec4& c1, const Vec4& c2)
            : m_start(c1), m_end(c2){};

        Vec4         m_start        = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
        Vec4         m_end          = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
        GradientType m_gradientType = GradientType::Horizontal;
        float        m_radialSize   = 1.0f;
    };

    LINAVG_API struct Vec2
    {
        Vec2(){};
        Vec2(float x, float y)
            : x(x), y(y){};
        Vec2(const Vec2& v)
        {
            x = v.x;
            y = v.y;
        }

        float x = 0.0f;
        float y = 0.0f;

        std::string ToString() const
        {
            std::stringstream ss;
            ss << "X:" << x << "   Y:" << y;
            return ss.str();
        }
    };

    LINAVG_API struct ThicknessGrad
    {
        ThicknessGrad(){};
        ThicknessGrad(float start)
            : m_start(start), m_end(start){};
        ThicknessGrad(float start, float end)
            : m_start(start), m_end(end){};

        float m_start = 1.0f;
        float m_end   = 1.0f;
    };

    typedef float Thickness;

    /// <summary>
    /// Custom array for fast-handling vertex & index buffers for vector drawing operations.
    /// Inspired by Dear ImGui's ImVector
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template <typename T>
    class Array
    {
    public:
        T*  m_data     = nullptr;
        int m_size     = 0;
        int m_lastSize = 0;
        int m_capacity = 0;

        Array()
        {
            m_size = m_capacity = m_lastSize = 0;
            m_data                           = nullptr;
        }

        Array(const Array&) = delete;
        Array& operator=(const Array<T>& a) = delete;

        ~Array()
        {
            clear();
        }

        inline void from(const Array& t)
        {
            clear();
            for (int i = 0; i < t.m_size; i++)
                push_back(t.m_data[i]);
        }

        inline void clear()
        {
            if (m_data)
            {
                m_size = m_capacity = m_lastSize = 0;
                std::free(m_data);
                m_data = nullptr;
            }
        }

        inline bool isEmpty()
        {
            return m_size == 0;
        }

        inline int sizeBytes() const
        {
            return m_size * (int)sizeof(T);
        }

        inline T* begin()
        {
            return m_data;
        }

        inline T* end()
        {
            return m_data + m_size;
        }

        inline int growCapacity(int sz) const
        {
            const int newCapacity = m_capacity ? (m_capacity + m_capacity / 2) : 8;
            return newCapacity > sz ? newCapacity : sz;
        }

        inline void resize(int newSize)
        {
            if (newSize > m_capacity)
                reserve(growCapacity(newSize));
            m_size = newSize;
        }

        inline void resize(int newSize, const T& v)
        {
            if (newSize > m_capacity)
                reserve(growCapacity(newSize));
            if (newSize > m_size)
                for (int n = m_size; n < newSize; n++)
                    std::memcpy(&m_data[n], &v, sizeof(v));
            m_size = newSize;
        }

        inline void reserve(int newCapacity)
        {
            if (newCapacity < m_capacity)
                return;
            T* newData = (T*)std::malloc((size_t)newCapacity * sizeof(T));

            if (m_data)
            {
                if (newData != 0)
                    std::memcpy(newData, m_data, (size_t)m_size * sizeof(T));
                std::free(m_data);
            }
            m_data     = newData;
            m_capacity = newCapacity;
        }

        inline void checkGrow()
        {
            if (m_size == m_capacity)
                reserve(growCapacity(m_size + 1));
        }

        inline T* push_back(const T& v)
        {
            checkGrow();
            auto s = sizeof(v);
            std::memcpy(&m_data[m_size], &v, s);
            m_size++;
            return last();
        }

        inline T* push_back_copy(const T& v)
        {
            checkGrow();
            auto s         = sizeof(v);
            m_data[m_size] = v;
            m_size++;
            return last();
        }

        inline T* erase(const T* it)
        {
            _ASSERT(it >= m_data && it < m_data + m_size);
            const ptrdiff_t off = it - m_data;
            std::memmove(m_data + off, m_data + off + 1, ((size_t)m_size - (size_t)off - 1) * sizeof(T));
            m_size--;
            return m_data + off;
        }

        inline T* last()
        {
            return &m_data[m_size - 1];
        }

        inline T& last_ref()
        {
            return m_data[m_size - 1];
        }

        inline T& operator[](int i)
        {
            _ASSERT(i >= 0 && i < m_size);
            return m_data[i];
        }

        inline const T& operator[](int i) const
        {
            _ASSERT(i >= 0 && i < m_size);
            return m_data[i];
        }

        inline int findIndex(const T& t) const
        {
            for (int i = 0; i < m_size; i++)
            {
                if (m_data[i] == t)
                    return i;
            }

            return -1;
        }

        inline T* findAddr(const T& v)
        {
            T*       data     = m_data;
            const T* data_end = m_data + m_size;
            while (data < data_end)
                if (*data == v)
                    break;
                else
                    ++data;
            return data;
        }

        inline void swap(int start, int end)
        {
            _ASSERT(start > -1 && start < m_size && end > -1 && end < m_size);
            T temp        = m_data[start];
            m_data[start] = m_data[end];
            m_data[end]   = temp;
        }
    };

    LINAVG_API enum class OutlineDrawDirection
    {
        Outwards,
        Inwards,
        Both
    };

    struct StyleOptions;

    LINAVG_API struct OutlineOptions
    {
        static OutlineOptions FromStyle(const StyleOptions& opts, OutlineDrawDirection drawDir);

        /// <summary>
        /// Outline thickness.
        /// </summary>
        float m_thickness = 0.0f;

        /// <summary>
        /// Determines where to draw the outline, has no effect on filled shapes and lines.
        /// </summary>
        OutlineDrawDirection m_drawDirection = OutlineDrawDirection::Outwards;

        /// <summary>
        /// Outline color, you can set this to 2 different colors & define a gradient type, or construct with a single color for flat shading.
        /// </summary>
        Vec4Grad m_color = Vec4Grad(Vec4(1, 1, 1, 1));

        /// <summary>
        /// Set this to a texture handle you've created on your program to draw a texture on the outline. Set to 0 to clear.
        /// </summary>
        BackendHandle m_textureHandle = 0;

        /// <summary>
        /// Defines the texture repetition.
        /// </summary>
        Vec2 m_textureUVTiling = Vec2(1.0f, 1.0f);

        /// <summary>
        /// Defines the texture offset.
        /// </summary>
        Vec2 m_textureUVOffset = Vec2(0.0f, 0.0f);
    };

    enum class TextAlignment
    {
        Left = 0,
        Center,
        Right
    };
    /// <summary>
    /// Text styling, DrawText will render the given text as normal or via signed-distance-field (SDF) methods.
    /// This depends on the font handle given with options (or default font if not-provided).
    /// If you DrawText() using a font handle which was generated with SDF option, it's gonna use SDF rendering.
    /// thickness, softness, drop shadow and outline options are only available on SDF rendering.
    /// </summary>
    LINAVG_API struct TextOptions
    {
        TextOptions(){};
        TextOptions(const TextOptions& opts)
        {
            m_font             = opts.m_font;
            m_color            = opts.m_color;
            m_textScale        = opts.m_textScale;
            m_dropShadowColor  = opts.m_dropShadowColor;
            m_dropShadowOffset = opts.m_dropShadowOffset;
        }

        /// <summary>
        /// Font to use while drawing this text. Handles are achieved through LoadFont() method.
        /// </summary>
        FontHandle m_font = 0;

        /// <summary>
        /// Text color, only flat color, horizontal or vertical gradients are supported.
        /// </summary>
        Vec4Grad m_color = Vec4Grad(Vec4(1.0f, 1.0f, 1.0f, 1.0f));

        /// <summary>
        /// Text alignment.
        /// </summary>
        TextAlignment m_alignment = TextAlignment::Left;

        /// <summary>
        /// Multiplies the text vertices, !it is not recommended to change text size based on this scale!
        /// Rather try to load the same font with bigger sizes.
        /// </summary>
        float m_textScale = 1.0f;

        /// <summary>
        /// Defines extra spacing between each letter.
        /// </summary>
        float m_spacing = 0.0f;

        /// <summary>
        /// Spacing used if the text is word-wrapped and dropped to a new line.
        /// </summary>
        float m_newLineSpacing = 0.0f;

        /// <summary>
        /// Text will wrap at, e.g. go to a new line when it reaches = position.x + m_wrapWidth
        /// </summary>
        float m_wrapWidth = 0.0f;

        /// <summary>
        /// Drop shadow color, lol.
        /// </summary>
        Vec4 m_dropShadowColor = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

        /// <summary>
        /// Defines how far the drop shadow is rendered, in screen-units.
        /// Set to 0.0f, 0.0f to disable drop-shadows.
        /// </summary>
        Vec2 m_dropShadowOffset = Vec2(0.0f, 0.0f);

    };

    LINAVG_API struct SDFTextOptions : public TextOptions
    {
        SDFTextOptions()
            : TextOptions(){};
        SDFTextOptions(const SDFTextOptions& opts)
        {
            m_font                   = opts.m_font;
            m_color                  = opts.m_color;
            m_textScale              = opts.m_textScale;
            m_dropShadowColor        = opts.m_dropShadowColor;
            m_dropShadowOffset       = opts.m_dropShadowOffset;
            m_sdfDropShadowThickness = opts.m_sdfDropShadowThickness;
            m_sdfOutlineColor        = opts.m_sdfOutlineColor;
            m_sdfOutlineThickness    = opts.m_sdfOutlineThickness;
            m_sdfSoftness            = opts.m_sdfSoftness;
            m_sdfThickness           = opts.m_sdfThickness;
            m_sdfDropShadowSoftness  = opts.m_sdfDropShadowSoftness;
        }

        /// <summary>
        /// 0.0f - 1.0f range, defines how strongly the text is extruded.
        /// !NOTE! 0.0f might make the text invisible
        /// </summary>
        float m_sdfThickness = 1.0f;

        /// <summary>
        /// Defines text blurring/smoothing. 0.0f - 1.0f range
        /// </summary>
        float m_sdfSoftness = 0.02f;

        /// <summary>
        /// 0.0f - 1.0f range, defines how strongly the outline is extruded.
        /// </summary>
        float m_sdfOutlineThickness = 0.0f;

        /// <summary>
        /// Well, outline color.
        /// </summary>
        Vec4 m_sdfOutlineColor = Vec4(1, 1, 1, 1);

        /// <summary>
        /// 0.0f - 1.0f range, defines how strongly the drop shadow is extruded.
        /// </summary>
        float m_sdfDropShadowThickness = 0.0f;

        /// <summary>
        /// Defines drop shadow blurring/smoothing. 0.0f - 1.0f range
        /// </summary>
        float m_sdfDropShadowSoftness = 0.02f;

        /// <summary>
        /// Flips the alpha mask in-out, can be used to create create cut-out font rendering, e.g. letter is transparent, surrounding
        /// quad is colored.
        /// </summary>
        bool m_flipAlpha = false;
    };
    /// <summary>
    /// Style options used to draw various effects around the target shape.
    /// </summary>
    LINAVG_API struct StyleOptions
    {

        StyleOptions(){};
        StyleOptions(const StyleOptions& opts)
        {
            m_color     = opts.m_color;
            m_thickness = opts.m_thickness;
            m_rounding  = opts.m_rounding;

            m_onlyRoundTheseCorners.from(opts.m_onlyRoundTheseCorners);
            m_outlineOptions  = opts.m_outlineOptions;
            m_textureHandle   = opts.m_textureHandle;
            m_textureUVTiling = opts.m_textureUVTiling;
            m_textureUVOffset = opts.m_textureUVOffset;
            m_isFilled        = opts.m_isFilled;
        }

        /// <summary>
        /// Color for the shape, you can set this to 2 different colors & define a gradient type, or construct with a single color for flat shading.
        /// </summary>
        Vec4Grad m_color = Vec4Grad(Vec4(1, 1, 1, 1));

        /// <summary>
        /// While drawing lines -> can make a straight line or a line with varying thickness based on start & end (only for single line API, e.g. DrawLine()).
        /// While drawing non-filled shapes -> only start thickness is used.
        /// While drawing filled shapes, this has no effect.
        /// </summary>
        ThicknessGrad m_thickness = ThicknessGrad(1.0f);

        /// <summary>
        /// Used for:
        /// - Rounding the corners of the shapes, e.g. rect, triangle
        /// - Line caps rounding
        /// - Line joints rounding
        /// </summary>
        float m_rounding = 0.0f;

        /// <summary>
        /// If rounding is to be applied, you can fill this array to only apply rounding to specific corners of the shape (only for shapes, not lines).
        /// </summary>
        Array<int> m_onlyRoundTheseCorners;

        /// <summary>
        /// Outline details.
        /// </summary>
        OutlineOptions m_outlineOptions;

        /// <summary>
        /// Set this to a texture handle you've created on your program to draw a texture on the shape/line. Set to 0 to clear.
        /// </summary>
        BackendHandle m_textureHandle = 0;

        /// <summary>
        /// Defines the texture repetition.
        /// </summary>
        Vec2 m_textureUVTiling = Vec2(1.0f, 1.0f);

        /// <summary>
        /// Defines the texture offset.
        /// </summary>
        Vec2 m_textureUVOffset = Vec2(0.0f, 0.0f);

        /// <summary>
        /// Fills inside the target shape, e.g. rect, tris, convex, circles, ngons, has no effect on lines.
        /// </summary>
        bool m_isFilled = false;
    };

    struct Vertex
    {
        Vec2 m_pos;
        Vec2 m_uv;
        Vec4 m_col;
    };

    LINAVG_API struct Configuration
    {
        /// <summary>
        /// Set this to your application's display pos (viewport pos).
        /// </summary>
        Vec2 m_displayPos = Vec2(0, 0);

        /// <summary>
        /// Set this to your application's display size.
        /// </summary>
        Vec2 m_displaySize = Vec2(0, 0);

        /// <summary>
        /// Set this to your application's framebuffer scale, e.g. OS scaling factor for high-dpi screens.
        /// </summary>
        Vec2 m_framebufferScale = Vec2(0, 0);

        /// <summary>
        /// Enable-disable anti-aliasing.
        /// </summary>
        bool m_enableAA = true;

        /// <summary>
        /// Size multiplier for AA vertices.
        /// </summary>
        float m_aaMultiplier = 1.0f;

        /// <summary>
        /// If the angle between two lines exceed this limit fall-back to bevel joints from miter joints.
        /// This is because miter joins the line points on intersection, ang with a very small angle (closer to 180) intersections get close to infinity.
        /// </summary>
        float m_miterLimit = 150;

        /// <summary>
        /// Flips the Y coordinate of texture UVs.
        /// </summary>
        bool m_flipTextureUVs = false;

        /// <summary>
        /// If true, texts will be drawn by interpreting each character w/ 32 bit encoding.
        /// Set this to true if you want to draw unicode characters.
        /// Also you need to load the glyph ranges using customRanges in LoadFont.
        /// Do not forget to send your text with in utf8-format, e.g. c++11 > u8"my string"
        /// </summary>
        bool m_useUnicodeEncoding = false;

        /// <summary>
        /// Every interval ticks system will garbage collect all vertex & index buffers, meaning that will clear all the arrays.
        /// On other ticks, arrays are simply resized to 0, avoiding re-allocations on the next frame.
        /// Set to 0 for instant flush on buffers at the end of every frame.
        /// </summary>
        int m_gcCollectInterval = 600;

        /// <summary>
        /// This amount of default buffers are reserved upon Renderer initialization. Saves time from allocating/deallocating buffers in runtime.
        /// </summary>
        int m_defaultBufferReserve = 10;

        /// <summary>
        /// This amount of gradient buffers are reserved upon Renderer initialization. Saves time from allocating/deallocating buffers in runtime.
        /// </summary>
        int m_gradientBufferReserve = 5;

        /// <summary>
        /// This amount of texture buffers are reserved upon Renderer initialization. Saves time from allocating/deallocating buffers in runtime.
        /// </summary>
        int m_textureBufferReserve = 5;

        /// <summary>
        /// This amount of text buffers for text rendering are reserved upon Renderer initialization. Saves time from allocating/deallocating buffers in runtime.
        /// General idea is that each font you load create a new buffer, so you can reserve the same amount of fonts you plan on using on your application.
        /// </summary>
        int m_textBuffersReserve = 10;

        /// <summary>
        /// Set this to your own function to receive error callbacks from LinaVG.
        /// </summary>
        std::function<void(const std::string&)> m_errorCallback;

        /// <summary>
        /// Set this to your own function to receive log callbacks from LinaVG.
        /// </summary>
        std::function<void(const std::string&)> m_logCallback;

        /// <summary>
        /// For debugging purposes, sets to draw polygon/wireframe mode.
        /// </summary>
        bool m_debugWireframeEnabled = false;

        /// <summary>
        /// For debugging purposes, current count of the trianlges being drawn.
        /// </summary>
        int m_debugCurrentTriangleCount = 0;

        /// <summary>
        /// For debugging purposes, current count of the vertices sent to backend buffers.
        /// </summary>
        int m_debugCurrentVertexCount = 0;

        /// <summary>
        /// For debugging purposes, current draw calls.
        /// </summary>
        int m_debugCurrentDrawCalls = 0;

        /// <summary>
        /// For debugging purposes, zooms the rendering ortho projection.
        /// </summary>
        float m_debugOrthoProjectionZoom = 1.0f;

        /// <summary>
        /// For debugging purposes, offsets the rendering ortho projection.
        /// </summary>
        Vec2 m_debugOrthoOffset = Vec2(0.0f, 0.0f);
    };

    /// <summary>
    /// Main configurations for LinaVG API, contains settings for debug options, line joint angles and AA.
    /// </summary>
    extern LINAVG_API Configuration Config;

    enum class DrawBufferType
    {
        Default,
        Gradient,
        Textured,
        SimpleText,
        SDFText,
    };

    enum class DrawBufferShapeType
    {
        DropShadow,
        Shape,
        Outline,
        AA,
    };

    struct DrawBuffer
    {
        DrawBuffer(){};
        DrawBuffer(int drawOrder, DrawBufferType type, DrawBufferShapeType shapeType)
            : m_drawOrder(drawOrder), m_drawBufferType(type), m_shapeType(shapeType){};

        Array<Vertex>       m_vertexBuffer;
        Array<Index>        m_indexBuffer;
        int                 m_drawOrder      = -1;
        DrawBufferType      m_drawBufferType = DrawBufferType::Default;
        DrawBufferShapeType m_shapeType      = DrawBufferShapeType::Shape;

        inline void Clear()
        {
            m_vertexBuffer.clear();
            m_indexBuffer.clear();
        }

        inline void ResizeZero()
        {
            m_vertexBuffer.resize(0);
            m_indexBuffer.resize(0);
        }

        inline void PushVertex(const Vertex& v)
        {
            m_vertexBuffer.push_back(v);
        }

        inline void PushIndex(Index i)
        {
            m_indexBuffer.push_back(i);
        }

        inline Vertex* LastVertex()
        {
            return m_vertexBuffer.last();
        }
    };

    struct GradientDrawBuffer : public DrawBuffer
    {
        GradientDrawBuffer(){};
        GradientDrawBuffer(const Vec4Grad& g, int drawOrder, DrawBufferShapeType shapeType)
            : m_isAABuffer(shapeType == DrawBufferShapeType::AA), m_color(g), DrawBuffer(drawOrder, DrawBufferType::Gradient, shapeType){};

        bool     m_isAABuffer = false;
        Vec4Grad m_color      = Vec4(1, 1, 1, 1);
    };

    struct TextureDrawBuffer : public DrawBuffer
    {
        TextureDrawBuffer(){};
        TextureDrawBuffer(BackendHandle h, const Vec2& tiling, const Vec2& offset, int drawOrder, DrawBufferShapeType shapeType)
            : m_isAABuffer(shapeType == DrawBufferShapeType::AA), m_textureHandle(h), m_textureUVTiling(tiling), m_textureUVOffset(offset),
              DrawBuffer(drawOrder, DrawBufferType::Textured, shapeType){};

        bool          m_isAABuffer      = false;
        BackendHandle m_textureHandle   = 0;
        Vec2          m_textureUVTiling = Vec2(1.0f, 1.0f);
        Vec2          m_textureUVOffset = Vec2(0.0f, 0.0f);
    };

    struct SimpleTextDrawBuffer : public DrawBuffer
    {
        SimpleTextDrawBuffer(){};
        SimpleTextDrawBuffer(BackendHandle glyphHandle, int drawOrder, bool isDropShadow)
            : m_textureHandle(glyphHandle), m_isDropShadow(isDropShadow),
              DrawBuffer(drawOrder, DrawBufferType::SimpleText, isDropShadow ? DrawBufferShapeType::DropShadow : DrawBufferShapeType::Shape){};

        BackendHandle m_textureHandle = 0;
        bool          m_isDropShadow  = false;
    };

    struct SDFTextDrawBuffer : public DrawBuffer
    {
        SDFTextDrawBuffer(){};
        SDFTextDrawBuffer(BackendHandle glyphHandle, int drawOrder, const SDFTextOptions& opts, bool isDropShadow)
            : m_textureHandle(glyphHandle), m_thickness(opts.m_sdfThickness),
              m_softness(opts.m_sdfSoftness), m_outlineThickness(opts.m_sdfOutlineThickness),
              m_outlineColor(opts.m_sdfOutlineColor), m_flipAlpha(opts.m_flipAlpha), m_isDropShadow(isDropShadow),
              DrawBuffer(drawOrder, DrawBufferType::SDFText, isDropShadow ? DrawBufferShapeType::DropShadow : DrawBufferShapeType::Shape){};

        bool          m_isDropShadow     = false;
        bool          m_flipAlpha        = false;
        float         m_thickness        = 0.0f;
        float         m_softness         = 0.0f;
        float         m_outlineThickness = 0.0f;
        Vec4          m_outlineColor     = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
        BackendHandle m_textureHandle    = 0;
    };

} // namespace LinaVG

#endif
