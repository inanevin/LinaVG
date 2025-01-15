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

#include <vector>
#include <string>

namespace LinaVG
{
	namespace Examples
	{

		struct SDFMaterial;

		class DemoScreens
		{
		public:
			void Initialize();
			void Terminate();

			void ShowBackground();
			void ShowDemoScreen1_Shapes();
			void ShowDemoScreen2_Colors();
			void ShowDemoScreen3_Outlines();
			void ShowDemoScreen4_Lines();
			void ShowDemoScreen5_Texts();
			void ShowDemoScreen6_DrawOrder();
			void ShowDemoScreen7_Clipping();
			void ShowDemoScreen8_Animated();
			void ShowDemoScreen9_Final();
			void PreEndFrame();

			float					 m_screenMS		   = 0.0f;
			bool					 m_clippingEnabled = true;
			bool					 m_statsWindowOn   = true;
			bool					 m_rotate		   = false;
			float					 m_rotateAngle	   = 0.0f;
			int						 m_drawCount	   = 0;
			int						 m_triangleCount   = 0;
			int						 m_vertexCount	   = 0;
			std::vector<std::string> m_screenTitles	   = {"SHAPES", "COLORS", "OUTLINES", "LINES", "TEXTS", "Z-ORDER", "CLIPPING", "ANIMATED", "FINAL"};
			std::vector<std::string> m_screenDescriptions;

			SDFMaterial* m_sdfMaterial0 = nullptr;
			SDFMaterial* m_sdfMaterial1 = nullptr;
			SDFMaterial* m_sdfMaterial2 = nullptr;
			SDFMaterial* m_sdfMaterial3 = nullptr;
			SDFMaterial* m_sdfMaterial4 = nullptr;
			SDFMaterial* m_sdfMaterial5 = nullptr;
			SDFMaterial* m_sdfMaterial6 = nullptr;
		};
	} // namespace Examples
} // namespace LinaVG
