#include "../hooks.hpp"
#include "../../features/visuals.hpp"
#include "../../features/misc.h"

int __stdcall Hooks::DoPostScreenEffects::hook(int a1)
{
	visuals::no_smoke();
	visuals::no_flash();
	if (g_LocalPlayer && g_Options.glow_enabled)
		Glow::Get().Run();

	misc::fog_controller();

	return o_DoPostScreenEffects(g_ClientMode, a1);
}