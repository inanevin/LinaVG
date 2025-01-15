/*
This file is a part of: LinaVG
https://github.com/inanevin/LinaVG

Author: Inan Evin
http://www.inanevin.com

The 2-Clause BSD License

Copyright (c) [2022-] Inan Evin

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this
	  list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
	  this list of conditions and the following disclaimer in the documentation
	  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "Backends/GLFWWindow.hpp"
#include "Main.hpp"
#include <iostream>

#include "glad/glad.h"

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

		bool GLFWWindow::InitWindow(int width, int height)
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
			auto*			   primaryMonitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode			  = glfwGetVideoMode(primaryMonitor);

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

		void GLFWWindow::Poll()
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

		void GLFWWindow::Clear()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClearColor((GLfloat)0.8f, (GLfloat)0.8f, (GLfloat)0.8f, (GLfloat)1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
		}

		void GLFWWindow::SwapBuffers()
		{
			glfwSwapBuffers(m_window);
		}

		void GLFWWindow::Terminate()
		{
			glfwTerminate();
			std::cout << "LinaVG: Example backend terminated successfully." << std::endl;
		}

		float GLFWWindow::GetTime()
		{
			return static_cast<float>(glfwGetTime());
		}

	} // namespace Examples
} // namespace LinaVG
