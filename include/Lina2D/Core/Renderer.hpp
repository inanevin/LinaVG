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

/*
Class: Application

Central application class, responsible for managing all the engines like input, physics, rendering etc.
as well as defining the game loop.

Timestamp: 12/29/2018 10:43:46 PM
*/

#pragma once
#ifndef LinaGUI_HPP
#define LinaGUI_HPP

#include "GLBackend.hpp"
#include "Drawer.hpp"
#include "Common.hpp"
#include <vector>
#include <list>
#include <memory>
#include <set>
#include <functional>

#ifdef LINA2D_CUSTOM_LINE_SHADER
#define DONTINIT_LINE_SHADER
#endif

namespace Lina2D
{
    class Renderer
    {
    public:
        void Initialize(const Options& initOptions);
        void StartFrame();
        void Render();
        void EndFrame();

        std::function<float()> m_mouseScrollCallback;
        std::function<Vec2()>  m_keyAxisCallback;

        inline Drawer& GetDrawer()
        {
            return m_drawer;
        }

    private:
        friend class Drawer;
        friend class Backend;

        static Renderer* g_renderer;
        Backend          m_backend;
        Drawer           m_drawer;
        Options          m_options;
        DrawData         m_drawData;
        int              m_gcFrameCounter;
    };

}; // namespace Lina2D

#endif
