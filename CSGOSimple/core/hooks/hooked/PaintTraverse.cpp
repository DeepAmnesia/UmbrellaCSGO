#include "../hooks.hpp"
#include "../../features/visuals.hpp"
#include "../../features/model.h"

void __stdcall Hooks::PaintTreverse::hook(vgui::VPANEL panel, bool forceRepaint, bool allowForce)
{
	static auto panelId = vgui::VPANEL{ 0 };

	static auto in_game = false;

	if (!in_game && g_EngineClient->IsInGame())
	{
		in_game = true;
		g_cheat.update_radar = true;
	}
	else if (in_game && !g_EngineClient->IsInGame())
	{
		in_game = false;
	}

	static uint32_t hud_zoom_panel = 0;

	if (!hud_zoom_panel)
		if (!strcmp("HudZoom", g_VGuiPanel->GetName(panel)))
			hud_zoom_panel = panel;

	if (hud_zoom_panel == panel && g_Options.remove_scope)
		return;

	o_PaintTreverse(g_VGuiPanel, panel, forceRepaint, allowForce);

	if (!panelId) {
		const auto panelName = g_VGuiPanel->GetName(panel);
		if (!strcmp(panelName, "MatSystemTopPanel")) {
			panelId = panel;
		}
	}
	else if (panelId == panel) 
	{
		visuals::force_sniper_crosshair();
		visuals::remove_panorama_blur();

		static auto zoom_sens = g_CVar->FindVar("zoom_sensitivity_ratio_mouse");
		if (g_Options.remove_scope)
			zoom_sens->SetValue(0);
		else
			zoom_sens->SetValue(1);

		if (Menu::Get().IsVisible())
			g_DrawModel->Instance();

		
		Render::Get().BeginScene();
	}
}