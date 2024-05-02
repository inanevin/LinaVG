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

#pragma once

#include "DemoScreens.hpp"
#include "LinaVG/LinaVG.hpp"

namespace LinaVG
{
    class Drawer;

	namespace Examples
	{
        class GLBackend;
    
		class ExampleApp
		{
		public:
			void Run();

			static inline ExampleApp* Get()
			{
				return s_exampleApp;
			}

			void OnHorizontalKeyCallback(float input);
			void OnVerticalKeyCallback(float input);
			void OnNumKeyCallback(int key);
			void OnPCallback();
			void OnRCallback();
			void OnFCallback();
			void OnCCallback();
			void OnECallback();
			void OnMouseScrollCallback(float val);
			void OnWindowResizeCallback(int width, int height);
			void OnWindowCloseCallback();

			inline int GetFPS()
			{
				return m_fps;
			}

			inline float GetFrameTime()
			{
				return m_deltaTime;
			}

			inline float GetFrameTimeRead()
			{
				return m_deltaTimeRead;
			}

			inline float GetElapsed()
			{
				return m_elapsedTime;
			}

			inline unsigned int GetLinaLogoTexture()
			{
				return m_linaTexture;
			}

			inline unsigned int GetCheckeredTexture()
			{
				return m_checkeredTexture;
			}

			inline int GetCurrentScreen()
			{
				return m_currentDemoScreen;
			}
            
            inline Drawer& GetLVGDrawer()
            {
                return m_lvgDrawer;
            }

		private:
			DemoScreens		   m_demoScreens;
			unsigned int	   m_linaTexture	   = 0;
			unsigned int	   m_checkeredTexture  = 0;
			int				   m_currentDemoScreen = 1;
			float			   m_deltaTime		   = 0.0f;
			float			   m_deltaTimeRead	   = 0.0f;
			float			   m_elapsedTime	   = 0.0f;
			int				   m_fps			   = 0;
			bool			   m_shouldClose	   = false;
			static ExampleApp* s_exampleApp;
            LinaVG::Drawer m_lvgDrawer;
            GLBackend* m_renderingBackend = nullptr;
		};
	} // namespace Examples
} // namespace LinaVG
