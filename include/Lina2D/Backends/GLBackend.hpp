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

#ifndef Lina2DGLBackend_HPP
#define Lina2DGLBackend_HPP

// Headers here.
#include "Lina2D/Core/Common.hpp"

namespace LinaVG::Backend
{
    void          Initialize();
    void          Terminate();
    void          StartFrame();
    void          DrawGradient(GradientDrawBuffer* buf);
    void          DrawTextured(TextureDrawBuffer* buf);
    void          DrawDefault(DrawBuffer* buf);
    void          EndFrame();
    void          AddShaderUniforms(BackendHandle shader);
    BackendHandle CreateShader(const char* vert, const char* frag);

} // namespace Lina2D::Backend

#endif