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

// Headers here.
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <cstring>
#include <cstddef>
#include "Vectors.hpp"

namespace LinaVG
{
#define LINAVG_STRING  std::string
#define LINAVG_MAP	   std::unordered_map
#define LINAVG_VEC	   std::vector
#define LINAVG_MEMCPY  std::memcpy
#define LINAVG_MEMSET  std::memset
#define LINAVG_MEMMOVE std::memmove
#define LINAVG_MALLOC  std::malloc
#define LINAVG_FREE	   std::free
#define LVG_RAD2DEG	   57.2957f
#define LVG_DEG2RAD	   0.0174533f
#define LINAVG_API	   // TODO

	typedef unsigned short Index;
	typedef unsigned int   BackendHandle;
	typedef void*		   TextureHandle;
	class Font;

#define NULL_TEXTURE nullptr

	LINAVG_API enum class GradientType
	{
		Horizontal = 0,
		Vertical,
		None,
	};

	LINAVG_API struct Vec4Grad
	{
		Vec4Grad() {};
		Vec4Grad(const Vec4& c1)
			: start(c1), end(c1) {};

		Vec4Grad(const Vec4& c1, const Vec4& c2)
			: start(c1), end(c2) {};

		Vec4		 start		  = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
		Vec4		 end		  = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		GradientType gradientType = GradientType::Horizontal;
	};

	LINAVG_API struct ThicknessGrad
	{
		ThicknessGrad() {};
		ThicknessGrad(float start)
			: start(start), end(start) {};
		ThicknessGrad(float start, float end)
			: start(start), end(end) {};

		float start = 1.0f;
		float end	= 1.0f;
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
		T*						  m_data	 = nullptr;
		int						  m_size	 = 0;
		int						  m_lastSize = 0;
		int						  m_capacity = 0;
		typedef T				  value_type;
		typedef value_type*		  iterator;
		typedef const value_type* const_iterator;
		Array()
		{
			m_size = m_capacity = m_lastSize = 0;
			m_data							 = nullptr;
		}

		Array(const Array<T>& other)
		{
			resize(other.m_size);
			LINAVG_MEMCPY(m_data, other.m_data, size_t(other.m_size) * sizeof(T));
		}

		// inline Array(const Array<T>& src)
		// {
		//     m_size = m_capacity = m_lastSize = 0;
		//     m_data = NULL;
		//     operator        =(src);
		// }

		inline Array<T>& operator=(const Array<T>& other)
		{
			clear();
			resize(other.m_size);
			LINAVG_MEMCPY(m_data, other.m_data, size_t(other.m_size) * sizeof(T));
			return *this;
		}
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
				LINAVG_FREE(m_data);
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
					LINAVG_MEMCPY(&m_data[n], &v, sizeof(v));
			m_size = newSize;
		}

		inline void shrink(int size)
		{
			assert(size <= m_size);
			m_size = size;
		}

		inline void reserve(int newCapacity)
		{
			if (newCapacity < m_capacity)
				return;
			T* newData = (T*)LINAVG_MALLOC((size_t)newCapacity * sizeof(T));

			if (m_data)
			{
				if (newData != 0)
					LINAVG_MEMCPY(newData, m_data, (size_t)m_size * sizeof(T));
				LINAVG_FREE(m_data);
			}
			m_data	   = newData;
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
			LINAVG_MEMCPY(&m_data[m_size], &v, s);
			m_size++;
			return last();
		}

		inline T* erase(const T* it)
		{
			assert(it >= m_data && it < m_data + m_size);
			const std::ptrdiff_t off = it - m_data;
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
			assert(i >= 0 && i < m_capacity);
			return m_data[i];
		}

		inline const T& operator[](int i) const
		{
			assert(i >= 0 && i < m_capacity);
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
			T*		 data	  = m_data;
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
			assert(start > -1 && start < m_size && end > -1 && end < m_size);
			T temp		  = m_data[start];
			m_data[start] = m_data[end];
			m_data[end]	  = temp;
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
		/// Outline m_thickness.
		/// </summary>
		float thickness = 0.0f;

		/// <summary>
		/// Determines where to draw the outline, has no effect on filled shapes and lines.
		/// </summary>
		OutlineDrawDirection drawDirection = OutlineDrawDirection::Outwards;

		/// <summary>
		/// Outline m_color, you can set this to 2 different colors & define a gradient type, or construct with a single m_color for flat shading.
		/// </summary>
		Vec4Grad color = Vec4Grad(Vec4(1, 1, 1, 1));

		/// <summary>
		/// Set this to a texture handle you've created on your program to draw a texture on the outline. Set to 0 to clear.
		/// </summary>
		TextureHandle textureHandle = 0;

		/// <summary>
		/// Defines the texture uv and offset.
		/// </summary>
		Vec4 textureTilingAndOffset = Vec4(1.0f, 1.0f, 0.0f, 0.0f);
	};

