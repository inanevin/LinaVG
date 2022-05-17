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
#include <iostream>

#include "glad/glad.h"

// After GLAD
#include "GLFW/glfw3.h"

namespace LinaVG
{
    namespace Examples
    {
        ExampleBackend* ExampleBackend::s_exampleBackend = nullptr;

        static void GLFWErrorCallback(int error, const char* desc)
        {
            std::cerr << "LinaVG: GLFW Error: " << error << " Description: " << desc << std::endl;
        }

        bool ExampleBackend::InitWindow(int width, int height)
        {
            s_exampleBackend = this;
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
                
            };

            auto windowCloseFunc = [](GLFWwindow* w) {
                s_exampleBackend->m_shouldClose = true;
            };

            auto windowKeyFunc = [](GLFWwindow* w, int key, int scancode, int action, int modes) {

            };

            auto windowButtonFunc = [](GLFWwindow* w, int button, int action, int modes) {

            };

            auto windowMouseScrollFunc = [](GLFWwindow* w, double xOff, double yOff) {

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

    } // namespace Examples
} // namespace LinaVG
