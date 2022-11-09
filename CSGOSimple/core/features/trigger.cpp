#pragma warning (disable: 4018)

#include "trigger.h"
#include "../../valve_sdk/misc/weaponids.h"
#include "../../options.hpp"
#include "../../imgui/imgui.h"
#include "../../render.hpp"

bool	trigger_in_attack;
bool	trigger_can_shot;

bool	trigger_target_fov;
bool	trigger_target_silent_fov;
bool	trigger_target_change;

float	trigger_p_fov;
float	trigger_kill_delay_timer;
float	trigger_shot_delay_timer;

QAngle	trigger_m_oldangle;
QAngle	trigger_aimpunch;

Vector	trigger_best_vector_hitbox;
Vector  trigger_best_hitboxscreen;
int		trigger_best_target;
int		trigger_best_pretarget;
int		trigger_best_hitbox;

int		trigger_cur_weapon;

int trigger::weapon_index_data[34] = {
	WEAPON_DEAGLE,WEAPON_ELITE,WEAPON_FIVESEVEN,
	WEAPON_GLOCK,WEAPON_HKP2000,WEAPON_P250,
	WEAPON_USP_SILENCER,WEAPON_CZ75A,WEAPON_REVOLVER,
	WEAPON_TEC9,

	WEAPON_AK47,WEAPON_AUG,WEAPON_FAMAS,WEAPON_GALILAR,
	WEAPON_M249,WEAPON_M4A1,WEAPON_M4A1_SILENCER,WEAPON_MAC10,
	WEAPON_P90,WEAPON_MP5,WEAPON_UMP45,WEAPON_XM1014,WEAPON_BIZON,
	WEAPON_MAG7,WEAPON_NEGEV,WEAPON_SAWEDOFF,
	WEAPON_MP7,WEAPON_MP9,WEAPON_NOVA,WEAPON_SG556,
	WEAPON_SCAR20,WEAPON_G3SG1,

	WEAPON_AWP,WEAPON_SSG08
};

const char* trigger::weapon_data[34] = {
	"Desert Eagle","Dual Berettas","Five-SeveN",
	"Glock-18","P2000","P250",
	"USP-S","CZ75 Auto","R8 Revolver",
	"Tec-9",

	"AK-47","AUG","FAMAS","Galil AR",
	"M249","M4A4","M4A1-S","MAC-10"	,
	"P90","MP5-SD","UMP-45","XM1014","PP-Bizon",
	"MAG-7","Negev","Sawed-Off",
	"MP7","MP9","Nova","SG 553",
	"SCAR-20","G3SG1",

	"AWP","SSG 08"

};

int trigger::get_weapon_select(C_BasePlayer* local_player) {
	for (size_t i = 0; i < (sizeof(weapon_data) / sizeof(*weapon_data)); i++) {
		if (weapon_index_data[i] == local_player->m_hActiveWeapon().Get()->get_idx())
			return i;
	}

	return -1;
}

void trigger::run(CUserCmd* cmd)
{
	if ((g_Options.trigget_key != 0 && Utils::key_bind(g_Options.trigget_key, g_Options.trigger_mode)) || g_Options.trigget_key == 0)
	{
		if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
			return;

		const auto activeWeapon = g_LocalPlayer->m_hActiveWeapon();

		if (!activeWeapon)
			return;

		if (!activeWeapon->IsGun())
			return;

		if (activeWeapon->is_empty() || activeWeapon->IsReloading())
			return;

		if (activeWeapon->get_weapon_type() == WT_Sniper && g_Options.trigger_only_in_scope && !g_LocalPlayer->m_bIsScoped())
			return;

		if (!activeWeapon->CanFire())
			return;

		const auto data = activeWeapon->GetWeaponData();

		trigger_cur_weapon = get_weapon_select(g_LocalPlayer);

		auto settings = g_Options.trigger_for_weapon[trigger_cur_weapon];

		if (!settings.enable)
			return;

		QAngle angLocalAngles;
		g_EngineClient->GetViewAngles(&angLocalAngles);

		Vector vecDirection;
		Math::AngleVectors(angLocalAngles, vecDirection);

		CGameTrace trace;
		g_EngineTrace->TraceLine(g_LocalPlayer->get_eye_position(), g_LocalPlayer->get_eye_position() + (vecDirection * data->m_flRange), MASK_SHOT_HULL | CONTENTS_HITBOX, g_LocalPlayer, NULL, &trace);

		if (trace.fraction == 1.0f)
			return;

		if (trace.hit_entity == nullptr || trace.hit_entity == g_EntityList->GetClientEntity(NULL))
			return;

		for (int i = 0; i < g_EntityList->GetHighestEntityIndex(); i++)
		{
			const auto entity = reinterpret_cast <C_BaseEntity*> (g_EntityList->GetClientEntity(i));

			if (entity && entity->IsPlayer())
			{
				C_BasePlayer* player = (C_BasePlayer*)(entity);
				if (player->IsAlive())
				{
					const auto hit_entity = (C_BasePlayer*)trace.hit_entity;
					if (hit_entity == player)
					{
						if (settings.legit_checks)
						{
							if (!(hit_entity->m_fFlags() & FL_ONGROUND)) return;
							if (g_LocalPlayer->IsFlashed()) return;
							if (Utils::line_goes_through_smoke(g_LocalPlayer->get_eye_position(), g_LocalPlayer->get_eye_position() + (vecDirection * data->m_flRange))) return;
						}
						if (hit_entity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum() || settings.legit_teamattack)
						{
							if (!settings.hitgroup || trace.hitgroup == settings.hitgroup)
							{
								const float damage = autowall::can_hit(g_LocalPlayer, trace.endpos, (C_BasePlayer*)trace.hit_entity);

								if (damage >= settings.minimal_damage)
								{
									if (rand() % 100 >= 100 - settings.trigger_chance)
									{
										cmd->buttons |= IN_ATTACK;
										//Utils::ConsolePrint("hit\n");
										//Utils::ConsolePrint(std::to_string(damage).c_str());
										//Utils::ConsolePrint("\n");
									}
								}
							}
						}
					}
				}
			}
		}
	}
}