	enum class TextAlignment
	{
		Left = 0,
		Center,
		Right
	};

	struct CharacterInfo
	{
		float x		= 0.0f;
		float y		= 0.0f;
		float sizeX = 0.0f;
		float sizeY = 0.0f;
	};

	LINAVG_API struct LineInfo
	{
		/// <summary>
		/// First character index of the line that corresponds to a character in TextOutData.characterInfo
		/// </summary>
		unsigned int startCharacterIndex = 0;

		/// <summary>
		/// Last character index of the line that corresponds to a character in TextOutData.characterInfo
		/// </summary>
		unsigned int endCharacterIndex = 0;

		/// <summary>
		/// Start position x of this line.
		/// </summary>
		float posX = 0.0f;

		/// <summary>
		/// Start position y of this line.
		/// </summary>
		float posY = 0.0f;
	};

	LINAVG_API struct TextOutData
	{
		/// <summary>
		/// Upon drawing a text, this vector contains position and size information for each character.
		/// </summary>
		Array<CharacterInfo> characterInfo;

		/// <summary>
		/// If wrapped text, contains information about each line, empty if not wrapped.
		/// </summary>
		Array<LineInfo> lineInfo;

		void Clear()
		{
			characterInfo.clear();
			lineInfo.clear();
		}

		void Shrink()
		{
			characterInfo.shrink(0);
			lineInfo.shrink(0);
		}
	};

	/// <summary>
	/// Text styling, DrawText will render the given text as normal or via signed-distance-field (SDF) methods.
	/// This depends on the font handle given with options (or default font if not-provided).
	/// If you DrawText() using a font handle which was generated with SDF option, it's gonna use SDF rendering.
	/// </summary>
	LINAVG_API struct TextOptions
	{
		TextOptions() {};
		TextOptions(const TextOptions& opts)
		{
			font		   = opts.font;
			color		   = opts.color;
			textScale	   = opts.textScale;
			alignment	   = opts.alignment;
			spacing		   = opts.spacing;
			newLineSpacing = opts.newLineSpacing;
			wrapWidth	   = opts.wrapWidth;
			wordWrap	   = opts.wordWrap;
			userData	   = opts.userData;
			uniqueID	   = opts.uniqueID;
		}

		bool CheckColors(const Vec4& c1, const Vec4& c2)
		{
			return c1.x == c2.x && c1.y == c2.y && c1.z == c2.z && c1.w == c2.w;
		}

		bool IsSame(const TextOptions& opts)
		{
			if (userData != opts.userData)
				return false;

			if (font != opts.font)
				return false;

			if (color.gradientType != opts.color.gradientType)
				return false;

			if (!CheckColors(color.start, opts.color.start))
				return false;

			if (!CheckColors(color.end, opts.color.end))
				return false;

			if (!CheckColors(cpuClipping, opts.cpuClipping))
				return false;

			if (wordWrap != opts.wordWrap)
				return false;

			if (uniqueID != opts.uniqueID)
				return false;

			return alignment == opts.alignment && textScale == opts.textScale && spacing == opts.spacing && newLineSpacing == opts.newLineSpacing && wrapWidth == opts.wrapWidth;
		}

		/// <summary>
		/// Font to use while drawing this text. Handles are achieved through LoadFont() method.
		/// </summary>
		Font* font = nullptr;

		/// <summary>
		/// Text m_color, only flat m_color, horizontal or vertical gradients are supported.
		/// </summary>
		Vec4Grad color = Vec4Grad(Vec4(1.0f, 1.0f, 1.0f, 1.0f));

		/// <summary>
		/// Text alignment.
		/// </summary>
		TextAlignment alignment = TextAlignment::Left;

		/// <summary>
		/// Multiplies the text vertices, !it is not recommended to change text size based on this scale!
		/// Rather try to load the same font with bigger sizes.
		/// </summary>
		float textScale = 1.0f;

		/// <summary>
		/// Defines extra spacing between each letter.
		/// </summary>
		float spacing = 0.0f;

		/// <summary>
		/// Spacing used if the text is word-wrapped and dropped to a new line.
		/// </summary>
		float newLineSpacing = 0.0f;

		/// <summary>
		/// Text will wrap at, e.g. go to a new line when it reaches = position.x + wrapWidth
		/// </summary>
		float wrapWidth = 0.0f;

		/// <summary>
		/// If wrapWidth != 0, wraps the text word by word. If false, it's per-character based.
		/// </summary>
		bool wordWrap = true;

		/// <summary>
		/// Use to store user data to be passed back to your call handler.
		/// </summary>
		void* userData = nullptr;

		/// <summary>
		/// Defines custom clip rectangle for text vertices.
		/// </summary>
		Vec4 cpuClipping = Vec4(0.0f, 0.0f, 0.0f, 0.0f);

		/// <summary>
		/// User supplied id.
		/// </summary>
		uint64_t uniqueID = 0;
	};

