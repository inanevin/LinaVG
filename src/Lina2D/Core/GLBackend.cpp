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

#include <glad/glad.h>
#include "Lina2D/Core/GLBackend.hpp"
#include "Lina2D/Core/Renderer.hpp"
#include "Lina2D/Core/Drawer.hpp"
#include "Lina2D/Core/Internal.hpp"
#include <iostream>
#include <stdio.h>

#if (__cplusplus >= 201100) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201100)
#define OFFSETOF(_TYPE, _MEMBER) offsetof(_TYPE, _MEMBER) // Offset of _MEMBER within _TYPE. Standardized as offsetof() in C++11
#else
#define OFFSETOF(_TYPE, _MEMBER) ((size_t) & (((_TYPE*)0)->_MEMBER)) // Offset of _MEMBER within _TYPE. Old style macro.
#endif

namespace Lina2D::Backend
{
    void Initialize()
    {
        Internal::g_backendData.m_defaultVtxShader = "#version 330 core\n"
                                                     "layout (location = 0) in vec2 pos;\n"
                                                     "layout (location = 1) in vec2 uv;\n"
                                                     "layout (location = 2) in vec4 col;\n"
                                                     "uniform mat4 proj; \n"
                                                     "out vec4 fCol;\n"
                                                     "out vec2 fUV;\n"
                                                     "out float fYPos;\n"
                                                     "void main()\n"
                                                     "{\n"
                                                     "   fCol = col;\n"
                                                     "   fUV = uv;\n"
                                                     "   gl_Position = proj * vec4(pos.x, pos.y, 0.0f, 1.0);\n"
                                                     "   fYPos = pos.y;\n"
                                                     "}\0";

        Internal::g_backendData.m_defaultFragShader = "#version 330 core\n"
                                                      "out vec4 fragColor;\n"
                                                      "in vec2 fUV;\n"
                                                      "in vec4 fCol;\n"
                                                      "in float fYPos;\n"
                                                      "void main()\n"
                                                      "{\n"
                                                      "   fragColor = fCol;\n"
                                                      "}\n\0";

        Internal::g_backendData.m_texturedFragShader = "#version 330 core\n"
                                                       "out vec4 fragColor;\n"
                                                       "in vec2 fUV;\n"
                                                       "in vec4 fCol;\n"
                                                       "in float fYPos;\n"
                                                       "uniform sampler2D diffuse;\n"
                                                       "uniform vec2 tiling;\n"
                                                       "void main()\n"
                                                       "{\n"
                                                       "   vec2 tiled = vec2(fUV.x * tiling.x, fUV.y * tiling.y);\n"
                                                       "   fragColor = texture(diffuse, fUV * tiling);\n"
                                                       "}\n\0";

        Internal::g_backendData.m_roundedGradientVtxShader = "#version 330 core\n"
                                                             "layout (location = 0) in vec2 pos;\n"
                                                             "layout (location = 1) in vec2 uv;\n"
                                                             "layout (location = 2) in vec4 col;\n"
                                                             "uniform mat4 proj; \n"
                                                             "out vec4 fCol;\n"
                                                             "out vec2 fUV;\n"
                                                             "out float fYPos;\n"
                                                             "void main()\n"
                                                             "{\n"
                                                             "   fCol = col;\n"
                                                             "   fUV = uv;\n"
                                                             "   gl_Position = proj * vec4(pos.x, pos.y, 0.0f, 1.0);\n"
                                                             "   fYPos = pos.y;\n"
                                                             "}\0";

        Internal::g_backendData.m_roundedGradientFragShader = "#version 330 core\n"
                                                              "out vec4 fragColor;\n"
                                                              "in vec2 fUV;\n"
                                                              "in vec4 fCol;\n"
                                                              "in float fYPos;\n"
                                                              "uniform vec4 startColor;\n"
                                                              "uniform vec4 endColor;\n"
                                                              "uniform int  gradientType;\n"
                                                              "uniform float radialSize;\n"
                                                              "void main()\n"
                                                              "{\n"
                                                              "   if(gradientType == 0)\n"
                                                              "     fragColor = mix(startColor, endColor, fUV.x);\n"
                                                              "   else if(gradientType == 1)\n"
                                                              "     fragColor = mix(startColor, endColor, fUV.y);\n"
                                                              "   else if(gradientType == 2) \n"
                                                              "   {\n"
                                                              "       vec2 uv = fUV - vec2(0.5, 0.5);"
                                                              "       float dist = length(uv * radialSize);\n"
                                                              "       fragColor = mix(startColor, endColor, dist);\n"
                                                              "    }\n"
                                                              "   else if(gradientType == 3) \n"
                                                              "   {\n"
                                                              "       float dist = length(fUV * radialSize);\n"
                                                              "       fragColor = mix(startColor, endColor, dist);\n"
                                                              "    }\n"
                                                              "}\n\0";

        Internal::g_backendData.m_defaultShaderHandle  = CreateShader(Internal::g_backendData.m_defaultVtxShader, Internal::g_backendData.m_defaultFragShader);
        Internal::g_backendData.m_gradientShaderHandle = CreateShader(Internal::g_backendData.m_roundedGradientVtxShader, Internal::g_backendData.m_roundedGradientFragShader);
        Internal::g_backendData.m_texturedShaderHandle = CreateShader(Internal::g_backendData.m_defaultVtxShader, Internal::g_backendData.m_texturedFragShader);

        glGenVertexArrays(1, &Internal::g_backendData.m_vao);
        glGenBuffers(1, &Internal::g_backendData.m_vbo);
        glGenBuffers(1, &Internal::g_backendData.m_ebo);

        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(Internal::g_backendData.m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, Internal::g_backendData.m_vbo);
        // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Internal::g_backendData.m_ebo);
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

        // buffer->m_vertexBuffer.reserve(8000);
        // buffer->m_indexBuffer.reserve(16000);
    }

