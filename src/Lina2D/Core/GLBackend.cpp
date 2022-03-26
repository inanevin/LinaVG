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

#include "Lina2D/Core/GLBackend.hpp"
#include "Lina2D/Core/Renderer.hpp"
#include <iostream>
#include <stdio.h>
#if (__cplusplus >= 201100) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201100)
#define OFFSETOF(_TYPE, _MEMBER) offsetof(_TYPE, _MEMBER) // Offset of _MEMBER within _TYPE. Standardized as offsetof() in C++11
#else
#define OFFSETOF(_TYPE, _MEMBER) ((size_t) & (((_TYPE*)0)->_MEMBER)) // Offset of _MEMBER within _TYPE. Old style macro.
#endif

namespace Lina2D
{
    void Backend::Initialize()
    {
        if (Renderer::g_renderer->m_options.m_customLineVertexShader == nullptr)
        {
            g_lineVertexShader = "#version 330 core\n"
                                 "layout (location = 0) in vec2 pos;\n"
                                 "layout (location = 1) in vec2 uv;\n"
                                 "layout (location = 2) in vec4 col;\n"
                                 "uniform mat4 proj; \n"
                                 "out vec4 fCol;\n"
                                 "out vec2 fUV;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   fCol = col;\n"
                                 "   fUV = uv;\n"
                                 "   gl_Position = proj * vec4(pos.x, pos.y, 0.0f, 1.0);\n"
                                 "}\0";
        }

        if (Renderer::g_renderer->m_options.m_customLineFragShader == nullptr)
        {
            g_lineFragShader = "#version 330 core\n"
                               "out vec4 fragColor;\n"
                               "in vec2 fUV;\n"
                               "in vec4 fCol;\n"
                               "void main()\n"
                               "{\n"
                               "   fragColor = fCol;\n"
                               "}\n\0";
        }

        unsigned int vertex, fragment;
        int          success;
        char         infoLog[512];

        // VTX
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &g_lineVertexShader, NULL);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cout << "Lina GUI ERROR -> Shader vertex compilation failed!\n"
                      << infoLog << std::endl;
        }

        // FRAG
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &g_lineFragShader, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cout << "Lina GUI ERROR -> Shader fragment compilation failed!\n"
                      << infoLog << std::endl;
        }

        g_lineShader = glCreateProgram();
        glAttachShader(g_lineShader, vertex);
        glAttachShader(g_lineShader, fragment);
        glLinkProgram(g_lineShader);

        glGetProgramiv(g_lineShader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(g_lineShader, 512, NULL, infoLog);
            std::cout << "Lina GUI ERROR -> Could not link shader program!\n"
                      << infoLog << std::endl;
        }

        glDeleteShader(vertex);
        glDeleteShader(fragment);

        g_projMatrixLoc = glGetUniformLocation(g_lineShader, "proj");

        glGenVertexArrays(1, &g_vao);
        glGenBuffers(1, &g_vbo);
        glGenBuffers(1, &g_ebo);

        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(g_vao);

        glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
        // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ebo);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));
        glEnableVertexAttribArray(2);

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);

        Renderer::g_renderer->m_drawData.m_vertexBuffer.reserve(8000);
        Renderer::g_renderer->m_drawData.m_indexBuffer.reserve(16000);
    }

    void Backend::StartFrame()
    {
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // glEnable(GL_SCISSOR_TEST);

        glViewport(0, 0, (GLsizei)Renderer::g_renderer->m_options.m_displaySize.x, (GLsizei)Renderer::g_renderer->m_options.m_displaySize.y);
        glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ebo);
    }

    void Backend::Render()
    {
        int fb_width  = (int)(Renderer::g_renderer->m_options.m_displaySize.x * Renderer::g_renderer->m_options.m_framebufferScale.x);
        int fb_height = (int)(Renderer::g_renderer->m_options.m_displaySize.y * Renderer::g_renderer->m_options.m_framebufferScale.y);
        if (fb_width <= 0 || fb_height <= 0 || Renderer::g_renderer->m_drawData.m_vertexBuffer.m_size == 0)
            return;
        // glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);

        float        L    = Renderer::g_renderer->m_options.m_displayPos.x;
        float        R    = Renderer::g_renderer->m_options.m_displayPos.x + Renderer::g_renderer->m_options.m_displaySize.x;
        float        T    = Renderer::g_renderer->m_options.m_displayPos.y;
        float        B    = Renderer::g_renderer->m_options.m_displayPos.y + Renderer::g_renderer->m_options.m_displaySize.y;
        static float zoom = -1.0f;

        Vec2        key    = Renderer::g_renderer->m_keyAxisCallback();
        static Vec2 keyVal = Vec2(0, 0);
        zoom += Renderer::g_renderer->m_mouseScrollCallback() * 0.2f;

        keyVal.x += key.x * 6;
        keyVal.y -= key.y * 6;

        L *= -zoom;
        R *= -zoom;
        T *= -zoom;
        B *= -zoom;

        L += keyVal.x;
        R += keyVal.x;
        T += keyVal.y;
        B += keyVal.y;
        const float ortho_projection[4][4] =
            {
                {2.0f / (R - L), 0.0f, 0.0f, 0.0f},
                {0.0f, 2.0f / (T - B), 0.0f, 0.0f},
                {0.0f, 0.0f, -1, 0.0f},
                {(R + L) / (L - R), (T + B) / (B - T), 0.0f, 1.0f},
            };

        // TODO: do we need to re-gen. vertex array since context won't be shared?
        // g_vao = 0;
        // glGenVertexArrays(1, &g_vao);

        glUseProgram(g_lineShader);
        glUniformMatrix4fv(g_projMatrixLoc, 1, GL_FALSE, &ortho_projection[0][0]);
        glBindVertexArray(g_vao); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

        glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
        glBufferData(GL_ARRAY_BUFFER, Renderer::g_renderer->m_drawData.m_vertexBuffer.m_size * sizeof(Vertex), (const GLvoid*)Renderer::g_renderer->m_drawData.m_vertexBuffer.begin(), GL_STREAM_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, Renderer::g_renderer->m_drawData.m_indexBuffer.m_size * sizeof(Index), (const GLvoid*)Renderer::g_renderer->m_drawData.m_indexBuffer.begin(), GL_STREAM_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawElements(GL_TRIANGLES, (GLsizei)Renderer::g_renderer->m_drawData.m_indexBuffer.m_size, sizeof(Index) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, 0);
    }

    void Backend::EndFrame()
    {
        // Restore state.
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

} // namespace Lina2D::Backend