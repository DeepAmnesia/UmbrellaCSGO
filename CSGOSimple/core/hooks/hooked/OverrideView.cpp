#include "../hooks.hpp"
#include "../../features/misc.h"
#include "../../features/third_person.h"

void __stdcall Hooks::OverrideView::hook(CViewSetup* vsView) 
{
	if (g_EngineClient->IsInGame() && vsView)
	{
		if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
			return;

		if ((g_Options.fov_changer && !g_LocalPlayer->m_bIsScoped()) || (g_LocalPlayer->m_bIsScoped() && g_Options.remove_scope))
			vsView->flFOV = g_Options.camera_fov;

		auto viewmodel = (C_BaseEntity*)g_EntityList->GetClientEntityFromHandle(g_LocalPlayer->get_view_model());

		if (viewmodel)
		{
			auto eyeAng = vsView->angView;

			eyeAng.z -= g_Options.view_offsets ? (float)g_Options.viewmodel_roll : 0;

			viewmodel->set_abs_angles(eyeAng);
		}
	}
	g_ThirdPerson->Instance();
	
	o_OverrideView(g_ClientMode, vsView);
}