	/// <summary>
	/// Style options used to draw various effects around the target shape.
	/// </summary>
	LINAVG_API struct StyleOptions
	{

		StyleOptions() {};
		StyleOptions(const StyleOptions& opts)
		{
			color		 = opts.color;
			thickness	 = opts.thickness;
			rounding	 = opts.rounding;
			aaMultiplier = opts.aaMultiplier;
			onlyRoundTheseCorners.from(opts.onlyRoundTheseCorners);
			outlineOptions		   = opts.outlineOptions;
			textureHandle		   = opts.textureHandle;
			textureTilingAndOffset = opts.textureTilingAndOffset;
			isFilled			   = opts.isFilled;
			aaEnabled			   = opts.aaEnabled;
			userData			   = opts.userData;
		}

		/// <summary>
		/// Color for the shape, you can set this to 2 different colors & define a gradient type, or construct with a single m_color for flat shading.
		/// </summary>
		Vec4Grad color = Vec4Grad(Vec4(1, 1, 1, 1));

		/// <summary>
		/// While drawing lines -> can make a straight line or a line with varying m_thickness based on start & end (only for single line API, e.g. DrawLine()).
		/// While drawing non-filled shapes -> only start m_thickness is used.
		/// While drawing filled shapes, this has no effect.
		/// </summary>
		ThicknessGrad thickness = ThicknessGrad(1.0f);

		/// <summary>
		/// Always pass between 0.0f and 1.0f
		/// Used for:
		/// - Rounding the corners of the shapes, e.g. rect, triangle
		/// - Line caps rounding
		/// - Line joints rounding
		/// </summary>
		float rounding = 0.0f;

		/// <summary>
		/// Enable/disable AA outlines for this shape.
		/// </summary>
		bool aaEnabled = false;

		/// <summary>
		/// Antialiasing multiplier for the shapes drawn with this style options.
		/// </summary>
		float aaMultiplier = 1.0f;

		/// <summary>
		/// If rounding is to be applied, you can fill this array to only apply rounding to specific corners of the shape (only for shapes, not lines).
		/// </summary>
		Array<int> onlyRoundTheseCorners;

		/// <summary>
		/// Outline details.
		/// </summary>
		OutlineOptions outlineOptions;

		/// <summary>
		/// Set this to a texture handle you've created on your program to draw a texture on the shape/line. Set to 0 to clear.
		/// </summary>
		TextureHandle textureHandle = 0;

		/// <summary>
		/// Defines the texture uv and offset.
		/// </summary>
		Vec4 textureTilingAndOffset = Vec4(1.0f, 1.0f, 0.0f, 0.0f);

		/// <summary>
		/// Fills inside the target shape, e.g. rect, tris, convex, circles, ngons, has no effect on lines.
		/// </summary>
		bool isFilled = true;

		/// <summary>
		/// Use to store user data to be passed back to your call handler.
		/// </summary>
		void* userData = nullptr;

		/// <summary>
		/// Use to store 64 bit unique ID per draw.
		/// </summary>
		uint64_t uniqueID = 0;
	};

	struct Vertex
	{
		Vec2 pos;
		Vec2 uv;
		Vec4 col;
	};

