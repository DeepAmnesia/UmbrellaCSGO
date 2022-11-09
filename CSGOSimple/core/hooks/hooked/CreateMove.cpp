#include "../hooks.hpp"
#include "../../../core/features/misc.h"
#include "../../features/prediction.h"
#include "../../features/legit_bot.h"
#include "../../features/trigger.h"
#include "../../../helpers/utils.hpp"
#include "../../features/grenades.h"
#include "../../features/buy_bot.h"

bool __stdcall Hooks::CreateMove::hook(float input_sample_frametime, CUserCmd* cmd)
{
	bool* send_packet = reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()) + 0x14);
	bool result = o_CreateMove(g_ClientMode, input_sample_frametime, cmd);

	if (!cmd || !cmd->command_number)
		return result;

	g_cheat.cmd = cmd;

	if (Menu::Get().IsVisible())
		cmd->buttons &= ~(IN_ATTACK | IN_ATTACK2 | IN_RELOAD);

	if (g_Options.misc_bhop)
		BunnyHop::OnCreateMove(cmd);

	auto unpred_flags = g_LocalPlayer->m_fFlags();

	misc::custom_shadows();
	misc::auto_strafe(cmd);
	misc::anti_afk_kick(cmd);

	if (!g_Options.safe_mode)
	{
		if (g_Options.misc_showranks && cmd->buttons & IN_SCORE)
			g_CHLClient->DispatchUserMessage(CS_UM_ServerRankRevealAll, 0, 0, nullptr);
	}

	g_BuyBot->OnCreateMove();

	if (g_LocalPlayer->m_hActiveWeapon())
	{
			const int weapon_settings_select = legitbot::get_weapon_select(g_LocalPlayer);
			if (weapon_settings_select >= 0)
				Utils::cur_weapon = weapon_settings_select;
	}

	*reinterpret_cast<int*>((DWORD)&g_CVar->FindVar("viewmodel_offset_x")->m_fnChangeCallbacks + 0xC) = 0;
	*reinterpret_cast<int*>((DWORD)&g_CVar->FindVar("viewmodel_offset_y")->m_fnChangeCallbacks + 0xC) = 0;
	*reinterpret_cast<int*>((DWORD)&g_CVar->FindVar("viewmodel_offset_z")->m_fnChangeCallbacks + 0xC) = 0;

	static auto viewmodel_offset_x = g_CVar->FindVar("viewmodel_offset_x");
	static auto viewmodel_offset_y = g_CVar->FindVar("viewmodel_offset_y");
	static auto viewmodel_offset_z = g_CVar->FindVar("viewmodel_offset_z");

	viewmodel_offset_x->SetValue(g_Options.view_offsets ? g_Options.viewmodel_x + 1.f : 1);
	viewmodel_offset_y->SetValue(g_Options.view_offsets ? g_Options.viewmodel_y + 1.f : 1);
	viewmodel_offset_z->SetValue(g_Options.view_offsets ? g_Options.viewmodel_z - 1.f : -1);

	static auto m_ClCsmShadows = g_CVar->FindVar("cl_csm_shadows");
	static auto m_ClFootContactShadows = g_CVar->FindVar("cl_foot_contact_shadows");
	static auto m_ClCsmStaticPropShadows = g_CVar->FindVar("cl_csm_static_prop_shadows");
	static auto m_ClCsmWorldShadows = g_CVar->FindVar("cl_csm_world_shadows");
	static auto m_ClCsmViewmodelShadows = g_CVar->FindVar("cl_csm_viewmodel_shadows");
	static auto m_ClCsmSpriteShadows = g_CVar->FindVar("cl_csm_sprite_shadows");
	static auto m_ClCsmRopeShadows = g_CVar->FindVar("cl_csm_rope_shadows");

	m_ClFootContactShadows->SetValue(!g_Options.remove_shadows);
	m_ClCsmStaticPropShadows->SetValue(!g_Options.remove_shadows);
	m_ClCsmWorldShadows->SetValue(!g_Options.remove_shadows);
	m_ClCsmShadows->SetValue(!g_Options.remove_shadows);
	m_ClCsmViewmodelShadows->SetValue(!g_Options.remove_shadows);
	m_ClCsmSpriteShadows->SetValue(!g_Options.remove_shadows);
	m_ClCsmRopeShadows->SetValue(!g_Options.remove_shadows);

	static auto m_AmbientLight_r = g_CVar->FindVar("mat_ambient_light_r");
	static auto m_AmbientLight_g = g_CVar->FindVar("mat_ambient_light_g");
	static auto m_AmbientLight_b = g_CVar->FindVar("mat_ambient_light_b");

	Color post_pr_color = Color(g_Options.color_post_processing);

	m_AmbientLight_r->SetValue(g_Options.misc_post_processing && !g_Options.misc_no_post_processing ? post_pr_color.r() / 255.f : 0);
	m_AmbientLight_g->SetValue(g_Options.misc_post_processing && !g_Options.misc_no_post_processing ? post_pr_color.g() / 255.f : 0);
	m_AmbientLight_b->SetValue(g_Options.misc_post_processing && !g_Options.misc_no_post_processing ? post_pr_color.b() / 255.f : 0);

	EnginePred::BeginPred(cmd);
	{
		if (g_Options.edge_jump && Utils::key_bind(g_Options.edge_jump_key, g_Options.edge_jump_mode))
		{
			if ((unpred_flags & FL_ONGROUND) && !(g_LocalPlayer->m_fFlags() & FL_ONGROUND))
				cmd->buttons |= IN_JUMP;
		}

		g_GrenadePrediction->OnCreateMove(cmd);
		legitbot::on_move(g_LocalPlayer, cmd);
		trigger::run(cmd);
		
	}
	EnginePred::EndPred();
	return false;
}
