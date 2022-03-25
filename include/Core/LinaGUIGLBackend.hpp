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
Class: LinaGUIGLBackend



Timestamp: 3/24/2022 11:33:52 PM
*/

#pragma once

#ifndef LinaGUIGLBackend_HPP
#define LinaGUIGLBackend_HPP

// Headers here.
#include "LinaGUI.hpp"
#include <glad/glad.h>

namespace Lina
{
    struct LGInitOptions;
} // namespace Lina
namespace Lina
{
    class Backend
    {
    public:
        static GLchar*      g_lineVertexShader;
        static GLchar*      g_lineFragShader;
        static GLint        g_projMatrixLoc;
        static GLuint       g_vbo;
        static GLuint       g_ebo;
        static GLuint       g_vao;
        static unsigned int g_lineShader;

        static void InitializeBackend();
        static void StartBackend();
        static void RenderBackend();
        static void EndBackend();
    };
} // namespace Lina

#endif