    void Terminate()
    {
    }

    void StartFrame()
    {
        Config.m_currentDrawCalls = 0;
        Config.m_currentTriangleCount = 0;

        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);

        if (Config.m_wireframeEnabled)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // glEnable(GL_SCISSOR_TEST);

        glViewport(0, 0, (GLsizei)Config.m_displaySize.x, (GLsizei)Config.m_displaySize.y);

        int fb_width  = (int)(Config.m_displaySize.x * Config.m_framebufferScale.x);
        int fb_height = (int)(Config.m_displaySize.y * Config.m_framebufferScale.y);
        if (fb_width <= 0 || fb_height <= 0)
        {
            Internal::g_backendData.m_skipDraw = true;
            return;
        }

        Internal::g_backendData.m_skipDraw = false;
        // glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);

        float        L    = Config.m_displayPos.x;
        float        R    = Config.m_displayPos.x + Config.m_displaySize.x;
        float        T    = Config.m_displayPos.y;
        float        B    = Config.m_displayPos.y + Config.m_displaySize.y;
        static float zoom = -1.0f;

        Vec2        key    = Config.m_keyAxisCallback();
        static Vec2 keyVal = Vec2(0, 0);
        zoom += Config.m_mouseScrollCallback() * 0.1f;

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

        Internal::g_backendData.m_proj[0][0] = 2.0f / (R - L);
        Internal::g_backendData.m_proj[0][1] = 0.0f;
        Internal::g_backendData.m_proj[0][2] = 0.0f;
        Internal::g_backendData.m_proj[0][3] = 0.0f;

        Internal::g_backendData.m_proj[1][0] = 0.0f;
        Internal::g_backendData.m_proj[1][1] = 2.0f / (T - B);
        Internal::g_backendData.m_proj[1][2] = 0.0f;
        Internal::g_backendData.m_proj[1][3] = 0.0f;

        Internal::g_backendData.m_proj[2][0] = 0.0f;
        Internal::g_backendData.m_proj[2][1] = 0.0f;
        Internal::g_backendData.m_proj[2][2] = -1.0f;
        Internal::g_backendData.m_proj[2][3] = 0.0f;

        Internal::g_backendData.m_proj[3][0] = (R + L) / (L - R);
        Internal::g_backendData.m_proj[3][1] = (T + B) / (B - T);
        Internal::g_backendData.m_proj[3][2] = 0.0f;
        Internal::g_backendData.m_proj[3][3] = 1.0f;

