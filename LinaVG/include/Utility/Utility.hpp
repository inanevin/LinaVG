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
Timestamp: 3/28/2022 2:38:21 PM
*/

#pragma once

#ifndef LinaVGUtility_HPP
#define LinaVGUtility_HPP

#include "Core/Common.hpp"

namespace LinaVG
{
	namespace Utility
	{
        template<typename T>
        int QuickSortPartition(Array<T>& arr, int start, int end)
        {
            int pivot = arr[start];

            int count = 0;
            for (int i = start + 1; i <= end; i++) {
                if (arr[i] <= pivot)
                    count++;
            }

            // Giving pivot element its correct position
            int pivotIndex = start + count;
            arr.swap(pivotIndex, start);

            // Sorting left and right parts of the pivot element
            int i = start, j = end;

            while (i < pivotIndex && j > pivotIndex) {

                while (arr[i] <= pivot) {
                    i++;
                }

                while (arr[j] > pivot) {
                    j--;
                }

                if (i < pivotIndex && j > pivotIndex) {
                    arr.swap(i++, j--);
                }
            }

            return pivotIndex;
        }

        template<typename T>
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

        int GetTextCharacterSize(const char* text);
        Vec4 HexToVec4(int hex);

	}
}
#endif
