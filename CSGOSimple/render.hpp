#pragma once
#include <string>
#include <sstream>
#include <stdint.h>
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

#include "singleton.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/impl/imgui_impl_dx9.h"
#include "imgui/impl/imgui_impl_win32.h"

#include "valve_sdk/misc/Color.hpp"
#include "helpers/math.hpp"

extern ImFont* g_pDefaultFont;
extern ImFont* g_pSecondFont;
extern ImFont* weapon_icons;
extern ImFont* m_BigIcons;

class Vector;

class Render
	: public Singleton<Render>
{
	friend class Singleton<Render>;

private:
	ImDrawList * draw_list_act;
	ImDrawList * draw_list_rendering;
	ImDrawList* draw_list;
	ImDrawData draw_data;
public:
	ImU32 GetU32(Color _color)
	{
		return ((_color[3] & 0xff) << 24) + ((_color[2] & 0xff) << 16) + ((_color[1] & 0xff) << 8)
			+ (_color[0] & 0xff);
	}

	void Initialize();
	void GetFonts();
	void ClearDrawList();
	void BeginScene();
	ImDrawList* RenderScene();

	float RenderText(const std::string& text, ImVec2 position, float size, Color color, bool center = false, bool outline = true, ImFont* pFont = g_pDefaultFont);

	void RenderCircle3D(Vector position, float points, float radius, Color color);

	void RenderImage(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a = ImVec2(0, 0), const ImVec2& uv_b = ImVec2(1, 1), ImU32 col = 0xFFFFFFFF) {
		draw_list->AddImage(user_texture_id, a, b, uv_a, uv_b, col);
	}

	template <class T>
	inline void RenderBoxByType(T x1, T y1, T x2, T y2, Color color, float thickness = 1.f, int type = 0) {
		if (type == 0)
			RenderBox(x1, y1, x2, y2, color, thickness);
		else if (type == 1)
			RenderCoalBox(x1, y1, x2, y2, color);
		else if (type == 2)
			RenderBox(x1, y1, x2, y2, color, thickness, 8.f);
	}

	template <class T>
	inline void RenderBoxFilledByType(T x1, T y1, T x2, T y2, Color color, float thickness = 1.f, int type = 0) {
		if (type == 0 || type == 1)
			RenderBoxFilled(x1, y1, x2, y2, color, thickness);
		else if (type == 2)
			RenderBoxFilled(x1, y1, x2, y2, color, thickness, 8.f);
	}

	inline void render_circle_3d(const Vector vecPosition, const int32_t iPointCount, const float_t flRadius, Color a_color, float width)
	{
		float_t flStep = (float_t)(3.14159265358979323846f) * 2.0f / (float_t)(iPointCount);
		std::vector<ImVec2> m_points;
		for (float a = 0; a < (3.14159265358979323846f * 2.0f); a += flStep)
		{
			Vector vecStart = Vector(flRadius * cosf(a) + vecPosition.x, flRadius * sinf(a) + vecPosition.y, vecPosition.z);
			Vector vecEnd = Vector(flRadius * cosf(a + flStep) + vecPosition.x, flRadius * sinf(a + flStep) + vecPosition.y, vecPosition.z);

			Vector vecStart2D, vecEnd2D;
			g_DebugOverlay->ScreenPosition(vecStart, vecStart2D);
			g_DebugOverlay->ScreenPosition(vecEnd, vecEnd2D);

			m_points.push_back(ImVec2(vecEnd2D.x, vecEnd2D.y));
		}
		draw_list->AddPolyline(m_points.data(), m_points.size(), GetU32(a_color), true, width);
	}

	inline void render_circle_3d_filled(const Vector vecPosition, const int32_t iPointCount, const float_t flRadius, Color a_color, float width)
	{
		float_t flStep = (float_t)(3.14159265358979323846f) * 2.0f / (float_t)(iPointCount);
		std::vector<ImVec2> m_points;
		for (float a = 0; a < (3.14159265358979323846f * 2.0f); a += flStep)
		{
			Vector vecStart = Vector(flRadius * cosf(a) + vecPosition.x, flRadius * sinf(a) + vecPosition.y, vecPosition.z);
			Vector vecEnd = Vector(flRadius * cosf(a + flStep) + vecPosition.x, flRadius * sinf(a + flStep) + vecPosition.y, vecPosition.z);

			Vector vecStart2D, vecEnd2D;
			g_DebugOverlay->ScreenPosition(vecStart, vecStart2D);
			g_DebugOverlay->ScreenPosition(vecEnd, vecEnd2D);

			m_points.push_back(ImVec2(vecEnd2D.x, vecEnd2D.y));
		}
		draw_list->AddConvexPolyFilled(m_points.data(), m_points.size(), GetU32(a_color));
	}

	inline void render_circle_3d_filled_border(const Vector vecPosition, const int32_t iPointCount, const float_t flRadius, Color a_color, float width)
	{
		float_t flStep = (float_t)(3.14159265358979323846f) * 2.0f / (float_t)(iPointCount);
		std::vector<ImVec2> m_points;
		for (float a = 0; a < (3.14159265358979323846f * 2.0f); a += flStep)
		{
			Vector vecStart = Vector(flRadius * cosf(a) + vecPosition.x, flRadius * sinf(a) + vecPosition.y, vecPosition.z);
			Vector vecEnd = Vector(flRadius * cosf(a + flStep) + vecPosition.x, flRadius * sinf(a + flStep) + vecPosition.y, vecPosition.z);

			Vector vecStart2D, vecEnd2D;
			g_DebugOverlay->ScreenPosition(vecStart, vecStart2D);
			g_DebugOverlay->ScreenPosition(vecEnd, vecEnd2D);

			m_points.push_back(ImVec2(vecEnd2D.x, vecEnd2D.y));
		}

		draw_list->AddConvexPolyFilled(m_points.data(), m_points.size(), GetU32(Color(a_color.r() / 255.f, a_color.g() / 255.f, a_color.b() / 255.f, a_color.a() / 255.f * 0.3f)));
		draw_list->AddPolyline(m_points.data(), m_points.size(), GetU32(a_color), true, width);
	}

	inline void arc(float x, float y, float radius, float min_angle, float max_angle, Color col, float thickness)
	{
		draw_list->PathArcTo(ImVec2(x, y), radius, DEG2RAD(min_angle), DEG2RAD(max_angle), 32);
		draw_list->PathStroke(GetU32(col), false, thickness);
	}

	inline void two_sided_arc(float x, float y, float radius, float scale, Color col, float thickness)
	{
		arc(x, y, radius, -270, -90 - 90 * (scale * 2), col, thickness);
		arc(x, y, radius, -90 + 90 * (scale * 2), 90, col, thickness);
	};

	inline void render_circle_3d_filled_border_progress(const Vector vecPosition, const int32_t iPointCount, const float_t flRadius, Color a_color, float width, float progress)
	{
		float_t flStep = (float_t)(3.14159265358979323846f) * 2.0f / (float_t)(iPointCount);
		std::vector<ImVec2> m_points;
		for (float a = 0; a < (3.14159265358979323846f * 2.0f); a += flStep)
		{
			Vector vecStart = Vector(flRadius * cosf(a) + vecPosition.x, flRadius * sinf(a) + vecPosition.y, vecPosition.z);
			Vector vecEnd = Vector(flRadius * cosf(a + flStep) + vecPosition.x, flRadius * sinf(a + flStep) + vecPosition.y, vecPosition.z);

			Vector vecStart2D, vecEnd2D;
			g_DebugOverlay->ScreenPosition(vecStart, vecStart2D);
			g_DebugOverlay->ScreenPosition(vecEnd, vecEnd2D);

			m_points.push_back(ImVec2(vecEnd2D.x, vecEnd2D.y));
		}

		draw_list->AddConvexPolyFilled(m_points.data(), m_points.size(), GetU32(Color(a_color.r() / 255.f, a_color.g() / 255.f, a_color.b() / 255.f, a_color.a() / 255.f * 0.3f)));
		draw_list->AddPolyline(m_points.data(), m_points.size() * progress, GetU32(a_color), progress == 1.f, width);
	}

	template <class T>
	inline void RenderCoalBox(T x1, T y1, T x2, T y2, Color color, float th = 1.f) {
		int w = x2 - x1;
		int h = y2 - y1;

		int iw = w / 4;
		int ih = h / 4;
		// top
		RenderLine(x1, y1, x1 + iw, y1, color, th);					// left
		RenderLine(x1 + w - iw, y1, x1 + w, y1, color, th);			// right
		RenderLine(x1, y1, x1, y1 + ih, color, th);					// top left
		RenderLine(x1 + w - 1, y1, x1 + w - 1, y1 + ih, color, th);	// top right
																	// bottom
		RenderLine(x1, y1 + h, x1 + iw, y1 + h, color, th);			// left
		RenderLine(x1 + w - iw, y1 + h, x1 + w, y1 + h, color, th);	// right
		RenderLine(x1, y1 + h - ih, x1, y1 + h, color, th);			// bottom left
		RenderLine(x1 + w - 1, y1 + h - ih, x1 + w - 1, y1 + h, color, th);	// bottom right
	}

	inline void render_rect_filled_multicolor(const int x1, const int y1, const int x2, const int y2, const Color color_up_left, const Color color_down_left, const Color color_up_right, const Color color_down_right) {
		draw_list->AddRectFilledMultiColor(ImVec2(static_cast<float>(x1), static_cast<float>(y1)), ImVec2(static_cast<float>(x2), static_cast<float>(y2)), ImColor(color_up_left.r(), color_up_left.g(), color_up_left.b(), color_up_left.a()), ImColor(color_up_right.r(), color_up_right.g(), color_up_right.b(), color_up_right.a()), ImColor(color_down_right.r(), color_down_right.g(), color_down_right.b(), color_down_right.a()), ImColor(color_down_left.r(), color_down_left.g(), color_down_left.b(), color_down_left.a()));
	}

	inline void TriangleFilled(float x1, float y1, float x2, float y2, float x3, float y3, Color clr)
	{
		draw_list->AddTriangleFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), GetU32(clr));
	}

	template <class T>
	inline void RenderBox(T x1, T y1, T x2, T y2, Color color, float thickness = 1.f, float rounding = 0.f) {
		draw_list->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), GetU32(color), rounding, 15, thickness);
	}

	inline void RenderBox(RECT r, Color color,float thickness = 1.f, float rounding = 0.f) {
		RenderBox(r.left, r.top, r.right, r.bottom, color, thickness, rounding);
	}

	template <class T>
	inline void RenderBoxFilled(T x1, T y1, T x2, T y2, Color color, float thickness = 1.f, float rounding = 0.f) {
		draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), GetU32(color), rounding, 15);
	}

	template <class T>
	inline void RenderLine(T x1, T y1, T x2, T y2, Color color, float thickness = 1.f) {
		draw_list->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), GetU32(color), thickness);
	}

	template <class T>
	inline float RenderText(const std::string& text, T x, T y, float size, Color clr, bool center = false, bool outline = true, ImFont* pFont = g_pDefaultFont) {
		return RenderText(text, ImVec2(x, y), size, clr, center, outline, pFont);
	}

	template <class T>
	inline void RenderCircle(T x, T y, float radius, int points, Color color, float thickness = 1.f) {
		draw_list->AddCircle(ImVec2(x, y), radius, GetU32(color), points, thickness);
	}

	template <class T>
	inline void RenderCircleFilled(T x, T y, float radius, int points, Color color) {
		draw_list->AddCircleFilled(ImVec2(x, y), radius, GetU32(color), points);
	}
};