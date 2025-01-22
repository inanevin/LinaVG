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

#include "LinaVG/Core/Drawer.hpp"
#include "LinaVG/Core/Math.hpp"
#include "LinaVG/Core/BufferStore.hpp"
#include "LinaVG/Core/Text.hpp"
#include "LinaVG/Utility/Utility.hpp"

namespace LinaVG
{

	namespace
	{
		void New_CalculateVertexUVs(DrawBuffer* buf, int startIndex, int endIndex, const Vec2& bbMin, const Vec2& bbMax)
		{
			for (int i = startIndex; i < endIndex; i++)
			{
				Vertex& vertex = buf->vertexBuffer[i];
				vertex.uv.x	   = Math::Remap(vertex.pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
				vertex.uv.y	   = Math::Remap(vertex.pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
			}
		}

		void New_CalculateVertexColors(DrawBuffer* buf, int startIndex, int endIndex, const Vec4Grad& color)
		{
			for (int i = startIndex; i < endIndex; i++)
			{
				Vertex& vertex = buf->vertexBuffer[i];

				if (color.gradientType == GradientType::None)
					vertex.col = color.start;
				else if (color.gradientType == GradientType::Horizontal)
					vertex.col = Math::Lerp(color.start, color.end, vertex.uv.x);
				else
					vertex.col = Math::Lerp(color.start, color.end, vertex.uv.y);
			}
		}

		void New_CalculateVertexUVsAndColor(DrawBuffer* buf, int startIndex, int endIndex, const Vec2& bbMin, const Vec2& bbMax, const Vec4Grad& color, bool preserveAlpha = false)
		{
			for (int i = startIndex; i < endIndex; i++)
			{
				Vertex& vertex = buf->vertexBuffer[i];
				vertex.uv.x	   = Math::Remap(vertex.pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
				vertex.uv.y	   = Math::Remap(vertex.pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);

				const float alpha = vertex.col.w;

				if (color.gradientType == GradientType::None)
					vertex.col = color.start;
				else if (color.gradientType == GradientType::Horizontal)
					vertex.col = Math::Lerp(color.start, color.end, vertex.uv.x);
				else
					vertex.col = Math::Lerp(color.start, color.end, vertex.uv.y);

				if (preserveAlpha)
					vertex.col.w = alpha;
			}
		}

		void New_GetConvexBB(DrawBuffer* buf, int startIndex, int endIndex, Vec2& outMin, Vec2& outMax)
		{
			outMin = Vec2(99999, 99999);
			outMax = Vec2(-99999, -99999);

			for (int i = startIndex; i < endIndex; i++)
			{
				Vertex& vertex = buf->vertexBuffer[i];

				if (vertex.pos.x < outMin.x)
					outMin.x = vertex.pos.x;
				else if (vertex.pos.x > outMax.x)
					outMax.x = vertex.pos.x;
				if (vertex.pos.y < outMin.y)
					outMin.y = vertex.pos.y;
				else if (vertex.pos.y > outMax.y)
					outMax.y = vertex.pos.y;
			}
		}
	} // namespace

	void Drawer::DrawBezier(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, StyleOptions& style, LineCapDirection cap, LineJointType jointType, int drawOrder, int segments)
	{
		float		acc		 = (float)Math::Clamp(segments, 0, 100);
		const float increase = Math::Remap(acc, 0.0f, 100.0f, 0.15f, 0.01f);
		Array<Vec2> points;

		bool addLast = true;
		for (float t = 0.0f; t < 1.0f; t += increase)
		{
			points.push_back(Math::SampleBezier(p0, p1, p2, p3, t));

			if (Math::IsEqualMarg(t, 1.0f, 0.001f))
				addLast = false;
		}

		if (addLast)
			points.push_back(Math::SampleBezier(p0, p1, p2, p3, 1.0f));

		/*
		const Vec4ui& clip = m_bufferStore.GetData().m_clipRect;
		if (clip.z != 0 || clip.w != 0)
		{
			Vec2 min = {}, max = {};
			GetConvexBoundingBox(&points[0], points.m_size, min, max);
			if (!clip.IsPointInside(min) && !clip.IsPointInside(max))
				return;
		}*/

		DrawLines(&points[0], points.m_size, style, cap, jointType, drawOrder);

		points.clear();
	}

	void Drawer::DrawPoint(const Vec2& p1, const Vec4& col)
	{
		StyleOptions style;
		style.color			 = col;
		style.isFilled		 = true;
		const float distance = 0.5f;
		DrawRect(Vec2(p1.x - distance, p1.y - distance), Vec2(p1.x + distance, p1.y + distance), style);
	}

	void Drawer::DrawLine(const Vec2& p1, const Vec2& p2, StyleOptions& style, LineCapDirection cap, float rotateAngle, int drawOrder)
	{
		SimpleLine	 l = CalculateSimpleLine(p1, p2, style);
		StyleOptions s = StyleOptions(style);
		s.isFilled	   = true;

		if (cap == LineCapDirection::Left || cap == LineCapDirection::Both)
		{
			s.onlyRoundTheseCorners.push_back(0);
			s.onlyRoundTheseCorners.push_back(3);
			s.rounding = 1.0f;
		}

		if (cap == LineCapDirection::Right || cap == LineCapDirection::Both)
		{
			s.onlyRoundTheseCorners.push_back(1);
			s.onlyRoundTheseCorners.push_back(2);
			s.rounding = 1.0f;
		}

		DrawSimpleLine(l, s, rotateAngle, drawOrder);
	}

	void Drawer::DrawLines(Vec2* points, int count, StyleOptions& opts, LineCapDirection cap, LineJointType jointType, int drawOrder)
	{
		if (count < 3)
		{
			if (Config.errorCallback)
				Config.errorCallback("LinaVG: Can't draw lines as the point array count is smaller than 3!");
			return;
		}

		/*const Vec4ui& clip = m_bufferStore.GetData().m_clipRect;
		if (clip.z != 0 || clip.w != 0)
		{
			Vec2 min = {}, max = {};
			GetConvexBoundingBox(points, count, min, max);
			if (!clip.IsPointInside(min) && !clip.IsPointInside(max))
				return;
		}*/

		// Generate line structs between each points.
		// Each line struct will contain -> line vertices, upper & below vertices.
		StyleOptions style = StyleOptions(opts);
		style.isFilled	   = true;

		// Determine which buffer to use.
		// Also correct the buffer pointer if getting a new buffer invalidated it.
		DrawBuffer* destBuf = &m_bufferStore.GetData().GetDefaultBuffer(style.userData, style.uniqueID, drawOrder, DrawBufferShapeType::Shape, style.textureHandle, style.textureTilingAndOffset);

		// Calculate the line points.
		Array<Line*> lines;
		lines.reserve(count - 1);
		LineCapDirection usedCapDir = LineCapDirection::None;

		for (int i = 0; i < count - 1; i++)
		{
			if (i == 0 && (cap == LineCapDirection::Left || cap == LineCapDirection::Both))
				usedCapDir = LineCapDirection::Left;
			else if (i == count - 2 && (cap == LineCapDirection::Right || cap == LineCapDirection::Both))
				usedCapDir = LineCapDirection::Right;
			else
				usedCapDir = LineCapDirection::None;

			const float t		  = static_cast<float>(i) / static_cast<float>(count - 1);
			const float t2		  = static_cast<float>(i + 1) / static_cast<float>(count - 1);
			style.thickness.start = Math::Lerp(opts.thickness.start, opts.thickness.end, t);
			style.thickness.end	  = Math::Lerp(opts.thickness.start, opts.thickness.end, t2);

			Line* line = new Line();
			CalculateLine(*line, points[i], points[i + 1], style, usedCapDir);
			lines.push_back(line);
		}

		// Calculate line joints.
		for (int i = 0; i < lines.m_size - 1; i++)
		{
			Line* curr = lines[i];
			Line* next = lines[i + 1];

			const Vec2 currDir = Math::Normalized(Vec2(curr->m_vertices[2].pos.x - curr->m_vertices[3].pos.x, curr->m_vertices[2].pos.y - curr->m_vertices[3].pos.y));
			const Vec2 nextDir = Math::Normalized(Vec2(next->m_vertices[2].pos.x - next->m_vertices[3].pos.x, next->m_vertices[2].pos.y - next->m_vertices[3].pos.y));

			if (!Math::AreLinesParallel(curr->m_vertices[3].pos, curr->m_vertices[2].pos, next->m_vertices[3].pos, next->m_vertices[2].pos))
			{
				// If next line is going below current one, angle is positive, and we merge lower vertices while joining upper.
				// Vice versa if angle is negative
				const float angle = Math::GetAngleBetweenDirs(currDir, nextDir);

				LineJointType usedJointType = jointType;

				if (jointType != LineJointType::VtxAverage)
				{
					if (Math::Abs(angle) < 15.0f)
						usedJointType = LineJointType::VtxAverage;
					else
					{
						// Joint type fallbacks.
						if (jointType == LineJointType::Miter && Math::Abs(angle) > Config.miterLimit)
							usedJointType = LineJointType::BevelRound;

						if (jointType == LineJointType::BevelRound && Math::IsEqualMarg(style.rounding, 0.0f))
							usedJointType = LineJointType::Bevel;
					}
				}

				JoinLines(*curr, *next, style, usedJointType, angle < 0.0f);
			}
			else
			{
				next->m_upperIndices.erase(next->m_upperIndices.findAddr(0));
				next->m_lowerIndices.erase(next->m_lowerIndices.findAddr(3));
			}
		}

		const int drawBufferStartBeforeLines = destBuf->vertexBuffer.m_size;

		// Actually draw the lines after all calculations are done & corrected.
		for (int i = 0; i < lines.m_size; i++)
		{
			int destBufStart = destBuf->vertexBuffer.m_size;
			for (int j = 0; j < lines[i]->m_vertices.m_size; j++)
			{
				destBuf->PushVertex(lines[i]->m_vertices[j]);
			}

			for (int j = 0; j < lines[i]->m_tris.m_size; j++)
			{
				destBuf->PushIndex(destBufStart + lines[i]->m_tris[j].m_indices[0]);
				destBuf->PushIndex(destBufStart + lines[i]->m_tris[j].m_indices[1]);
				destBuf->PushIndex(destBufStart + lines[i]->m_tris[j].m_indices[2]);
			}
		}

		Vec2 bbMin, bbMax;
		New_GetConvexBB(destBuf, drawBufferStartBeforeLines, destBuf->vertexBuffer.m_size, bbMin, bbMax);
		New_CalculateVertexUVsAndColor(destBuf, drawBufferStartBeforeLines, destBuf->vertexBuffer.m_size, bbMin, bbMax, opts.color);

		if (Math::IsEqualMarg(style.outlineOptions.thickness, 0.0f) && !style.aaEnabled)
		{
			for (int i = 0; i < lines.m_size; i++)
				delete lines[i];

			lines.clear();
			return;
		}

		int drawBufferStartForOutlines = drawBufferStartBeforeLines;

		Array<int> totalUpperIndices;
		Array<int> totalLowerIndices;

		for (int i = 0; i < lines.m_size; i++)
		{
			for (int j = 0; j < lines[i]->m_upperIndices.m_size; j++)
				totalUpperIndices.push_back(drawBufferStartForOutlines + lines[i]->m_upperIndices[j]);

			for (int j = 0; j < lines[i]->m_lowerIndices.m_size; j++)
				totalLowerIndices.push_back(drawBufferStartForOutlines + lines[i]->m_lowerIndices[j]);

			drawBufferStartForOutlines += lines[i]->m_vertices.m_size;
		}

		if (!Math::IsEqualMarg(style.outlineOptions.thickness, 0.0f))
		{
			Array<int> indicesOrder;
			for (int i = 0; i < totalLowerIndices.m_size; i++)
				indicesOrder.push_back(totalLowerIndices[i]);

			for (int i = totalUpperIndices.m_size - 1; i > -1; i--)
				indicesOrder.push_back(totalUpperIndices[i]);

			DrawOutlineAroundShape(destBuf, style, &indicesOrder[0], indicesOrder.m_size, style.outlineOptions.thickness, false, drawOrder, OutlineCallType::Normal);
		}
		else if (style.aaEnabled)
		{
			StyleOptions opts2	 = StyleOptions(style);
			opts2.outlineOptions = OutlineOptions::FromStyle(style, OutlineDrawDirection::Both);

			Array<int> indicesOrder;
			for (int i = 0; i < totalLowerIndices.m_size; i++)
				indicesOrder.push_back(totalLowerIndices[i]);

			for (int i = totalUpperIndices.m_size - 1; i > -1; i--)
				indicesOrder.push_back(totalUpperIndices[i]);

			for (int i = 0; i < indicesOrder.m_size; i++)
			{
				Vertex& v	= destBuf->vertexBuffer[indicesOrder[i]];
				Vec4	col = v.col;
			}
			DrawOutlineAroundShape(destBuf, opts2, &indicesOrder[0], indicesOrder.m_size, opts2.outlineOptions.thickness, false, drawOrder, OutlineCallType::AA);
		}

		for (int i = 0; i < lines.m_size; i++)
			delete lines[i];

		lines.clear();
	}

	void Drawer::DrawImage(TextureHandle textureHandle, const Vec2& pos, const Vec2& size, Vec4 tint, float rotateAngle, int drawOrder, Vec4 uvTilingAndOffset, Vec2 uvTL, Vec2 uvBR)
	{
		StyleOptions style;
		style.aaEnabled				 = false;
		style.color					 = tint;
		style.isFilled				 = true;
		style.textureHandle			 = textureHandle;
		style.textureTilingAndOffset = uvTilingAndOffset;
		const Vec2 min				 = Vec2(pos.x - size.x / 2.0f, pos.y - size.y / 2.0f);
		const Vec2 max				 = Vec2(pos.x + size.x / 2.0f, pos.y + size.y / 2.0f);

		m_bufferStore.GetData().m_uvOverride.m_override = true;
		m_bufferStore.GetData().m_uvOverride.m_uvTL		= uvTL;
		m_bufferStore.GetData().m_uvOverride.m_uvBR		= uvBR;
		DrawRect(min, max, style, rotateAngle, drawOrder);
		m_bufferStore.GetData().m_uvOverride.m_override = false;
		m_bufferStore.GetData().m_uvOverride.m_uvTL		= Vec2(0, 0);
		m_bufferStore.GetData().m_uvOverride.m_uvBR		= Vec2(1, 1);
	}

	void Drawer::DrawTriangle(const Vec2& top, const Vec2& right, const Vec2& left, StyleOptions& style, float rotateAngle, int drawOrder)
	{
		// NR - SC - def buf
		// NR - SC - text
		// NR - VH - DEF
		// NR - VH - text

		/*const Vec4ui& clip = m_bufferStore.GetData().m_clipRect;
		if (clip.z != 0 || clip.w != 0)
		{
			Vec2 min = {}, max = {};
			GetTriangleBoundingBox(top, right, left, min, max);
			if (!clip.IsPointInside(min) && !clip.IsPointInside(max))
				return;
		}*/

		if (Math::IsEqualMarg(style.rounding, 0.0f))
			FillTri_NoRound(&m_bufferStore.GetData().GetDefaultBuffer(style.userData, style.uniqueID, drawOrder, DrawBufferShapeType::Shape, style.textureHandle, style.textureTilingAndOffset), rotateAngle, top, right, left, style, drawOrder);
		else
			FillTri_Round(&m_bufferStore.GetData().GetDefaultBuffer(style.userData, style.uniqueID, drawOrder, DrawBufferShapeType::Shape, style.textureHandle, style.textureTilingAndOffset), style.onlyRoundTheseCorners, rotateAngle, top, right, left, style.rounding, style, drawOrder);
	}

	void Drawer::DrawRect(const Vec2& min, const Vec2& max, StyleOptions& style, float rotateAngle, int drawOrder)
	{
		/*const Vec4ui& clip = m_bufferStore.GetData().m_clipRect;
		if (clip.z != 0 || clip.w != 0)
		{
			if (!clip.IsPointInside(min) && !clip.IsPointInside(max))
				return;
		}*/

		if (Math::IsEqualMarg(style.rounding, 0.0f))
			FillRect_NoRound(&m_bufferStore.GetData().GetDefaultBuffer(style.userData, style.uniqueID, drawOrder, DrawBufferShapeType::Shape, style.textureHandle, style.textureTilingAndOffset), rotateAngle, min, max, style, drawOrder);
		else
			FillRect_Round(&m_bufferStore.GetData().GetDefaultBuffer(style.userData, style.uniqueID, drawOrder, DrawBufferShapeType::Shape, style.textureHandle, style.textureTilingAndOffset), style.onlyRoundTheseCorners, rotateAngle, min, max, style.rounding, style, drawOrder);
	}

	void Drawer::DrawNGon(const Vec2& center, float radius, int n, StyleOptions& style, float rotateAngle, int drawOrder)
	{
		/*const Vec4ui& clip = m_bufferStore.GetData().m_clipRect;
		if (clip.z != 0 || clip.w != 0)
		{
			Vec2 min = {center.x - radius, center.y - radius};
			Vec2 max = {center.x + radius, center.y + radius};
			if (!clip.IsPointInside(min) && !clip.IsPointInside(max))
				return;
		}*/

		FillNGon(&m_bufferStore.GetData().GetDefaultBuffer(style.userData, style.uniqueID, drawOrder, DrawBufferShapeType::Shape, style.textureHandle, style.textureTilingAndOffset), rotateAngle, center, radius, n, style, drawOrder);
	}

	void Drawer::DrawConvex(Vec2* points, int size, StyleOptions& style, float rotateAngle, int drawOrder)
	{
		if (size < 3)
		{
			if (Config.errorCallback)
				Config.errorCallback("LinaVG: Can't draw a convex shape that has less than 3 corners!");
			return;
		}

		/*const Vec4ui& clip = m_bufferStore.GetData().m_clipRect;
		if (clip.z != 0 || clip.w != 0)
		{
			Vec2 min = {}, max = {};
			GetConvexBoundingBox(points, size, min, max);
			if (!clip.IsPointInside(min) && !clip.IsPointInside(max))
				return;
		}*/

		const Vec2 avgCenter = Math::GetPolygonCentroidFast(points, size);
		FillConvex(&m_bufferStore.GetData().GetDefaultBuffer(style.userData, style.uniqueID, drawOrder, DrawBufferShapeType::Shape, style.textureHandle, style.textureTilingAndOffset), rotateAngle, points, size, avgCenter, style, drawOrder);
	}

	void Drawer::DrawCircle(const Vec2& center, float radius, StyleOptions& style, int segments, float rotateAngle, float startAngle, float endAngle, int drawOrder)
	{
		if (startAngle == endAngle)
			endAngle = startAngle + 360.0f;

		/*const Vec4ui& clip = m_bufferStore.GetData().m_clipRect;
		if (clip.z != 0 || clip.w != 0)
		{
			Vec2 min = {center.x - radius, center.y - radius};
			Vec2 max = {center.x + radius, center.y + radius};
			if (!clip.IsPointInside(min) && !clip.IsPointInside(max))
				return;
		}*/

		FillCircle(&m_bufferStore.GetData().GetDefaultBuffer(style.userData, style.uniqueID, drawOrder, DrawBufferShapeType::Shape, style.textureHandle, style.textureTilingAndOffset), rotateAngle, center, radius, segments, startAngle, endAngle, style, drawOrder);
	}

#ifndef LINAVG_DISABLE_TEXT_SUPPORT

	LINAVG_API void Drawer::DrawTextDefault(const char* text, const Vec2& position, const TextOptions& opts, float rotateAngle, int drawOrder, bool skipCache, TextOutData* outData)
	{
		if (text == NULL || text[0] == '\0')
			return;

		Font* font = opts.font;

		DrawBuffer* buf		   = &m_bufferStore.GetData().GetDefaultBuffer(opts.userData, opts.uniqueID, drawOrder, font->isSDF ? DrawBufferShapeType::SDFText : DrawBufferShapeType::Text, font->atlas, Vec4(1, 1, 0, 0));
		const int	vtxStart   = buf->vertexBuffer.m_size;
		const int	indexStart = buf->indexBuffer.m_size;

		const bool clipTexts = false; // linavg side cpu clipping is disabled for now.

		if (!Config.textCachingEnabled || skipCache)
			ProcessText(buf, font, text, position, Vec2(0.0f, 0.0f), opts.color, opts, rotateAngle, outData, clipTexts);
		else
		{
			const int vtxSize = buf->vertexBuffer.m_size;
			const int idxSize = buf->indexBuffer.m_size;

			uint32_t sid = Utility::FnvHash(text);
			if (m_bufferStore.GetData().CheckTextCache(sid, opts, buf) == nullptr)
			{
				ProcessText(buf, font, text, Vec2(0, 0), Vec2(0.0f, 0.0f), opts.color, opts, rotateAngle, outData, false);
				m_bufferStore.GetData().AddTextCache(sid, opts, buf, vtxStart, indexStart);
			}

			// Update position
			for (int i = vtxStart; i < buf->vertexBuffer.m_size; i++)
			{
				auto& vtx = buf->vertexBuffer[i];
				vtx.pos.x += static_cast<float>(Math::CustomRound(position.x));
				vtx.pos.y += static_cast<float>(Math::CustomRound(position.y));
			}
		}
	}

	LINAVG_API Vec2 Drawer::CalculateTextSize(const char* text, TextOptions& opts)
	{
		if (Math::IsEqualMarg(opts.wrapWidth, 0.0f, 0.1f))
			return CalcTextSize(text, opts);
		else
			return CalcTextSizeWrapped(text, opts);
	}

#endif

	void Drawer::FillRect_NoRound(DrawBuffer* buf, float rotateAngle, const Vec2& min, const Vec2& max, StyleOptions& opts, int drawOrder)
	{
		Vertex v[4];
		FillRectData(v, false, min, max);

		const int current = buf->vertexBuffer.m_size;

		for (int i = 0; i < 4; i++)
			buf->PushVertex(v[i]);

		const Vec2 center = Vec2((min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f);

		if (opts.isFilled)
		{
			buf->PushIndex(current);
			buf->PushIndex(current + 1);
			buf->PushIndex(current + 3);
			buf->PushIndex(current + 1);
			buf->PushIndex(current + 2);
			buf->PushIndex(current + 3);
		}
		else
			ConvexExtrudeVertices(buf, opts, center, current, current + 3, opts.thickness.start);

		const int currentNow = buf->vertexBuffer.m_size;
		New_CalculateVertexUVsAndColor(buf, current, currentNow, v[0].pos, v[2].pos, opts.color);

		RotateVertices(buf->vertexBuffer, center, current, opts.isFilled ? current + 3 : current + 7, rotateAngle);

		if (!Math::IsEqualMarg(opts.outlineOptions.thickness, 0.0f))
			DrawOutline(buf, opts, opts.isFilled ? 4 : 8, false, drawOrder);
		else if (opts.aaEnabled)
		{
			StyleOptions opts2	 = StyleOptions(opts);
			opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
			DrawOutline(buf, opts2, opts2.isFilled ? 4 : 8, false, drawOrder, OutlineCallType::AA);
		}
	}

	void Drawer::FillRect_Round(DrawBuffer* buf, Array<int>& roundedCorners, float rotateAngle, const Vec2& min, const Vec2& max, float rounding, StyleOptions& opts, int drawOrder)
	{
		rounding = Math::Clamp(rounding, 0.0f, 0.9f);

		Vertex v[4];
		FillRectData(v, false, min, max);

		// Calculate direction vectors
		const Vec2	center			 = Vec2((min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f);
		Vec2		up				 = Vec2(v[0].pos.x - v[3].pos.x, v[0].pos.y - v[3].pos.y);
		Vec2		right			 = Vec2(v[1].pos.x - v[0].pos.x, v[1].pos.y - v[0].pos.y);
		const float verticalMag		 = Math::Mag(up);
		const float horizontalMag	 = Math::Mag(right);
		const float halfShortestSide = ((verticalMag > horizontalMag ? horizontalMag : verticalMag) / 2.0f); // 0.9f is internal offset just to make sure we never get to full
		up							 = Math::Normalized(up);
		right						 = Math::Normalized(right);

		// Max rounding.
		const float roundingMag = rounding * halfShortestSide;

		// For each corner vertices, first we inflate them towards the center by the magnitude.
		float		startAngle	  = 180.0f;
		float		endAngle	  = 270.0f;
		const float angleIncrease = GetAngleIncrease(rounding);
		const int	startIndex	  = buf->vertexBuffer.m_size;
		int			vertexCount	  = 0;

		// Eventually we will be filling a convex shape, so we need a center vertex, it's position is already "center",
		// now calculate it's vertex m_color & push it down the pipeline.
		if (opts.isFilled)
		{
			Vertex c;
			c.pos = center;
			// c.uv  = Vec2(0.5f, 0.5f);
			buf->PushVertex(c);
		}

		for (int i = 0; i < 4; i++)
		{
			const int found = roundedCorners.findIndex(i);
			if (roundedCorners.m_size != 0 && found == -1)
			{
				Vertex cornerVertex;
				cornerVertex.pos = v[i].pos;
				buf->PushVertex(cornerVertex);
				vertexCount++;
				startAngle += 90.0f;
				endAngle += 90.0f;
				continue;
			}

			const Vec2 usedRight = (i == 0 || i == 3) ? right : Vec2(-right.x, -right.y);
			const Vec2 usedUp	 = (i == 0 || i == 1) ? Vec2(-up.x, -up.y) : up;
			Vec2	   inf0		 = Vec2(v[i].pos.x + usedUp.x * roundingMag, v[i].pos.y + usedUp.y * roundingMag);
			Vec2	   inf1		 = Vec2(inf0.x + usedRight.x * roundingMag, inf0.y + usedRight.y * roundingMag);

			// After a vector is inflated, we use it as a center to draw an arc, arc range is based on which corner we are currently drawing.
			for (float k = startAngle; k < endAngle + 2.5f; k += angleIncrease)
			{
				const Vec2 p = Math::GetPointOnCircle(inf1, roundingMag, k);
				Vertex	   cornerVertex;
				cornerVertex.pos = p;
				buf->PushVertex(cornerVertex);
				vertexCount++;
			}
			startAngle += 90.0f;
			endAngle += 90.0f;
		}

		const int currentIndex = buf->vertexBuffer.m_size;
		New_CalculateVertexUVsAndColor(buf, startIndex, currentIndex, v[0].pos, v[2].pos, opts.color);

		if (opts.isFilled)
		{
			CalculateVertexUVs(buf, startIndex, opts.isFilled ? startIndex + vertexCount : startIndex + vertexCount - 1);
			ConvexFillVertices(startIndex, startIndex + vertexCount, buf->indexBuffer);
		}
		else
			ConvexExtrudeVertices(buf, opts, center, startIndex, startIndex + vertexCount - 1, opts.thickness.start);

		RotateVertices(buf->vertexBuffer, center, opts.isFilled ? startIndex + 1 : startIndex, opts.isFilled ? startIndex + vertexCount : startIndex + (vertexCount * 2) - 1, rotateAngle);

		if (!Math::IsEqualMarg(opts.outlineOptions.thickness, 0.0f))
			DrawOutline(buf, opts, opts.isFilled ? vertexCount : vertexCount * 2, false, drawOrder);
		else if (opts.aaEnabled)
		{
			StyleOptions opts2	 = StyleOptions(opts);
			opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
			DrawOutline(buf, opts2, opts2.isFilled ? vertexCount : vertexCount * 2, false, drawOrder, OutlineCallType::AA);
		}
	}

	void Drawer::FillRectData(Vertex* v, bool hasCenter, const Vec2& min, const Vec2& max)
	{
		const int  i	  = hasCenter ? 1 : 0;
		const Vec2 center = Vec2((max.x + min.x) / 2.0f, (max.y + min.y) / 2.0f);

		if (hasCenter)
		{
			v[0].pos = center;
			// v[0].uv	 = Vec2((m_bufferStore.GetData().m_uvOverride.m_uvTL.x + m_bufferStore.GetData().m_uvOverride.m_uvBR.x) / 2.0f, (m_bufferStore.GetData().m_uvOverride.m_uvTL.y + m_bufferStore.GetData().m_uvOverride.m_uvBR.y) / 2.0f);
		}

		if (!m_bufferStore.GetData().m_rectOverrideData.overrideRectPositions)
		{
			v[i].pos	   = min;
			v[i + 1].pos.x = max.x;
			v[i + 1].pos.y = min.y;
			v[i + 2].pos.x = max.x;
			v[i + 2].pos.y = max.y;
			v[i + 3].pos.x = min.x;
			v[i + 3].pos.y = max.y;
		}
		else
		{
			v[i].pos	 = m_bufferStore.GetData().m_rectOverrideData.m_p1;
			v[i + 1].pos = m_bufferStore.GetData().m_rectOverrideData.m_p2;
			v[i + 2].pos = m_bufferStore.GetData().m_rectOverrideData.m_p3;
			v[i + 3].pos = m_bufferStore.GetData().m_rectOverrideData.m_p4;
		}

		// v[i].uv		= m_bufferStore.GetData().m_uvOverride.m_uvTL;
		// v[i + 1].uv = Vec2(m_bufferStore.GetData().m_uvOverride.m_uvBR.x, m_bufferStore.GetData().m_uvOverride.m_uvTL.y);
		// v[i + 2].uv = m_bufferStore.GetData().m_uvOverride.m_uvBR;
		// v[i + 3].uv = Vec2(m_bufferStore.GetData().m_uvOverride.m_uvTL.x, m_bufferStore.GetData().m_uvOverride.m_uvBR.y);
	}

	void Drawer::FillTri_NoRound(DrawBuffer* buf, float rotateAngle, const Vec2& p3, const Vec2& p2, const Vec2& p1, StyleOptions& opts, int drawOrder)
	{
		Vertex v[3];
		FillTriData(v, false, p3, p2, p1);

		const int startIndex = buf->vertexBuffer.m_size;

		for (int i = 0; i < 3; i++)
			buf->PushVertex(v[i]);

		const Vec2 center = Vec2((p1.x + p2.x + p3.x) / 3.0f, (p1.y + p2.y + p3.y) / 3.0f);

		if (opts.isFilled)
		{
			buf->PushIndex(startIndex);
			buf->PushIndex(startIndex + 1);
			buf->PushIndex(startIndex + 2);
		}
		else
			ConvexExtrudeVertices(buf, opts, center, startIndex, startIndex + 2, opts.thickness.start);

		const int currentIndex = buf->vertexBuffer.m_size;

		Vec2 bbMin, bbMax;
		GetTriangleBoundingBox(p1, p2, p3, bbMin, bbMax);
		New_CalculateVertexUVsAndColor(buf, startIndex, currentIndex, bbMin, bbMax, opts.color);

		RotateVertices(buf->vertexBuffer, center, startIndex, opts.isFilled ? startIndex + 2 : startIndex + 5, rotateAngle);

		if (!Math::IsEqualMarg(opts.outlineOptions.thickness, 0.0f))
			DrawOutline(buf, opts, opts.isFilled ? 3 : 6, false, drawOrder);
		else if (opts.aaEnabled)
		{
			StyleOptions opts2	 = StyleOptions(opts);
			opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
			DrawOutline(buf, opts2, opts2.isFilled ? 3 : 6, false, drawOrder, OutlineCallType::AA);
		}
	}

	void Drawer::FillTri_Round(DrawBuffer* buf, Array<int>& onlyRoundCorners, float rotateAngle, const Vec2& p3, const Vec2& p2, const Vec2& p1, float rounding, StyleOptions& opts, int drawOrder)
	{
		rounding = Math::Clamp(rounding, 0.0f, 1.0f);

		Vertex v[3];
		FillTriData(v, false, p3, p2, p1);

		const Vec2	center			= Vec2((p1.x + p2.x + p3.x) / 3.0f, (p1.y + p2.y + p3.y) / 3.0f);
		const Vec2	v01Edge			= Vec2(v[0].pos.x - v[1].pos.x, v[0].pos.y - v[1].pos.y);
		const Vec2	v02Edge			= Vec2(v[0].pos.x - v[2].pos.x, v[0].pos.y - v[2].pos.y);
		const Vec2	v12Edge			= Vec2(v[1].pos.x - v[2].pos.x, v[1].pos.y - v[2].pos.y);
		const Vec2	v01Center		= Vec2((v[0].pos.x + v[1].pos.x) / 2.0f, (v[0].pos.y + v[1].pos.y) / 2.0f);
		const Vec2	v02Center		= Vec2((v[0].pos.x + v[2].pos.x) / 2.0f, (v[0].pos.y + v[2].pos.y) / 2.0f);
		const Vec2	v12Center		= Vec2((v[1].pos.x + v[2].pos.x) / 2.0f, (v[1].pos.y + v[2].pos.y) / 2.0f);
		const float mag01			= Math::Mag(v01Edge);
		const float mag02			= Math::Mag(v02Edge);
		const float mag12			= Math::Mag(v12Edge);
		const float ang0102			= Math::GetAngleBetween(v01Edge, v02Edge);
		const float ang0112			= Math::GetAngleBetween(v01Edge, v12Edge);
		const float ang0212			= Math::GetAngleBetween(v02Edge, v12Edge);
		const float maxAngle		= Math::Max(Math::Max(ang0102, ang0112), ang0212);
		const float shortestEdgeMag = Math::Min(Math::Min(mag01, mag02), mag12);
		const float roundingMag		= rounding * shortestEdgeMag / 2.0f;

		// For each corner vertices, first we inflate them towards the center by the magnitude.
		const int startIndex  = buf->vertexBuffer.m_size;
		int		  vertexCount = 0;

		// Eventually we will be filling a convex shape, so we need a center vertex, it's position is already "center",
		// now calculate it's vertex m_color & push it down the pipeline.
		if (opts.isFilled)
		{
			Vertex c;
			c.pos = center;
			buf->PushVertex(c);
		}

		const float angleOffset = maxAngle > 90.0f ? maxAngle - 90.0f : 45.0f;

		for (int i = 0; i < 3; i++)
		{

			if (onlyRoundCorners.m_size != 0 && onlyRoundCorners.findIndex(i) == -1)
			{
				Vertex cornerVertex;
				cornerVertex.pos = v[i].pos;
				vertexCount++;
				buf->PushVertex(cornerVertex);
				continue;
			}

			if (i == 0)
			{
				const Vec2	toCenter01 = Math::Normalized(Vec2(v01Center.x - v[i].pos.x, v01Center.y - v[i].pos.y));
				const Vec2	toCenter02 = Math::Normalized(Vec2(v02Center.x - v[i].pos.x, v02Center.y - v[i].pos.y));
				const Vec2	inter1	   = Vec2(v[i].pos.x + toCenter01.x * roundingMag, v[i].pos.y + toCenter01.y * roundingMag);
				const Vec2	inter2	   = Vec2(v[i].pos.x + toCenter02.x * roundingMag, v[i].pos.y + toCenter02.y * roundingMag);
				Array<Vec2> arc;

				GetArcPoints(arc, inter1, inter2, v[i].pos, 0.0f, 36, false, angleOffset);

				for (int j = 0; j < arc.m_size; j++)
				{
					Vertex cornerVertex;
					cornerVertex.pos = arc[j];
					buf->PushVertex(cornerVertex);
					vertexCount++;
				}
			}
			else if (i == 1)
			{

				const Vec2	toCenter01 = Math::Normalized(Vec2(v01Center.x - v[i].pos.x, v01Center.y - v[i].pos.y));
				const Vec2	toCenter12 = Math::Normalized(Vec2(v12Center.x - v[i].pos.x, v12Center.y - v[i].pos.y));
				const Vec2	inter1	   = Vec2(v[i].pos.x + toCenter01.x * roundingMag, v[i].pos.y + toCenter01.y * roundingMag);
				const Vec2	inter2	   = Vec2(v[i].pos.x + toCenter12.x * roundingMag, v[i].pos.y + toCenter12.y * roundingMag);
				Array<Vec2> arc;
				GetArcPoints(arc, inter1, inter2, v[i].pos, 0.0f, 36, false, angleOffset);
				for (int j = 0; j < arc.m_size; j++)
				{
					Vertex cornerVertex;
					cornerVertex.pos = arc[j];
					buf->PushVertex(cornerVertex);
					vertexCount++;
				}
			}
			else if (i == 2)
			{

				const Vec2	toCenter12 = Math::Normalized(Vec2(v12Center.x - v[i].pos.x, v12Center.y - v[i].pos.y));
				const Vec2	toCenter02 = Math::Normalized(Vec2(v02Center.x - v[i].pos.x, v02Center.y - v[i].pos.y));
				const Vec2	inter1	   = Vec2(v[i].pos.x + toCenter12.x * roundingMag, v[i].pos.y + toCenter12.y * roundingMag);
				const Vec2	inter2	   = Vec2(v[i].pos.x + toCenter02.x * roundingMag, v[i].pos.y + toCenter02.y * roundingMag);
				Array<Vec2> arc;
				GetArcPoints(arc, inter1, inter2, v[i].pos, 0.0f, 36, false, angleOffset);
				for (int j = 0; j < arc.m_size; j++)
				{
					Vertex cornerVertex;
					cornerVertex.pos = arc[j];
					buf->PushVertex(cornerVertex);
					vertexCount++;
				}
			}
		}

		if (opts.isFilled)
			ConvexFillVertices(startIndex, startIndex + vertexCount, buf->indexBuffer);
		else
			ConvexExtrudeVertices(buf, opts, center, startIndex, startIndex + vertexCount - 1, opts.thickness.start);

		Vec2 bbMin, bbMax;
		GetTriangleBoundingBox(p1, p2, p3, bbMin, bbMax);
		New_CalculateVertexUVsAndColor(buf, startIndex, buf->vertexBuffer.m_size, bbMin, bbMax, opts.color);

		RotateVertices(buf->vertexBuffer, center, opts.isFilled ? startIndex + 1 : startIndex, opts.isFilled ? startIndex + vertexCount : startIndex + (vertexCount * 2) - 1, rotateAngle);

		if (!Math::IsEqualMarg(opts.outlineOptions.thickness, 0.0f))
			DrawOutline(buf, opts, opts.isFilled ? vertexCount : vertexCount * 2, false, drawOrder);
		else if (opts.aaEnabled)
		{
			StyleOptions opts2	 = StyleOptions(opts);
			opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
			DrawOutline(buf, opts2, opts2.isFilled ? vertexCount : vertexCount * 2, false, drawOrder, OutlineCallType::AA);
		}
	}

	void Drawer::FillTriData(Vertex* v, bool hasCenter, const Vec2& p3, const Vec2& p2, const Vec2& p1)
	{
		const int i = hasCenter ? 1 : 0;
		if (hasCenter)
		{
			Vec2 center = Vec2((p1.x + p2.x + p3.x) / 3.0f, (p1.y + p2.y + p3.y) / 3.0f);
			v[0].pos	= center;
		}

		v[i].pos	 = p3;
		v[i + 1].pos = p2;
		v[i + 2].pos = p1;
	}

	void Drawer::FillNGon(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int n, StyleOptions& opts, int drawOrder)
	{
		Array<Vertex> v;
		FillNGonData(v, opts.isFilled, center, radius, n);

		const int startIndex = buf->vertexBuffer.m_size;

		for (int i = 0; i < v.m_size; i++)
		{
			buf->PushVertex(v[i]);
		}

		if (opts.isFilled)
			ConvexFillVertices(startIndex, startIndex + n, buf->indexBuffer);
		else
			ConvexExtrudeVertices(buf, opts, center, startIndex, startIndex + n - 1, opts.thickness.start);

		const Vec2 bbMin = Vec2(center.x - radius, center.y - radius);
		const Vec2 bbMax = Vec2(center.x + radius, center.y + radius);
		New_CalculateVertexUVsAndColor(buf, startIndex, buf->vertexBuffer.m_size, bbMin, bbMax, opts.color);

		RotateVertices(buf->vertexBuffer, center, opts.isFilled ? startIndex + 1 : startIndex, opts.isFilled ? startIndex + n : startIndex + (n * 2) - 1, rotateAngle);

		if (!Math::IsEqualMarg(opts.outlineOptions.thickness, 0.0f))
			DrawOutline(buf, opts, opts.isFilled ? n : n * 2, false, drawOrder);
		else if (opts.aaEnabled)
		{
			StyleOptions opts2	 = StyleOptions(opts);
			opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
			DrawOutline(buf, opts2, opts2.isFilled ? n : n * 2, false, drawOrder, OutlineCallType::AA);
		}
	}

	void Drawer::FillNGonData(Array<Vertex>& vertArray, bool hasCenter, const Vec2& center, float radius, int n)
	{
		const float angleIncrease = 360.0f / (float)n;
		if (hasCenter)
		{
			Vertex v;
			v.pos = center;
			vertArray.push_back(v);
		}

		int count = 0;
		for (float i = 0.0f; i < 360.0f; i += angleIncrease)
		{
			Vertex v;
			v.pos = Math::GetPointOnCircle(center, radius, i);
			vertArray.push_back(v);
			count++;
			if (count == n)
				return;
		}
	}

	void Drawer::FillCircle(DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int segments, float startAngle, float endAngle, StyleOptions& opts, int drawOrder)
	{
		Array<Vertex> v;
		FillCircleData(v, opts.isFilled, center, radius, segments, startAngle, endAngle);

		const int startIndex = buf->vertexBuffer.m_size;

		for (int i = 0; i < v.m_size; i++)
			buf->PushVertex(v[i]);

		const bool isFullCircle = Math::Abs(endAngle - startAngle) == 360.0f;
		const int  totalSize	= v.m_size - 1;

		if (opts.isFilled)
			ConvexFillVertices(startIndex, startIndex + totalSize, buf->indexBuffer, !isFullCircle);
		else
			ConvexExtrudeVertices(buf, opts, center, startIndex, startIndex + totalSize, opts.thickness.start, !isFullCircle);

		RotateVertices(buf->vertexBuffer, center, opts.isFilled ? startIndex + 1 : startIndex, opts.isFilled ? startIndex + totalSize : startIndex + (totalSize * 2) + 1, rotateAngle);

		const Vec2 bbMin = Vec2(center.x - radius, center.y - radius);
		const Vec2 bbMax = Vec2(center.x + radius, center.y + radius);
		New_CalculateVertexUVsAndColor(buf, startIndex, buf->vertexBuffer.m_size, bbMin, bbMax, opts.color);

		if (!Math::IsEqualMarg(opts.outlineOptions.thickness, 0.0f))
		{
			if (isFullCircle)
				DrawOutline(buf, opts, opts.isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder);
			else
			{
				if (opts.isFilled)
				{
					Array<int> indices;

					for (int i = 0; i < v.m_size; i++)
						indices.push_back(startIndex + i);

					DrawOutlineAroundShape(buf, opts, &indices[0], v.m_size, opts.outlineOptions.thickness, true, drawOrder);
				}
				else if (opts.outlineOptions.drawDirection == OutlineDrawDirection::Both)
				{
					Array<int> indices;

					const int halfSize = v.m_size;
					const int fullSize = halfSize * 2;
					for (int i = 0; i < halfSize; i++)
						indices.push_back(startIndex + i);

					for (int i = fullSize - 1; i > halfSize - 1; i--)
						indices.push_back(startIndex + i);

					DrawOutlineAroundShape(buf, opts, &indices[0], halfSize * 2, opts.outlineOptions.thickness, false, drawOrder);
				}
				else
					DrawOutline(buf, opts, opts.isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder);
			}
		}
		else if (opts.aaEnabled)
		{
			StyleOptions opts2	 = StyleOptions(opts);
			opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);

			if (opts.isFilled)
			{
				if (isFullCircle)
				{
					DrawOutline(buf, opts, opts.isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder, OutlineCallType::AA);
				}
				else
				{
					Array<int> indices;

					for (int i = 0; i < v.m_size; i++)
						indices.push_back(startIndex + i);

					DrawOutlineAroundShape(buf, opts2, &indices[0], v.m_size, opts2.outlineOptions.thickness, true, drawOrder, OutlineCallType::AA);
				}
			}
			else if (opts.outlineOptions.drawDirection == OutlineDrawDirection::Both)
			{
				Array<int> indices;

				const int halfSize = v.m_size;
				const int fullSize = halfSize * 2;
				for (int i = 0; i < halfSize; i++)
					indices.push_back(startIndex + i);

				for (int i = fullSize - 1; i > halfSize - 1; i--)
					indices.push_back(startIndex + i);

				DrawOutlineAroundShape(buf, opts2, &indices[0], halfSize * 2, opts2.outlineOptions.thickness, false, drawOrder, OutlineCallType::AA);
			}
			else
				DrawOutline(buf, opts2, opts2.isFilled ? totalSize : (totalSize + 1) * 2, !isFullCircle, drawOrder, OutlineCallType::AA);
		}
	}

	void Drawer::FillCircleData(Array<Vertex>& vertices, bool hasCenter, const Vec2& center, float radius, int segments, float startAngle, float endAngle)
	{
		if (startAngle < 0.0f)
			startAngle += 360.0f;
		if (endAngle < 0.0f)
			endAngle += 360.0f;

		if (endAngle == startAngle)
		{
			endAngle   = 0.0f;
			startAngle = 360.0f;
		}

		segments				  = Math::Clamp(segments, 6, 180);
		const float angleIncrease = 360.0f / (float)segments;

		if (hasCenter)
		{
			Vertex c;
			c.pos = center;
			vertices.push_back(c);
		}

		const float end		  = Math::Abs(startAngle - endAngle) == 360.0f ? endAngle : endAngle + angleIncrease;
		Vec2		nextPoint = Vec2(-1.0f, -1.0f);
		Vec2		lastPoint = Vec2(-1.0f, -1.0f);
		for (float i = startAngle; i < end; i += angleIncrease)
		{
			Vertex v;
			v.pos = Math::GetPointOnCircle(center, radius, i);
			vertices.push_back(v);
		}
	}

	void Drawer::FillConvex(DrawBuffer* buf, float rotateAngle, Vec2* points, int size, const Vec2& center, StyleOptions& opts, int drawOrder)
	{
		const int startIndex = buf->vertexBuffer.m_size;

		if (opts.isFilled)
		{
			Vertex c;
			c.pos = center;
			buf->PushVertex(c);
		}

		for (int i = 0; i < size; i++)
		{
			Vertex v;
			v.pos = points[i];
			buf->PushVertex(v);
		}

		if (opts.isFilled)
			ConvexFillVertices(startIndex, startIndex + size, buf->indexBuffer);
		else
			ConvexExtrudeVertices(buf, opts, center, startIndex, startIndex + size - 1, opts.thickness.start);

		Vec2 bbMin, bbMax;
		GetConvexBoundingBox(points, size, bbMin, bbMax);
		New_CalculateVertexUVsAndColor(buf, startIndex, buf->vertexBuffer.m_size, bbMin, bbMax, opts.color);

		RotateVertices(buf->vertexBuffer, center, opts.isFilled ? startIndex + 1 : startIndex, opts.isFilled ? startIndex + size : startIndex + (size * 2) - 1, rotateAngle);

		if (!Math::IsEqualMarg(opts.outlineOptions.thickness, 0.0f))
			DrawOutline(buf, opts, opts.isFilled ? size : size * 2, false, drawOrder);
		else if (opts.aaEnabled)
		{
			StyleOptions opts2	 = StyleOptions(opts);
			opts2.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Both);
			DrawOutline(buf, opts2, opts2.isFilled ? size : size * 2, false, drawOrder, OutlineCallType::AA);
		}
	}

	void Drawer::ConvexFillVertices(int startIndex, int endIndex, Array<Index>& indices, bool skipLastTriangle)
	{
		// i = 0 is center.
		for (int i = startIndex + 1; i < endIndex; i++)
		{
			indices.push_back(startIndex);
			indices.push_back(i);
			indices.push_back(i + 1);
		}

		// Last fill.
		if (!skipLastTriangle)
		{
			indices.push_back(startIndex);
			indices.push_back(startIndex + 1);
			indices.push_back(endIndex);
		}
	}

	void Drawer::ConvexExtrudeVertices(DrawBuffer* buf, const StyleOptions& opts, const Vec2& center, int startIndex, int endIndex, float thickness, bool skipEndClosing)
	{
		const int totalSize = endIndex - startIndex + 1;

		// Extrude vertices.
		for (int i = startIndex; i < startIndex + totalSize; i++)
		{
			// take two edges, this vertex to next and previous to this in order to calculate vertex normals.
			const int previous = i == startIndex ? endIndex : i - 1;
			const int next	   = i == endIndex ? startIndex : i + 1;
			Vertex	  v;
			v.col = buf->vertexBuffer[i].col;
			if (skipEndClosing && i == startIndex)
			{
				const Vec2 toNext  = Math::Normalized(Vec2(buf->vertexBuffer[next].pos.x - buf->vertexBuffer[i].pos.x, buf->vertexBuffer[next].pos.y - buf->vertexBuffer[i].pos.y));
				const Vec2 rotated = Math::Rotate90(toNext, true);
				v.pos			   = Vec2(buf->vertexBuffer[i].pos.x + rotated.x * thickness * 2, buf->vertexBuffer[i].pos.y + rotated.y * thickness * 2);
			}
			else if (skipEndClosing && i == endIndex)
			{
				const Vec2 fromPrev = Math::Normalized(Vec2(buf->vertexBuffer[i].pos.x - buf->vertexBuffer[previous].pos.x, buf->vertexBuffer[i].pos.y - buf->vertexBuffer[previous].pos.y));
				const Vec2 rotated	= Math::Rotate90(fromPrev, true);
				v.pos				= Vec2(buf->vertexBuffer[i].pos.x + rotated.x * thickness * 2, buf->vertexBuffer[i].pos.y + rotated.y * thickness * 2);
			}
			else
			{
				// const Vec2 vertexNormalAverage = Math::GetVertexNormal(buf->m_vertexBuffer[i].pos, buf->m_vertexBuffer[previous].pos, buf->m_vertexBuffer[next].pos);
				// v.pos						   = Vec2(buf->m_vertexBuffer[i].pos.x + vertexNormalAverage.x * thickness, buf->m_vertexBuffer[i].pos.y + vertexNormalAverage.y * thickness);
				v.pos = Math::GetExtrudedFromNormal(buf->vertexBuffer[i].pos, buf->vertexBuffer[previous].pos, buf->vertexBuffer[next].pos, thickness);
			}

			buf->PushVertex(v);
		}

		// CalculateVertexUVs(buf, startIndex, endIndex + totalSize);

		for (int i = startIndex; i < startIndex + totalSize; i++)
		{
			int next = i + 1;
			if (next >= startIndex + totalSize)
				next = startIndex;

			if (skipEndClosing && i == startIndex + totalSize - 1)
				return;

			buf->PushIndex(i);
			buf->PushIndex(next);
			buf->PushIndex(i + totalSize);
			buf->PushIndex(next);
			buf->PushIndex(next + totalSize);
			buf->PushIndex(i + totalSize);
		}
	}

	void Drawer::RotateVertices(Array<Vertex>& vertices, const Vec2& center, int startIndex, int endIndex, float angle)
	{
		if (Math::IsEqualMarg(angle, 0.0f))
			return;

		for (int i = startIndex; i < endIndex + 1; i++)
		{
			vertices[i].pos = Math::RotateAround(vertices[i].pos, center, angle);
		}
	}

	void Drawer::RotatePoints(Vec2* points, int size, const Vec2& center, float angle)
	{
		for (int i = 0; i < size; i++)
		{
			points[i] = Math::RotateAround(points[i], center, angle);
		}
	}

	void Drawer::GetArcPoints(Array<Vec2>& points, const Vec2& p1, const Vec2& p2, Vec2 directionHintPoint, float radius, float segments, bool flip, float angleOffset)
	{
		const float halfMag = Math::Mag(Vec2(p2.x - p1.x, p2.y - p1.y)) / 2.0f;
		const Vec2	center	= Vec2((p1.x + p2.x) / 2.0f, (p1.y + p2.y) / 2.0f);
		const Vec2	dir		= Vec2(p2.x - p1.x, p2.y - p1.y);

		// Determine flip if we have a hint point.
		if (!Math::IsEqual(directionHintPoint, Vec2(-1.0f, -1.0f)))
		{
			if (Math::IsEqualMarg(p1.x - p2.x, 0.0f))
			{
				// Flipped Y axis, if p1 is above.
				if (p1.y < p2.y)
				{
					if (directionHintPoint.x < p1.x)
						flip = true;
				}
				else
				{
					// if p2 is above.
					if (directionHintPoint.x > p1.x)
						flip = true;
				}
			}
			else
			{
				const Vec2 centerToDirHint = Vec2(directionHintPoint.x - center.x, directionHintPoint.y - center.y);
				// p2 is on the right, p1 on the left
				if (p2.x > p1.x)
				{
					if (centerToDirHint.y > 0.0f)
						flip = true;
					else if (Math::IsEqualMarg(centerToDirHint.y, 0.0f))
					{
						if (centerToDirHint.x < 0.0f)
							flip = true;
					}
				}
				else
				{
					// p2 is on the left, p1 is on the right.
					if (centerToDirHint.y < 0.0f)
						flip = true;
					else if (Math::IsEqualMarg(centerToDirHint.y, 0.0f))
					{
						if (centerToDirHint.x > 0.0f)
							flip = true;
					}
				}
			}
		}

		float angle1 = Math::GetAngleFromCenter(center, flip ? p2 : p1);
		float angle2 = Math::GetAngleFromCenter(center, flip ? p1 : p2);

		if (Math::IsEqualMarg(angleOffset, 0.0f))
			points.push_back(flip ? p2 : p1);

		if (angle2 < angle1)
			angle2 += 360.0f;

		// const float midAngle	  = (angle2 + angle1) / 2.0f;
		const float angleIncrease = (segments >= 180.0f || segments < 0.0f) ? 1.0f : 180.0f / (float)segments;

		for (float i = angle1 + angleIncrease + angleOffset; i < angle2 - angleOffset; i += angleIncrease)
		{
			Vec2 p = Vec2(0, 0);

			if (Math::IsEqualMarg(radius, 0.0f))
				p = Math::GetPointOnCircle(center, halfMag, i);
			else
			{
				const Vec2 out = Math::Normalized(Math::Rotate90(dir, !flip));
				p			   = Math::SampleParabola(p1, p2, out, radius, Math::Remap(i, angle1, angle2, 0.0f, 1.0f));
			}

			points.push_back(p);
		}
	}

	void Drawer::GetTriangleBoundingBox(const Vec2& p1, const Vec2& p2, const Vec2& p3, Vec2& outMin, Vec2& outMax)
	{
		outMin = Vec2(Math::Min(Math::Min(p1.x, p2.x), p3.x), Math::Min(Math::Min(p1.y, p2.y), p3.y));
		outMax = Vec2(Math::Max(Math::Max(p1.x, p2.x), p3.x), Math::Max(Math::Max(p1.y, p2.y), p3.y));
	}

	void Drawer::GetConvexBoundingBox(Vec2* points, int size, Vec2& outMin, Vec2& outMax)
	{

		outMin = Vec2(99999, 99999);
		outMax = Vec2(-99999, -99999);

		for (int i = 0; i < size; i++)
		{
			if (points[i].x < outMin.x)
				outMin.x = points[i].x;
			else if (points[i].x > outMax.x)
				outMax.x = points[i].x;
			if (points[i].y < outMin.y)
				outMin.y = points[i].y;
			else if (points[i].y > outMax.y)
				outMax.y = points[i].y;
		}
	}

	void Drawer::GetConvexBoundingBox(Vertex* points, int size, Vec2& outMin, Vec2& outMax)
	{
		outMin = Vec2(99999, 99999);
		outMax = Vec2(-99999, -99999);

		for (int i = 0; i < size; i++)
		{
			if (points[i].pos.x < outMin.x)
				outMin.x = points[i].pos.x;
			else if (points[i].pos.x > outMax.x)
				outMax.x = points[i].pos.x;
			if (points[i].pos.y < outMin.y)
				outMin.y = points[i].pos.y;
			else if (points[i].pos.y > outMax.y)
				outMax.y = points[i].pos.y;
		}
	}

	void Drawer::GetConvexBoundingBox(DrawBuffer* buf, int startIndex, int endIndex, Vec2& outMin, Vec2& outMax)
	{
		outMin = Vec2(99999, 99999);
		outMax = Vec2(-99999, -99999);

		for (int i = startIndex; i < endIndex + 1; i++)
		{
			if (buf->vertexBuffer[i].pos.x < outMin.x)
				outMin.x = buf->vertexBuffer[i].pos.x;
			else if (buf->vertexBuffer[i].pos.x > outMax.x)
				outMax.x = buf->vertexBuffer[i].pos.x;
			if (buf->vertexBuffer[i].pos.y < outMin.y)
				outMin.y = buf->vertexBuffer[i].pos.y;
			else if (buf->vertexBuffer[i].pos.y > outMax.y)
				outMax.y = buf->vertexBuffer[i].pos.y;
		}
	}

	void Drawer::CalculateVertexUVs(DrawBuffer* buf, int startIndex, int endIndex)
	{
		Vec2 bbMin, bbMax;
		GetConvexBoundingBox(&buf->vertexBuffer.m_data[startIndex], endIndex - startIndex + 1, bbMin, bbMax);

		// Recalculate UVs.
		for (int i = startIndex; i <= endIndex; i++)
		{
			buf->vertexBuffer[i].uv.x = Math::Remap(buf->vertexBuffer[i].pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
			buf->vertexBuffer[i].uv.y = Math::Remap(buf->vertexBuffer[i].pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
		}
	}

	float Drawer::GetAngleIncrease(float rounding)
	{
		if (rounding < 0.25f)
			return 20.0f;
		else if (rounding < 0.5f)
			return 15.0f;
		else if (rounding < 0.75f)
			return 10.0f;
		else
			return 5.0f;
	}

	Vec2 Drawer::GetArcDirection(const Vec2& center, float radius, float startAngle, float endAngle)
	{
		const Vec2 centerAnglePoint = Math::GetPointOnCircle(center, radius, (startAngle + endAngle) / 2.0f);
		return Vec2(centerAnglePoint.x - center.x, centerAnglePoint.y - center.y);
	}

	Vec2 Drawer::GetVerticesCenter(DrawBuffer* buf, int startIndex, int endIndex)
	{
		Vec2 total = Vec2(0.0f, 0.0f);

		for (int i = startIndex; i <= endIndex; i++)
		{
			total.x += buf->vertexBuffer[i].pos.x;
			total.y += buf->vertexBuffer[i].pos.y;
		}

		const int count = endIndex - startIndex + 1;
		total.x /= static_cast<float>(count);
		total.y /= static_cast<float>(count);
		return total;
	}

	bool Drawer::IsPointInside(const Vec2& point, const Vec4& rect)
	{
		return point.x > rect.x && point.x < (rect.x + rect.z) && point.y > rect.y && point.y < (rect.y + rect.w);
	}

	void Drawer::CalculateLine(Line& line, const Vec2& p1, const Vec2& p2, StyleOptions& style, LineCapDirection lineCapToAdd)
	{
		const Vec2 up = Math::Normalized(Math::Rotate90(Vec2(p2.x - p1.x, p2.y - p1.y), true));
		Vertex	   v0, v1, v2, v3;

		v0.pos = Vec2(p1.x + up.x * style.thickness.start / 2.0f, p1.y + up.y * style.thickness.start / 2.0f);
		v3.pos = Vec2(p1.x - up.x * style.thickness.start / 2.0f, p1.y - up.y * style.thickness.start / 2.0f);
		v1.pos = Vec2(p2.x + up.x * style.thickness.end / 2.0f, p2.y + up.y * style.thickness.end / 2.0f);
		v2.pos = Vec2(p2.x - up.x * style.thickness.end / 2.0f, p2.y - up.y * style.thickness.end / 2.0f);
		v0.col = v3.col = style.color.start;
		v1.col = v2.col = style.color.end;
		line.m_vertices.push_back(v0);
		line.m_vertices.push_back(v1);
		line.m_vertices.push_back(v2);
		line.m_vertices.push_back(v3);

		const Vec2 upRaw		  = Vec2(v0.pos.x - v3.pos.x, v0.pos.y - v3.pos.y);
		const bool willAddLineCap = lineCapToAdd == LineCapDirection::Left || lineCapToAdd == LineCapDirection::Right;

		if (willAddLineCap)
		{
			Vertex vmLeft, vmRight;
			vmLeft.pos	= Math::Lerp(v0.pos, v3.pos, 0.5f);
			vmRight.pos = Math::Lerp(v1.pos, v2.pos, 0.5f);
			vmLeft.col	= style.color.start;
			vmRight.col = style.color.end;
			line.m_vertices.push_back(vmLeft);
			line.m_vertices.push_back(vmRight);
			line.m_hasMidpoints = true;
		}

		if (willAddLineCap)
		{
			const Vertex* upVtx	  = lineCapToAdd == LineCapDirection::Left ? &v0 : &v1;
			const Vertex* downVtx = lineCapToAdd == LineCapDirection::Left ? &v3 : &v2;

			const float increase = Math::Remap(style.rounding, 0.0f, 1.0f, 0.4f, 0.1f);
			const float radius	 = (Math::Mag(upRaw) / 2.0f) * 0.6f;
			const Vec2	dir		 = Math::Rotate90(up, lineCapToAdd == LineCapDirection::Left);

			Array<int> upperParabolaPoints;
			Array<int> lowerParabolaPoints;

			for (float k = 0.0f + increase; k < 1.0f; k += increase)
			{
				const Vec2 p = Math::SampleParabola(upVtx->pos, downVtx->pos, dir, radius, k);
				Vertex	   v;
				v.col = lineCapToAdd == LineCapDirection::Left ? style.color.start : style.color.end;
				v.pos = p;

				line.m_vertices.push_back(v);
				line.m_lineCapVertexCount++;

				const float distToUp   = Math::Mag(Vec2(upVtx->pos.x - p.x, upVtx->pos.y - p.y));
				const float distToDown = Math::Mag(Vec2(downVtx->pos.x - p.x, downVtx->pos.y - p.y));

				if (distToUp < distToDown)
					upperParabolaPoints.push_back(line.m_vertices.m_size - 1);
				else
					lowerParabolaPoints.push_back(line.m_vertices.m_size - 1);
			}

			if (lineCapToAdd == LineCapDirection::Left)
			{

				for (int i = upperParabolaPoints.m_size - 1; i > -1; i--)
					line.m_upperIndices.push_back(upperParabolaPoints[i]);

				line.m_upperIndices.push_back(0);
				line.m_upperIndices.push_back(1);

				for (int i = 0; i < lowerParabolaPoints.m_size; i++)
					line.m_lowerIndices.push_back(lowerParabolaPoints[i]);

				line.m_lowerIndices.push_back(3);
				line.m_lowerIndices.push_back(2);
			}
			else
			{
				line.m_upperIndices.push_back(0);
				line.m_upperIndices.push_back(1);

				for (int i = 0; i < upperParabolaPoints.m_size; i++)
					line.m_upperIndices.push_back(upperParabolaPoints[i]);

				line.m_lowerIndices.push_back(3);
				line.m_lowerIndices.push_back(2);

				for (int i = lowerParabolaPoints.m_size - 1; i > -1; i--)
					line.m_lowerIndices.push_back(lowerParabolaPoints[i]);
			}
		}
		else
		{
			line.m_upperIndices.push_back(0);
			line.m_upperIndices.push_back(1);
			line.m_lowerIndices.push_back(3);
			line.m_lowerIndices.push_back(2);
		}

		// Draw 4 triangles if there are middle vertices.
		// Draw 2 if no middle vertices.
		if (willAddLineCap)
		{
			LineTriangle tri1, tri2, tri3, tri4;
			tri1.m_indices[0] = 0;
			tri1.m_indices[1] = 1;
			tri1.m_indices[2] = 4;
			tri2.m_indices[0] = 1;
			tri2.m_indices[1] = 4;
			tri2.m_indices[2] = 5;
			tri3.m_indices[0] = 4;
			tri3.m_indices[1] = 5;
			tri3.m_indices[2] = 3;
			tri4.m_indices[0] = 5;
			tri4.m_indices[1] = 2;
			tri4.m_indices[2] = 3;
			line.m_tris.push_back(tri1);
			line.m_tris.push_back(tri2);
			line.m_tris.push_back(tri3);
			line.m_tris.push_back(tri4);
		}
		else
		{
			LineTriangle tri1, tri2;
			tri1.m_indices[0] = 0;
			tri1.m_indices[1] = 1;
			tri1.m_indices[2] = 3;
			tri2.m_indices[0] = 1;
			tri2.m_indices[1] = 2;
			tri2.m_indices[2] = 3;
			line.m_tris.push_back(tri1);
			line.m_tris.push_back(tri2);
		}

		// Triangles for cap vertices.
		if (willAddLineCap)
		{
			const int	 middleIndex = lineCapToAdd == LineCapDirection::Left ? 4 : 5;
			const int	 upperIndex	 = lineCapToAdd == LineCapDirection::Left ? 0 : 1;
			const int	 lowerIndex	 = lineCapToAdd == LineCapDirection::Left ? 3 : 2;
			LineTriangle tri1, tri2; // start / end.
			tri1.m_indices[0] = upperIndex;
			tri1.m_indices[1] = 6; // start of cap rounding.
			tri1.m_indices[2] = middleIndex;
			tri2.m_indices[0] = lowerIndex;
			tri2.m_indices[1] = line.m_vertices.m_size - 1; // end of cap rounding.
			tri2.m_indices[2] = middleIndex;
			line.m_tris.push_back(tri1);
			line.m_tris.push_back(tri2);

			for (int i = 6; i < line.m_vertices.m_size - 1; i++)
			{
				LineTriangle tri;
				tri.m_indices[0] = i;
				tri.m_indices[1] = i + 1;
				tri.m_indices[2] = middleIndex;
				line.m_tris.push_back(tri);
			}
		}
	}

	SimpleLine Drawer::CalculateSimpleLine(const Vec2& p1, const Vec2& p2, StyleOptions& style)
	{
		const Vec2 up = Math::Normalized(Math::Rotate90(Vec2(p2.x - p1.x, p2.y - p1.y), true));
		SimpleLine line;
		line.m_points[0] = Vec2(p1.x + up.x * style.thickness.start / 2.0f, p1.y + up.y * style.thickness.start / 2.0f);
		line.m_points[3] = Vec2(p1.x - up.x * style.thickness.start / 2.0f, p1.y - up.y * style.thickness.start / 2.0f);
		line.m_points[1] = Vec2(p2.x + up.x * style.thickness.end / 2.0f, p2.y + up.y * style.thickness.end / 2.0f);
		line.m_points[2] = Vec2(p2.x - up.x * style.thickness.end / 2.0f, p2.y - up.y * style.thickness.end / 2.0f);

		return line;
	}

	void Drawer::JoinLines(Line& line1, Line& line2, StyleOptions& opts, LineJointType jointType, bool mergeUpperVertices)
	{
		const bool addUpperLowerIndices = opts.aaEnabled || !Math::IsEqualMarg(opts.outlineOptions.thickness, 0.0f);

		if (jointType == LineJointType::VtxAverage)
		{
			const Vec2 upperAvg = Vec2((line1.m_vertices[1].pos.x + line2.m_vertices[0].pos.x) / 2.0f, (line1.m_vertices[1].pos.y + line2.m_vertices[0].pos.y) / 2.0f);
			const Vec2 lowerAvg = Vec2((line1.m_vertices[2].pos.x + line2.m_vertices[3].pos.x) / 2.0f, (line1.m_vertices[2].pos.y + line2.m_vertices[3].pos.y) / 2.0f);

			line1.m_vertices[1].pos = line2.m_vertices[0].pos = upperAvg;
			line1.m_vertices[2].pos = line2.m_vertices[3].pos = lowerAvg;

			if (addUpperLowerIndices)
			{
				line2.m_upperIndices.erase(line2.m_upperIndices.findAddr(0));
				line2.m_lowerIndices.erase(line2.m_lowerIndices.findAddr(3));
			}
		}
		else if (jointType == LineJointType::Miter)
		{
			const Vec2 upperIntersection = Math::LineIntersection(line1.m_vertices[0].pos, line1.m_vertices[1].pos, line2.m_vertices[0].pos, line2.m_vertices[1].pos);
			const Vec2 lowerIntersection = Math::LineIntersection(line1.m_vertices[3].pos, line1.m_vertices[2].pos, line2.m_vertices[3].pos, line2.m_vertices[2].pos);
			line1.m_vertices[1].pos = line2.m_vertices[0].pos = upperIntersection;
			line1.m_vertices[2].pos = line2.m_vertices[3].pos = lowerIntersection;
			if (addUpperLowerIndices)
			{
				line2.m_upperIndices.erase(line2.m_upperIndices.findAddr(0));
				line2.m_lowerIndices.erase(line2.m_lowerIndices.findAddr(3));
			}
		}
		else if (jointType == LineJointType::Bevel)
		{
			const int intersection0 = mergeUpperVertices ? 0 : 3;
			const int intersection1 = mergeUpperVertices ? 1 : 2;
			const int intersection2 = mergeUpperVertices ? 2 : 1;
			const int intersection3 = mergeUpperVertices ? 3 : 0;

			if (addUpperLowerIndices)
			{
				if (mergeUpperVertices)
					line2.m_upperIndices.erase(line2.m_upperIndices.findAddr(0));
				else
					line2.m_lowerIndices.erase(line2.m_lowerIndices.findAddr(3));
			}

			const Vec2 intersection				= Math::LineIntersection(line1.m_vertices[intersection0].pos, line1.m_vertices[intersection1].pos, line2.m_vertices[intersection0].pos, line2.m_vertices[intersection1].pos);
			line1.m_vertices[intersection1].pos = line2.m_vertices[intersection0].pos = intersection;

			const int vLowIndex = line1.m_vertices.m_size;
			Vertex	  vLow;
			vLow.col = opts.color.start;
			vLow.pos = line2.m_vertices[intersection3].pos;
			line1.m_vertices.push_back(vLow);

			LineTriangle tri1;
			tri1.m_indices[0] = intersection1;
			tri1.m_indices[1] = intersection2;
			tri1.m_indices[2] = vLowIndex;
			line1.m_tris.push_back(tri1);
		}
		else if (jointType == LineJointType::BevelRound)
		{
			const int	intersection0	   = mergeUpperVertices ? 0 : 3;
			const int	intersection1	   = mergeUpperVertices ? 1 : 2;
			const int	intersection2	   = mergeUpperVertices ? 2 : 1;
			const int	intersection3	   = mergeUpperVertices ? 3 : 0;
			const Vec2	upperIntersection  = Math::LineIntersection(line1.m_vertices[intersection0].pos, line1.m_vertices[intersection1].pos, line2.m_vertices[intersection0].pos, line2.m_vertices[intersection1].pos);
			const Vec2	lowerIntersection  = Math::LineIntersection(line1.m_vertices[intersection3].pos, line1.m_vertices[intersection2].pos, line2.m_vertices[intersection3].pos, line2.m_vertices[intersection2].pos);
			const Vec2	intersectionCenter = Vec2((upperIntersection.x + lowerIntersection.x) / 2.0f, (upperIntersection.y + lowerIntersection.y) / 2.0f);
			const float ang2			   = Math::GetAngleFromCenter(intersectionCenter, line1.m_vertices[intersection2].pos);
			const float ang1			   = Math::GetAngleFromCenter(intersectionCenter, line2.m_vertices[intersection3].pos);
			const float startAngle		   = ang2 > ang1 ? ang1 : ang2;
			const float endAngle		   = ang2 > ang1 ? ang2 : ang1;
			const float arcRad			   = Math::Mag(Vec2(line1.m_vertices[intersection2].pos.x - intersectionCenter.x, line1.m_vertices[intersection2].pos.y - intersectionCenter.y));

			// Merge
			line1.m_vertices[intersection1].pos = line2.m_vertices[intersection0].pos = upperIntersection;

			if (addUpperLowerIndices)
			{
				if (mergeUpperVertices)
					line2.m_upperIndices.erase(line2.m_upperIndices.findAddr(0));
				else
					line2.m_lowerIndices.erase(line2.m_lowerIndices.findAddr(3));
			}

			const int vLowIndex = line1.m_vertices.m_size;
			Vertex	  vLow;
			vLow.col = opts.color.start;
			vLow.pos = line2.m_vertices[intersection3].pos;
			line1.m_vertices.push_back(vLow);

			const float increase	  = Math::Remap(opts.rounding, 0.0f, 1.0f, 45.0f, 6.0f);
			const int	parabolaStart = line1.m_vertices.m_size;

			Array<int> lowerIndicesToAdd;
			Array<int> upperIndicesToAdd;

			for (float k = startAngle + increase; k < endAngle; k += increase)
			{
				const Vec2 p = Math::GetPointOnCircle(intersectionCenter, arcRad, k);
				Vertex	   v;
				v.col = opts.color.start;
				v.pos = p;

				if (addUpperLowerIndices)
				{
					if (mergeUpperVertices)
						lowerIndicesToAdd.push_back(line1.m_vertices.m_size);
					else
						upperIndicesToAdd.push_back(line1.m_vertices.m_size);
				}

				line1.m_vertices.push_back(v);
			}

			if (addUpperLowerIndices)
			{
				if (mergeUpperVertices)
				{
					if (ang1 > ang2)
					{
						for (int i = 0; i < lowerIndicesToAdd.m_size; i++)
							line1.m_lowerIndices.push_back(lowerIndicesToAdd[i]);
					}
					else
					{
						for (int i = lowerIndicesToAdd.m_size - 1; i > -1; i--)
							line1.m_lowerIndices.push_back(lowerIndicesToAdd[i]);
					}
				}
				else
				{
					if (ang1 > ang2)
					{
						for (int i = 0; i < upperIndicesToAdd.m_size; i++)
							line1.m_upperIndices.push_back(upperIndicesToAdd[i]);
					}
					else
					{
						for (int i = upperIndicesToAdd.m_size - 1; i > -1; i--)
							line1.m_upperIndices.push_back(upperIndicesToAdd[i]);
					}
				}
			}

			LineTriangle tri1, tri2;
			tri1.m_indices[0] = intersection1;
			tri1.m_indices[1] = intersection2;
			tri1.m_indices[2] = ang1 > ang2 ? parabolaStart : line1.m_vertices.m_size - 1;
			tri2.m_indices[0] = intersection1;
			tri2.m_indices[1] = vLowIndex;
			tri2.m_indices[2] = ang1 > ang2 ? line1.m_vertices.m_size - 1 : parabolaStart;
			line1.m_tris.push_back(tri1);
			line1.m_tris.push_back(tri2);

			for (int i = parabolaStart; i < line1.m_vertices.m_size - 1; i++)
			{
				LineTriangle tri;
				tri.m_indices[0] = intersection1;
				tri.m_indices[1] = i;
				tri.m_indices[2] = i + 1;
				line1.m_tris.push_back(tri);
			}
		}
	}

	void Drawer::DrawSimpleLine(SimpleLine& line, StyleOptions& opts, float rotateAngle, int drawOrder)
	{
		m_bufferStore.GetData().m_rectOverrideData.m_p1					 = line.m_points[0];
		m_bufferStore.GetData().m_rectOverrideData.m_p4					 = line.m_points[3];
		m_bufferStore.GetData().m_rectOverrideData.m_p2					 = line.m_points[1];
		m_bufferStore.GetData().m_rectOverrideData.m_p3					 = line.m_points[2];
		m_bufferStore.GetData().m_rectOverrideData.overrideRectPositions = true;
		DrawRect(m_bufferStore.GetData().m_rectOverrideData.m_p1, m_bufferStore.GetData().m_rectOverrideData.m_p3, opts, rotateAngle, drawOrder);
		m_bufferStore.GetData().m_rectOverrideData.overrideRectPositions = false;
	}

	void Drawer::CalculateLineUVs(Line& line)
	{
		Vec2 bbMin, bbMax;
		GetConvexBoundingBox(&line.m_vertices[0], line.m_vertices.m_size, bbMin, bbMax);

		// Recalculate UVs.
		for (int i = 0; i < line.m_vertices.m_size; i++)
		{
			line.m_vertices[i].uv.x = Math::Remap(line.m_vertices[i].pos.x, bbMin.x, bbMax.x, 0.0f, 1.0f);
			line.m_vertices[i].uv.y = Math::Remap(line.m_vertices[i].pos.y, bbMin.y, bbMax.y, 0.0f, 1.0f);
		}
	}

	DrawBuffer* Drawer::DrawOutlineAroundShape(DrawBuffer* sourceBuffer, StyleOptions& opts, int* indicesOrder, int vertexCount, float defThickness, bool ccw, int drawOrder, OutlineCallType outlineType)
	{
		float	   thickness   = outlineType != OutlineCallType::Normal ? opts.aaMultiplier * Config.globalAAMultiplier : (defThickness);
		const bool isAAOutline = outlineType != OutlineCallType::Normal;

		// Determine which buffer to use.
		// Also correct the buffer pointer if getting a new buffer invalidated it.
		DrawBuffer* destBuf = nullptr;

		const int sourceIndex = m_bufferStore.GetData().GetBufferIndexInDefaultArray(sourceBuffer);
		destBuf				  = &m_bufferStore.GetData().GetDefaultBuffer(opts.userData, opts.uniqueID, drawOrder, isAAOutline ? DrawBufferShapeType::AA : DrawBufferShapeType::Shape, outlineType == OutlineCallType::AA ? opts.textureHandle : opts.outlineOptions.textureHandle, outlineType == OutlineCallType::AA ? opts.textureTilingAndOffset : opts.outlineOptions.textureTilingAndOffset);

		if (sourceIndex != -1)
			sourceBuffer = &m_bufferStore.GetData().m_defaultBuffers[sourceIndex];

		// only used if we are drawing AA.
		Array<int> copiedVerticesOrder;

		const int destBufStart = destBuf->vertexBuffer.m_size;
		// First copy the given vertices, add them to the destination buffer.
		for (int i = 0; i < vertexCount; i++)
		{
			Vertex v;
			v.col = isAAOutline ? sourceBuffer->vertexBuffer[indicesOrder[i]].col : opts.outlineOptions.color.start;
			v.pos = sourceBuffer->vertexBuffer[indicesOrder[i]].pos;
			v.uv  = sourceBuffer->vertexBuffer[indicesOrder[i]].uv;

			// if (isAAOutline)
			//     v.col.w = 1.0f;

			if (opts.aaEnabled && !isAAOutline)
				copiedVerticesOrder.push_back(destBuf->vertexBuffer.m_size);

			destBuf->PushVertex(v);
		}

		// const int halfVC = vertexCount / 2;

		// only used if we are drawing AA.
		Array<int> extrudedVerticesOrder;

		// Now traverse the destination buffer from the point we started adding to it, extrude the border towards m_thickness.
		for (int i = 0; i < vertexCount; i++)
		{
			const int prev	  = i == 0 ? destBufStart + vertexCount - 1 : destBufStart + i - 1;
			const int next	  = i == vertexCount - 1 ? destBufStart : destBufStart + i + 1;
			const int current = destBufStart + i;
			Vertex	  v;
			v.uv  = destBuf->vertexBuffer[current].uv;
			v.col = isAAOutline ? sourceBuffer->vertexBuffer[indicesOrder[i]].col : opts.outlineOptions.color.end;

			if (isAAOutline)
				v.col.w = 0.0f;

			const Vec2 prevP = destBuf->vertexBuffer[prev].pos;
			const Vec2 nextP = destBuf->vertexBuffer[next].pos;
			// const Vec2 vertexNormalAverage = Math::GetVertexNormalFlatCheck(destBuf->m_vertexBuffer[current].pos, prevP, nextP, ccw);
			// v.pos						   = Vec2(destBuf->m_vertexBuffer[current].pos.x + vertexNormalAverage.x * thickness, destBuf->m_vertexBuffer[current].pos.y + vertexNormalAverage.y * thickness);
			v.pos = Math::GetExtrudedFromNormalFlatCheck(destBuf->vertexBuffer[current].pos, prevP, nextP, thickness, ccw);

			if (opts.aaEnabled && !isAAOutline)
				extrudedVerticesOrder.push_back(destBuf->vertexBuffer.m_size);

			destBuf->PushVertex(v);
		}

		if (!isAAOutline)
			CalculateVertexUVs(destBuf, destBufStart, (destBufStart + vertexCount * 2) - 1);

		for (int i = 0; i < vertexCount; i++)
		{
			const int current = destBufStart + i;
			int		  next	  = destBufStart + i + 1;
			if (i == vertexCount - 1)
				next = destBufStart;

			destBuf->PushIndex(current);
			destBuf->PushIndex(next);
			destBuf->PushIndex(current + vertexCount);
			destBuf->PushIndex(next);
			destBuf->PushIndex(next + vertexCount);
			destBuf->PushIndex(current + vertexCount);
		}

		if (opts.aaEnabled && !isAAOutline)
		{
			StyleOptions opts2 = StyleOptions(opts);
			destBuf			   = DrawOutlineAroundShape(destBuf, opts2, &extrudedVerticesOrder[0], extrudedVerticesOrder.m_size, defThickness, ccw, drawOrder, OutlineCallType::OutlineAA);
			DrawOutlineAroundShape(destBuf, opts2, &copiedVerticesOrder[0], copiedVerticesOrder.m_size, -defThickness, !ccw, drawOrder, OutlineCallType::OutlineAA);
		}

		return sourceBuffer;
	}

	DrawBuffer* Drawer::DrawOutline(DrawBuffer* sourceBuffer, StyleOptions& opts, int vertexCount, bool skipEnds, int drawOrder, OutlineCallType outlineType, bool reverseDrawDir)
	{
		const bool isAAOutline = outlineType != OutlineCallType::Normal;
		float	   thickness   = isAAOutline ? opts.aaMultiplier * Config.globalAAMultiplier : (opts.outlineOptions.thickness);

		if (reverseDrawDir)
			thickness = -thickness;

		// Determine which buffer to use.
		// Also correct the buffer pointer if getting a new buffer invalidated it.
		DrawBuffer* destBuf = nullptr;

		const int sourceIndex = m_bufferStore.GetData().GetBufferIndexInDefaultArray(sourceBuffer);
		destBuf				  = &m_bufferStore.GetData().GetDefaultBuffer(opts.userData, opts.uniqueID, drawOrder, isAAOutline ? DrawBufferShapeType::AA : DrawBufferShapeType::Shape, opts.outlineOptions.textureHandle, opts.outlineOptions.textureTilingAndOffset);

		if (sourceIndex != -1)
			sourceBuffer = &m_bufferStore.GetData().m_defaultBuffers[sourceIndex];

		int startIndex, endIndex;

		if (opts.isFilled)
		{
			endIndex   = sourceBuffer->vertexBuffer.m_size - 1;
			startIndex = sourceBuffer->vertexBuffer.m_size - vertexCount;
		}
		else
		{
			// Take the outer half.
			if (opts.outlineOptions.drawDirection == OutlineDrawDirection::Outwards)
			{
				endIndex   = sourceBuffer->vertexBuffer.m_size - 1;
				startIndex = sourceBuffer->vertexBuffer.m_size - vertexCount / 2;
			}
			else if (opts.outlineOptions.drawDirection == OutlineDrawDirection::Inwards)
			{
				endIndex   = sourceBuffer->vertexBuffer.m_size - vertexCount / 2 - 1;
				startIndex = sourceBuffer->vertexBuffer.m_size - vertexCount;
			}
			else
			{
				endIndex   = sourceBuffer->vertexBuffer.m_size - 1;
				startIndex = sourceBuffer->vertexBuffer.m_size - vertexCount;
			}
		}

		Vec2 bbMin, bbMax;
		GetConvexBoundingBox(sourceBuffer, startIndex, endIndex, bbMin, bbMax);
		// const bool reCalcUVs	= useTextureBuffer || useGradBuffer;
		// const int  destBufStart = destBuf->m_vertexBuffer.m_size;

		auto copyAndFill = [&](DrawBuffer* sourceBuffer, DrawBuffer* destBuf, int startIndex, int endIndex, float thickness) {
			const int destBufStart = destBuf->vertexBuffer.m_size;
			const int totalSize	   = endIndex - startIndex + 1;

			// First copy vertices.
			for (int i = startIndex; i < endIndex + 1; i++)
			{
				Vertex& srcVertex = sourceBuffer->vertexBuffer[i];
				Vertex	v;
				v.pos = srcVertex.pos;
				v.uv  = srcVertex.uv;
				v.col = srcVertex.col;
				destBuf->PushVertex(v);
			}

			// Now extrude & re-add extruded.
			for (int i = startIndex; i < endIndex + 1; i++)
			{
				// take two edges, this vertex to next and previous to this in order to calculate vertex normals.
				const int previous = i == startIndex ? endIndex : i - 1;
				const int next	   = i == endIndex ? startIndex : i + 1;
				Vertex	  v;
				v.uv	= sourceBuffer->vertexBuffer[i].uv;
				v.col	= sourceBuffer->vertexBuffer[i].col;
				v.col.w = 0.0f; // Color will be recalculated if not AA outline.

				if (skipEnds && i == startIndex)
				{
					const Vec2 nextP = sourceBuffer->vertexBuffer[next].pos;
					v.pos			 = Math::GetExtrudedFromNormal(sourceBuffer->vertexBuffer[i].pos, Vec2(-1, -1), nextP, thickness);
				}
				else if (skipEnds && i == endIndex)
				{
					const Vec2 prevP = sourceBuffer->vertexBuffer[previous].pos;
					v.pos			 = Math::GetExtrudedFromNormal(sourceBuffer->vertexBuffer[i].pos, prevP, Vec2(-1, -1), thickness);
				}
				else
				{
					const Vec2 prevP = sourceBuffer->vertexBuffer[previous].pos;
					const Vec2 nextP = sourceBuffer->vertexBuffer[next].pos;
					v.pos			 = Math::GetExtrudedFromNormal(sourceBuffer->vertexBuffer[i].pos, prevP, nextP, thickness);
				}
				destBuf->PushVertex(v);
			}

			Vec2 bbMin, bbMax;
			New_GetConvexBB(destBuf, destBufStart, destBuf->vertexBuffer.m_size, bbMin, bbMax);

			if (!isAAOutline)
				New_CalculateVertexUVsAndColor(destBuf, destBufStart, destBuf->vertexBuffer.m_size, bbMin, bbMax, opts.outlineOptions.color);

			for (int i = destBufStart; i < destBufStart + totalSize; i++)
			{
				int next = i + 1;
				if (next >= destBufStart + totalSize)
					next = destBufStart;

				if (skipEnds && i == destBufStart + totalSize - 1)
					return;

				destBuf->PushIndex(i);
				destBuf->PushIndex(next);
				destBuf->PushIndex(i + totalSize);
				destBuf->PushIndex(next);
				destBuf->PushIndex(next + totalSize);
				destBuf->PushIndex(i + totalSize);
			}
		};

		const bool useAA = opts.aaEnabled && !isAAOutline;

		if (opts.isFilled)
		{
			if (opts.outlineOptions.drawDirection == OutlineDrawDirection::Outwards || opts.outlineOptions.drawDirection == OutlineDrawDirection::Both)
			{
				copyAndFill(sourceBuffer, destBuf, startIndex, endIndex, thickness);

				if (useAA)
				{
					StyleOptions opts2				   = StyleOptions(opts);
					opts2.isFilled					   = false;
					opts2.outlineOptions.drawDirection = OutlineDrawDirection::Outwards;
					destBuf							   = DrawOutline(destBuf, opts2, vertexCount * 2, skipEnds, drawOrder, OutlineCallType::OutlineAA);

					opts2.outlineOptions.drawDirection = OutlineDrawDirection::Inwards;
					DrawOutline(destBuf, opts2, vertexCount * 2, skipEnds, drawOrder, OutlineCallType::OutlineAA);
				}
			}
			else if (opts.outlineOptions.drawDirection == OutlineDrawDirection::Inwards)
			{
				copyAndFill(sourceBuffer, destBuf, startIndex, endIndex, -thickness);

				if (useAA)
				{
					// AA outline to the current outline we are drawing
					StyleOptions opts2				   = StyleOptions(opts);
					opts2.outlineOptions.drawDirection = OutlineDrawDirection::Outwards;
					destBuf							   = DrawOutline(destBuf, opts2, vertexCount, skipEnds, drawOrder, OutlineCallType::OutlineAA, true);

					opts2.outlineOptions.drawDirection = OutlineDrawDirection::Inwards;
					opts2.isFilled					   = false;
					DrawOutline(destBuf, opts2, vertexCount * 2, skipEnds, drawOrder, OutlineCallType::OutlineAA, true);
				}
			}
		}
		else
		{
			if (opts.outlineOptions.drawDirection == OutlineDrawDirection::Outwards)
			{
				if (useAA)
				{
					// AA outline to the shape we are drawing
					StyleOptions opts3	 = StyleOptions(opts);
					opts3.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Inwards);
					DrawOutline(sourceBuffer, opts3, vertexCount, skipEnds, drawOrder, OutlineCallType::OutlineAA);
				}

				copyAndFill(sourceBuffer, destBuf, startIndex, endIndex, thickness);

				if (useAA)
				{
					// AA outline to the current outline we are drawing
					StyleOptions opts2				   = StyleOptions(opts);
					opts2.outlineOptions.drawDirection = OutlineDrawDirection::Outwards;
					destBuf							   = DrawOutline(destBuf, opts2, vertexCount, skipEnds, drawOrder, OutlineCallType::OutlineAA);

					opts2.outlineOptions.drawDirection = OutlineDrawDirection::Inwards;
					DrawOutline(destBuf, opts2, vertexCount, skipEnds, drawOrder, OutlineCallType::OutlineAA);
				}
			}
			else if (opts.outlineOptions.drawDirection == OutlineDrawDirection::Inwards)
			{
				if (useAA)
				{
					// AA outline to the shape we are drawing
					StyleOptions opts3	 = StyleOptions(opts);
					opts3.outlineOptions = OutlineOptions::FromStyle(opts, OutlineDrawDirection::Outwards);
					DrawOutline(sourceBuffer, opts3, vertexCount, skipEnds, drawOrder, OutlineCallType::OutlineAA);
				}

				copyAndFill(sourceBuffer, destBuf, startIndex, endIndex, -thickness);

				if (useAA)
				{
					// AA outline to the current outline we are drawing
					StyleOptions opts2				   = StyleOptions(opts);
					opts2.outlineOptions.drawDirection = OutlineDrawDirection::Outwards;
					destBuf							   = DrawOutline(destBuf, opts2, vertexCount, skipEnds, drawOrder, OutlineCallType::OutlineAA, true);

					opts2.outlineOptions.drawDirection = OutlineDrawDirection::Inwards;
					DrawOutline(destBuf, opts2, vertexCount, skipEnds, drawOrder, OutlineCallType::OutlineAA, true);
				}
			}
			else
			{
				copyAndFill(sourceBuffer, destBuf, startIndex, startIndex + vertexCount / 2 - 1, -thickness);

				if (useAA)
				{
					// AA outline to the current outline we are drawing
					StyleOptions opts2				   = StyleOptions(opts);
					opts2.outlineOptions.drawDirection = OutlineDrawDirection::Outwards;
					destBuf							   = DrawOutline(destBuf, opts2, vertexCount, skipEnds, drawOrder, OutlineCallType::OutlineAA, true);

					opts2.outlineOptions.drawDirection = OutlineDrawDirection::Inwards;
					DrawOutline(destBuf, opts2, vertexCount, skipEnds, drawOrder, OutlineCallType::OutlineAA, true);
				}

				copyAndFill(sourceBuffer, destBuf, startIndex + vertexCount / 2, endIndex, thickness);

				if (useAA)
				{
					// AA outline to the current outline we are drawing
					StyleOptions opts2				   = StyleOptions(opts);
					opts2.outlineOptions.drawDirection = OutlineDrawDirection::Outwards;
					destBuf							   = DrawOutline(destBuf, opts2, vertexCount, skipEnds, drawOrder, OutlineCallType::OutlineAA);

					opts2.outlineOptions.drawDirection = OutlineDrawDirection::Inwards;
					DrawOutline(destBuf, opts2, vertexCount, skipEnds, drawOrder, OutlineCallType::OutlineAA);
				}
			}
		}

		return sourceBuffer;
	}

#ifndef LINAVG_DISABLE_TEXT_SUPPORT

	void Drawer::ParseTextIntoWords(Array<TextPart*>& arr, const char* text, Font* font, float scale, float spacing)
	{
		bool		  added	 = false;
		Vec2		  size	 = Vec2(0.0f, 0.0f);
		LINAVG_STRING word	 = "";
		LINAVG_STRING strTxt = text;

		for (auto x : strTxt)
		{
			if (x == ' ')
			{
				if (added)
				{
					TextPart* w = new TextPart();
					w->m_size	= size;
					w->m_str	= word;
					arr.push_back(w);
				}
				added = false;
				size  = Vec2(0.0f, 0.0f);
				word  = "";
			}
			else
			{
				auto& ch = font->glyphs[x];
				size.y	 = Math::Max(size.y, (ch.m_size.y) * scale);
				size.x += ch.m_advance.x * scale + spacing;
				word  = word + x;
				added = true;
			}
		}

		TextPart* w = new TextPart();
		w->m_size	= size;
		w->m_str	= word;
		arr.push_back(w);
	}

	void Drawer::ParseWordsIntoLines(Array<TextPart*>& lines, const Array<TextPart*>& words, Font* font, float scale, float spacing, float wrapWidth)
	{
		const float	  spaceAdvance = font->spaceAdvance * scale + spacing;
		float		  maxHeight	   = 0.0f;
		float		  totalWidth   = 0.0f;
		LINAVG_STRING append	   = "";
		// float		maxBearingDiffY = 0.0f;
		// float        remap        = font->m_isSDF ? Math::Remap(sdfThickness, 0.5f, 1.0f, 2.0f, 0.0f) : 0.0f;
		// const Vec2	offset = CalcMaxCharOffset(words[0]->m_str.c_str(), font, scale);

		for (int i = 0; i < words.m_size; i++)
		{
			totalWidth += words[i]->m_size.x;
			// maxHeight = Math::Max(words[i]->m_size.y - offset.y * remap, maxHeight);
			maxHeight = Math::Max(words[i]->m_size.y, maxHeight);

			if (totalWidth > wrapWidth)
			{
				// Can't even render the first word.
				if (i == 0)
					break;

				TextPart* newLine = new TextPart();
				newLine->m_size.x = totalWidth - words[i]->m_size.x - spaceAdvance;
				newLine->m_size.y = maxHeight;
				newLine->m_str	  = append;
				lines.push_back(newLine);
				append	   = words[i]->m_str + " ";
				totalWidth = words[i]->m_size.x + spaceAdvance;
				// maxHeight = words[i]->m_size.y - offset.y * remap;
				maxHeight = words[i]->m_size.y;
			}
			else
			{
				totalWidth += spaceAdvance;
				append += words[i]->m_str + " ";
			}
		}

		TextPart* newLine = new TextPart();
		newLine->m_size.x = totalWidth - spaceAdvance;
		newLine->m_size.y = maxHeight;
		newLine->m_str	  = append.substr(0, append.size() - 1);
		lines.push_back(newLine);
	}

	void AppendUTF8(LINAVG_STRING& str, unsigned long cp)
	{
		if (cp < 0x80)
		{ // 1-byte character
			str += static_cast<char>(cp);
		}
		else if (cp < 0x800)
		{ // 2-byte character
			str += static_cast<char>(0xC0 | cp >> 6);
			str += static_cast<char>(0x80 | (cp & 0x3F));
		}
		else if (cp < 0x10000)
		{ // 3-byte character
			str += static_cast<char>(0xE0 | cp >> 12);
			str += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
			str += static_cast<char>(0x80 | (cp & 0x3F));
		}
		else if (cp <= 0x10FFFF)
		{ // 4-byte character
			str += static_cast<char>(0xF0 | cp >> 18);
			str += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
			str += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
			str += static_cast<char>(0x80 | (cp & 0x3F));
		}
		else
		{
			// the code point is not valid
		}
	}

	void Drawer::WrapText(LINAVG_VEC<TextPart>& lines, const char* text, const TextOptions& opts)
	{
		TextPart line = {};
		TextPart word = {};

		const uint8_t* c;
		const float	   spaceAdvance = opts.font->spaceAdvance * opts.textScale + opts.spacing;

		auto process = [&](TextCharacter& ch, GlyphEncoding c) {
			if (!opts.wordWrap)
			{
				if (line.m_size.x + ch.m_size.x * opts.textScale > opts.wrapWidth)
				{
					lines.push_back(line);
					line.m_str.clear();
					line.m_size = Vec2(0.0f, 0.0f);
				}

				line.m_str += static_cast<char>(c);
				line.m_size.x += ch.m_advance.x * opts.textScale;
				line.m_size.y = Math::Max(ch.m_size.y * opts.textScale, line.m_size.y);
				return;
			}

			// Add character to current word
			if (c != ' ')
			{
				if (opts.font->supportsUnicode)
					AppendUTF8(word.m_str, c);
				else
					word.m_str += static_cast<char>(c);

				word.m_size.x += ch.m_advance.x * opts.textScale;
				word.m_size.y = Math::Max(word.m_size.y, ch.m_size.y * opts.textScale);
			}
			else
			{
				// If adding the current word to the current line would
				// make it too long, add the current line to the list of
				// lines and start a new line with the current word.
				if (!line.m_str.empty() && line.m_size.x + word.m_size.x > opts.wrapWidth)
				{
					lines.push_back(line);
					line.m_str.clear();
					line.m_size = Vec2(0.0f, 0.0f);
				}

				// Add current word to current line (with a leading space if it's not empty)
				// if (!line.m_str.empty())
				//{
				//    line.m_str += ' ';
				//    line.m_size.x += spaceAdvance;
				//}

				line.m_str += word.m_str + ' ';
				line.m_size.x += word.m_size.x + spaceAdvance;
				line.m_size.y = Math::Max(line.m_size.y, word.m_size.y);

				// Clear current word
				word.m_str.clear();
				word.m_size = Vec2(0.0f, 0.0f);
			}
		};

		if (opts.font->supportsUnicode)
		{
			auto codepoints = GetUtf8Codepoints(text);

			for (auto cp : codepoints)
			{
				auto ch = opts.font->glyphs[cp];
				process(ch, cp);
			}
		}
		else
		{
			for (c = (uint8_t*)text; *c; c++)
			{
				auto character = *c;
				auto ch		   = opts.font->glyphs[character];
				process(ch, character);
			}
		}

		// If there's still a word left that wasn't added to the lines, try to add it
		if (!word.m_str.empty())
		{
			if (!line.m_str.empty() && line.m_size.x + word.m_size.x > opts.wrapWidth)
			{
				lines.push_back(line);
				line = word;
			}
			else
			{
				// if (!line.m_str.empty())
				//{
				//     line.m_str += ' ';
				//     line.m_size.x += spaceAdvance;
				// }

				line.m_str += word.m_str;
				line.m_size.x += word.m_size.x;
				line.m_size.y = Math::Max(line.m_size.y, word.m_size.y);
			}
		}

		// If there's still a line left that wasn't added to the lines, add it
		if (!line.m_str.empty())
		{
			lines.push_back(line);
		}
	}

	void Drawer::ProcessText(DrawBuffer* buf, Font* font, const char* text, const Vec2& pos, const Vec2& offset, const Vec4Grad& color, const TextOptions& opts, float rotateAngle, TextOutData* outData, bool checkClip)
	{
		const int  bufStart = buf->vertexBuffer.m_size;
		const Vec2 size		= CalcTextSize(text, opts);
		Vec2	   usedPos	= pos;
		// usedPos.y += size.y;

		// float      remap    = font->m_isSDF ? Math::Remap(sdfThickness, 0.5f, 1.0f, 0.0f, 1.0f) : 0.0f;
		// remap               = Math::Clamp(remap, 0.0f, 1.0f);
		// const Vec2 off = CalcMaxCharOffset(text, font, scale);
		// usedPos.x += Math::Abs(off.x) * remap;
		// usedPos.y += font->m_ascent + font->m_descent;

		if (outData != nullptr)
		{
			outData->characterInfo.reserve(200);
			outData->lineInfo.reserve(10);
		}

		if (Math::IsEqualMarg(opts.wrapWidth, 0.0f) || size.x < opts.wrapWidth)
		{
			if (opts.alignment == TextAlignment::Center)
			{
				usedPos.x -= size.x / 2.0f;
			}
			else if (opts.alignment == TextAlignment::Right)
				usedPos.x -= size.x;

			DrawText(buf, text, usedPos, offset, color, opts, outData, checkClip);
		}
		else
		{
			LINAVG_VEC<TextPart> lines;
			lines.reserve(20);
			WrapText(lines, text, opts);

			const size_t sz	 = lines.size();
			size_t		 ctr = 0;
			for (const auto& line : lines)
			{
				if (ctr < sz - 1)
					usedPos.y -= font->newLineHeight * opts.textScale + opts.newLineSpacing;
				ctr++;
			}

			for (const auto& line : lines)
			{
				if (outData != nullptr)
				{
					LineInfo lineInfo;
					lineInfo.startCharacterIndex = static_cast<unsigned int>(outData->characterInfo.m_size);
					lineInfo.posX				 = usedPos.x;
					lineInfo.posY				 = usedPos.y;
					outData->lineInfo.push_back(lineInfo);
				}

				if (opts.alignment == TextAlignment::Center)
				{
					usedPos.x = pos.x - line.m_size.x / 2.0f;
				}
				else if (opts.alignment == TextAlignment::Right)
					usedPos.x = pos.x - line.m_size.x;

				DrawText(buf, line.m_str.c_str(), usedPos, offset, color, opts, outData, checkClip);
				usedPos.y += font->newLineHeight * opts.textScale + opts.newLineSpacing;

				if (outData != nullptr)
				{
					auto& thisLine			   = outData->lineInfo[outData->lineInfo.m_size - 1];
					thisLine.endCharacterIndex = static_cast<unsigned int>(outData->characterInfo.m_size - 1);
				}
			}

			lines.clear();
		}

		if (!Math::IsEqualMarg(rotateAngle, 0.0f))
		{
			const Vec2 center = GetVerticesCenter(buf, bufStart, buf->vertexBuffer.m_size - 1);
			RotateVertices(buf->vertexBuffer, center, bufStart, buf->vertexBuffer.m_size - 1, rotateAngle);
		}
	}

	Vec2 Drawer::CalcMaxCharOffset(const char* text, Font* font, float scale)
	{
		const uint8_t* c;
		Vec2		   offset  = Vec2(0.0f, 0.0f);
		int			   counter = 0;

		// Iterate through the whole text and determine max width & height
		// As well as line breaks based on wrapping.
		for (c = (const uint8_t*)text; *c; c++)
		{
			auto& ch = font->glyphs[*c];
			// float x	 = ch.m_advance.x * scale;
			// float y	 = ch.m_size.y * scale;

			if (counter == 0)
				offset.x = ch.m_bearing.x < 0.0f ? ch.m_bearing.x * scale : 0.0f;
			offset.y = Math::Max(offset.y, (ch.m_size.y - ch.m_bearing.y) * scale);
			counter++;
		}

		return offset;
	}

	void Drawer::DrawText(DrawBuffer* buf, const char* text, const Vec2& position, const Vec2& offset, const Vec4Grad& color, const TextOptions& opts, TextOutData* outData, bool checkClip)
	{
		const uint8_t* c;
		const int	   totalCharacterCount = Utility::GetTextCharacterSize(text);
		// const int	   bufStart			   = buf->m_vertexBuffer.m_size;
		Vec4 lastMinGrad	= color.start;
		Vec2 pos			= position;
		int	 characterCount = 0;
		// bool		   first			   = true;

		pos.x = static_cast<float>(Math::CustomRound(pos.x));
		pos.y = static_cast<float>(Math::CustomRound(pos.y));

		GlyphEncoding previousCharacter = 0;

		auto drawChar = [&](TextCharacter& ch, GlyphEncoding c) {
			const int startIndex = buf->vertexBuffer.m_size;

			unsigned long kerning = 0;
			if (opts.font->supportsKerning && previousCharacter != 0)
			{
				auto& table = opts.font->kerningTable[previousCharacter];
				auto  it	= table.xAdvances.find(c);
				if (it != table.xAdvances.end())
					kerning = it->second / 64;
			}

			previousCharacter = c;
			float ytop		  = pos.y - ch.m_bearing.y * opts.textScale;
			float ybot		  = pos.y + (ch.m_size.y - ch.m_bearing.y) * opts.textScale;

			float x2 = pos.x + (kerning + ch.m_bearing.x) * opts.textScale;
			float w	 = ch.m_size.x * opts.textScale;
			float h	 = ch.m_size.y * opts.textScale;

			pos.x += (kerning + ch.m_advance.x) * opts.textScale + opts.spacing;
			pos.y += ch.m_advance.y * opts.textScale;

			Vertex v0, v1, v2, v3;

			if (color.gradientType == GradientType::None)
				v0.col = v1.col = v2.col = v3.col = color.start;
			else if (color.gradientType == GradientType::Horizontal)
			{
				const float maxT	   = static_cast<float>(characterCount + 1) / static_cast<float>(totalCharacterCount);
				const Vec4	currentMin = lastMinGrad;
				const Vec4	currentMax = Math::Lerp(color.start, color.end, maxT);
				lastMinGrad			   = currentMax;

				v0.col = currentMin;
				v1.col = currentMax;
				v2.col = currentMax;
				v3.col = currentMin;
			}
			else // fallback is vertical
			{
				v0.col = color.start;
				v1.col = color.start;
				v2.col = color.end;
				v3.col = color.end;
			}

			v0.pos = Vec2(x2 + offset.x, ytop + offset.y);
			v1.pos = Vec2(x2 + offset.x + w, ytop + offset.y);
			v2.pos = Vec2(x2 + offset.x + w, ybot + offset.y);
			v3.pos = Vec2(x2 + offset.x, ybot + offset.y);

			if (!Math::IsEqualMarg(opts.cpuClipping.z, 0.0f, 0.1f) && !Math::IsEqualMarg(opts.cpuClipping.w, 0.0f, 0.1f))
			{
				if (!IsPointInside(v0.pos, opts.cpuClipping))
					return;

				if (!IsPointInside(v1.pos, opts.cpuClipping))
					return;

				if (!IsPointInside(v2.pos, opts.cpuClipping))
					return;

				if (!IsPointInside(v3.pos, opts.cpuClipping))
					return;
			}

			v0.uv = Vec2(ch.m_uv12.x, ch.m_uv12.y);
			v1.uv = Vec2(ch.m_uv12.z, ch.m_uv12.w);
			v2.uv = Vec2(ch.m_uv34.x, ch.m_uv34.y);
			v3.uv = Vec2(ch.m_uv34.z, ch.m_uv34.w);

			if (outData != nullptr)
			{
				CharacterInfo ci;
				ci.x	 = v0.pos.x;
				ci.y	 = v3.pos.y;
				ci.sizeX = Math::IsEqualMarg(w, 0.0f) ? (kerning + ch.m_advance.x) * opts.textScale : w;
				ci.sizeY = ybot - ytop;
				outData->characterInfo.push_back(ci);
			}

			if (Math::IsEqualMarg(w, 0.0f) || Math::IsEqualMarg(h, 0.0f))
				return;

			buf->PushVertex(v0);
			buf->PushVertex(v1);
			buf->PushVertex(v2);
			buf->PushVertex(v3);

			buf->PushIndex(startIndex);
			buf->PushIndex(startIndex + 1);
			buf->PushIndex(startIndex + 3);
			buf->PushIndex(startIndex + 1);
			buf->PushIndex(startIndex + 2);
			buf->PushIndex(startIndex + 3);
			characterCount++;
		};

		if (opts.font->supportsUnicode)
		{
			auto codepoints = GetUtf8Codepoints(text);

			for (auto cp : codepoints)
			{
				auto ch = opts.font->glyphs[cp];
				drawChar(ch, cp);
			}
		}
		else
		{
			for (c = (uint8_t*)text; *c; c++)
			{
				auto character = *c;
				auto ch		   = opts.font->glyphs[character];
				drawChar(ch, character);
			}
		}
	}

	LINAVG_VEC<int32_t> Drawer::GetUtf8Codepoints(const char* str)
	{
		LINAVG_VEC<int32_t> codepoints;
		const char*			p = str;
		while (*p != '\0')
		{
			int32_t		  codepoint = 0;
			unsigned char c			= *p;
			if (c < 0x80)
			{ // 1-byte sequence
				codepoint = c;
				p += 1;
			}
			else if (c < 0xE0)
			{ // 2-byte sequence
				codepoint = ((p[0] & 0x1F) << 6) | (p[1] & 0x3F);
				p += 2;
			}
			else if (c < 0xF0)
			{ // 3-byte sequence
				codepoint = ((p[0] & 0x0F) << 12) | ((p[1] & 0x3F) << 6) | (p[2] & 0x3F);
				p += 3;
			}
			else
			{ // 4-byte sequence
				codepoint = ((p[0] & 0x07) << 18) | ((p[1] & 0x3F) << 12) | ((p[2] & 0x3F) << 6) | (p[3] & 0x3F);
				p += 4;
			}

			codepoints.push_back(codepoint);
		}
		return codepoints;
	}

	Vec2 Drawer::CalcTextSize(const char* text, const TextOptions& opts)
	{
		float		   maxCharacterHeight = 0.0f;
		float		   totalWidth		  = 0.0f;
		const uint8_t* c;

		auto calcSizeChar = [&](TextCharacter& ch, GlyphEncoding c) {
			float x = ch.m_advance.x * opts.textScale;
			float y = (ch.m_bearing.y + (opts.font->isSDF ? ch.m_ascent : 0.0f)) * opts.textScale;

			if (opts.font->isSDF)
			{
				// const float ratio = ch.m_advance.x / ch.m_size.x;
				// const float yBase = ch.m_size.y * ratio;
				// const float yFull = ch.m_size.y;
				// y = std::floor(Math::Lerp(yBase, yFull, 0.0f));
				// y *= scale;

				// const float xBase = ch.m_advance.x;
				// const float xFull = ch.m_size.x;
				// x = std::floor(Math::Lerp(xBase, xFull, sdfSoftness));
				// x *= scale;
			}
			totalWidth += x + opts.spacing;
			maxCharacterHeight = Math::Max(maxCharacterHeight, y);
		};

		if (opts.font->supportsUnicode)
		{
			auto codepoints = GetUtf8Codepoints(text);

			// const size_t sz = codepoints.size();

			for (auto cp : codepoints)
			{
				auto ch = opts.font->glyphs[cp];
				calcSizeChar(ch, cp);
			}
		}
		else
		{
			for (c = (uint8_t*)text; *c; c++)
			{
				auto character = *c;
				auto ch		   = opts.font->glyphs[character];
				calcSizeChar(ch, character);
			}
		}

		/*
		if (opts.font->m_isSDF)
		{
			maxCharacterHeight += font->m_descent;

			float remapY   = Math::Remap(sdfThickness, 0.5f, 1.0f, 2.0f, 0.0f);
			float remapX   = Math::Remap(sdfThickness, 0.5f, 1.0f, 0.0f, 1.0f);
			remapX         = Math::Clamp(remapX, 0.0f, 1.0f);
			remapY         = Math::Clamp(remapY, 0.0f, 2.0f);
			const Vec2 off = CalcMaxCharOffset(text, font, scale);
			maxCharacterHeight -= off.y * remapY;
			totalWidth += Math::Abs(off.x) * remapX;
		}*/

		return Vec2(totalWidth, maxCharacterHeight);
	}

	Vec2 Drawer::CalcTextSizeWrapped(const char* text, const TextOptions& opts)
	{
		LINAVG_VEC<TextPart> lines;
		lines.reserve(15);
		WrapText(lines, text, opts);

		if (lines.size() == 1)
		{
			const Vec2 finalSize = lines[0].m_size;
			lines.clear();
			return finalSize;
		}

		Vec2 size = Vec2(0.0f, 0.0f);

		const int sz = static_cast<int>(lines.size());

		for (int i = 0; i < sz; i++)
		{
			const Vec2 calcSize = lines[i].m_size;
			size.x				= Math::Max(calcSize.x, size.x);
			if (i < sz - 1)
				size.y += opts.font->newLineHeight * opts.textScale + opts.newLineSpacing;
			else
				size.y += calcSize.y;

			// if (i < lines.m_size - 1)
			//     size.y += font->m_newLineHeight + newLineSpacing + lines[i]->m_maxBearingYDiff;
			// size.y += newLineSpacing + font->m_newLineHeight;
		}

		lines.clear();
		// size.y -= offset.y * remap;
		return size;
	}
#endif

} // namespace LinaVG