	LINAVG_API struct Configuration
	{
		/// <summary>
		/// Used as an additional scale on AA thickness.
		/// All Style options also have their own/local framebuffer scale, which is multiplied by this value.
		/// </summary>
		float globalAAMultiplier = 1.0f;

		/// <summary>
		/// If the angle between two lines exceed this limit fall-back to bevel joints from miter joints.
		/// This is because miter joins the line points on intersection, ang with a very small angle (closer to 180) intersections get close to infinity.
		/// </summary>
		float miterLimit = 150;

		/// <summary>
		/// Maximum size a font texture atlas can have, all atlasses are square, so this is used for both width and height.
		/// Increase if you are loading a lot of characters or fonts with big sizes (e.g. 100)
		/// You can use Internal::DrawDebugFontAtlas to visualize the atlas target font belongs to.
		/// </summary>
		unsigned int maxFontAtlasSize = 768;

		/// <summary>
		/// Every interval ticks system will garbage collect all vertex and index buffers, meaning that will clear all the arrays.
		/// On other ticks, arrays are simply resized to 0, avoiding re-allocations on the next frame.
		/// Set to 0 for instant flush on buffers at the end of every frame.
		/// Disable using gcCollectEnabled
		/// </summary>
		int gcCollectInterval = 600;

		/// <summary>
		/// Whether gcCollectInterval is used to clear all vertex/index buffers.
		/// </summary>
		bool gcCollectEnabled = true;

		/// <summary>
		/// This amount of buffers are reserved upon Renderer initialization. Saves time from allocating/deallocating buffers in runtime.
		/// </summary>
		int defaultBufferReserve = 50;

		/// <summary>
		/// Amount of vertex buffers reserved for each buffer reserve upon initialization.
		/// </summary>
		int defaultVtxBufferReserve = 100;

		/// <summary>
		/// Amount of idx buffers reserved for each buffer reserve upon initialization.
		/// </summary>
		int defaultIdxBufferReserve = 100;

		/// <summary>
		/// Set this to your own function to receive error callbacks from LinaVG.
		/// </summary>
		std::function<void(const LINAVG_STRING&)> errorCallback;

		/// <summary>
		/// Set this to your own function to receive log callbacks from LinaVG.
		/// </summary>
		std::function<void(const LINAVG_STRING&)> logCallback;

		/// <summary>
		/// Enabling caching allows faster text rendering in exchange for more memory consumption.
		/// Note: dynamic texts you render will not benefit from this.
		/// </summary>
		bool textCachingEnabled = false;

		/// <summary>
		/// Initial reserve for normal text cache, will grow if needed.
		/// </summary>
		int textCacheReserve = 300;

		/// <summary>
		/// Every this amount of ticks the text caches will be cleared up to prevent memory bloating.
		/// </summary>
		int textCacheExpireInterval = 3000;
	};

	/// <summary>
	/// Main configurations for LinaVG API, contains settings for debug options, line joint angles and AA.
	/// </summary>
	extern LINAVG_API Configuration Config;

	enum class DrawBufferType
	{
		Default,
		Text,
		SDFText,
	};

	enum class DrawBufferShapeType
	{
		Shape,
		Text,
		SDFText,
		AA,
	};

	struct DrawBuffer
	{
		DrawBuffer() {};
		DrawBuffer(void* userData, uint64_t uniqueID, int drawOrder, DrawBufferShapeType shapeType, TextureHandle txtHandle, const Vec4& txtUV, const Vec4i& clip)
			: drawOrder(drawOrder), uid(uniqueID), shapeType(shapeType), userData(userData), textureHandle(txtHandle), textureUV(txtUV)
		{
			this->clip = clip;
		};

		Array<Vertex>		vertexBuffer;
		Array<Index>		indexBuffer;
		DrawBufferShapeType shapeType	  = DrawBufferShapeType::Shape;
		TextureHandle		textureHandle = NULL_TEXTURE;
		Vec4				textureUV	  = Vec4(1.0f, 1.0f, 0.0f, 0.0f);
		Vec4i				clip		  = Vec4i(0.0f, 0.0f, 0.0f, 0.0f);
		void*				userData	  = nullptr;
		int					drawOrder	  = -1;
		uint64_t			uid			  = 0;

		bool IsClipDifferent(const Vec4i& clip)
		{
			return !(this->clip == clip);
		}

		inline void Clear()
		{
			vertexBuffer.clear();
			indexBuffer.clear();
		}

		inline void ShrinkZero()
		{
			vertexBuffer.shrink(0);
			indexBuffer.shrink(0);
		}

		inline void PushVertex(const Vertex& v)
		{
			vertexBuffer.push_back(v);
		}

		inline void PushIndex(Index i)
		{
			indexBuffer.push_back(i);
		}

		inline Vertex* LastVertex()
		{
			return vertexBuffer.last();
		}
	};

} // namespace LinaVG
