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

#pragma once
#ifndef LinaVGExampleBackendGL_HPP
#define LinaVGExampleBackendGL_HPP

struct GLFWwindow;

namespace LinaVG
{
    namespace Examples
    {
        class ExampleBackend
        {
        public:
            ExampleBackend()  = default;
            ~ExampleBackend() = default;

            bool InitWindow(int width, int height);
            void Poll();
            void Render();
            void SwapBuffers();
            void Terminate();
            float GetTime();
            
            inline float GetFramebufferScale()
            {
                return m_frameBufferScaleX;
            }

            unsigned int CreateTexture(const char* file);

        private:
            GLFWwindow*            m_window            = nullptr;
            float                  m_frameBufferScaleX = 0.0f;
            float                  m_frameBufferScaleY = 0.0f;
        };
    } // namespace Examples
} // namespace LinaVG

#endif