#pragma warning (disable: 4018)

#include "legit_bot.h"
#include "../../valve_sdk/misc/weaponids.h"
#include "../../options.hpp"
#include "../../imgui/imgui.h"
#include "../../render.hpp"

bool	in_attack;
bool	can_shot;

bool	target_fov;
bool	target_silent_fov;
bool	target_change;

float	p_fov;
float	kill_delay_timer;
float	shot_delay_timer;

QAngle	m_oldangle;
QAngle	aimpunch;

Vector	best_vector_hitbox;
Vector  best_hitboxscreen;
int		best_target;
int		best_pretarget;
int		best_hitbox;

int		cur_weapon;

int legitbot::weapon_index_data[34] = {
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

const char* legitbot::weapon_data[34] = {
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

int legitbot::get_weapon_select(C_BasePlayer* local_player) {
	for (size_t i = 0; i < (sizeof(weapon_data) / sizeof(*weapon_data)); i++) {
		if (weapon_index_data[i] == local_player->m_hActiveWeapon().Get()->get_idx())
			return i;
	}

	return -1;
}

bool legitbot::is_valid(C_BasePlayer* local_player)
{

	if (!g_Options.legit_enable)
		return false;

	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return false;

	if (!local_player || !local_player->IsAlive())
		return false;

	auto weapon = local_player->m_hActiveWeapon();

	if (!weapon)
		return false;

	if (!weapon->IsGun())
		return false;

	if (weapon->is_empty() || weapon->IsReloading())
		return false;

	if (weapon->get_weapon_type() == WT_Sniper && g_Options.legit_onlyscope && !local_player->m_bIsScoped())
		return false;

	cur_weapon = get_weapon_select(local_player);

	return true;
}

float legitbot::get_player_fov(C_BasePlayer* local_player) {
	float fov_best_value = 10.f;
	float fov_value;
	float fov;
	float p_fov;

	const auto weapon = local_player->m_hActiveWeapon().Get();
	const auto target = reinterpret_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(best_target));

	if (g_Options.legit_resize_fov) {
		if (weapon->get_zoom_level() == 1)
			p_fov = 2.72f;
		else if (weapon->get_zoom_level() == 2)
			p_fov = 3.36f;
		else
			p_fov = 1.f;
	}
	else
		p_fov = 1.f;

	if (g_Options.legitbot_for_weapon[cur_weapon].legit_fov_after > 0 && local_player->m_iShotsFired() > 1 && (weapon->get_weapon_type() == WT_Rifle || weapon->get_weapon_type() == WT_Machinegun || weapon->get_weapon_type() == WT_Submg))
		fov_value = g_Options.legitbot_for_weapon[cur_weapon].legit_fov_after * fov_best_value;
	else {
		if (g_Options.legitbot_for_weapon[cur_weapon].legit_psilent_enable && g_Options.legitbot_for_weapon[cur_weapon].legit_psilent_type == 0 && local_player->m_iShotsFired() <= 1)
			fov_value = g_Options.legitbot_for_weapon[cur_weapon].legit_psilent_fov * fov_best_value;
		else
			fov_value = g_Options.legitbot_for_weapon[cur_weapon].legit_fov * fov_best_value;
	}

	if (g_Options.legitbot_for_weapon[cur_weapon].legit_dynamic_fov) {
		if (target)
			fov = (powf(fov_value + 30.f, 2.0f) / target->GetRenderOrigin().DistTo(local_player->GetRenderOrigin()) * p_fov);
		else
			fov = (powf(fov_value + 30.f, 2.0f) / 200.f * p_fov);
	}
	else
		fov = (powf(fov_value + 30.f, 2.0f) / 200.f * p_fov);

	return fov;
}

float legitbot::get_player_silent_fov(C_BasePlayer* local_player) {
	float fov_best_value = 10.f;
	float fov_value;
	float fov;

	const auto weapon = local_player->m_hActiveWeapon().Get();
	const auto target = reinterpret_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(best_target));

	if (g_Options.legit_resize_fov) {
		if (weapon->get_zoom_level() == 1)
			p_fov = 2.72f;
		else if (weapon->get_zoom_level() == 2)
			p_fov = 3.36f;
		else
			p_fov = 1.f;
	}
	else
		p_fov = 1.f;

	fov_value = g_Options.legitbot_for_weapon[cur_weapon].legit_psilent_fov * fov_best_value;

	if (g_Options.legitbot_for_weapon[cur_weapon].legit_dynamic_fov) {
		if (target)
			fov = (powf(fov_value + 30.f, 2.0f) / target->GetRenderOrigin().DistTo(local_player->GetRenderOrigin()) * p_fov);
		else
			fov = (powf(fov_value + 30.f, 2.0f) / 200.f * p_fov);
	}
	else
		fov = (powf(fov_value + 30.f, 2.0f) / 200.f * p_fov);

	return fov;
}

