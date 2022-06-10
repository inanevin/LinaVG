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

#include "Backends/OpenGLGLFW.hpp"
#include "Main.hpp"
#include <iostream>

#include "glad/glad.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Utility/stb_image.h"

// After GLAD
#include "GLFW/glfw3.h"

namespace LinaVG
{
    namespace Examples
    {
        static void GLFWErrorCallback(int error, const char* desc)
        {
            std::cerr << "LinaVG: GLFW Error: " << error << " Description: " << desc << std::endl;
        }

        bool ExampleBackend::InitWindow(int width, int height)
        {
            int init = glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_SAMPLES, 4);
            glfwWindowHint(GLFW_DECORATED, true);
            glfwWindowHint(GLFW_RESIZABLE, true);

#ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

            // Build window
            auto*              primaryMonitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode           = glfwGetVideoMode(primaryMonitor);

            m_window = (glfwCreateWindow(width, height, "Lina VG OpenGL/GLFW Example", NULL, NULL));
            glfwGetMonitorContentScale(primaryMonitor, &m_frameBufferScaleX, &m_frameBufferScaleY);

            if (!m_window)
            {
                // Assert window creation.
                std::cerr << "LinaVG: GLFW window failed to initialize!" << std::endl;
                return false;
            }
            // Set error callback
            glfwSetErrorCallback(GLFWErrorCallback);

            // Set context.
            glfwMakeContextCurrent(m_window);

            // Load glad
            bool loaded = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
            if (!loaded)
            {
                std::cerr << "LinaVG: GLAD Loader failed!" << std::endl;
                return false;
            }

            // Update OpenGL about the window data.
            glViewport(0, 0, width, height);

            glfwSwapInterval(0);

            // set user pointer for callbacks.
            glfwSetWindowUserPointer(m_window, this);

            auto windowResizeFunc = [](GLFWwindow* w, int wi, int he) {
                ExampleApp::Get()->OnWindowResizeCallback(wi, he);
            };

            auto windowCloseFunc = [](GLFWwindow* w) {
                ExampleApp::Get()->OnWindowCloseCallback();
            };

            auto windowKeyFunc = [](GLFWwindow* w, int key, int scancode, int action, int modes) {
                if (action == GLFW_PRESS)
                {
                    if (key == GLFW_KEY_F)
                        ExampleApp::Get()->OnFCallback();
                    else if (key == GLFW_KEY_P)
                        ExampleApp::Get()->OnPCallback();
                    else if (key == GLFW_KEY_R)
                        ExampleApp::Get()->OnRCallback();
                    else if (key == GLFW_KEY_C)
                        ExampleApp::Get()->OnCCallback();
                    else if (key == GLFW_KEY_E)
                        ExampleApp::Get()->OnECallback();

                    else if (key == GLFW_KEY_0)
                        ExampleApp::Get()->OnNumKeyCallback(0);
                    else if (key == GLFW_KEY_1)
                        ExampleApp::Get()->OnNumKeyCallback(1);
                    else if (key == GLFW_KEY_2)
                        ExampleApp::Get()->OnNumKeyCallback(2);
                    else if (key == GLFW_KEY_3)
                        ExampleApp::Get()->OnNumKeyCallback(3);
                    else if (key == GLFW_KEY_4)
                        ExampleApp::Get()->OnNumKeyCallback(4);
                    else if (key == GLFW_KEY_5)
                        ExampleApp::Get()->OnNumKeyCallback(5);
                    else if (key == GLFW_KEY_6)
                        ExampleApp::Get()->OnNumKeyCallback(6);
                    else if (key == GLFW_KEY_7)
                        ExampleApp::Get()->OnNumKeyCallback(7);
                    else if (key == GLFW_KEY_8)
                        ExampleApp::Get()->OnNumKeyCallback(8);
                    else if (key == GLFW_KEY_9)
                        ExampleApp::Get()->OnNumKeyCallback(9);
                }
            };

            auto windowButtonFunc = [](GLFWwindow* w, int button, int action, int modes) {

            };

            auto windowMouseScrollFunc = [](GLFWwindow* w, double xOff, double yOff) {
                ExampleApp::Get()->OnMouseScrollCallback(static_cast<float>(yOff));
            };

            auto windowCursorPosFunc = [](GLFWwindow* w, double xPos, double yPos) {

            };

            auto windowFocusFunc = [](GLFWwindow* w, int f) {

            };

            // Register window callbacks.
            glfwSetFramebufferSizeCallback(m_window, windowResizeFunc);
            glfwSetWindowCloseCallback(m_window, windowCloseFunc);
            glfwSetKeyCallback(m_window, windowKeyFunc);
            glfwSetMouseButtonCallback(m_window, windowButtonFunc);
            glfwSetScrollCallback(m_window, windowMouseScrollFunc);
            glfwSetCursorPosCallback(m_window, windowCursorPosFunc);
            glfwSetWindowFocusCallback(m_window, windowFocusFunc);

            std::cout << "LinaVG: OpenGL/GLFW window initialized successfully." << std::endl;
            return true;
        }

        void ExampleBackend::Poll()
        {
            glfwPollEvents();

            if (glfwGetKey(m_window, GLFW_KEY_W))
                ExampleApp::Get()->OnVerticalKeyCallback(1.0f);
            else if (glfwGetKey(m_window, GLFW_KEY_S))
                ExampleApp::Get()->OnVerticalKeyCallback(-1.0f);
            else if (glfwGetKey(m_window, GLFW_KEY_D))
                ExampleApp::Get()->OnHorizontalKeyCallback(1.0f);
            else if (glfwGetKey(m_window, GLFW_KEY_A))
                ExampleApp::Get()->OnHorizontalKeyCallback(-1.0f);

        }

        void ExampleBackend::Render()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor((GLfloat)0.8f, (GLfloat)0.8f, (GLfloat)0.8f, (GLfloat)1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        void ExampleBackend::SwapBuffers()
        {
            glfwSwapBuffers(m_window);
        }

        void ExampleBackend::Terminate()
        {
            glfwTerminate();
            std::cout << "LinaVG: Example backend terminated successfully." << std::endl;
        }

        float ExampleBackend::GetTime()
        {
            return static_cast<float>(glfwGetTime());
        }

        unsigned int ExampleBackend::CreateTexture(const char* file)
        {
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            // set the texture wrapping/filtering options (on the currently bound texture object)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // load and generate the texture
            int            width, height, nrChannels;
            unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 4);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            else
            {
                std::cout << "Failed to load texture" << std::endl;
            }
            stbi_image_free(data);
            return texture;
        }

    } // namespace Examples
} // namespace LinaVG
