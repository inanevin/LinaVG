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
Class: GLBackend



Timestamp: 3/24/2022 11:33:52 PM
*/

#pragma once

#ifndef LinaVGGLBackend_HPP
#define LinaVGGLBackend_HPP

// Headers here.
#include "Core/Common.hpp"

namespace LinaVG
{
    namespace Internal
    {
        class ShaderData;
    }
} // namespace LinaVG
namespace LinaVG::Backend
{

    struct GLState
    {
        bool m_blendEnabled       = false;
        bool m_cullFaceEnabled    = false;
        bool m_stencilTestEnabled = false;
        bool m_depthTestEnabled   = false;
        bool m_scissorTestEnabled = false;
        bool m_depthMaskEnabled   = false;
        int  m_blendEq            = 0;
        int  m_blendSrcAlpha      = 0;
        int  m_blendSrcRGB        = 0;
        int  m_blendDestAlpha     = 0;
        int  m_blendDestRGB       = 0;
        int  m_unpackAlignment    = 0;
    };

    // Public API, if you want to implement your own backend, it needs to define these exact signatures.
    bool          Initialize();
    void          Terminate();
    void          StartFrame();
    void          DrawGradient(GradientDrawBuffer* buf);
    void          DrawTextured(TextureDrawBuffer* buf);
    void          DrawDefault(DrawBuffer* buf);
    void          DrawSimpleText(SimpleTextDrawBuffer* buf);
    void          DrawSDFText(SDFTextDrawBuffer* buf);
    void          EndFrame();
    void          BufferFontTextureAtlas(int width, int height, int offsetX, int offsetY, unsigned char* data);
    void          BindFontTexture(BackendHandle texture);
    void          SaveAPIState();
    void          RestoreAPIState();
    BackendHandle CreateFontTexture(int width, int height);

    // Private functionality.
    void SetScissors(BackendHandle x, BackendHandle y, BackendHandle width, BackendHandle height);
    void AddShaderUniforms(Internal::ShaderData& data);
    void CreateShader(Internal::ShaderData& data, const char* vert, const char* frag);

    extern LINAVG_API GLState g_glState;
} // namespace LinaVG::Backend

#endif