void legitbot::on_draw(C_BasePlayer* local_player) {

	if (!is_valid(local_player))
		return;

	int width, height;

	height = int(ImGui::GetIO().DisplaySize.y / 2);
	width = int(ImGui::GetIO().DisplaySize.x / 2);

	if (!g_Options.legit_enable)
		return;



	if (g_Options.visuals_draw_fov && get_player_fov(local_player) >= 1.f)
	{
		if (g_Options.fov_draw_type == 0)
		{
			Render::Get().RenderCircle(width, height, get_player_fov(local_player), int(get_player_fov(local_player) * 3), Color::Black, 1.5f);
			Render::Get().RenderCircle(width, height, get_player_fov(local_player), int(get_player_fov(local_player) * 3), Color(g_Options.fov_color), 1.0f);
		}
		else if (g_Options.fov_draw_type == 1)
		{
			Render::Get().RenderCircle(width, height, get_player_fov(local_player), int(get_player_fov(local_player) * 3), Color(g_Options.fov_color), 1.0f);
			Color cache_clr = Color(g_Options.fov_color);
			Render::Get().RenderCircleFilled(width, height, get_player_fov(local_player), int(get_player_fov(local_player) * 3), Color(cache_clr.r() / 255.f, cache_clr.g() / 255.f, cache_clr.b() / 255.f, 0.2f));
		}

	}
}

float legitbot::get_player_smooth(C_BasePlayer* local_player) {
	float smooth = g_Options.legitbot_for_weapon[cur_weapon].legit_smooth;

	const auto weapon = local_player->m_hActiveWeapon().Get();

	if (g_Options.legitbot_for_weapon[cur_weapon].legit_smooth_after > 0 && local_player->m_iShotsFired() > 1 && (weapon->get_weapon_type() == WT_Rifle || weapon->get_weapon_type() == WT_Machinegun || weapon->get_weapon_type() == WT_Submg))
		smooth = g_Options.legitbot_for_weapon[cur_weapon].legit_smooth_after;
	else
		smooth = g_Options.legitbot_for_weapon[cur_weapon].legit_smooth;

	return smooth;
}

void legitbot::get_player_rcs(C_BasePlayer* local_player, QAngle& angle, C_BasePlayer* entity) {
	int rcs_x = g_Options.legitbot_for_weapon[cur_weapon].legit_rcs_x;
	int rcs_y = g_Options.legitbot_for_weapon[cur_weapon].legit_rcs_y;

	if (rcs_x == 0 || rcs_y == 0)
		return;

	if (entity) {
		aimpunch = local_player->get_punch();
		angle.x -= aimpunch.x * (rcs_x / 50.f);
		angle.y -= aimpunch.y * (rcs_y / 50.f);
	}
}

bool legitbot::check_player_fov(Vector screen, float fov) {
	if (Vector(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2, 0).DistTo(screen) <= fov)
		return true;

	return false;
}

