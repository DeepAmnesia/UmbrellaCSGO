#include "../hooks.hpp"

bool __stdcall Hooks::FireEvent::hook(IGameEvent* pEvent)
{
	//changer::on_event(event);
	radar::load_map(Utils::find_hud_element("CCSGO_MapOverview"));

	if (get_hash(pEvent->GetName()) == Utils::get_const("server_spawn"))
	{
	}
	return o_FireEvent(g_GameEvents, pEvent);
}