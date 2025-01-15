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

namespace LinaVG
{

	/// <summary>
	/// Color vector, range 0.0f - 1.0f
	/// </summary>
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
	};

	struct Vec2ui
	{
		Vec2ui(){};
		Vec2ui(unsigned int x, unsigned int y)
			: x(x), y(y){};
		Vec2ui(const Vec2ui& v)
		{
			x = v.x;
			y = v.y;
		}
		Vec2ui(const Vec2& v)
		{
			x = static_cast<unsigned int>(v.x);
			y = static_cast<unsigned int>(v.y);
		}

		unsigned int x = 0;
		unsigned int y = 0;
	};

	struct Vec4i
	{
		Vec4i(){};
		Vec4i(int x, int y, int z, int w)
			: x(x), y(y), z(z), w(w){};
		Vec4i(const Vec4i& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			w = v.w;
		}

		bool operator==(const Vec4i& other) const
		{
			return x == other.x && y == other.y && z == other.z && w == other.w;
		}
		bool IsOtherInside(const Vec4i& other) const
		{
			return other.x >= x && other.y >= y && other.x + other.z <= x + z && other.y + other.w <= y + w;
		}

		int x = 0;
		int y = 0;
		int z = 0;
		int w = 0;
	};
	struct Vec4ui
	{
		Vec4ui(){};
		Vec4ui(unsigned int x, unsigned int y, unsigned int z, unsigned int w)
			: x(x), y(y), z(z), w(w){};
		Vec4ui(const Vec4ui& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			w = v.w;
		}

		Vec4ui(const Vec2ui& p, const Vec2ui& s)
		{
			x = p.x;
			y = p.y;
			z = s.x;
			w = s.y;
		}

		bool operator==(const Vec4ui& other) const
		{
			return x == other.x && y == other.y && z == other.z && w == other.w;
		}
		bool IsOtherInside(const Vec4ui& other) const
		{
			return other.x >= x && other.y >= y && other.x + other.z <= x + z && other.y + other.w <= y + w;
		}

		bool IsPointInside(const Vec2ui& point) const
		{
			return point.x >= x && point.x <= x + z && point.y >= y && point.y <= y + w;
		}

		unsigned int x = 0;
		unsigned int y = 0;
		unsigned int z = 0;
		unsigned int w = 0;
	};

} // namespace LinaVG