void legitbot::check_player_smooth(QAngle& view_angles, QAngle angles, QAngle& out_angles, float smoothing) {
	out_angles = angles;
	out_angles.Normalize();

	auto tickrate = 1.0f / g_GlobalVars->interval_per_tick;
	if (tickrate != 64.f)
		smoothing = tickrate * smoothing / 64.f;

	if (smoothing < 1.1f)
		return;

	Vector aim_vector;
	Math::AngleVectors(angles, aim_vector);

	Vector current_vector;
	Math::AngleVectors(view_angles, current_vector);

	auto delta = aim_vector - current_vector;
	const auto smoothed = current_vector + delta / smoothing;

	Math::VectorAngles(smoothed, out_angles);
	out_angles.Normalize();
}

int legitbot::get_best_target(C_BasePlayer* local_player) {
	float best_dist = 1000.f;
	int p_best_target = -1;

	for (int i = 0; i < g_EntityList->GetHighestEntityIndex(); i++) {
		const auto entity = reinterpret_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));

		if (entity && !entity->get_update(local_player)) {

			if (!g_Options.legitbot_for_weapon[cur_weapon].legit_teamattack && entity->is_teammate(local_player))
				continue;

			if (g_Options.legitbot_for_weapon[cur_weapon].legit_checks && !(entity->m_fFlags() & FL_ONGROUND))
				continue;

			Vector p_hitboxscreen;
			Vector p_hitbox = entity->get_bone_position(best_hitbox);

			if (g_Options.legitbot_for_weapon[cur_weapon].legit_autowall && !local_player->is_visible(entity, p_hitbox)) {

				float damage = autowall::can_hit(local_player, p_hitbox, entity);
				if (damage < g_Options.legitbot_for_weapon[cur_weapon].legit_mindamage)
					continue;
			}
			else if (!local_player->is_visible(entity, p_hitbox)) {
				continue;
			}

			if (Math::WorldToScreen(p_hitbox, p_hitboxscreen)) {
				Vector2D player_scr = Vector2D(p_hitboxscreen.x, p_hitboxscreen.y);
				float dist = (float)(sqrt(pow(player_scr.x - Vector2D(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2).x, 2) + pow(player_scr.y - Vector2D(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2).y, 2)));
				if (dist < best_dist) {
					best_dist = dist;
					p_best_target = i;
				}
			}

		}
	}

	return p_best_target;
}

