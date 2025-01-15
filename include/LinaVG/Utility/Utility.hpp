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

/*
Timestamp: 3/28/2022 2:38:21 PM
*/

#pragma once

#ifndef LinaVGUtility_HPP
#define LinaVGUtility_HPP

#include "../Core/Common.hpp"

namespace LinaVG
{
	namespace Utility
	{
		// https://gist.github.com/hwei/1950649d523afd03285c
		class FnvHash
		{
			static const unsigned int FNV_PRIME	   = 16777619u;
			static const unsigned int OFFSET_BASIS = 2166136261u;
			template <unsigned int N>
			static constexpr unsigned int fnvHashConst(const char (&str)[N], unsigned int I = N)
			{
				return I == 1 ? (OFFSET_BASIS ^ str[0]) * FNV_PRIME : (fnvHashConst(str, I - 1) ^ str[I - 1]) * FNV_PRIME;
			}
			static uint32_t fnvHash(const char* str)
			{
				const size_t length = strlen(str) + 1;
				uint32_t	 hash	= OFFSET_BASIS;
				for (size_t i = 0; i < length; ++i)
				{
					hash ^= *str++;
					hash *= FNV_PRIME;
				}
				return hash;
			}
			struct Wrapper
			{
				Wrapper(const char* str)
					: str(str)
				{
				}
				const char* str;
			};
			unsigned int hash_value;

		public:
			// calulate in run-time
			FnvHash(Wrapper wrapper)
				: hash_value(fnvHash(wrapper.str))
			{
			}
			// calulate in compile-time
			template <unsigned int N>
			constexpr FnvHash(const char (&str)[N])
				: hash_value(fnvHashConst(str))
			{
			}
			// output result
			constexpr operator unsigned int() const
			{
				return this->hash_value;
			}
		};

		template <typename T>
		int QuickSortPartition(Array<T>& arr, int start, int end)
		{
			int pivot = arr[start];

			int count = 0;
			for (int i = start + 1; i <= end; i++)
			{
				if (arr[i] <= pivot)
					count++;
			}

			// Giving pivot element its correct position
			int pivotIndex = start + count;
			arr.swap(pivotIndex, start);

			// Sorting left and right parts of the pivot element
			int i = start, j = end;

			while (i < pivotIndex && j > pivotIndex)
			{

				while (arr[i] <= pivot)
				{
					i++;
				}

				while (arr[j] > pivot)
				{
					j--;
				}

				if (i < pivotIndex && j > pivotIndex)
				{
					arr.swap(i++, j--);
				}
			}

			return pivotIndex;
		}

		template <typename T>
		void QuickSortArray(Array<T>& arr, int start, int end)
		{
			if (start >= end)
				return;

			int p = QuickSortPartition<T>(arr, start, end);

			//  Left
			QuickSortArray<T>(arr, start, p - 1);

			// Right
			QuickSortArray<T>(arr, p + 1, end);
		}

		int	 GetTextCharacterSize(const char* text);
		Vec4 HexToVec4(int hex);

	} // namespace Utility
} // namespace LinaVG
#endif
