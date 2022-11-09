#include "render.hpp"

#include <mutex>

#include "core/features/visuals.hpp"
#include "valve_sdk/csgostructs.hpp"
#include "helpers/input.hpp"
#include "menu.hpp"
#include "options.hpp"
#include "fonts/fonts.hpp"
#include "helpers/math.hpp"
#include "fonts/weapon_icons.h"
#include "core/features/legit_bot.h"
#include "fonts/weapon_esp_icons.h"
#include "core/features/Radar.h"

ImFont* g_pDefaultFont;
ImFont* g_pSecondFont;
ImFont* weapon_icons;
ImFont* m_BigIcons;

ImDrawListSharedData _data;

std::mutex render_mutex;

void Render::Initialize()
{
	ImGui::CreateContext();

	ImGui_ImplWin32_Init(InputSys::Get().GetMainWindow());
	ImGui_ImplDX9_Init(g_D3DDevice9);

	draw_list = new ImDrawList(ImGui::GetDrawListSharedData());
	draw_list_act = new ImDrawList(ImGui::GetDrawListSharedData());
	draw_list_rendering = new ImDrawList(ImGui::GetDrawListSharedData());

	GetFonts();
}

void Render::GetFonts() 
{
	ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
		Fonts::Droid_compressed_data,
		Fonts::Droid_compressed_size,
		14.f);
	
	g_pDefaultFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
		Fonts::Droid_compressed_data,
		Fonts::Droid_compressed_size,
		18.f);
	
	ImFontConfig cfg;
	cfg.PixelSnapH = 0;
	cfg.OversampleH = 5;
	cfg.OversampleV = 5;
	cfg.RasterizerMultiply = 1.2f;

	g_pSecondFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:/windows/fonts/segoeui.ttf", 16.f, &cfg, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	weapon_icons = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(weapon_compressed_data, weapon_compressed_size, 18.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	m_BigIcons = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(WeaponIcons_compressed_data_base85, 24.f, &cfg);
}

void Render::ClearDrawList() 
{
	render_mutex.lock();
	draw_list_act->Clear();
	render_mutex.unlock();
}

void Render::BeginScene() 
{
	draw_list->Clear();
	draw_list->PushClipRectFullScreen();

	if (g_Options.remove_scope)
	{
		if (g_LocalPlayer && g_LocalPlayer->IsAlive())
		{
			C_BaseCombatWeapon* pCombatWeapon = g_LocalPlayer->m_hActiveWeapon().Get();
			if (pCombatWeapon)
			{
				if (pCombatWeapon->get_idx() == WEAPON_AWP || pCombatWeapon->get_idx() == WEAPON_SSG08
					|| pCombatWeapon->get_idx() == WEAPON_SCAR20 || pCombatWeapon->get_idx() == WEAPON_G3SG1)
				{
					if (g_LocalPlayer->m_bIsScoped())
					{
						int w, h;
						g_EngineClient->GetScreenSize(w, h);
						Render::Get().RenderLine(0, h / 2, w, h / 2, Color(0, 0, 0), 1.f);
						Render::Get().RenderLine(w / 2, 0, w / 2, h, Color(0, 0, 0), 1.f);
					}
				}
			}
		}
	}

	if (g_EngineClient->IsInGame() && g_LocalPlayer)
		visuals::draw();

	//legit fov
	legitbot::on_draw(g_LocalPlayer);
	visuals::draw_damage_indicator(g_LocalPlayer);
	visuals::draw_hit_marker(g_LocalPlayer);
	visuals::draw_hit_marker2(g_LocalPlayer);
	visuals::render_dopped_throwed();
	visuals::grenades();
	visuals::render_hit_arrows();
	visuals::render_sound_arrows();

	render_mutex.lock();
	*draw_list_act = *draw_list;
	render_mutex.unlock();
}

ImDrawList* Render::RenderScene() 
{
	if (render_mutex.try_lock()) {
		*draw_list_rendering = *draw_list_act;
		render_mutex.unlock();
	}

	return draw_list_rendering;
}

float Render::RenderText(const std::string& text, ImVec2 pos, float size, Color color, bool center, bool outline, ImFont* pFont)
{
	ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, text.c_str());
	if (!pFont->ContainerAtlas) return 0.f;
	draw_list->PushTextureID(pFont->ContainerAtlas->TexID);

	if (center)
		pos.x -= textSize.x / 2.0f;

	if (outline) {
		draw_list->AddText(pFont, size, ImVec2(pos.x + 1, pos.y + 1), GetU32(Color(30 / 255.f, 30 / 255.f, 36 / 255.f, color.a() / 255.f)), text.c_str());
	//	draw_list->AddText(pFont, size, ImVec2(pos.x - 1, pos.y - 1), GetU32(Color(30, 30, 36, color.a())), text.c_str());
	//	draw_list->AddText(pFont, size, ImVec2(pos.x + 1, pos.y - 1), GetU32(Color(30, 30, 36, color.a())), text.c_str());
	//	draw_list->AddText(pFont, size, ImVec2(pos.x - 1, pos.y + 1), GetU32(Color(30, 30, 36, color.a())), text.c_str());
	}

	draw_list->AddText(pFont, size, pos, GetU32(color), text.c_str());

	draw_list->PopTextureID();

	return pos.y + textSize.y;
}

void Render::RenderCircle3D(Vector position, float points, float radius, Color color)
{
	float step = (float)M_PI * 2.0f / points;

	for (float a = 0; a < (M_PI * 2.0f); a += step)
	{
		Vector start(radius * cosf(a) + position.x, radius * sinf(a) + position.y, position.z);
		Vector end(radius * cosf(a + step) + position.x, radius * sinf(a + step) + position.y, position.z);

		Vector start2d, end2d;
		if (g_DebugOverlay->ScreenPosition(start, start2d) || g_DebugOverlay->ScreenPosition(end, end2d))
			return;

		RenderLine(start2d.x, start2d.y, end2d.x, end2d.y, color);
	}
}
