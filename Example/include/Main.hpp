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

#ifndef MAIN_HPP
#define MAIN_HPP

#include "DemoScreens.hpp"

namespace LinaVG
{
    namespace Examples
    {
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

        private:
            DemoScreens        m_demoScreens;
            unsigned int       m_linaTexture       = 0;
            unsigned int       m_checkeredTexture  = 0;
            int                m_currentDemoScreen = 1;
            float              m_deltaTime         = 0.0f;
            float              m_deltaTimeRead     = 0.0f;
            float              m_elapsedTime       = 0.0f;
            int                m_fps               = 0;
            bool               m_shouldClose       = false;
            static ExampleApp* s_exampleApp;
        };
    } // namespace Examples
} // namespace LinaVG

#endif