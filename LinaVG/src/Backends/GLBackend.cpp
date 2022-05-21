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
#include "Core/Backend.hpp"
#include "Core/Renderer.hpp"
#include "Core/Drawer.hpp"
#include <iostream>
#include <stdio.h>

namespace LinaVG::Backend
{
    GLState g_glState;

    bool Initialize()
    {
        Internal::g_backendData.m_defaultVtxShader = "#version 330 core\n"
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

        Internal::g_backendData.m_defaultFragShader = "#version 330 core\n"
                                                      "out vec4 fragColor;\n"
                                                      "in vec4 fCol;\n"
                                                      "void main()\n"
                                                      "{\n"
                                                      "   fragColor = fCol;\n"
                                                      "}\0";

        Internal::g_backendData.m_texturedFragShader = "#version 330 core\n"
                                                       "out vec4 fragColor;\n"
                                                       "in vec2 fUV;\n"
                                                       "in vec4 fCol;\n"
                                                       "uniform sampler2D diffuse;\n"
                                                       "uniform vec2 tiling;\n"
                                                       "uniform vec2 offset;\n"
                                                       "uniform int isAABuffer;\n"
                                                       "void main()\n"
                                                       "{\n"
                                                       "   vec2 tiled = vec2(fUV.x * tiling.x, fUV.y * tiling.y);\n"
                                                       "   vec4 col = texture(diffuse, fUV * tiling + offset);\n"
                                                       "   fragColor = vec4(col.rgb, isAABuffer == 1 ? fCol.a : col.a); \n"
                                                       "}\0";

        Internal::g_backendData.m_textFragShader = "#version 330 core\n"
                                                   "out vec4 fragColor;\n"
                                                   "in vec2 fUV;\n"
                                                   "in vec4 fCol;\n"
                                                   "uniform sampler2D diffuse;\n"
                                                   "void main()\n"
                                                   "{\n"
                                                   "   vec4 sampled = vec4(1,1,1,texture(diffuse, fUV).r); \n"
                                                   "   if(sampled.a < 0.5) discard; \n"
                                                   "   fragColor = vec4(fCol.rgb, sampled.a); \n"
                                                   "}\0";

        Internal::g_backendData.m_roundedGradientFragShader = "#version 330 core\n"
                                                              "out vec4 fragColor;\n"
                                                              "in vec2 fUV;\n"
                                                              "in vec4 fCol;\n"
                                                              "uniform vec4 startColor;\n"
                                                              "uniform vec4 endColor;\n"
                                                              "uniform int  gradientType;\n"
                                                              "uniform float radialSize;\n"
                                                              "uniform int isAABuffer;\n"
                                                              "void main()\n"
                                                              "{\n"
                                                              "   if(gradientType == 0) {\n"
                                                              "     vec4 col = mix(startColor, endColor, fUV.x);\n"
                                                              "     fragColor = vec4(col.rgb, isAABuffer == 1 ? fCol.a : col.a); \n"
                                                              "}\n"
                                                              "   else if(gradientType == 1){\n"
                                                              "     vec4 col = mix(startColor, endColor, fUV.y);\n"
                                                              "     fragColor = vec4(col.rgb, isAABuffer == 1 ? fCol.a : col.a); \n"
                                                              "}\n"
                                                              "   else if(gradientType == 2) \n"
                                                              "   {\n"
                                                              "       vec2 uv = fUV - vec2(0.5, 0.5);\n"
                                                              "       float dist = length(uv * radialSize);\n"
                                                              "       vec4 col = mix(startColor, endColor, dist);\n"
                                                              "       fragColor = vec4(col.rgb, isAABuffer == 1 ? fCol.a : col.a); \n"
                                                              "    }\n"
                                                              "   else if(gradientType == 3) \n"
                                                              "   {\n"
                                                              "       float dist = length(fUV * radialSize);\n"
                                                              "       vec4 col = mix(startColor, endColor, dist);\n"
                                                              "       fragColor = vec4(col.rgb, isAABuffer == 1 ? fCol.a : col.a); \n"
                                                              "    }\n"
                                                              "}\n\0";

        try
        {
            Internal::g_backendData.m_defaultShaderHandle  = CreateShader(Internal::g_backendData.m_defaultVtxShader, Internal::g_backendData.m_defaultFragShader);
            Internal::g_backendData.m_gradientShaderHandle = CreateShader(Internal::g_backendData.m_defaultVtxShader, Internal::g_backendData.m_roundedGradientFragShader);
            Internal::g_backendData.m_texturedShaderHandle = CreateShader(Internal::g_backendData.m_defaultVtxShader, Internal::g_backendData.m_texturedFragShader);
            Internal::g_backendData.m_textShaderHandle     = CreateShader(Internal::g_backendData.m_defaultVtxShader, Internal::g_backendData.m_textFragShader);
        }
        catch (const std::runtime_error& err)
        {
            Config.m_errorCallback("LinaVG: Backend shader creation failed!");
            Config.m_errorCallback(err.what());
            return false;
        }

        glGenVertexArrays(1, &Internal::g_backendData.m_vao);
        glGenBuffers(1, &Internal::g_backendData.m_vbo);
        glGenBuffers(1, &Internal::g_backendData.m_ebo);

        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(Internal::g_backendData.m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, Internal::g_backendData.m_vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Internal::g_backendData.m_ebo);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glGenVertexArrays(1, &Internal::g_backendData.m_textVAO);
        glGenBuffers(1, &Internal::g_backendData.m_textVBO);
        glGenBuffers(1, &Internal::g_backendData.m_textEBO);

        glBindVertexArray(Internal::g_backendData.m_textVAO);

        glBindBuffer(GL_ARRAY_BUFFER, Internal::g_backendData.m_textVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Internal::g_backendData.m_textEBO);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));
        glEnableVertexAttribArray(2);

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);

        return true;
    }

    void StartFrame()
    {
        Config.m_debugCurrentDrawCalls     = 0;
        Config.m_debugCurrentTriangleCount = 0;
        Config.m_debugCurrentVertexCount   = 0;

        // Save GL state
        SaveAPIState();

        // Apply GL state
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_SCISSOR_TEST);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        if (Config.m_debugWireframeEnabled)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glViewport(0, 0, (GLsizei)Config.m_displaySize.x, (GLsizei)Config.m_displaySize.y);

        // Ortho projection matrix.
        int fb_width  = (int)(Config.m_displaySize.x * Config.m_framebufferScale.x);
        int fb_height = (int)(Config.m_displaySize.y * Config.m_framebufferScale.y);
        if (fb_width <= 0 || fb_height <= 0)
        {
            Internal::g_backendData.m_skipDraw = true;
            return;
        }

        Internal::g_backendData.m_skipDraw = false;

        float       L    = Config.m_displayPos.x;
        float       R    = Config.m_displayPos.x + Config.m_displaySize.x;
        float       T    = Config.m_displayPos.y;
        float       B    = Config.m_displayPos.y + Config.m_displaySize.y;
        const float zoom = Config.m_debugOrthoProjectionZoom;

        L *= zoom;
        R *= zoom;
        T *= zoom;
        B *= zoom;

        L += Config.m_debugOrthoOffset.x;
        R += Config.m_debugOrthoOffset.x;
        T += Config.m_debugOrthoOffset.y;
        B += Config.m_debugOrthoOffset.y;

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
    }

    void DrawGradient(GradientDrawBuffer* buf)
    {
        if (Internal::g_backendData.m_skipDraw)
            return;

        BindVAO(Internal::g_backendData.m_vao);

        glUseProgram(Internal::g_backendData.m_gradientShaderHandle);
        glUniformMatrix4fv(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_gradientShaderHandle]["proj"], 1, GL_FALSE, &Internal::g_backendData.m_proj[0][0]);
        glUniform4f(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_gradientShaderHandle]["startColor"], (GLfloat)buf->m_color.m_start.x, (GLfloat)buf->m_color.m_start.y, (GLfloat)buf->m_color.m_start.z, (GLfloat)buf->m_color.m_start.w);
        glUniform4f(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_gradientShaderHandle]["endColor"], (GLfloat)buf->m_color.m_end.x, (GLfloat)buf->m_color.m_end.y, (GLfloat)buf->m_color.m_end.z, (GLfloat)buf->m_color.m_end.w);
        glUniform1i(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_gradientShaderHandle]["gradientType"], (GLint)((int)buf->m_color.m_gradientType));
        glUniform1f(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_gradientShaderHandle]["radialSize"], (GLfloat)buf->m_color.m_radialSize);
        glUniform1i(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_gradientShaderHandle]["isAABuffer"], (GLint)((int)buf->m_isAABuffer));

        glBindBuffer(GL_ARRAY_BUFFER, Internal::g_backendData.m_vbo);
        glBufferData(GL_ARRAY_BUFFER, buf->m_vertexBuffer.m_size * sizeof(Vertex), (const GLvoid*)buf->m_vertexBuffer.begin(), GL_STREAM_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Internal::g_backendData.m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, buf->m_indexBuffer.m_size * sizeof(Index), (const GLvoid*)buf->m_indexBuffer.begin(), GL_STREAM_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawElements(GL_TRIANGLES, (GLsizei)buf->m_indexBuffer.m_size, sizeof(Index) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, 0);
        Config.m_debugCurrentDrawCalls++;
        Config.m_debugCurrentTriangleCount += int((float)buf->m_indexBuffer.m_size / 3.0f);
        Config.m_debugCurrentVertexCount += buf->m_vertexBuffer.m_size;
    }

    void DrawTextured(TextureDrawBuffer* buf)
    {
        if (Internal::g_backendData.m_skipDraw)
            return;

        BindVAO(Internal::g_backendData.m_vao);

        const Vec2 uv = Config.m_flipTextureUVs ? Vec2(buf->m_textureUVTiling.x, -buf->m_textureUVTiling.y) : buf->m_textureUVTiling;

        glUseProgram(Internal::g_backendData.m_texturedShaderHandle);
        glUniformMatrix4fv(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_texturedShaderHandle]["proj"], 1, GL_FALSE, &Internal::g_backendData.m_proj[0][0]);

        glUniform1i(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_texturedShaderHandle]["diffuse"], 0);
        glUniform2f(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_texturedShaderHandle]["tiling"], (GLfloat)uv.x, (GLfloat)uv.y);
        glUniform2f(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_texturedShaderHandle]["offset"], (GLfloat)buf->m_textureUVOffset.x, (GLfloat)buf->m_textureUVOffset.y);
        glUniform1i(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_gradientShaderHandle]["isAABuffer"], (GLint)((int)buf->m_isAABuffer));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, buf->m_textureHandle);

        glBindBuffer(GL_ARRAY_BUFFER, Internal::g_backendData.m_vbo);
        glBufferData(GL_ARRAY_BUFFER, buf->m_vertexBuffer.m_size * sizeof(Vertex), (const GLvoid*)buf->m_vertexBuffer.begin(), GL_STREAM_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Internal::g_backendData.m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, buf->m_indexBuffer.m_size * sizeof(Index), (const GLvoid*)buf->m_indexBuffer.begin(), GL_STREAM_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawElements(GL_TRIANGLES, (GLsizei)buf->m_indexBuffer.m_size, sizeof(Index) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, 0);
        Config.m_debugCurrentDrawCalls++;
        Config.m_debugCurrentTriangleCount += int((float)buf->m_indexBuffer.m_size / 3.0f);
        Config.m_debugCurrentVertexCount += buf->m_vertexBuffer.m_size;
    }

    void DrawDefault(DrawBuffer* buf)
    {
        if (Internal::g_backendData.m_skipDraw)
            return;

        BindVAO(Internal::g_backendData.m_vao);

        glUseProgram(Internal::g_backendData.m_defaultShaderHandle);
        glUniformMatrix4fv(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_defaultShaderHandle]["proj"], 1, GL_FALSE, &Internal::g_backendData.m_proj[0][0]);

        glBindBuffer(GL_ARRAY_BUFFER, Internal::g_backendData.m_vbo);
        glBufferData(GL_ARRAY_BUFFER, buf->m_vertexBuffer.m_size * sizeof(Vertex), (const GLvoid*)buf->m_vertexBuffer.begin(), GL_STREAM_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Internal::g_backendData.m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, buf->m_indexBuffer.m_size * sizeof(Index), (const GLvoid*)buf->m_indexBuffer.begin(), GL_STREAM_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawElements(GL_TRIANGLES, (GLsizei)buf->m_indexBuffer.m_size, sizeof(Index) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, 0);
        Config.m_debugCurrentDrawCalls++;
        Config.m_debugCurrentTriangleCount += int((float)buf->m_indexBuffer.m_size / 3.0f);
        Config.m_debugCurrentVertexCount += buf->m_vertexBuffer.m_size;
    }

    void DrawText(CharDrawBuffer* buf)
    {
        if (Internal::g_backendData.m_skipDraw)
            return;

        BindVAO(Internal::g_backendData.m_textVAO);

        glUseProgram(Internal::g_backendData.m_textShaderHandle);
        glUniformMatrix4fv(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_textShaderHandle]["proj"], 1, GL_FALSE, &Internal::g_backendData.m_proj[0][0]);

        glUniform1i(Internal::g_backendData.m_shaderUniformMap[Internal::g_backendData.m_textShaderHandle]["diffuse"], 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, buf->m_textureHandle);

        glBindBuffer(GL_ARRAY_BUFFER, Internal::g_backendData.m_textVBO);
        glBufferData(GL_ARRAY_BUFFER, buf->m_vertexBuffer.m_size * sizeof(Vertex), (const GLvoid*)buf->m_vertexBuffer.begin(), GL_STREAM_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Internal::g_backendData.m_textEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, buf->m_indexBuffer.m_size * sizeof(Index), (const GLvoid*)buf->m_indexBuffer.begin(), GL_STREAM_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawElements(GL_TRIANGLES, (GLsizei)buf->m_indexBuffer.m_size, sizeof(Index) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, 0);
        Config.m_debugCurrentDrawCalls++;
        Config.m_debugCurrentTriangleCount += int((float)buf->m_indexBuffer.m_size / 3.0f);
        Config.m_debugCurrentVertexCount += buf->m_vertexBuffer.m_size;
    }

    void SaveAPIState()
    {
        GLboolean blendEnabled;
        GLboolean cullFaceEnabled;
        GLboolean stencilTestEnabled;
        GLboolean depthTestEnabled;
        GLboolean scissorTestEnabled;
        GLint     blendEq;
        GLint     blendSrcAlpha;
        GLint     blendSrcRGB;
        GLint     blendDestAlpha;
        GLint     blendDestRGB;
        GLint     unpackAlignment;
        glGetIntegerv(GL_BLEND_EQUATION, &blendEq);
        glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrcAlpha);
        glGetIntegerv(GL_BLEND_SRC_RGB, &blendSrcRGB);
        glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDestAlpha);
        glGetIntegerv(GL_BLEND_DST_RGB, &blendDestRGB);
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpackAlignment);
        glGetBooleanv(GL_BLEND, &blendEnabled);
        glGetBooleanv(GL_CULL_FACE, &cullFaceEnabled);
        glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
        glGetBooleanv(GL_STENCIL_TEST, &stencilTestEnabled);
        glGetBooleanv(GL_SCISSOR_TEST, &scissorTestEnabled);
        g_glState.m_blendDestAlpha     = static_cast<int>(blendDestAlpha);
        g_glState.m_blendDestRGB       = static_cast<int>(blendDestRGB);
        g_glState.m_blendEq            = static_cast<int>(blendEq);
        g_glState.m_blendSrcAlpha      = static_cast<int>(blendSrcAlpha);
        g_glState.m_blendSrcRGB        = static_cast<int>(blendSrcRGB);
        g_glState.m_unpackAlignment    = static_cast<int>(unpackAlignment);
        g_glState.m_blendEnabled       = static_cast<bool>(blendEnabled);
        g_glState.m_cullFaceEnabled    = static_cast<bool>(cullFaceEnabled);
        g_glState.m_depthTestEnabled   = static_cast<bool>(depthTestEnabled);
        g_glState.m_scissorTestEnabled = static_cast<bool>(scissorTestEnabled);
        g_glState.m_stencilTestEnabled = static_cast<bool>(stencilTestEnabled);
    }

    void RestoreAPIState()
    {
        if (g_glState.m_blendEnabled)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);

        if (g_glState.m_depthTestEnabled)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);

        if (g_glState.m_cullFaceEnabled)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);

        if (g_glState.m_stencilTestEnabled)
            glEnable(GL_STENCIL_TEST);
        else
            glDisable(GL_STENCIL_TEST);

        if (g_glState.m_scissorTestEnabled)
            glEnable(GL_SCISSOR_TEST);
        else
            glDisable(GL_SCISSOR_TEST);

        glBlendEquation(static_cast<GLenum>(g_glState.m_blendEq));
        glBlendFuncSeparate(static_cast<GLenum>(g_glState.m_blendSrcRGB), static_cast<GLenum>(g_glState.m_blendDestRGB), static_cast<GLenum>(g_glState.m_blendSrcAlpha), static_cast<GLenum>(g_glState.m_blendDestAlpha));
        glPixelStorei(GL_UNPACK_ALIGNMENT, g_glState.m_unpackAlignment);
    }

    void BindVAO(BackendHandle vao)
    {
        if (Internal::g_backendData.m_boundVAO != vao)
        {
            glBindVertexArray(vao);
            Internal::g_backendData.m_boundVAO = vao;
        }
    }

    void EndFrame()
    {
        // Restore state.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDepthMask(GL_TRUE);

        // Reset GL state
        RestoreAPIState();
    }

    void Terminate()
    {
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
            Config.m_errorCallback("LinaVG: Backend Error -> Shader vertex compilation failed!");
            Config.m_errorCallback(infoLog);
            throw std::runtime_error("");
        }

        // FRAG
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &frag, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            Config.m_errorCallback("LinaVG: Backend Error -> Shader fragment compilation failed!");
            Config.m_errorCallback(infoLog);
            throw std::runtime_error("");
        }

        GLuint handle = glCreateProgram();
        glAttachShader(handle, vertex);
        glAttachShader(handle, fragment);
        glLinkProgram(handle);

        glGetProgramiv(handle, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(handle, 512, NULL, infoLog);
            Config.m_errorCallback("LinaVG: Backend Error -> Could not link shader program!");
            Config.m_errorCallback(infoLog);
            throw std::runtime_error("");
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

    BackendHandle CreateFontTexture(int width, int height)
    {

        GLuint tex;
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        return tex;
    }

    void BufferFontTextureAtlas(int width, int height, int offsetX, int offsetY, void* data)
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, offsetX, offsetY, width, height, GL_RED, GL_UNSIGNED_BYTE, data);
    }

    //  BackendHandle GenerateFontTexture(int width, int height, void* data)
    //  {
    //      if (data == nullptr)
    //          return 0;
    //
    //      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //
    //      // Generate texture
    //      unsigned int texture;
    //      glGenTextures(1, &texture);
    //      glBindTexture(GL_TEXTURE_2D, texture);
    //      glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, static_cast<GLvoid*>(data));
    //
    //      // Options
    //      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //      return static_cast<BackendHandle>(texture);
    //  }

} // namespace LinaVG::Backend