int legitbot::get_best_hitbox(C_BasePlayer* local_player) {
	bool nearest = g_Options.legitbot_for_weapon[cur_weapon].legit_nearest;

	int p_best_hitbox = 5;
	float best_dist = 1000.f;

	std::vector<int> legit_hitboxes;
	if (g_Options.legitbot_for_weapon[cur_weapon].legit_hitbox[0])
	{
		legit_hitboxes.push_back(HITBOX_HEAD);
		legit_hitboxes.push_back(HITBOX_NECK);
	}
	if (g_Options.legitbot_for_weapon[cur_weapon].legit_hitbox[1])
	{
		legit_hitboxes.push_back(HITBOX_CHEST);
		legit_hitboxes.push_back(HITBOX_STOMACH);
		legit_hitboxes.push_back(HITBOX_UPPER_CHEST);

	}
	if (g_Options.legitbot_for_weapon[cur_weapon].legit_hitbox[2])
	{
		legit_hitboxes.push_back(HITBOX_LOWER_CHEST);
		legit_hitboxes.push_back(HITBOX_PELVIS);
	}

	best_hitboxes = legit_hitboxes;

	const auto weapon = local_player->m_hActiveWeapon();

	if (weapon->get_weapon_type() == WT_Rifle || weapon->get_weapon_type() == WT_Machinegun || weapon->get_weapon_type() == WT_Submg) {
		if (best_hitbox && in_attack && !target_change)
			p_best_hitbox = best_hitbox;
	}

	const auto entity = reinterpret_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(best_target));

	if (entity && !entity->get_update(local_player))
	{
		if (nearest)
		{
			for (int hitbox = 0; hitbox <= 6; hitbox++) {
				Vector p_hitbox = entity->get_bone_position(hitbox);
				Vector p_hitboxscreen;

				if (Math::WorldToScreen(p_hitbox, p_hitboxscreen)) {

					Vector2D hitbox_src = Vector2D(p_hitboxscreen.x, p_hitboxscreen.y);
					float dist = (float)(sqrt(pow(hitbox_src.x - Vector2D(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2).x, 2) + pow(hitbox_src.y - Vector2D(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2).y, 2)));

					if (dist < best_dist) {
						best_dist = dist;
						p_best_hitbox = hitbox;
					}
				}
			}
		}
		else if (legit_hitboxes.size() > 0)
		{
			for (int d = 0; d < legit_hitboxes.size(); d++) {
				Vector p_hitbox = entity->get_bone_position(legit_hitboxes.at(d));
				Vector p_hitboxscreen;

				if (Math::WorldToScreen(p_hitbox, p_hitboxscreen)) {

					Vector2D hitbox_src = Vector2D(p_hitboxscreen.x, p_hitboxscreen.y);
					float dist = (float)(sqrt(pow(hitbox_src.x - Vector2D(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2).x, 2) + pow(hitbox_src.y - Vector2D(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2).y, 2)));

					if (dist < best_dist) {
						best_dist = dist;
						p_best_hitbox = legit_hitboxes.at(d);
					}
				}
			}
		}


		Vector p_hitbox = entity->get_bone_position(p_best_hitbox);
		if (Math::WorldToScreen(p_hitbox, best_hitboxscreen)) {
			best_vector_hitbox = p_hitbox;
			return p_best_hitbox;
		}

	}

	return p_best_hitbox;
}

void legitbot::on_events(C_BasePlayer* local_player, IGameEvent* event) {

	if (!g_Options.legit_enable)
		return;

	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (!local_player || !local_player->IsAlive())
		return;

	int userid = g_EngineClient->GetPlayerForUserID(event->GetInt("userid"));
	int attacker = g_EngineClient->GetPlayerForUserID(event->GetInt("attacker"));

	if (attacker == g_EngineClient->GetLocalPlayer() && userid != g_EngineClient->GetLocalPlayer()) {
		if (g_Options.legitbot_for_weapon[cur_weapon].legit_delay_after_kill > 0.f)
			kill_delay_timer = g_GlobalVars->curtime + (g_Options.legitbot_for_weapon[cur_weapon].legit_delay_after_kill / 25.f);
	}

	if (!strcmp(event->GetName(), "game_newmap"))
		kill_delay_timer = 0.f;
}

