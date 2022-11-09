#include "events.h"
#include <algorithm>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>
#include <limits>
#include <ctime>
#include "legit_bot.h"
#include "visuals.hpp"
#include "buy_bot.h"

bool weapon_is_aim(const std::string& weapon)
{
	return weapon.find("decoy")		== std::string::npos && weapon.find("flashbang")	== std::string::npos &&
		   weapon.find("hegrenade") == std::string::npos && weapon.find("inferno")		== std::string::npos &&
		   weapon.find("molotov")	== std::string::npos && weapon.find("smokegrenade") == std::string::npos;
}

template<class T>
static T* FindHudElement(const char* name)
{
	static auto pThis = *reinterpret_cast<DWORD**>(Utils::PatternScan(GetModuleHandleA("client.dll"), "B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08") + 1);

	static auto FindHudElement = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28"));
	return (T*)FindHudElement(pThis, name);
}

void C_HookedEvents::FireGameEvent(IGameEvent* pEvent)
{
	if (!g_EngineClient->IsConnected() || !g_EngineClient->IsInGame() || !g_LocalPlayer)
		return;

	if (!pEvent)
		return;

	if (!strcmp(pEvent->GetName(), "player_hurt"))
	{
		visuals::on_events_damage_indicator(pEvent);
		std::string weapon = pEvent->GetString("weapon");
		if (weapon_is_aim(weapon) )
		{
			visuals::on_events_hit_marker(pEvent);
		}

		const auto hurt_player = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(pEvent->GetInt("userid"))));
		if (!hurt_player || !hurt_player->IsPlayer())
			return;

		const auto hurt_player_atacker = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(pEvent->GetInt("attacker"))));
		if (!hurt_player_atacker || !hurt_player_atacker->IsPlayer())
			return;

		if (g_EngineClient->GetPlayerForUserID(pEvent->GetInt("attacker")) == g_EngineClient->GetLocalPlayer())
		{
			if (g_Options.misc_hiteffect && g_LocalPlayer && g_LocalPlayer->IsAlive())
				g_LocalPlayer->get_health_boost_time() = g_GlobalVars->curtime + g_Options.misc_hiteffect_duration;
		}
		else
		{
			if (hurt_player == g_LocalPlayer && hurt_player_atacker != g_LocalPlayer && g_Options.hit_arrow)
			{
				Vector vec_to_screen = Vector(0, 0, 0);
				if ((g_DebugOverlay->ScreenPosition(hurt_player_atacker->GetAbsOrigin(), vec_to_screen) && !g_Options.hit_arrow_only_visible)
					|| (!g_DebugOverlay->ScreenPosition(hurt_player_atacker->GetAbsOrigin(), vec_to_screen) && g_Options.hit_arrow_only_visible))
				{
					if (weapon_is_aim(weapon))
					{
						hit_arrow_data data{ hurt_player_atacker, g_GlobalVars->curtime + 2.f, pEvent->GetInt("dmg_health"), weapon };
						visuals::add_hit_arrow(data);
					}
				}
			}
		}
	}

	if (!strcmp(pEvent->GetName(), "player_footstep"))
	{
		const auto player = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(pEvent->GetInt("userid"))));
		if (!player || !player->IsPlayer())
			return;

		bool is_enemy = player->m_iTeamNum() != g_LocalPlayer->m_iTeamNum() && g_Options.esp_settings[0].sound_esp;
		bool is_team = player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum() && player != g_LocalPlayer && g_Options.esp_settings[1].sound_esp;
		bool is_local = player == g_LocalPlayer && g_Input->m_fCameraInThirdPerson && g_Options.esp_settings[2].sound_esp;

		if (is_enemy)
		{
			soung_esp_info info;
			info.pos = Vector(player->m_vecOrigin() + Vector(0.0f, 0.0f, 5.0f));
			info.clr = Color(g_Options.esp_settings[0].sound_clr);
			info.time = g_GlobalVars->realtime;
			g_cheat.sounds_info.push_back(info);
		}
		else if (is_team)
		{
			soung_esp_info info;
			info.pos = Vector(player->m_vecOrigin() + Vector(0.0f, 0.0f, 5.0f));
			info.clr = Color(g_Options.esp_settings[1].sound_clr);
			info.time = g_GlobalVars->realtime;
			g_cheat.sounds_info.push_back(info);
		}
		else if (is_local)
		{
			soung_esp_info info;
			info.pos = Vector(player->m_vecOrigin() + Vector(0.0f, 0.0f, 5.0f));
			info.clr = Color(g_Options.esp_settings[2].sound_clr);
			info.time = g_GlobalVars->realtime;
			g_cheat.sounds_info.push_back(info);
		}

		if (player->m_iTeamNum() != g_LocalPlayer->m_iTeamNum())
		{
			Vector vec_to_screen = Vector(0, 0, 0);
			if ((g_DebugOverlay->ScreenPosition(player->GetAbsOrigin(), vec_to_screen) && !g_Options.esp_settings[0].sound_arrows_only_visible)
				|| (!g_DebugOverlay->ScreenPosition(player->GetAbsOrigin(), vec_to_screen) && g_Options.esp_settings[0].sound_arrows_only_visible))
			{
				sound_arrow_data data{ player, g_GlobalVars->curtime + 2.f };
				visuals::add_sound_arrow_enemy(data);
			}
		}
		else if (player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum() && player != g_LocalPlayer)
		{
			Vector vec_to_screen = Vector(0, 0, 0);
			if ((g_DebugOverlay->ScreenPosition(player->GetAbsOrigin(), vec_to_screen) && !g_Options.esp_settings[1].sound_arrows_only_visible)
				|| (!g_DebugOverlay->ScreenPosition(player->GetAbsOrigin(), vec_to_screen) && g_Options.esp_settings[1].sound_arrows_only_visible))
			{
				sound_arrow_data data{ player, g_GlobalVars->curtime + 2.f };
				visuals::add_sound_arrow_team(data);
			}
		}
	}
	if (!strcmp(pEvent->GetName(), "bullet_impact"))
	{
		
	}
	if (!strcmp(pEvent->GetName(), "bomb_beginplant"))
	{
		g_cheat.bomb_begin_defuse = false;
		g_cheat.bomb_is_planting = true;
		g_cheat.plant_time = g_GlobalVars->curtime + 3.4f;
	}
	if (!strcmp(pEvent->GetName(), "bomb_abortplant"))
	{
		g_cheat.bomb_begin_defuse = false;
		g_cheat.bomb_is_planting = false;
		g_cheat.plant_time = 0;
	}
	if (!strcmp(pEvent->GetName(), "bomb_begindefuse"))
	{
		g_cheat.bomb_is_planting = false;
		g_cheat.bomb_begin_defuse = true;
		g_cheat.defuser_has_kits = pEvent->GetBool("haskit");
		if (g_cheat.defuser_has_kits)
			g_cheat.bomb_timer_stamp = g_GlobalVars->curtime + 5;
		else
			g_cheat.bomb_timer_stamp = g_GlobalVars->curtime + 10;
	}
	if (!strcmp(pEvent->GetName(), "bomb_abortdefuse"))
	{
		g_cheat.bomb_is_planting = false;
		g_cheat.bomb_begin_defuse = false;
		g_cheat.bomb_timer_stamp = 0;
	}
	if (!strcmp(pEvent->GetName(), "bomb_exploded"))
	{
		g_cheat.bomb_is_planting = false;
		g_cheat.bomb_begin_defuse = false;
		g_cheat.plant_time = 0;
		g_cheat.bomb_timer_stamp = 0;
	}
	if (!strcmp(pEvent->GetName(), "player_death"))
	{
		legitbot::on_events(g_LocalPlayer, pEvent);
	}
	if (!strcmp(pEvent->GetName(), "round_start"))
	{
		g_cheat.plant_time = 0;
		g_cheat.bomb_timer_stamp = 0;
		g_cheat.bomb_begin_defuse = false;
		g_cheat.bomb_is_planting = false;
		g_cheat.clear_bomb_data = true;
		visuals::reset_data();
		visuals::clear_hit_markers();
		visuals::clear_arrows();
		g_BuyBot->OnRoundStart();
	}
	if (!strcmp(pEvent->GetName(), "bomb_planted"))
	{
		g_cheat.bomb_is_planting = false;
		g_cheat.bomb_begin_defuse = false;
		g_cheat.plant_time = 0;
		g_cheat.bomb_timer_stamp = 0;
	}
	if (!strcmp(pEvent->GetName(), "round_end"))
	{
		g_cheat.bomb_begin_defuse = false;
		g_cheat.bomb_is_planting = false;
		g_cheat.clear_bomb_data = true;
		for (int i = 1; i < g_GlobalVars->maxClients; ++i)
		{
			visuals::player_data[i].prev_ammo = 0.0f;
			visuals::player_data[i].prev_health = 0.0f;

			visuals::dormant_players[i].dormant = 0.0f;
			visuals::dormant_players[i].receive_time = 0.0f;
			visuals::dormant_players[i].origin = Vector(0, 0, 0);
		}
	}
}

