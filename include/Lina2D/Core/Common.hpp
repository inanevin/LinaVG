/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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
Class: Lina2DCommon



Timestamp: 3/26/2022 10:36:46 AM
*/

#pragma once

#ifndef Lina2DCommon_HPP
#define Lina2DCommon_HPP

// Headers here.
#include <iostream>
#include <sstream>
#include <functional>
#include <unordered_map>

namespace Lina2D
{
#define L2D_RAD2DEG 57.2957f
#define L2D_DEG2RAD 0.0174533f
#define LINA2D_API  // TODO

    typedef unsigned int Index;
    typedef unsigned int BackendHandle;

    template <typename T>
    class Array
    {
    public:
        T*  m_data     = nullptr;
        int m_size     = 0;
        int m_lastSize = 0;
        int m_capacity;

        Array()
        {
            m_size = m_capacity = 0;
            m_data              = nullptr;
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
                m_size = m_capacity = 0;
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
            std::memcpy(&m_data[m_size], &v, sizeof(v));
            m_size++;
            return last();
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

        inline int find(const T& t) const
        {
            for (int i = 0; i < m_size; i++)
            {
                if (m_data[i] == t)
                    return i;
            }

            return -1;
        }
    };
    struct Vec4
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

    enum class GradientType
    {
        Horizontal   = 0,
        Vertical     = 1,
        Radial       = 2,
        RadialCorner = 3
    };

    struct Vec4Grad
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

    struct Vec2
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

    struct ThicknessGrad
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

    enum class OutlineDrawDirection
    {
        Outwards,
        Inwards,
        Both
    };

    struct StyleOptions;

    struct OutlineOptions
    {
        static OutlineOptions FromStyle(const StyleOptions& opts, OutlineDrawDirection drawDir);

        /// Outline thickness.
        float m_thickness = 0.0f;

        // If true, bypasses the gradient type property in the outline's color, and always uses vertex colors to colorize from inwards towards outwards.
        bool m_useVertexColors = false;

        /// Where to draw the outline, have no effect on filled shapes.
        OutlineDrawDirection m_drawDirection = OutlineDrawDirection::Outwards;

        /// Outline color, you can set this to 2 different colors & define a gradient type, or construct with a single color for flat shading.
        Vec4Grad m_color = Vec4Grad(Vec4(1, 1, 1, 1));

        /// Set this to a texture handle you've created on your program to draw a texture on top of the shape/line. Set to 0 to clear.
        BackendHandle m_textureHandle = 0;

        /// Defines the texture repetition.
        Vec2 m_textureUVTiling = Vec2(1.0f, 1.0f);

        /// Defines the texture offset.
        Vec2 m_textureUVOffset = Vec2(0.0f, 0.0f);
    };

    /// <summary>
    /// Multicolors: If you want to use one color for each vertex, should point to an array of N colors, while drawing a convex shape.
    /// Passing a pointer to an array with wrong size is undefined behavior. Set to nullptr if you want to use a gradient instead. Construct your gradient with
    /// a single color if you want to use a single color instead.
    /// Rounding: between 0.0f - 1.0f
    /// </summary>
    struct StyleOptions
    {

        StyleOptions(){};
        StyleOptions(const StyleOptions& opts)
        {
            m_color     = opts.m_color;
            m_thickness = opts.m_thickness;
            m_rounding  = opts.m_rounding;

            m_onlyRoundTheseCorners.from(opts.m_onlyRoundTheseCorners);
            m_outlineOptions  = opts.m_outlineOptions;
            m_dropShadow      = opts.m_dropShadow;
            m_dropShadowColor = opts.m_dropShadowColor;
            m_textureHandle   = opts.m_textureHandle;
            m_textureUVTiling = opts.m_textureUVTiling;
            m_textureUVOffset = opts.m_textureUVOffset;
            m_isFilled        = opts.m_isFilled;
        }

        /// Color for the shape, you can set this to 2 different colors & define a gradient type, or construct with a single color for flat shading.
        Vec4Grad m_color = Vec4Grad(Vec4(1, 1, 1, 1));

        /// While drawing lines -> can make a straight line or a line with varying thickness based on start & end.
        /// While drawing non-filled shapes -> only start thickness is used.
        /// While drawing filled shapes, this has no effect.
        ThicknessGrad m_thickness = ThicknessGrad(1.0f);

        /// Rounding for shapes such as rectangles and triangles.
        float m_rounding = 0.0f;

        /// If rounding is to be applied, you can fill this array to only apply rounding to specific corners of the shape.
        Array<int> m_onlyRoundTheseCorners;

        // Outline details.
        OutlineOptions m_outlineOptions;

        /// For both lines and shapes, defines drop shadow angle & radius;
        Vec2 m_dropShadow = Vec2(0.0f, 0.0f);

        /// For both lines and shapes, defines the drop shadow color.
        Vec4 m_dropShadowColor = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

        /// Set this to a texture handle you've created on your program to draw a texture on top of the shape/line. Set to 0 to clear.
        BackendHandle m_textureHandle = 0;

        /// Defines the texture repetition.
        Vec2 m_textureUVTiling = Vec2(1.0f, 1.0f);