bool legitbot::on_move(C_BasePlayer* local_player, CUserCmd* cmd) {

	if (!is_valid(local_player))
		return false;

	//if (g_Options.legit_enable)
	//	for (int i = 0; i < 11; i++)
	//		g_sdk.remove_rage[i] = true;

	const auto weapon = local_player->m_hActiveWeapon();

	can_shot = false;
	target_fov = false;
	target_silent_fov = false;

	in_attack = cmd->buttons & IN_ATTACK;

	best_pretarget = best_target;
	best_target = get_best_target(local_player);

	if (best_target == -1)
		return false;

	target_change = (best_pretarget != best_target && in_attack) ? true : false;
	best_hitbox = get_best_hitbox(local_player);

	if (best_hitbox == -1)
		return false;

	if (check_player_fov(best_hitboxscreen, get_player_fov(local_player)))
		target_fov = true;
	if (check_player_fov(best_hitboxscreen, get_player_silent_fov(local_player)))
		target_silent_fov = true;

	static bool p_in_attack = false;
	static bool p_check_in_attack = false;

	const auto entity = reinterpret_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(best_target));

	if (weapon->get_weapon_type() == WT_Rifle || weapon->get_weapon_type() == WT_Submg || weapon->get_weapon_type() == WT_Shotgun || weapon->get_weapon_type() == WT_Machinegun)
		p_check_in_attack = (in_attack && target_fov) ? true : false;
	else if (weapon->get_weapon_type() == WT_Sniper || weapon->get_weapon_type() == WT_Pistol) {

		if ((g_Options.legitbot_for_weapon[cur_weapon].legit_psilent_enable && g_Options.legitbot_for_weapon[cur_weapon].legit_psilent_type == 1 && (get_player_silent_fov(local_player) > get_player_fov(local_player)) ? target_silent_fov : target_fov) && !in_attack)
			p_in_attack = true;
		else if ((g_Options.legitbot_for_weapon[cur_weapon].legit_psilent_enable && g_Options.legitbot_for_weapon[cur_weapon].legit_psilent_type == 1 && (get_player_silent_fov(local_player) > get_player_fov(local_player)) ? !target_silent_fov : !target_fov) && in_attack)
			p_in_attack = false;
		else if ((g_Options.legitbot_for_weapon[cur_weapon].legit_psilent_enable && g_Options.legitbot_for_weapon[cur_weapon].legit_psilent_type == 1 && (get_player_silent_fov(local_player) > get_player_fov(local_player)) ? !target_silent_fov : !target_fov) && !in_attack)
			p_in_attack = false;

		p_check_in_attack = (in_attack && p_in_attack && (g_Options.legitbot_for_weapon[cur_weapon].legit_psilent_enable && g_Options.legitbot_for_weapon[cur_weapon].legit_psilent_type == 1 && (get_player_silent_fov(local_player) > get_player_fov(local_player)) ? target_silent_fov : target_fov)) ? true : false;
	}

	if (g_Options.legitbot_for_weapon[cur_weapon].legit_autofire ? (g_Options.legitbot_for_weapon[cur_weapon].legit_psilent_enable && g_Options.legitbot_for_weapon[cur_weapon].legit_psilent_type == 1 && (get_player_silent_fov(local_player) > get_player_fov(local_player)) ? target_silent_fov : target_fov) : (g_Options.legitbot_for_weapon[cur_weapon].legit_aimlock ? (g_Options.legitbot_for_weapon[cur_weapon].legit_psilent_enable && g_Options.legitbot_for_weapon[cur_weapon].legit_psilent_type == 1 && (get_player_silent_fov(local_player) > get_player_fov(local_player)) ? target_silent_fov : target_fov) : p_check_in_attack)) {

		QAngle angles, smoothed;

		Math::VectorAngles(best_vector_hitbox - local_player->get_eye_position(), angles);

		if (g_Options.legitbot_for_weapon[cur_weapon].legit_checks && Utils::line_goes_through_smoke(local_player->get_eye_position(), best_vector_hitbox))
			return false;

		if (g_Options.legitbot_for_weapon[cur_weapon].legit_checks && local_player->IsFlashed())
			return false;

		if (g_Options.legitbot_for_weapon[cur_weapon].legit_delay_after_kill > 0.f && kill_delay_timer > g_GlobalVars->curtime)
			return false;

		Math::normalize_angles(angles);
		Math::ClampAngles(angles);

		
		if (!g_Options.legitbot_for_weapon[cur_weapon].legit_psilent_enable && local_player->m_iShotsFired() < 1) {
			if (g_Options.legitbot_for_weapon[cur_weapon].legit_delay_shot > 0.f && !shot_delay_timer)
				shot_delay_timer = g_GlobalVars->curtime + (g_Options.legitbot_for_weapon[cur_weapon].legit_delay_shot / 25.f);

			QAngle angLocalAngles;
			g_EngineClient->GetViewAngles(&angLocalAngles);

			Vector vecDirection;
			Math::AngleVectors(angLocalAngles, vecDirection);

			const auto wpn_data = weapon->GetWeaponData();

			CGameTrace trace;
			g_EngineTrace->TraceLine(g_LocalPlayer->get_eye_position(), g_LocalPlayer->get_eye_position() + (vecDirection * wpn_data->m_flRange), MASK_SHOT_HULL | CONTENTS_HITBOX, g_LocalPlayer, NULL, &trace);

			const float damage = autowall::can_hit(g_LocalPlayer, trace.endpos, (C_BasePlayer*)trace.hit_entity);

			if (
				shot_delay_timer <= g_GlobalVars->curtime
				|| 
				(trace.fraction != 1.0f && (C_BasePlayer*)trace.hit_entity == entity)
				&&  (
						(!local_player->is_visible(entity, trace.endpos) && g_Options.legitbot_for_weapon[cur_weapon].legit_autowall && damage >= g_Options.legitbot_for_weapon[cur_weapon].legit_mindamage)
						||
						(local_player->is_visible(entity, trace.endpos))
					)
				&&  (
						(	
							(!g_Options.legitbot_for_weapon[cur_weapon].legit_nearest && valid_hitgroup(trace.hitbox) && best_hitboxes.size() > 0) 
							|| 
							(!g_Options.legitbot_for_weapon[cur_weapon].legit_nearest && best_hitboxes.size() == 0)
						)
						|| 
						g_Options.legitbot_for_weapon[cur_weapon].legit_nearest
					) 
				)
				shot_delay_timer = 0.f;

			if (shot_delay_timer)
				cmd->buttons &= ~IN_ATTACK;
		}

		if (local_player->m_iShotsFired() > 1)
			get_player_rcs(local_player, angles, entity);

		float smooth = get_player_smooth(local_player);

		if (g_Options.legitbot_for_weapon[cur_weapon].legit_psilent_enable && g_Options.legitbot_for_weapon[cur_weapon].legit_psilent_type == 1) {
			if (!target_silent_fov) {
				check_player_smooth(cmd->viewangles, angles, smoothed, smooth);
				Math::ClampAngles(smoothed);
				Math::normalize_angles(smoothed);

				cmd->viewangles = smoothed;

				g_EngineClient->SetViewAngles(smoothed);
			}
			else {
				if (local_player->m_iShotsFired() > 1 && weapon->get_weapon_type() == WT_Rifle || weapon->get_weapon_type() == WT_Submg || weapon->get_weapon_type() == WT_Shotgun)
					get_player_rcs(local_player, angles, entity);

				check_player_smooth(cmd->viewangles, angles, smoothed, 0.f);
				Math::ClampAngles(smoothed);
				Math::normalize_angles(smoothed);

				cmd->viewangles = smoothed;
			}
		}
		else if (g_Options.legitbot_for_weapon[cur_weapon].legit_psilent_enable && g_Options.legitbot_for_weapon[cur_weapon].legit_psilent_type == 0 && local_player->m_iShotsFired() <= 1) {
			check_player_smooth(cmd->viewangles, angles, smoothed, 0.f);
			Math::ClampAngles(smoothed);
			Math::normalize_angles(smoothed);

			cmd->viewangles = smoothed;
		}
		else {
			check_player_smooth(cmd->viewangles, angles, smoothed, smooth);
			Math::ClampAngles(smoothed);
			Math::normalize_angles(smoothed);

			cmd->viewangles = smoothed;

			g_EngineClient->SetViewAngles(cmd->viewangles);

		}

		/*if (entity && g_Options.legitbot_for_weapon[cur_weapon].legit_autofire) {
			if (!weapon->get_weapon_data()->full_auto) {
				if (cmd->command_number % 2 == 0) {
					cmd->buttons &= ~IN_ATTACK;
				}
				else {
					cmd->buttons |= IN_ATTACK;
				}
			}
			else {
				cmd->buttons |= IN_ATTACK;
			}
		}*/

	}

	return true;
}