        glBindVertexArray(Internal::g_backendData.m_vao);
    }

    void DrawGradient(Array<Vertex>& vertices, Array<Index>& indices, const Vec4& startColor, const Vec4& endColor, GradientType type, float radialGradientSize)
    {
        if (Internal::g_backendData.m_skipDraw)
            return;

        glUseProgram(Internal::g_backendData.m_gradientShaderHandle);
        glUniformMatrix4fv(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_gradientShaderHandle]["proj"], 1, GL_FALSE, &Internal::g_backendData.m_proj[0][0]);
        glUniform4f(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_gradientShaderHandle]["startColor"], (GLfloat)startColor.x, (GLfloat)startColor.y, (GLfloat)startColor.z, (GLfloat)startColor.w);
        glUniform4f(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_gradientShaderHandle]["endColor"], (GLfloat)endColor.x, (GLfloat)endColor.y, (GLfloat)endColor.z, (GLfloat)endColor.w);
        glUniform1i(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_gradientShaderHandle]["gradientType"], (GLint)((int)type));
        glUniform1f(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_gradientShaderHandle]["radialSize"], (GLfloat)radialGradientSize);

        glBindBuffer(GL_ARRAY_BUFFER, Internal::g_backendData.m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.m_size * sizeof(Vertex), (const GLvoid*)vertices.begin(), GL_STREAM_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Internal::g_backendData.m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.m_size * sizeof(Index), (const GLvoid*)indices.begin(), GL_STREAM_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.m_size, sizeof(Index) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, 0);
        Config.m_currentDrawCalls++;
        Config.m_currentTriangleCount += int((float)indices.m_size / 3.0f);
    }

    void DrawTextured(Array<Vertex>& vertices, Array<Index>& indices, BackendHandle texture, const Vec2& uvOffset, const Vec2& uvTiling)
    {
        if (Internal::g_backendData.m_skipDraw)
            return;
        const Vec2 uv = Config.m_flipTextureUVs ? Vec2(uvTiling.x, -uvTiling.y) : uvTiling;

        glUseProgram(Internal::g_backendData.m_texturedShaderHandle);
        glUniformMatrix4fv(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_texturedShaderHandle]["proj"], 1, GL_FALSE, &Internal::g_backendData.m_proj[0][0]);

        glUniform1i(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_texturedShaderHandle]["diffuse"], 0);
        glUniform2f(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_texturedShaderHandle]["tiling"], (GLfloat)uv.x, (GLfloat)uv.y);
       // glUniform2f(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_texturedShaderHandle]["offset"], (GLfloat)uvOffset.x, (GLfloat)uvOffset.y);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBindBuffer(GL_ARRAY_BUFFER, Internal::g_backendData.m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.m_size * sizeof(Vertex), (const GLvoid*)vertices.begin(), GL_STREAM_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Internal::g_backendData.m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.m_size * sizeof(Index), (const GLvoid*)indices.begin(), GL_STREAM_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.m_size, sizeof(Index) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, 0);
        Config.m_currentDrawCalls++;
        Config.m_currentTriangleCount += int((float)indices.m_size / 3.0f);
    }

    void DrawDefault(Array<Vertex>& vertices, Array<Index>& indices)
    {
        if (Internal::g_backendData.m_skipDraw)
            return;

        glUseProgram(Internal::g_backendData.m_defaultShaderHandle);
        glUniformMatrix4fv(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_defaultShaderHandle]["proj"], 1, GL_FALSE, &Internal::g_backendData.m_proj[0][0]);

        glBindBuffer(GL_ARRAY_BUFFER, Internal::g_backendData.m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.m_size * sizeof(Vertex), (const GLvoid*)vertices.begin(), GL_STREAM_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Internal::g_backendData.m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.m_size * sizeof(Index), (const GLvoid*)indices.begin(), GL_STREAM_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.m_size, sizeof(Index) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, 0);
        Config.m_currentDrawCalls++;
        Config.m_currentTriangleCount += int((float)indices.m_size / 3.0f);
    }

    void EndFrame()
    {
        // Restore state.
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    BackendHandle CreateShader(const char* vert, const char* frag)
    {
        unsigned int vertex, fragment;
        int          success;
        char         infoLog[512];

        // VTX
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vert, NULL);
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
        glShaderSource(fragment, 1, &frag, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cout << "Lina GUI ERROR -> Shader fragment compilation failed!\n"
                      << infoLog << std::endl;
        }

        GLuint handle = glCreateProgram();
        glAttachShader(handle, vertex);
        glAttachShader(handle, fragment);
        glLinkProgram(handle);

        glGetProgramiv(handle, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(handle, 512, NULL, infoLog);
            std::cout << "Lina GUI ERROR -> Could not link shader program!\n"
                      << infoLog << std::endl;
        }

        glDeleteShader(vertex);
        glDeleteShader(fragment);

        AddShaderUniforms((BackendHandle)handle);

        return (BackendHandle)handle;
    }

    void AddShaderUniforms(BackendHandle shader)
    {
        // Load uniforms.
        GLint numUniforms = 0;
        glGetProgramiv(shader, GL_ACTIVE_UNIFORMS, &numUniforms);

        // Iterate through uniforms.
        std::vector<GLchar> uniformName(256);
        for (GLint uniform = 0; uniform < numUniforms; ++uniform)
        {
            GLint   arraySize    = 0;
            GLenum  type         = 0;
            GLsizei actualLength = 0;

            // Get sampler uniform data & store it on our sampler map.
            glGetActiveUniform(shader, uniform, (GLsizei)uniformName.size(), &actualLength, &arraySize, &type, &uniformName[0]);

            std::string name((char*)&uniformName[0], actualLength - 1);
            GLint       loc                                                     = glGetUniformLocation(shader, (char*)&uniformName[0]);
            Internal::g_backendData.m_shaderUniformMap[shader][&uniformName[0]] = loc;

            for (int i = 1; i < arraySize; i++)
            {
                std::string name((char*)&uniformName[0], actualLength - 2);
                name = name + std::to_string(i) + "]";

                std::string newName(name.c_str(), actualLength - 1);
                GLint       loc                                                     = glGetUniformLocation(shader, name.c_str());
                Internal::g_backendData.m_shaderUniformMap[shader][&uniformName[0]] = loc;
            }
        }
    }

} // namespace Lina2D::Backend