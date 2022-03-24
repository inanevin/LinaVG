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

#include "Core/LinaGUIGLBackend.hpp"
#include "Core/LinaGUI.hpp"
#include <iostream>
#include <stdio.h>

namespace Lina::GUI
{
    GLchar*      Backend::g_lineVertexShader = nullptr;
    GLchar*      Backend::g_lineFragShader   = nullptr;
    unsigned int Backend::g_lineShader       = 0;

    void Backend::Initialize(const LGInitOptions& initOptions)
    {
        if (initOptions.m_customLineVertexShader == nullptr)
        {
            g_lineVertexShader = "layout (location = 0) in vec2 position;\n"
                                 "layout (location = 1) in vec2 uv;\n"
                                 "layout (location = 2) in vec4 color;\n"
                                 "uniform mat4 proj;\n"
                                 "out vec2 fUV;\n"
                                 "out vec4 fColor;\n"
                                 "void main()\n"
                                 "{\n"
                                 "    fUV = uv;\n"
                                 "    fColor = color;\n"
                                 "    gl_Position = proj * vec4(position.xy,0,1);\n"
                                 "}\n";
        }

        if (initOptions.m_customLineFragShader == nullptr)
        {
            g_lineFragShader = "in vec2 fUV;\n"
                               "in vec4 fColor;\n"
                               "layout (location = 0) out vec4 fragOut;\n"
                               "void main()\n"
                               "{\n"
                               "    fragOut = fColor;\n"
                               "}\n";
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
    }

    void Backend::Start()
    {
    }

    void Backend::Render()
    {
        glUseProgram(g_lineShader);
    }

    void Backend::End()
    {
    }
} // namespace Lina::GUI