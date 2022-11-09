#include "../hooks.hpp"
#include "../../features/visuals.hpp"
#include "../../features/misc.h"

void __stdcall Hooks::FrameStageNotify::hook(ClientFrameStage_t stage)
{
	if (!g_LocalPlayer)
	{
		visuals::reset_data();

		return o_FrameStageNotify(g_CHLClient, stage);
	}
	o_FrameStageNotify(g_CHLClient, stage);
	visuals::disable_post_processing();
	g_cheat.m_vecShootPosition = g_LocalPlayer->GetEyePos();
	if (stage == FRAME_RENDER_START)
	{
		//misc::ragdoll();
	}
	if (stage == FRAME_RENDER_END)
	{
		misc::skybox_changer();
	}
	
}