int C_HookedEvents::GetEventDebugID(void)
{
	return EVENT_DEBUG_ID_INIT;
}

void C_HookedEvents::RegisterSelf()
{
	m_iDebugId = EVENT_DEBUG_ID_INIT;
	g_GameEvents->AddListener(this, "player_footstep", false);
	g_GameEvents->AddListener(this, "player_hurt", false);
	g_GameEvents->AddListener(this, "player_death", false);
	g_GameEvents->AddListener(this, "weapon_fire", false);
	g_GameEvents->AddListener(this, "item_purchase", false);
	g_GameEvents->AddListener(this, "bullet_impact", false);
	g_GameEvents->AddListener(this, "round_start", false);
	g_GameEvents->AddListener(this, "round_freeze_end", false);
	g_GameEvents->AddListener(this, "bomb_defused", false);
	g_GameEvents->AddListener(this, "bomb_begindefuse", false);
	g_GameEvents->AddListener(this, "bomb_beginplant", false);
	g_GameEvents->AddListener(this, "bomb_abortplant", false);
	g_GameEvents->AddListener(this, "bomb_abortdefuse", false);
	g_GameEvents->AddListener(this, "bomb_exploded", false);
	g_GameEvents->AddListener(this, "bomb_planted", false);

}

void C_HookedEvents::RemoveSelf()
{
	g_GameEvents->RemoveListener(this);
}