        /// Defines the texture offset.
        Vec2 m_textureUVOffset = Vec2(0.0f, 0.0f);

        // Fills inside the shape if true.
        // This does nothing for lines and for any style option sent as "m_outlineOptions"
        bool m_isFilled = false;
    };

    struct Vertex
    {
        Vec2 m_pos;
        Vec2 m_uv;
        Vec4 m_col;
    };

    enum class JointType
    {
        None,
        Miter,
        Bevel,
        MiterBevelAuto,
        Round,
    };

    enum class AAType
    {
        None,
        VertexAA,
        GPU
    };

    struct Configuration
    {
        Vec2 m_displayPos       = Vec2(0, 0);
        Vec2 m_displaySize      = Vec2(0, 0);
        Vec2 m_framebufferScale = Vec2(0, 0);

        JointType m_lineJointType      = JointType::None;
        AAType    m_featheringType     = AAType::None;
        float     m_featheringDistance = 0.0f;

        /// Every m_gcCollectInterval ticks, the system will call garbage collection on vertex & index buffers.
        int m_gcCollectInterval = 180;

        /// For debugging purposes, sets to draw polygon/wireframe mode.
        bool m_wireframeEnabled = false;

        std::function<float()> m_mouseScrollCallback;
        std::function<Vec2()>  m_keyAxisCallback;

        /// Set this to your own function to receive error callbacks from Lina2D.
        std::function<void(const std::string&, int)> m_errorCallback;

        /// For debugging purposes, current count of the trianlges being drawn.
        int m_currentTriangleCount = 0;

        /// For debugging purposes, current count of the vertices sent to backend buffers.
        int m_currentVertexCount = 0;

        /// For debugging purposes, current draw calls.
        int m_currentDrawCalls = 0;

        /// Flips the Y coordinate of texture UVs.
        bool m_flipTextureUVs = false;

        /// Enable-disable anti-aliasing.
        bool m_enableAA = true;
    };

    struct DrawBuffer
    {
        DrawBuffer(){};
        DrawBuffer(int drawOrder)
            : m_drawOrder(drawOrder){};

        Array<Vertex> m_vertexBuffer;
        Array<Index>  m_indexBuffer;
        int           m_drawOrder = -1;

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
        GradientDrawBuffer(const Vec4Grad& g, int drawOrder, bool isAABuffer)
            : m_isAABuffer(isAABuffer), m_color(g), DrawBuffer(drawOrder){};

        bool     m_isAABuffer = false;
        Vec4Grad m_color      = Vec4(1, 1, 1, 1);
    };

    struct TextureDrawBuffer : public DrawBuffer
    {
        TextureDrawBuffer(){};
        TextureDrawBuffer(BackendHandle h, const Vec2& tiling, const Vec2& offset, int drawOrder, bool isAABuffer)
            : m_isAABuffer(isAABuffer), m_textureHandle(h), m_textureUVTiling(tiling), m_textureUVOffset(offset),
              DrawBuffer(drawOrder){};

        bool          m_isAABuffer      = false;
        BackendHandle m_textureHandle   = 0;
        Vec2          m_textureUVTiling = Vec2(1.0f, 1.0f);
        Vec2          m_textureUVOffset = Vec2(0.0f, 0.0f);
    };

    struct RendererData
    {
        Array<DrawBuffer>         m_defaultBuffers;
        Array<GradientDrawBuffer> m_gradientBuffers;
        Array<TextureDrawBuffer>  m_textureBuffers;
        int                       m_gcFrameCounter;
        int                       m_minDrawOrder = -1;
        int                       m_maxDrawOrder = -1;

        void                SetDrawOrderLimits(int drawOrder);
        int                 GetBufferIndexInGradientArray(DrawBuffer* buf);
        int                 GetBufferIndexInTextureArray(DrawBuffer* buf);
        int                 GetBufferIndexInDefaultArray(DrawBuffer* buf);
        DrawBuffer&         GetDefaultBuffer(int drawOrder);
        GradientDrawBuffer& GetGradientBuffer(Vec4Grad& grad, int drawOrder, bool isAABuffer = false);
        TextureDrawBuffer&  GetTextureBuffer(BackendHandle textureHandle, const Vec2& tiling, const Vec2& uvOffset, int drawOrder, bool isAABuffer = false);
    };

    struct BackendData
    {
        BackendHandle                                                                     m_vbo                  = 0;
        BackendHandle                                                                     m_vao                  = 0;
        BackendHandle                                                                     m_ebo                  = 0;
        BackendHandle                                                                     m_defaultShaderHandle  = 0;
        BackendHandle                                                                     m_gradientShaderHandle = 0;
        BackendHandle                                                                     m_texturedShaderHandle = 0;
        std::unordered_map<BackendHandle, std::unordered_map<std::string, BackendHandle>> m_shaderUniformMap;
        float                                                                             m_proj[4][4]                = {0};
        char*                                                                             m_defaultVtxShader          = nullptr;
        char*                                                                             m_defaultFragShader         = nullptr;
        char*                                                                             m_roundedGradientFragShader = nullptr;
        char*                                                                             m_texturedFragShader        = nullptr;
        bool                                                                              m_skipDraw                  = false;
    };

} // namespace Lina2D

#endif
