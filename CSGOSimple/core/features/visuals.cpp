#pragma warning (disable: 4018)

#include "visuals.hpp"
#include "grenades.h"

RECT get_bbox(C_BaseEntity* ent)
{
	RECT aRect{ };

	auto pCollideable = ent->GetCollideable();
	if (!pCollideable)
		return aRect;

	auto min = pCollideable->OBBMins();
	auto max = pCollideable->OBBMaxs();

	const matrix3x4_t& trans = ent->m_rgflCoordinateFrame();
	Vector points[] = {
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector aPointsTransformed[8];
	for (int i = 0; i < 8; i++)
		Math::VectorTransform(points[i], trans, aPointsTransformed[i]);

	Vector aScreenPoints[8] = { };
	for (int i = 0; i < 8; i++)
		if (g_DebugOverlay->ScreenPosition(aPointsTransformed[i], aScreenPoints[i]))
			return aRect;

	auto flLeft = aScreenPoints[0].x;
	auto flTop = aScreenPoints[0].y;
	auto flRight = aScreenPoints[0].x;
	auto flBottom = aScreenPoints[0].y;

	for (int i = 1; i < 8; i++)
	{
		if (flLeft > aScreenPoints[i].x)
			flLeft = aScreenPoints[i].x;
		if (flTop < aScreenPoints[i].y)
			flTop = aScreenPoints[i].y;
		if (flRight < aScreenPoints[i].x)
			flRight = aScreenPoints[i].x;
		if (flBottom > aScreenPoints[i].y)
			flBottom = aScreenPoints[i].y;
	}

	return RECT{ (long)(flLeft), (long)(flBottom), (long)(flRight), (long)(flTop) };
}

void visuals::on_events_damage_indicator(IGameEvent* event) {

	if (!g_Options.misc_damage_indicator)
		return;

	const int hitted_id = g_EngineClient->GetPlayerForUserID(event->GetInt("userid"));
	const int attacker_id = g_EngineClient->GetPlayerForUserID(event->GetInt("attacker"));

	if (hitted_id != g_EngineClient->GetLocalPlayer() && attacker_id == g_EngineClient->GetLocalPlayer()) {
		const int hp = event->GetInt("dmg_health");
		const auto entity = reinterpret_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(hitted_id));

		const damage_indicator hit_log(hp, g_GlobalVars->curtime + 2.f, entity->get_bone_position(HITBOX_HEAD));
		damage_indicator_log.push_back(hit_log);
	}
}

void visuals::grenades()
{
	if (!g_LocalPlayer)
		return;

	if (!g_Options.grenade_prediction)
		return;

	static auto last_server_tick = g_ClientState->m_ClockDriftMgr.m_nServerTick;
	if (last_server_tick != g_ClientState->m_ClockDriftMgr.m_nServerTick) {
		g_GrenadePrediction->get_list().clear();

		last_server_tick = g_ClientState->m_ClockDriftMgr.m_nServerTick;
	}

	for (int32_t i = 1; i < g_EntityList->GetHighestEntityIndex(); i++)
	{
		C_BaseEntity* pBaseEntity = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));
		if (!pBaseEntity || pBaseEntity->IsDormant())
			continue;

		const auto client_class = pBaseEntity->GetClientClass();

		if (!client_class
			|| client_class->m_ClassID != 114 && client_class->m_ClassID != classids::get("CBaseCSGrenadeProjectile"))
			continue;

		if (client_class->m_ClassID == classids::get("CBaseCSGrenadeProjectile")) {
			const auto model = pBaseEntity->GetModel();
			if (!model)
				continue;

			const auto studio_model = g_MdlInfo->GetStudiomodel(model);
			if (!studio_model
				|| std::string_view(studio_model->szName).find("fraggrenade") == std::string::npos)
				continue;
		}

		const auto handle = pBaseEntity->GetRefEHandle().ToLong();
		if (pBaseEntity->m_nExplodeEffectTickBegin())
		{
			g_GrenadePrediction->get_list().erase(handle);
			continue;
		}

		if (g_GrenadePrediction->get_list().find(handle) == g_GrenadePrediction->get_list().end()) {
			float_t flSpawnTime = *(float_t*)((DWORD)(pBaseEntity)+0x2D8);

			g_GrenadePrediction->get_list()[handle] =
				C_GrenadePrediction::data_t
				(
					reinterpret_cast<C_BaseCombatWeapon*>(pBaseEntity)->m_hThrower().Get(),
					client_class->m_ClassID == 114 ? WEAPON_MOLOTOV : WEAPON_HEGRENADE,
					pBaseEntity->m_vecOrigin(),
					reinterpret_cast<C_BasePlayer*>(pBaseEntity)->m_vecVelocity(),
					flSpawnTime,
					TIME_TO_TICKS(reinterpret_cast<C_BasePlayer*>(pBaseEntity)->m_flSimulationTime() - flSpawnTime)
				);
		}

		if (g_GrenadePrediction->get_list().at(handle).draw())
			continue;

		g_GrenadePrediction->get_list().erase(handle);
	}

	g_GrenadePrediction->get_local_data().draw();
}

void visuals::draw_damage_indicator(C_BasePlayer* local_player) {

	if (!g_Options.misc_damage_indicator)
		return;

	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (!local_player || !local_player->IsAlive())
		return;

	for (size_t id = 0; id < damage_indicator_log.size(); id++) {

		if (damage_indicator_log.at(id).bone.IsZero() || damage_indicator_log.at(id).time <= g_GlobalVars->curtime) {
			damage_indicator_log.erase(damage_indicator_log.begin() + id);
			continue;
		}

		if (!damage_indicator_log.at(id).bone.IsZero()) {
			damage_indicator_log.at(id).bone.z -= (g_Options.damage_indicator_speed * (g_GlobalVars->curtime - damage_indicator_log.at(id).time));


			if (damage_indicator_log.at(id).time - g_GlobalVars->curtime < 1.5f)
				damage_indicator_log.at(id).alpha /= 1.2f;

			Color def_clr = Color(g_Options.color_damage_indicator);
			Color clr = Color(def_clr.r() / 255.f, def_clr.g() / 255.f, def_clr.b() / 255.f, damage_indicator_log.at(id).alpha / 255.f);

			Vector out;
			if (Math::WorldToScreen(damage_indicator_log.at(id).bone, out) && damage_indicator_log.at(id).time > g_GlobalVars->curtime)
				Render::Get().RenderText(("- " + std::to_string(damage_indicator_log.at(id).hp)).c_str(), static_cast<int>(out.x), static_cast<int>(out.y), g_Options.damage_indicator_size, clr,  false, true, g_pSecondFont);
		}
	}
}

void visuals::render_sounds()
{
	for (int i = 0; i < g_cheat.sounds_info.size(); i++)
	{
		if (g_cheat.sounds_info.at(i).time + 2 < g_GlobalVars->realtime)
			g_cheat.sounds_info.at(i).alpha -= g_GlobalVars->frametime;

		if (g_cheat.sounds_info.at(i).alpha <= 0.0f)
			continue;

		float deltaTime = g_GlobalVars->realtime - g_cheat.sounds_info.at(i).time;

		auto factor = deltaTime / 2;
		if (factor > 1.0f)
			factor = 1.0f;

		float radius = 15 * factor;
		Color color = Color(g_cheat.sounds_info.at(i).clr.r() / 255.f, g_cheat.sounds_info.at(i).clr.g() / 255.f, g_cheat.sounds_info.at(i).clr.b() / 255.f, g_cheat.sounds_info.at(i).alpha);
		Render::Get().render_circle_3d(g_cheat.sounds_info.at(i).pos, 15, radius, color, 1.f);
	}
	for (int i = 0; i < g_cheat.sounds_info.size(); i++)
	{
		if (g_cheat.sounds_info.at(i).alpha <= 0.0f)
			g_cheat.sounds_info.erase(g_cheat.sounds_info.begin() + i);
	}
}

void visuals::cache_dormant_players()
{
	g_EngineSound->GetActiveSounds(current_sound_data);
	if (!current_sound_data.Count())
		return;

	for (auto i = 0; i < current_sound_data.Count(); i++)
	{
		auto& cur_sound = current_sound_data[i];
		if (cur_sound.sound_source < 1 || cur_sound.sound_source > g_GlobalVars->maxClients)
			continue;

		if (cur_sound.origin->IsZero() || !is_sound_valid(cur_sound))
			continue;

		C_BasePlayer* player = (C_BasePlayer*)g_EntityList->GetClientEntity(cur_sound.sound_source);
		if (!player || !player->IsAlive() || !player->IsDormant())
			continue;

		Vector src3D, dst3D;
		trace_t tr;

		src3D = *cur_sound.origin + Vector(0.0f, 0.0f, 1.0f);
		dst3D = src3D - Vector(0.0f, 0.0f, 100.0f);

		g_EngineTrace->TraceLine(src3D, dst3D, MASK_PLAYERSOLID, player, 0, &tr);
		if (tr.allsolid)
			dormant_players[cur_sound.sound_source].receive_time = -1;

		*cur_sound.origin = tr.fraction <= 0.97f ? tr.endpos : *cur_sound.origin;

		dormant_players[cur_sound.sound_source].origin = *cur_sound.origin;
		dormant_players[cur_sound.sound_source].receive_time = g_GlobalVars->realtime;
	}

	cached_sound_data = current_sound_data;
}

bool visuals::force_dormant(int index)
{
	C_BasePlayer* player = (C_BasePlayer*)g_EntityList->GetClientEntity(index);
	if (!player || !player->IsAlive() || !player->IsDormant() || player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
	{
		dormant_players[player->EntIndex()].dormant = 0.0f;
		dormant_players[player->EntIndex()].receive_time = 0.0f;

		return false;
	}

	if (dormant_players[player->EntIndex()].dormant > dormant_players[player->EntIndex()].receive_time)
	{
		float since_dormant = g_GlobalVars->realtime - dormant_players[player->EntIndex()].dormant;
		if (since_dormant < 10.0f)
			player->set_abs_origin(dormant_players[player->EntIndex()].last_origin);
		else if (g_GlobalVars->realtime - dormant_players[player->EntIndex()].receive_time < 10.0f)
			player->set_abs_origin(dormant_players[player->EntIndex()].origin);

		return true;
	}

	if (g_GlobalVars->realtime - dormant_players[player->EntIndex()].receive_time > 10.0f)
	{
		float since_dormant = g_GlobalVars->realtime - dormant_players[player->EntIndex()].dormant;
		if (since_dormant < 10.0f)
		{
			player->set_abs_origin(dormant_players[player->EntIndex()].last_origin);
			return true;
		}

		return false;
	}

	player->set_abs_origin(dormant_players[player->EntIndex()].origin);
	return true;
}

bool visuals::is_sound_valid(SndInfo_t sound)
{
	for (int i = 0; i < cached_sound_data.Count(); i++)
		if (sound.guid == cached_sound_data[i].guid)
			return false;

	return true;
}

void visuals::reset_data()
{
	cached_sound_data.RemoveAll();
	current_sound_data.RemoveAll();
	dormant_players = { };
}

void visuals::draw()
{
	current_sound_data.RemoveAll();
	if (!g_LocalPlayer)
		return;

	cache_dormant_players();
	for (int i = 1; i < g_GlobalVars->maxClients; ++i)
	{
		C_BasePlayer* player = C_BasePlayer::GetPlayerByIndex(i);

		if (!player || !player->IsAlive() || !player->IsPlayer())
		{
			player_data[i].prev_ammo = 0.0f;
			player_data[i].prev_health = 0.0f;

			dormant_players[i].dormant = 0.0f;
			dormant_players[i].receive_time = 0.0f;
			dormant_players[i].origin = Vector(0, 0, 0);

			continue;
		}

		if (player->IsDormant())
		{
			if (!force_dormant(i))
				continue;
		}
		c_esp_settings settings;
		settings = g_Options.esp_settings[0];
		if (player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
		{
			settings = g_Options.esp_settings[1];
			if (player == g_LocalPlayer)
				settings = g_Options.esp_settings[2];
		}

		dormant_players[i].dormant = g_GlobalVars->realtime;
		dormant_players[i].last_origin = player->GetAbsOrigin();

		int scr_size_x = 0;
		int scr_size_y = 0;

		g_EngineClient->GetScreenSize(scr_size_x, scr_size_y);
		if (player == g_LocalPlayer)
		{
			if (!g_Input->m_fCameraInThirdPerson)
				continue;
		}
		else
		{
			Vector vec_to_screen = Vector(0, 0, 0);
			if (g_DebugOverlay->ScreenPosition(player->GetAbsOrigin(), vec_to_screen))
			{
				render_oof_arrows(settings, player);
				continue;
			}

			if (vec_to_screen.x < 0 || vec_to_screen.x > scr_size_x || vec_to_screen.y < 0 || vec_to_screen.y > scr_size_y)
				continue;
		}

		player_data[player->EntIndex()].bbox = get_bbox(player);
		player_data[player->EntIndex()].width = abs(player_data[player->EntIndex()].bbox.right - player_data[player->EntIndex()].bbox.left) * 0.8f;
		player_data[player->EntIndex()].height = abs(player_data[player->EntIndex()].bbox.top - player_data[player->EntIndex()].bbox.bottom);

		auto slow = g_GlobalVars->frametime * 3;
		const bool is_visible = g_LocalPlayer->is_visible_with_smoke_check(player, HITBOX_CHEST);
		const bool use_dormant = settings.dormant;

		if (player->IsDormant() && use_dormant)
		{
			player->m_alpha -= g_GlobalVars->frametime * 400;
			player->m_alpha = Math::clamp(player->m_alpha, 50.f, 255.f);
		}
		else
		{
			player->m_alpha += g_GlobalVars->frametime * 400;
			player->m_alpha = Math::clamp(player->m_alpha, 0.f, 255.f);
		}

		manage_flags(player);

		/*
		right_table.table_content.clear();
		right_table.table_content.push_back(scoped_text_struct);
		right_table.table_content.push_back(defuser_kit_text_struct);
		right_table.table_content.push_back(flashed_text_struct);
		right_table.table_content.push_back(money_text_struct);
		right_table.table_content.push_back(weapon_icon_text_struct);
		right_table.table_content.push_back(weapon_text_struct);
		right_table.table_content.push_back(armor_text_struct);
		right_table.table_content.push_back(ammo_text_struct);
		right_table.table_content.push_back(name_text_struct);
		right_table.table_content.push_back(health_text_struct);
		right_table.table_content.push_back(reloading_text_struct);

		left_table.table_content.clear();
		left_table.table_content.push_back(scoped_text_struct);
		left_table.table_content.push_back(defuser_kit_text_struct);
		left_table.table_content.push_back(flashed_text_struct);
		left_table.table_content.push_back(money_text_struct);
		left_table.table_content.push_back(weapon_icon_text_struct);
		left_table.table_content.push_back(weapon_text_struct);
		left_table.table_content.push_back(armor_text_struct);
		left_table.table_content.push_back(ammo_text_struct);
		left_table.table_content.push_back(name_text_struct);
		left_table.table_content.push_back(health_text_struct);
		left_table.table_content.push_back(reloading_text_struct);

		top_table.table_content.clear();
		top_table.table_content.push_back(scoped_text_struct);
		top_table.table_content.push_back(defuser_kit_text_struct);
		top_table.table_content.push_back(flashed_text_struct);
		top_table.table_content.push_back(money_text_struct);
		top_table.table_content.push_back(weapon_icon_text_struct);
		top_table.table_content.push_back(weapon_text_struct);
		top_table.table_content.push_back(armor_text_struct);
		top_table.table_content.push_back(ammo_text_struct);
		top_table.table_content.push_back(name_text_struct);
		top_table.table_content.push_back(health_text_struct);
		top_table.table_content.push_back(reloading_text_struct);

		down_table.table_content.clear();
		down_table.table_content.push_back(scoped_text_struct);
		down_table.table_content.push_back(defuser_kit_text_struct);
		down_table.table_content.push_back(flashed_text_struct);
		down_table.table_content.push_back(money_text_struct);
		down_table.table_content.push_back(weapon_icon_text_struct);
		down_table.table_content.push_back(weapon_text_struct);
		down_table.table_content.push_back(armor_text_struct);
		down_table.table_content.push_back(ammo_text_struct);
		down_table.table_content.push_back(name_text_struct);
		down_table.table_content.push_back(health_text_struct);
		down_table.table_content.push_back(reloading_text_struct);
		*/

		right_table.table_content.clear();
		right_table.table_content.push_back(money_text_struct);
		right_table.table_content.push_back(scoped_text_struct);
		right_table.table_content.push_back(defuser_kit_text_struct);
		right_table.table_content.push_back(reloading_text_struct);
		right_table.table_content.push_back(flashed_text_struct);
		right_table.table_content.push_back(armor_text_struct);
		right_table.table_content.push_back(ammo_text_struct);


		left_table.table_content.clear();
		left_table.table_content.push_back(health_text_struct);
		left_table.table_content.push_back(armor_text_struct);


		top_table.table_content.clear();
		top_table.table_content.push_back(name_text_struct);

		down_table.table_content.clear();
		down_table.table_content.push_back(weapon_text_struct);
		down_table.table_content.push_back(weapon_icon_text_struct);

		if (settings.enable)
		{
			render_box(settings, player, is_visible);
			render_health(settings, player, is_visible);
			skeleton(settings, player, is_visible);

			render_right_table(settings, player, is_visible);
			render_left_table(settings, player, is_visible);
			render_top_table(settings, player, is_visible);
			render_bottom_table(settings, player, is_visible);

			render_sounds();
		}
	}
}

void visuals::manage_flags(C_BasePlayer* player)
{
	int idx = player->m_iTeamNum() != g_LocalPlayer->m_iTeamNum() ? 0 : player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum() && g_LocalPlayer != player ? 1 : 2;
	c_esp_settings settings = g_Options.esp_settings[idx];
	player_info_t info;
	info = player->GetPlayerInfo();

	name_text_struct.content = info.szName;
	flashed_text_struct.content = player->IsFlashed() ? "FLASHED" : "";
	health_text_struct.content = std::to_string(player->m_iHealth()).c_str();
	armor_text_struct.content = std::to_string(player->m_ArmorValue()).c_str();
	scoped_text_struct.content = player->m_bIsScoped() ? "SCOPED" : "";
	money_text_struct.content = std::to_string(player->m_iAccountID()).c_str();
	money_text_struct.content.append("$");
	defuser_kit_text_struct.content = player->m_bHasDefuser() ? "DEFUSER" : "";

	if (player->m_hActiveWeapon().Get())
	{
		ammo_text_struct.content = std::to_string(player->m_hActiveWeapon().Get()->m_iClip1()).c_str();
		weapon_icon_text_struct.content = player->m_hActiveWeapon().Get()->get_weapon_icon();
		weapon_icon_text_struct.content.append("_wi_");
		weapon_text_struct.content = player->m_hActiveWeapon().Get()->get_weapon();
		weapon_text_struct.content.append("_w_");
		reloading_text_struct.content = player->m_hActiveWeapon().Get()->IsReloading() ? "RELOADING" : "";
	}

	fakeduck_text_struct.content = "";

	name_text_struct.clr = Color(settings.names_clr);
	flashed_text_struct.clr = Color(settings.flashed_clr);
	health_text_struct.clr = Color(settings.health_clr);
	armor_text_struct.clr = Color(settings.armor_clr);
	scoped_text_struct.clr = Color(settings.scoped_clr);
	money_text_struct.clr = Color(settings.money_clr);
	ammo_text_struct.clr = Color(settings.ammo_clr);
	defuser_kit_text_struct.clr = Color(settings.defuser_clr);
	fakeduck_text_struct.clr = Color(settings.fakeduck_clr);
	weapon_icon_text_struct.clr = Color(settings.weapon_ico_clr);
	weapon_text_struct.clr = Color(settings.weapon_clr);
	reloading_text_struct.clr = Color(settings.reloading_clr);

	name_text_struct.font_size = settings.esp_names_size;
	flashed_text_struct.font_size = settings.esp_flashed_size;
	health_text_struct.font_size = settings.esp_health_size;
	armor_text_struct.font_size = settings.esp_armor_size;
	scoped_text_struct.font_size = settings.esp_scoped_size;
	money_text_struct.font_size = settings.esp_money_size;
	ammo_text_struct.font_size = settings.esp_ammo_size;
	defuser_kit_text_struct.font_size = settings.esp_defuser_size;
	fakeduck_text_struct.font_size = settings.esp_fakeduck_size;
	weapon_icon_text_struct.font_size = settings.esp_weapon_ico_size;
	weapon_text_struct.font_size = settings.esp_weapon_size;
	reloading_text_struct.font_size = settings.esp_reloading_size;

	name_text_struct.enabled = settings.esp_names;
	flashed_text_struct.enabled = settings.esp_flashed;
	health_text_struct.enabled = settings.esp_health_text;
	armor_text_struct.enabled = settings.esp_armor_text;
	scoped_text_struct.enabled = settings.esp_scoped;
	money_text_struct.enabled = settings.esp_money;
	ammo_text_struct.enabled = settings.esp_ammo_text;
	defuser_kit_text_struct.enabled = settings.esp_defuser;
	fakeduck_text_struct.enabled = settings.esp_fakeduck;
	weapon_icon_text_struct.enabled = settings.esp_weapon_ico;
	weapon_text_struct.enabled = settings.esp_weapon;
	reloading_text_struct.enabled = settings.esp_reloading;
}

void visuals::render_box(c_esp_settings settings, C_BasePlayer* player, bool visible)
{
	if (!settings.esp_box || (!visible && settings.only_visible && player != g_LocalPlayer))
		return;

	Color clr = player != g_LocalPlayer ?
		visible ?
		Color(settings.box_clr_vis[0], settings.box_clr_vis[1], settings.box_clr_vis[2], player->m_alpha / 255.f)
		:
		Color(settings.box_clr_invis[0], settings.box_clr_invis[1], settings.box_clr_invis[2], player->m_alpha / 255.f)
		:
		Color(settings.box_clr_vis[0], settings.box_clr_vis[1], settings.box_clr_vis[2], player->m_alpha / 255.f);
	if (settings.gradient_fade)
	{
		Render::Get().render_rect_filled_multicolor(player_data[player->EntIndex()].bbox.left, player_data[player->EntIndex()].bbox.top, player_data[player->EntIndex()].bbox.right, player_data[player->EntIndex()].bbox.bottom,
			Color(0, 0, 0, 0),
			Color(clr.r() / 255.f, clr.g() / 255.f, clr.b() / 255.f,
				player->m_alpha /
				255.f / 2.f), Color(0, 0, 0, 0),
			Color(clr.r() / 255.f, clr.g() / 255.f, clr.b() / 255.f,
				player->m_alpha /
				255.f / 2.f));
	}
	else if (settings.filled_box)
	{
		Render::Get().RenderBoxFilled(player_data[player->EntIndex()].bbox.left, player_data[player->EntIndex()].bbox.top, player_data[player->EntIndex()].bbox.right, player_data[player->EntIndex()].bbox.bottom,
			Color(clr.r() / 255.f, clr.g() / 255.f, clr.b() / 255.f,
				player->m_alpha /
				255.f / 2.f),
			0);
	}

	Render::Get().RenderBox(player_data[player->EntIndex()].bbox.left, player_data[player->EntIndex()].bbox.top, player_data[player->EntIndex()].bbox.right, player_data[player->EntIndex()].bbox.bottom, Color(0.f, 0.f, 0.f, player->m_alpha / 255.f), 2.f);
	Render::Get().RenderBox(player_data[player->EntIndex()].bbox.left, player_data[player->EntIndex()].bbox.top, player_data[player->EntIndex()].bbox.right, player_data[player->EntIndex()].bbox.bottom, clr, 1.f);
}

void visuals::render_health(c_esp_settings settings, C_BasePlayer* player, bool visible)
{
	if (!settings.esp_health || (!visible && settings.only_visible && player != g_LocalPlayer))
		return;

	float box_height = static_cast<float>(player_data[player->EntIndex()].bbox.bottom - player_data[player->EntIndex()].bbox.top);

	Color clr = Color(settings.health_bar_clr[0], settings.health_bar_clr[1], settings.health_bar_clr[2], player->m_alpha / 255.f);
	float clr_bar_height = ((box_height * min(player->m_iHealth(), 100)) / 100.0f);
	float clr_bar_max_height = ((box_height * 100.0f) / 100.0f);

	Render::Get().RenderBoxFilled(
		player_data[player->EntIndex()].bbox.left - 7.0f,
		player_data[player->EntIndex()].bbox.top - 1.f,
		player_data[player->EntIndex()].bbox.left - 2.0f,
		player_data[player->EntIndex()].bbox.top + clr_bar_max_height + 1,
		Color(0.f, 0.f, 0.f, player->m_alpha * 0.7 / 255.f), 10.f);
	Render::Get().RenderBoxFilled(
		player_data[player->EntIndex()].bbox.left - 6.0f,
		player_data[player->EntIndex()].bbox.top + (clr_bar_max_height - clr_bar_height),
		player_data[player->EntIndex()].bbox.left - 3.0f,
		player_data[player->EntIndex()].bbox.top + clr_bar_max_height, clr, 10.f);
}

void visuals::render_right_table(c_esp_settings settings, C_BasePlayer* player, bool visible)
{
	if (!visible && settings.only_visible && player != g_LocalPlayer)
		return;

	float spacing_x = 0;
	float spacing_y = 0;

	if (settings.esp_health && (settings.esp_health_pos == bar_pos::right_1 || settings.esp_health_pos == bar_pos::right_2))
		spacing_x += 5.f;

	if (settings.esp_armor && (settings.esp_armor_pos == bar_pos::right_1 || settings.esp_armor_pos == bar_pos::right_2))
		spacing_x += 5.f;

	for (int i = 0; i < right_table.table_content.size(); i++)
	{
		if (right_table.table_content.at(i).content != "" && right_table.table_content.at(i).enabled)
		{
			if (!std::strstr(right_table.table_content.at(i).content.c_str(), "_wi_") && !std::strstr(right_table.table_content.at(i).content.c_str(), "_w_"))
			{
				Render::Get().RenderText(right_table.table_content.at(i).content.c_str(), ImVec2(player_data[player->EntIndex()].bbox.right + 3 + spacing_x, player_data[player->EntIndex()].bbox.top + spacing_y), right_table.table_content.at(i).font_size, Color(right_table.table_content.at(i).clr.r() / 255.f, right_table.table_content.at(i).clr.g() / 255.f, right_table.table_content.at(i).clr.b() / 255.f, player->m_alpha / 255.f), false, true, g_pSecondFont);
				spacing_y += right_table.table_content.at(i).font_size + 2.f;
			}
			else if (std::strstr(right_table.table_content.at(i).content.c_str(), "_wi_"))
			{
				std::size_t pos = right_table.table_content.at(i).content.find("_wi_");
				std::string res_str = right_table.table_content.at(i).content.substr(0, pos);
				Render::Get().RenderText(res_str.c_str(), ImVec2(player_data[player->EntIndex()].bbox.right + 3 + spacing_x, player_data[player->EntIndex()].bbox.top + spacing_y), right_table.table_content.at(i).font_size, Color(right_table.table_content.at(i).clr.r() / 255.f, right_table.table_content.at(i).clr.g() / 255.f, right_table.table_content.at(i).clr.b() / 255.f, player->m_alpha / 255.f), false, true, weapon_icons);
				spacing_y += right_table.table_content.at(i).font_size + 2.f;
			}
			else if (std::strstr(right_table.table_content.at(i).content.c_str(), "_w_"))
			{
				std::size_t pos = right_table.table_content.at(i).content.find("_w_");
				std::string res_str = right_table.table_content.at(i).content.substr(0, pos);
				Render::Get().RenderText(res_str.c_str(), ImVec2(player_data[player->EntIndex()].bbox.right + 3 + spacing_x, player_data[player->EntIndex()].bbox.top + spacing_y), right_table.table_content.at(i).font_size, Color(right_table.table_content.at(i).clr.r() / 255.f, right_table.table_content.at(i).clr.g() / 255.f, right_table.table_content.at(i).clr.b() / 255.f, player->m_alpha / 255.f), false, true, g_pSecondFont);
				spacing_y += right_table.table_content.at(i).font_size + 2.f;
			}
		}
	}
}

void visuals::render_left_table(c_esp_settings settings, C_BasePlayer* player, bool visible)
{
	if (!visible && settings.only_visible && player != g_LocalPlayer)
		return;

	float spacing_x = 0;
	float spacing_y = 0;

	if (settings.esp_health && (settings.esp_health_pos == bar_pos::left_1 || settings.esp_health_pos == bar_pos::left_2))
		spacing_x += 5.f;

	if (settings.esp_armor && (settings.esp_armor_pos == bar_pos::left_1 || settings.esp_armor_pos == bar_pos::left_2))
		spacing_x += 5.f;

	for (int i = 0; i < left_table.table_content.size(); i++)
	{
		if (left_table.table_content.at(i).content != "" && left_table.table_content.at(i).enabled)
		{
			if (!std::strstr(left_table.table_content.at(i).content.c_str(), "_wi_") && !std::strstr(left_table.table_content.at(i).content.c_str(), "_w_"))
			{
				auto sz = g_pSecondFont->CalcTextSizeA(left_table.table_content.at(i).font_size, FLT_MAX, 0.0f, left_table.table_content.at(i).content.c_str());
				Render::Get().RenderText(left_table.table_content.at(i).content.c_str(), ImVec2(player_data[player->EntIndex()].bbox.left - 3 - spacing_x - sz.x, player_data[player->EntIndex()].bbox.top + spacing_y), left_table.table_content.at(i).font_size, Color(left_table.table_content.at(i).clr.r() / 255.f, left_table.table_content.at(i).clr.g() / 255.f, left_table.table_content.at(i).clr.b() / 255.f, player->m_alpha / 255.f), false, true, g_pSecondFont);
				spacing_y += left_table.table_content.at(i).font_size + 2.f;
			}
			else if (std::strstr(left_table.table_content.at(i).content.c_str(), "_wi_"))
			{
				std::size_t pos = left_table.table_content.at(i).content.find("_wi_");
				std::string res_str = left_table.table_content.at(i).content.substr(0, pos);

				auto sz = weapon_icons->CalcTextSizeA(left_table.table_content.at(i).font_size, FLT_MAX, 0.0f, res_str.c_str());
				Render::Get().RenderText(res_str.c_str(), ImVec2(player_data[player->EntIndex()].bbox.left - 3 - spacing_x - sz.x, player_data[player->EntIndex()].bbox.top + spacing_y), left_table.table_content.at(i).font_size, Color(left_table.table_content.at(i).clr.r() / 255.f, left_table.table_content.at(i).clr.g() / 255.f, left_table.table_content.at(i).clr.b() / 255.f, player->m_alpha / 255.f), false, true, weapon_icons);
				spacing_y += left_table.table_content.at(i).font_size + 2.f;

			}
			else if (std::strstr(left_table.table_content.at(i).content.c_str(), "_w_"))
			{
				std::size_t pos = left_table.table_content.at(i).content.find("_w_");
				std::string res_str = left_table.table_content.at(i).content.substr(0, pos);

				auto sz = g_pSecondFont->CalcTextSizeA(left_table.table_content.at(i).font_size, FLT_MAX, 0.0f, res_str.c_str());
				Render::Get().RenderText(res_str.c_str(), ImVec2(player_data[player->EntIndex()].bbox.left - 3 - spacing_x - sz.x, player_data[player->EntIndex()].bbox.top + spacing_y), left_table.table_content.at(i).font_size, Color(left_table.table_content.at(i).clr.r() / 255.f, left_table.table_content.at(i).clr.g() / 255.f, left_table.table_content.at(i).clr.b() / 255.f, player->m_alpha / 255.f), false, true, g_pSecondFont);
				spacing_y += left_table.table_content.at(i).font_size + 2.f;
			}
		}
	}
}

void visuals::render_top_table(c_esp_settings settings, C_BasePlayer* player, bool visible)
{
	if (!visible && settings.only_visible && player != g_LocalPlayer)
		return;

	float spacing_y = 2.f;

	if (settings.esp_health && (settings.esp_health_pos == bar_pos::top_1 || settings.esp_health_pos == bar_pos::top_2))
		spacing_y += 5.f;

	if (settings.esp_armor && (settings.esp_armor_pos == bar_pos::top_1 || settings.esp_armor_pos == bar_pos::top_2))
		spacing_y += 5.f;

	for (int i = 0; i < top_table.table_content.size(); i++)
	{
		if (top_table.table_content.at(i).content != "" && top_table.table_content.at(i).enabled)
		{
			if (!std::strstr(top_table.table_content.at(i).content.c_str(), "_wi_") && !std::strstr(top_table.table_content.at(i).content.c_str(), "_w_"))
			{
				auto sz = g_pSecondFont->CalcTextSizeA(top_table.table_content.at(i).font_size, FLT_MAX, 0.0f, top_table.table_content.at(i).content.c_str());
				Render::Get().RenderText(top_table.table_content.at(i).content.c_str(), ImVec2(player_data[player->EntIndex()].bbox.left + player_data[player->EntIndex()].width * 0.5f - sz.x * 0.5f, player_data[player->EntIndex()].bbox.top - top_table.table_content.at(i).font_size - spacing_y), top_table.table_content.at(i).font_size, Color(top_table.table_content.at(i).clr.r() / 255.f, top_table.table_content.at(i).clr.g() / 255.f, top_table.table_content.at(i).clr.b() / 255.f, player->m_alpha / 255.f), false, true, g_pSecondFont);
				spacing_y += top_table.table_content.at(i).font_size + 2.f;
			}
			else if (std::strstr(top_table.table_content.at(i).content.c_str(), "_wi_"))
			{
				std::size_t pos = top_table.table_content.at(i).content.find("_wi_");
				std::string res_str = top_table.table_content.at(i).content.substr(0, pos);

				auto sz = weapon_icons->CalcTextSizeA(top_table.table_content.at(i).font_size, FLT_MAX, 0.0f, res_str.c_str());
				Render::Get().RenderText(res_str.c_str(), ImVec2(player_data[player->EntIndex()].bbox.left + player_data[player->EntIndex()].width * 0.5f - sz.x * 0.5f, player_data[player->EntIndex()].bbox.top - top_table.table_content.at(i).font_size - spacing_y), top_table.table_content.at(i).font_size, Color(top_table.table_content.at(i).clr.r() / 255.f, top_table.table_content.at(i).clr.g() / 255.f, top_table.table_content.at(i).clr.b() / 255.f, player->m_alpha / 255.f), false, true, weapon_icons);
				spacing_y += top_table.table_content.at(i).font_size + 2.f;

			}
			else if (std::strstr(top_table.table_content.at(i).content.c_str(), "_w_"))
			{
				std::size_t pos = top_table.table_content.at(i).content.find("_w_");
				std::string res_str = top_table.table_content.at(i).content.substr(0, pos);

				auto sz = g_pSecondFont->CalcTextSizeA(top_table.table_content.at(i).font_size, FLT_MAX, 0.0f, res_str.c_str());
				Render::Get().RenderText(res_str.c_str(), ImVec2(player_data[player->EntIndex()].bbox.left + player_data[player->EntIndex()].width * 0.5f - sz.x * 0.5f, player_data[player->EntIndex()].bbox.top - top_table.table_content.at(i).font_size - spacing_y), top_table.table_content.at(i).font_size, Color(top_table.table_content.at(i).clr.r() / 255.f, top_table.table_content.at(i).clr.g() / 255.f, top_table.table_content.at(i).clr.b() / 255.f, player->m_alpha / 255.f), false, true, g_pSecondFont);
				spacing_y += top_table.table_content.at(i).font_size + 2.f;
			}
		}
	}
}

void visuals::render_bottom_table(c_esp_settings settings, C_BasePlayer* player, bool visible)
{
	if (!visible && settings.only_visible && player != g_LocalPlayer)
		return;

	float spacing_y = 2.f;

	if (settings.esp_health && (settings.esp_health_pos == bar_pos::bottom_1 || settings.esp_health_pos == bar_pos::bottom_2))
		spacing_y += 5.f;

	if (settings.esp_armor && (settings.esp_armor_pos == bar_pos::bottom_1 || settings.esp_armor_pos == bar_pos::bottom_2))
		spacing_y += 5.f;

	for (int i = 0; i < down_table.table_content.size(); i++)
	{
		if (down_table.table_content.at(i).content != "" && down_table.table_content.at(i).enabled)
		{
			if (!std::strstr(down_table.table_content.at(i).content.c_str(), "_wi_") && !std::strstr(down_table.table_content.at(i).content.c_str(), "_w_"))
			{
				auto sz = g_pSecondFont->CalcTextSizeA(down_table.table_content.at(i).font_size, FLT_MAX, 0.0f, down_table.table_content.at(i).content.c_str());
				Render::Get().RenderText(down_table.table_content.at(i).content.c_str(), ImVec2(player_data[player->EntIndex()].bbox.left + player_data[player->EntIndex()].width * 0.5f - sz.x * 0.5f, player_data[player->EntIndex()].bbox.bottom + spacing_y), down_table.table_content.at(i).font_size, Color(down_table.table_content.at(i).clr.r() / 255.f, down_table.table_content.at(i).clr.g() / 255.f, down_table.table_content.at(i).clr.b() / 255.f, player->m_alpha / 255.f), false, true, g_pSecondFont);
				spacing_y += down_table.table_content.at(i).font_size + 2.f;
			}
			else if (std::strstr(down_table.table_content.at(i).content.c_str(), "_wi_"))
			{
				std::size_t pos = down_table.table_content.at(i).content.find("_wi_");
				std::string res_str = down_table.table_content.at(i).content.substr(0, pos);

				auto sz = weapon_icons->CalcTextSizeA(down_table.table_content.at(i).font_size, FLT_MAX, 0.0f, res_str.c_str());
				Render::Get().RenderText(res_str.c_str(), ImVec2(player_data[player->EntIndex()].bbox.left + player_data[player->EntIndex()].width * 0.5f - sz.x * 0.5f, player_data[player->EntIndex()].bbox.bottom + spacing_y), down_table.table_content.at(i).font_size, Color(down_table.table_content.at(i).clr.r() / 255.f, down_table.table_content.at(i).clr.g() / 255.f, down_table.table_content.at(i).clr.b() / 255.f, player->m_alpha / 255.f), false, true, weapon_icons);
				spacing_y += down_table.table_content.at(i).font_size + 2.f;

			}
			else if (std::strstr(down_table.table_content.at(i).content.c_str(), "_w_"))
			{
				std::size_t pos = down_table.table_content.at(i).content.find("_w_");
				std::string res_str = down_table.table_content.at(i).content.substr(0, pos);

				auto sz = g_pSecondFont->CalcTextSizeA(down_table.table_content.at(i).font_size, FLT_MAX, 0.0f, res_str.c_str());
				Render::Get().RenderText(res_str.c_str(), ImVec2(player_data[player->EntIndex()].bbox.left + player_data[player->EntIndex()].width * 0.5f - sz.x * 0.5f, player_data[player->EntIndex()].bbox.bottom + spacing_y), down_table.table_content.at(i).font_size, Color(down_table.table_content.at(i).clr.r() / 255.f, down_table.table_content.at(i).clr.g() / 255.f, down_table.table_content.at(i).clr.b() / 255.f, player->m_alpha / 255.f), false, true, g_pSecondFont);
				spacing_y += down_table.table_content.at(i).font_size + 2.f;
			}
		}
	}
}

void visuals::skeleton(c_esp_settings settings, C_BasePlayer* player, bool visible)
{
	if (!settings.skeleton || (!visible && settings.only_visible && player != g_LocalPlayer))
		return;

	auto model = player->GetModel();

	if (model)
	{
		auto studio_model = g_MdlInfo->GetStudiomodel(model);

		if (studio_model)
		{
			static matrix3x4_t bone_to_world[128];
			if (player->SetupBones(bone_to_world, 128, 256, g_GlobalVars->curtime)) {

				auto get_bone_position = [&](int bone) -> Vector
				{
					return Vector(bone_to_world[bone][0][3], bone_to_world[bone][1][3], bone_to_world[bone][2][3]);
				};

				auto upper_direction = get_bone_position(7) - get_bone_position(6);
				auto breast_bone = get_bone_position(6) + upper_direction * 0.5f;

				for (auto i = 0; i < studio_model->numbones; i++)
				{
					auto bone = studio_model->GetBone(i);

					if (!bone)
						continue;

					if (bone->parent == -1)
						continue;

					if (!(bone->flags & BONE_USED_BY_HITBOX))
						continue;

					auto child = get_bone_position(i);
					auto parent = get_bone_position(bone->parent);

					auto delta_child = child - breast_bone;
					auto delta_parent = parent - breast_bone;

					if (delta_parent.Length() < 9.0f && delta_child.Length() < 9.0f)
						parent = breast_bone;

					if (i == 5)
						child = breast_bone;

					if (fabs(delta_child.z) < 5.0f && delta_parent.Length() < 5.0f && delta_child.Length() < 5.0f || i == 6)
						continue;

					Vector schild;
					Vector sparent;

					if (Math::WorldToScreen(child, schild) && Math::WorldToScreen(parent, sparent))
						Render::Get().RenderLine(schild.x, schild.y, sparent.x, sparent.y, Color(settings.skeleton_clr[0], settings.skeleton_clr[1], settings.skeleton_clr[2], player->m_alpha / 255.f));
				}
			}
		}
	}
}

void visuals::add_hit_arrow(hit_arrow_data data)
{
	hit_arrow_log.push_back(data);
}

void visuals::add_sound_arrow_enemy(sound_arrow_data data)
{
	sound_arrow_log_enemy.push_back(data);
}

void visuals::add_sound_arrow_team(sound_arrow_data data)
{
	sound_arrow_log_team.push_back(data);
}

void visuals::render_hit_arrows() 
{
	if (!g_Options.hit_arrow)
		return;

	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return;

	for (size_t id = 0; id < hit_arrow_log.size(); id++) 
	{

		if (hit_arrow_log.at(id).time <= g_GlobalVars->curtime || !hit_arrow_log.at(id).player->IsAlive())
		{
			hit_arrow_log.erase(hit_arrow_log.begin() + id);
			continue;
		}

		if (g_Options.hit_arrow_style == 0)
		{
			float width = 7.f;
			QAngle viewangles;
			g_EngineClient->GetViewAngles(&viewangles);

			auto angle = viewangles.y - Math::CalcAngle(g_LocalPlayer->m_vecOrigin() + g_LocalPlayer->m_vecViewOffset(), hit_arrow_log.at(id).player->GetAbsOrigin()).y - 90;

			int scr_size_x, scr_size_y;
			g_EngineClient->GetScreenSize(scr_size_x, scr_size_y);

			Render::Get().arc(scr_size_x / 2, scr_size_y / 2, 256, angle - width, angle + width, Color(g_Options.hit_arrow_clr), 4.f);
			Render::Get().arc(scr_size_x / 2, scr_size_y / 2, 250, angle - width, angle + width, Color(g_Options.hit_arrow_clr[0], g_Options.hit_arrow_clr[1], g_Options.hit_arrow_clr[2], 255.0f * 0.5f), 1.5f);
		}
		else if (g_Options.hit_arrow_style == 1)
		{
			QAngle viewangles;
			g_EngineClient->GetViewAngles(&viewangles);

			static int width, height;
			g_EngineClient->GetScreenSize(width, height);

			const auto screenCenter = Vector2D(width * 0.5f, height * 0.5f);
			const auto angleYawRad = DEG2RAD(viewangles.y - Math::CalcAngle(g_cheat.m_vecShootPosition, hit_arrow_log.at(id).player->GetAbsOrigin()).y - 90.0f);

			const auto newPointX = screenCenter.x + ((((width - (7 * 3)) * 0.5f) * (50 / 100.0f)) * cos(angleYawRad)) + (int)(6.0f * (((float)7 - 4.0f) / 16.0f));
			const auto newPointY = screenCenter.y + ((((height - (7 * 3)) * 0.5f) * (50 / 100.0f)) * sin(angleYawRad));

			std::array <Vector2D, 3> points
			{
				Vector2D(newPointX - 7, newPointY - 7),
				Vector2D(newPointX + 7, newPointY),
				Vector2D(newPointX - 7, newPointY + 7)
			};

			Math::rotate_triangle(points, viewangles.y - Math::CalcAngle(g_cheat.m_vecShootPosition, hit_arrow_log.at(id).player->GetAbsOrigin()).y - 90.0f);
			
			float top_y = points[0].y;
			float need_x = points[0].x;
			float down_y = points[0].y;

			for (int i = 0; i < points.size(); i++)
			{
				if (points[i].y < top_y)
				{
					top_y = points[i].y;
					need_x = points[i].x;
				}
			}

			for (int i = 0; i < points.size(); i++)
			{
				if (points[i].y > down_y)
				{
					down_y = points[i].y;
				}
			}

			auto sz = g_pSecondFont->CalcTextSizeA(16.f, FLT_MAX, 0.0f, hit_arrow_log.at(id).weapon.c_str());
			auto sz2 = g_pSecondFont->CalcTextSizeA(16.f, FLT_MAX, 0.0f, std::to_string(hit_arrow_log.at(id).hp).c_str());

			Render::Get().RenderText(hit_arrow_log.at(id).weapon.c_str(), ImVec2(need_x - sz.x / 2, top_y - sz.y), 16.f, Color(g_Options.hit_arrow_clr), false, true, g_pSecondFont);
			Render::Get().RenderText(std::to_string(hit_arrow_log.at(id).hp).c_str(), ImVec2(need_x - sz2.x / 2, down_y + sz2.y), 16.f, Color(g_Options.hit_arrow_clr), false, true, g_pSecondFont);
			
			Render::Get().TriangleFilled(points.at(0).x, points.at(0).y, points.at(1).x, points.at(1).y, points.at(2).x, points.at(2).y, Color(g_Options.hit_arrow_clr));	
		}
	}
}

void visuals::render_sound_arrows()
{
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return;

	if (g_Options.esp_settings[0].sound_arrows)
	{
		for (size_t id = 0; id < sound_arrow_log_enemy.size(); id++) {

			if (sound_arrow_log_enemy.at(id).time <= g_GlobalVars->curtime || !sound_arrow_log_enemy.at(id).player->IsAlive()) 
			{
				sound_arrow_log_enemy.erase(sound_arrow_log_enemy.begin() + id);
				continue;
			}

			if (g_Options.esp_settings[0].sound_arrows_style == 0)
			{
				float width = 7.f;
				QAngle viewangles;
				g_EngineClient->GetViewAngles(&viewangles);

				auto angle = viewangles.y - Math::CalcAngle(g_LocalPlayer->m_vecOrigin() + g_LocalPlayer->m_vecViewOffset(), sound_arrow_log_enemy.at(id).player->GetAbsOrigin()).y - 90;

				int scr_size_x, scr_size_y;
				g_EngineClient->GetScreenSize(scr_size_x, scr_size_y);

				Render::Get().arc(scr_size_x / 2, scr_size_y / 2, 256, angle - width, angle + width, Color(g_Options.esp_settings[0].sound_arrow_clr), 4.f);
				Render::Get().arc(scr_size_x / 2, scr_size_y / 2, 250, angle - width, angle + width, Color(g_Options.esp_settings[0].sound_arrow_clr[0], g_Options.esp_settings[0].sound_arrow_clr[1], g_Options.esp_settings[0].sound_arrow_clr[2], 255.0f * 0.5f), 1.5f);
			}
			else if (g_Options.esp_settings[0].sound_arrows_style == 1)
			{
				QAngle viewangles;
				g_EngineClient->GetViewAngles(&viewangles);

				static int width, height;
				g_EngineClient->GetScreenSize(width, height);

				const auto screenCenter = Vector2D(width * 0.5f, height * 0.5f);
				const auto angleYawRad = DEG2RAD(viewangles.y - Math::CalcAngle(g_cheat.m_vecShootPosition, sound_arrow_log_enemy.at(id).player->GetAbsOrigin()).y - 90.0f);

				const auto newPointX = screenCenter.x + ((((width - (7 * 3)) * 0.5f) * (50 / 100.0f)) * cos(angleYawRad)) + (int)(6.0f * (((float)7 - 4.0f) / 16.0f));
				const auto newPointY = screenCenter.y + ((((height - (7 * 3)) * 0.5f) * (50 / 100.0f)) * sin(angleYawRad));

				std::array <Vector2D, 3> points
				{
					Vector2D(newPointX - 7, newPointY - 7),
					Vector2D(newPointX + 7, newPointY),
					Vector2D(newPointX - 7, newPointY + 7)
				};

				Math::rotate_triangle(points, viewangles.y - Math::CalcAngle(g_cheat.m_vecShootPosition, sound_arrow_log_enemy.at(id).player->GetAbsOrigin()).y - 90.0f);
				Render::Get().TriangleFilled(points.at(0).x, points.at(0).y, points.at(1).x, points.at(1).y, points.at(2).x, points.at(2).y, Color(g_Options.esp_settings[0].sound_arrow_clr));
			}
		}
	}

	if (g_Options.esp_settings[1].sound_arrows)
	{
		for (size_t id = 0; id < sound_arrow_log_team.size(); id++) 
		{
			if (sound_arrow_log_team.at(id).time <= g_GlobalVars->curtime) 
			{
				sound_arrow_log_team.erase(sound_arrow_log_team.begin() + id);
				continue;
			}

			if (g_Options.esp_settings[1].sound_arrows_style == 0)
			{
				float width = 7.f;
				QAngle viewangles;
				g_EngineClient->GetViewAngles(&viewangles);

				auto angle = viewangles.y - Math::CalcAngle(g_LocalPlayer->m_vecOrigin() + g_LocalPlayer->m_vecViewOffset(), sound_arrow_log_team.at(id).player->m_angAbsOrigin()).y - 90;

				int scr_size_x, scr_size_y;
				g_EngineClient->GetScreenSize(scr_size_x, scr_size_y);

				Render::Get().arc(scr_size_x / 2, scr_size_y / 2, 256, angle - width, angle + width, Color(g_Options.esp_settings[1].sound_arrow_clr), 4.f);
				Render::Get().arc(scr_size_x / 2, scr_size_y / 2, 250, angle - width, angle + width, Color(g_Options.esp_settings[1].sound_arrow_clr[0], g_Options.esp_settings[1].sound_arrow_clr[1], g_Options.esp_settings[1].sound_arrow_clr[2], 255.0f * 0.5f), 1.5f);
			}
			else if (g_Options.esp_settings[1].sound_arrows_style == 1)
			{
				QAngle viewangles;
				g_EngineClient->GetViewAngles(&viewangles);

				static int width, height;
				g_EngineClient->GetScreenSize(width, height);

				const auto screenCenter = Vector2D(width * 0.5f, height * 0.5f);
				const auto angleYawRad = DEG2RAD(viewangles.y - Math::CalcAngle(g_cheat.m_vecShootPosition, sound_arrow_log_team.at(id).player->m_angAbsOrigin()).y - 90.0f);

				const auto newPointX = screenCenter.x + ((((width - (7 * 3)) * 0.5f) * (50 / 100.0f)) * cos(angleYawRad)) + (int)(6.0f * (((float)7 - 4.0f) / 16.0f));
				const auto newPointY = screenCenter.y + ((((height - (7 * 3)) * 0.5f) * (50 / 100.0f)) * sin(angleYawRad));

				std::array <Vector2D, 3> points
				{
					Vector2D(newPointX - 7, newPointY - 7),
					Vector2D(newPointX + 7, newPointY),
					Vector2D(newPointX - 7, newPointY + 7)
				};

				Math::rotate_triangle(points, viewangles.y - Math::CalcAngle(g_cheat.m_vecShootPosition, sound_arrow_log_team.at(id).player->m_angAbsOrigin()).y - 90.0f);
				Render::Get().TriangleFilled(points.at(0).x, points.at(0).y, points.at(1).x, points.at(1).y, points.at(2).x, points.at(2).y, Color(g_Options.esp_settings[1].sound_arrow_clr));
			}
		}
	}
}

void visuals::render_oof_arrows(c_esp_settings settings, C_BasePlayer* player)
{
	if (!settings.oof)
		return;

	if (!g_LocalPlayer->IsAlive())
		return;

	if (settings.oof_style == 0)
	{
		float width = 7.f;
		QAngle viewangles;
		g_EngineClient->GetViewAngles(&viewangles);

		auto angle = viewangles.y - Math::CalcAngle(g_LocalPlayer->m_vecOrigin() + g_LocalPlayer->m_vecViewOffset(), player->GetAbsOrigin()).y - 90;

		int scr_size_x, scr_size_y;
		g_EngineClient->GetScreenSize(scr_size_x, scr_size_y);

		Render::Get().arc(scr_size_x / 2, scr_size_y / 2, 256, angle - width, angle + width, Color(settings.oof_clr), 4.f);
		Render::Get().arc(scr_size_x / 2, scr_size_y / 2, 250, angle - width, angle + width, Color(settings.oof_clr[0], settings.oof_clr[1], settings.oof_clr[2], 255.0f * 0.5f), 1.5f);
	}
	else if (settings.oof_style == 1)
	{
		QAngle viewangles;
		g_EngineClient->GetViewAngles(&viewangles);

		static int width, height;
		g_EngineClient->GetScreenSize(width, height);

		const auto screenCenter = Vector2D(width * 0.5f, height * 0.5f);
		const auto angleYawRad = DEG2RAD(viewangles.y - Math::CalcAngle(g_cheat.m_vecShootPosition, player->GetAbsOrigin()).y - 90.0f);

		const auto newPointX = screenCenter.x + ((((width - (7 * 3)) * 0.5f) * (50 / 100.0f)) * cos(angleYawRad)) + (int)(6.0f * (((float)7 - 4.0f) / 16.0f));
		const auto newPointY = screenCenter.y + ((((height - (7 * 3)) * 0.5f) * (50 / 100.0f)) * sin(angleYawRad));

		std::array <Vector2D, 3> points
		{
			Vector2D(newPointX - 7, newPointY - 7),
			Vector2D(newPointX + 7, newPointY),
			Vector2D(newPointX - 7, newPointY + 7)
		};

		Math::rotate_triangle(points, viewangles.y - Math::CalcAngle(g_cheat.m_vecShootPosition, player->GetAbsOrigin()).y - 90.0f);
		Render::Get().TriangleFilled(points.at(0).x, points.at(0).y, points.at(1).x, points.at(1).y, points.at(2).x, points.at(2).y, Color(settings.oof_clr));
	}
}

void visuals::no_smoke()
{
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (!g_LocalPlayer)
		return;

	const std::vector smoke_materials{"particle/vistasmokev1/vistasmokev1_emods", "particle/vistasmokev1/vistasmokev1_emods_impactdust", "particle/vistasmokev1/vistasmokev1_fire", "particle/vistasmokev1/vistasmokev1_smokegrenade" };

	for (const auto mat : smoke_materials)
	{
		const auto material = g_MatSystem->FindMaterial(mat, TEXTURE_GROUP_OTHER);

		if (!material)
			continue;

		material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, g_Options.misc_no_smoke && !g_Options.safe_mode);
	}

	if (g_Options.misc_no_smoke && !g_Options.safe_mode) {
		static int* get_smoke_count = *(int**)(g_cheat.smoke_count);
		*get_smoke_count = 0;
	}
}

void visuals::draw_hit_marker2(C_BasePlayer* local_player) {

	if (!g_Options.misc_hit_marker_1)
		return;

	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (!local_player || !local_player->IsAlive())
		return;

	for (size_t id = 0; id < hit_markers_log.size(); id++)
	{
		if (hit_markers_log.at(id).bone.IsZero() || hit_markers_log.at(id).time <= g_GlobalVars->curtime) {
			hit_markers_log.erase(hit_markers_log.begin() + id);
			continue;
		}

		hit_markers_log.at(id).alpha /= 1.1f;
		Color def_clr = Color(g_Options.color_hit_marker_1);
		Color clr = Color(def_clr.r() / 255.f, def_clr.g() / 255.f, def_clr.b() / 255.f, hit_markers_log.at(id).alpha / 255.f);

		Vector out;
		if (Math::WorldToScreen(hit_markers_log.at(id).bone, out) && hit_markers_log.at(id).time > g_GlobalVars->curtime)
		{
			Render::Get().RenderLine(out.x - 6, out.y - 6, out.x - 1, out.y - 1, clr, 1.f);
			Render::Get().RenderLine(out.x - 6, out.y + 6, out.x - 1, out.y + 1, clr, 1.f);
			Render::Get().RenderLine(out.x + 6, out.y + 6, out.x + 1, out.y + 1, clr, 1.f);
			Render::Get().RenderLine(out.x + 6, out.y - 6, out.x + 1, out.y - 1, clr, 1.f);
		}
	}
}

float get_hit_marker_time;
float get_hit_marker_time_end;

void visuals::draw_hit_marker(C_BasePlayer* local_player) {

	if (!g_Options.misc_hit_marker_2)
		return;

	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (!local_player || !local_player->IsAlive())
		return;

	if (get_hit_marker_time + 1.f >= g_GlobalVars->curtime) {
		const int center_x = static_cast<int>(ImGui::GetIO().DisplaySize.x / 2);
		const int center_y = static_cast<int>(ImGui::GetIO().DisplaySize.y / 2);

		Color def_clr = Color(g_Options.color_hit_marker_2);

		Render::Get().RenderLine(center_x - 12, center_y - 12, center_x - 5, center_y - 5, Color(def_clr.r() / 255.f, def_clr.g() / 255.f, def_clr.b() / 255.f, get_hit_marker_time_end - g_GlobalVars->curtime), 1.f);
		Render::Get().RenderLine(center_x - 12, center_y + 12, center_x - 5, center_y + 5, Color(def_clr.r() / 255.f, def_clr.g() / 255.f, def_clr.b() / 255.f, get_hit_marker_time_end - g_GlobalVars->curtime), 1.f);
		Render::Get().RenderLine(center_x + 12, center_y + 12, center_x + 5, center_y + 5, Color(def_clr.r() / 255.f, def_clr.g() / 255.f, def_clr.b() / 255.f, get_hit_marker_time_end - g_GlobalVars->curtime), 1.f);
		Render::Get().RenderLine(center_x + 12, center_y - 12, center_x + 5, center_y - 5, Color(def_clr.r() / 255.f, def_clr.g() / 255.f, def_clr.b() / 255.f, get_hit_marker_time_end - g_GlobalVars->curtime), 1.f);
	}
}

void visuals::on_events_hit_marker(IGameEvent* event) {

	if (g_EngineClient->GetPlayerForUserID(event->GetInt("attacker")) == g_EngineClient->GetLocalPlayer()) {
		get_hit_marker_time = g_GlobalVars->curtime;
		get_hit_marker_time_end = g_GlobalVars->curtime + 1.f;

		if (g_Options.misc_hitsound)
		{
			switch (g_Options.misc_hitsound)
			{
			case 1:
				g_VGuiSurface->PlaySound_("metallic.wav");
				break;
			case 2:
				g_VGuiSurface->PlaySound_("cod.wav");
				break;
			case 3:
				g_VGuiSurface->PlaySound_("bubble.wav");
				break;
			case 4:
				g_VGuiSurface->PlaySound_("stapler.wav");
				break;
			case 5:
				g_VGuiSurface->PlaySound_("bell.wav");
				break;
			case 6:
				g_VGuiSurface->PlaySound_("flick.wav");
				break;
			case 7:
				if (std::string(g_Options.custom_hit_sound).size() > 0)
				g_EngineClient->ClientCmd_Unrestricted(("play " + std::string(g_Options.custom_hit_sound)).c_str());
				break;
			}
		}
	}

	const int hitted_id = g_EngineClient->GetPlayerForUserID(event->GetInt("userid"));
	const int attacker_id = g_EngineClient->GetPlayerForUserID(event->GetInt("attacker"));

	auto get_hitbox_by_hitgroup = [](const int32_t hitgroup) -> int
	{
		switch (hitgroup)
		{
		case HITGROUP_HEAD:
			return HITBOX_HEAD;
		case HITGROUP_CHEST:
			return HITBOX_CHEST;
		case HITGROUP_STOMACH:
			return HITBOX_STOMACH;
		case HITGROUP_LEFTARM:
			return HITBOX_LEFT_HAND;
		case HITGROUP_RIGHTARM:
			return HITBOX_RIGHT_HAND;
		case HITGROUP_LEFTLEG:
			return HITBOX_RIGHT_CALF;
		case HITGROUP_RIGHTLEG:
			return HITBOX_LEFT_CALF;
		default:
			return HITBOX_PELVIS;
		}
	};

	if (hitted_id != g_EngineClient->GetLocalPlayer() && attacker_id == g_EngineClient->GetLocalPlayer()) {
		const int hp = event->GetInt("dmg_health");
		const auto entity = reinterpret_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(hitted_id));

		const damage_indicator hit_log(hp, g_GlobalVars->curtime + 2.f, entity->get_bone_position(get_hitbox_by_hitgroup(event->GetInt("hitgroup"))));
		hit_markers_log.push_back(hit_log);
	}
}

void visuals::clear_hit_markers()
{
	hit_markers_log.clear();
	get_hit_marker_time = 0;
	get_hit_marker_time_end = 0;
}

void visuals::no_flash()
{
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (!g_LocalPlayer)
		return;

	const std::vector flash_materials{ "effects\\flashbang", "effects\\flashbang_white" };

	for (const auto mat : flash_materials) {
		const auto material = g_MatSystem->FindMaterial(mat, TEXTURE_GROUP_CLIENT_EFFECTS);
		material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, g_Options.misc_no_flash);
	}
}

void visuals::disable_post_processing()
{
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	static auto PostProcVar = g_CVar->FindVar("mat_postprocess_enable");
	PostProcVar->SetValue(!g_Options.misc_no_post_processing);
}

void visuals::remove_panorama_blur()
{
	static auto blur = g_CVar->FindVar("@panorama_disable_blur");
	blur->SetValue(g_Options.remove_panorama_blur);
}

void visuals::force_sniper_crosshair()
{
	static auto show_spread = g_CVar->FindVar("weapon_debug_spread_show");
	show_spread->SetValue(g_Options.misc_force_crosshair && g_LocalPlayer && !g_LocalPlayer->m_bIsScoped() ? 3 : 0);
}

void visuals::render_dopped_throwed()
{
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	for (int i = 1; i <= g_EntityList->GetHighestEntityIndex(); i++) {
		const auto entity = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));

		if (!entity)
			continue;

		if (entity->IsPlayer())
			continue;

		if (entity->IsDormant())
			continue;

		const auto client_class = entity->GetClientClass();

		if (!client_class)
			continue;

		
		grenade_projectiles(entity);
		if (client_class->m_ClassID == classids::get("CAK47") || client_class->m_ClassID == classids::get("CDEagle") || client_class->m_ClassID >= classids::get("CWeaponAug") && client_class->m_ClassID <= classids::get("CWeaponZoneRepulsor"))
		dropped_weapons(entity);
	}
}

void visuals::grenade_projectiles(C_BaseEntity* entity)
{
	auto client_class = entity->GetClientClass();

	if (!client_class)
		return;

	auto model = entity->GetModel();

	if (!model)
		return;

	auto studio_model = g_MdlInfo->GetStudiomodel(model);

	if (!studio_model)
		return;

	auto name = (std::string)studio_model->szName;

	if (name.find("thrown") != std::string::npos ||
		client_class->m_ClassID == classids::get("CBaseCSGrenadeProjectile") || client_class->m_ClassID == classids::get("CDecoyProjectile") || client_class->m_ClassID == classids::get("CMolotovProjectile"))
	{
		auto grenade_origin = entity->m_vecOrigin();
		auto grenade_position = Vector(0, 0, 0);

		if (!Math::WorldToScreen(grenade_origin, grenade_position))
			return;

		grenade_name_struct.clr = Color(g_Options.color_grenade_text);
		grenade_icon_struct.clr = Color(g_Options.color_grenade_icon);
		grenade_name_struct.enabled = g_Options.grenade_text;
		grenade_icon_struct.enabled = g_Options.grenade_icon;
		grenade_name_struct.font_size = g_Options.grenade_text_size;
		grenade_icon_struct.font_size = g_Options.grenade_icon_size;

		if (name.find("flashbang") != std::string::npos)
		{
			grenade_name_struct.content = "FLASHBANG";
			grenade_name_struct.content.append("_w_");
			grenade_icon_struct.content = "i";
			grenade_icon_struct.content.append("_wi_");
		}
		else if (name.find("smokegrenade") != std::string::npos)
		{
			grenade_name_struct.content = "SMOKE";
			grenade_name_struct.content.append("_w_");
			grenade_icon_struct.content = "k";
			grenade_icon_struct.content.append("_wi_");
		}
		else if (name.find("incendiarygrenade") != std::string::npos)
		{
			grenade_name_struct.content = "INCENDIARY";
			grenade_name_struct.content.append("_w_");
			grenade_icon_struct.content = "n";
			grenade_icon_struct.content.append("_wi_");
		}
		else if (name.find("molotov") != std::string::npos)
		{
			grenade_name_struct.content = "MOLOTOV";
			grenade_name_struct.content.append("_w_");
			grenade_icon_struct.content = "l";
			grenade_icon_struct.content.append("_wi_");
		}
		else if (name.find("fraggrenade") != std::string::npos)
		{
			grenade_name_struct.content = "HE GRENADE";
			grenade_name_struct.content.append("_w_");
			grenade_icon_struct.content = "j";
			grenade_icon_struct.content.append("_wi_");
		}
		else if (name.find("decoy") != std::string::npos)
		{
			grenade_name_struct.content = "DECOY";
			grenade_name_struct.content.append("_w_");
			grenade_icon_struct.content = "m";
			grenade_icon_struct.content.append("_wi_");
		}
		else
			return;

		right_table_grenade.table_content.clear();
		//right_table_grenade.table_content.push_back(grenade_name_struct);
		//right_table_grenade.table_content.push_back(grenade_icon_struct);

		left_table_grenade.table_content.clear();
		//left_table_grenade.table_content.push_back(grenade_name_struct);
		//left_table_grenade.table_content.push_back(grenade_icon_struct);

		top_table_grenade.table_content.clear();
		top_table_grenade.table_content.push_back(grenade_name_struct);
		//top_table_grenade.table_content.push_back(grenade_icon_struct);

		down_table_grenade.table_content.clear();
		//down_table_grenade.table_content.push_back(grenade_name_struct);
		down_table_grenade.table_content.push_back(grenade_icon_struct);

		RECT box = get_bbox(entity);

		if (g_Options.grenade_projectiles)
		{
			if (g_Options.grenade_box)
			{
				Render::Get().RenderBox(box.left, box.top, box.right, box.bottom, Color::Black, 2.f);
				Render::Get().RenderBox(box.left, box.top, box.right, box.bottom, Color(g_Options.color_grenade_box), 1.f);

			}

			float right_spacing_x = 0;
			float right_spacing_y = 0;

			if (right_table_grenade.table_content.size())
			{
				for (int i = 0; i < right_table_grenade.table_content.size(); i++)
				{
					if (right_table_grenade.table_content.at(i).content != "" && right_table_grenade.table_content.at(i).enabled)
					{
						if (!std::strstr(right_table_grenade.table_content.at(i).content.c_str(), "_wi_") && !std::strstr(right_table_grenade.table_content.at(i).content.c_str(), "_w_"))
						{
							Render::Get().RenderText(right_table_grenade.table_content.at(i).content.c_str(), ImVec2(box.right + 3 + right_spacing_x, box.top + right_spacing_y), right_table_grenade.table_content.at(i).font_size, Color(right_table_grenade.table_content.at(i).clr.r() / 255.f, right_table_grenade.table_content.at(i).clr.g() / 255.f, right_table_grenade.table_content.at(i).clr.b() / 255.f, right_table_grenade.table_content.at(i).clr.a() / 255.f), false, true, g_pSecondFont);
							right_spacing_y += right_table_grenade.table_content.at(i).font_size + 2.f;
						}
						else if (std::strstr(right_table_grenade.table_content.at(i).content.c_str(), "_wi_"))
						{
							std::size_t pos = right_table_grenade.table_content.at(i).content.find("_wi_");
							std::string res_str = right_table_grenade.table_content.at(i).content.substr(0, pos);
							Render::Get().RenderText(res_str.c_str(), ImVec2(box.right + 3 + right_spacing_x, box.top + right_spacing_y), right_table_grenade.table_content.at(i).font_size, Color(right_table_grenade.table_content.at(i).clr.r() / 255.f, right_table_grenade.table_content.at(i).clr.g() / 255.f, right_table_grenade.table_content.at(i).clr.b() / 255.f, right_table_grenade.table_content.at(i).clr.a() / 255.f), false, true, m_BigIcons);
							right_spacing_y += right_table_grenade.table_content.at(i).font_size + 2.f;
						}
						else if (std::strstr(right_table_grenade.table_content.at(i).content.c_str(), "_w_"))
						{
							std::size_t pos = right_table_grenade.table_content.at(i).content.find("_w_");
							std::string res_str = right_table_grenade.table_content.at(i).content.substr(0, pos);
							Render::Get().RenderText(res_str.c_str(), ImVec2(box.right + 3 + right_spacing_x, box.top + right_spacing_y), right_table_grenade.table_content.at(i).font_size, Color(right_table_grenade.table_content.at(i).clr.r() / 255.f, right_table_grenade.table_content.at(i).clr.g() / 255.f, right_table_grenade.table_content.at(i).clr.b() / 255.f, right_table_grenade.table_content.at(i).clr.a() / 255.f), false, true, g_pSecondFont);
							right_spacing_y += right_table_grenade.table_content.at(i).font_size + 2.f;
						}
					}
				}
			}
			float left_spacing_x = 0;
			float left_spacing_y = 0;

			if (left_table_grenade.table_content.size())
			{
				for (int i = 0; i < left_table_grenade.table_content.size(); i++)
				{
					if (left_table_grenade.table_content.at(i).content != "" && left_table_grenade.table_content.at(i).enabled)
					{
						if (!std::strstr(left_table_grenade.table_content.at(i).content.c_str(), "_wi_") && !std::strstr(left_table_grenade.table_content.at(i).content.c_str(), "_w_"))
						{
							auto sz = g_pSecondFont->CalcTextSizeA(left_table_grenade.table_content.at(i).font_size, FLT_MAX, 0.0f, left_table_grenade.table_content.at(i).content.c_str());
							Render::Get().RenderText(left_table_grenade.table_content.at(i).content.c_str(), ImVec2(box.left - 3 - left_spacing_x - sz.x, box.top + left_spacing_y), left_table_grenade.table_content.at(i).font_size, Color(left_table_grenade.table_content.at(i).clr.r() / 255.f, left_table_grenade.table_content.at(i).clr.g() / 255.f, left_table_grenade.table_content.at(i).clr.b() / 255.f, left_table_grenade.table_content.at(i).clr.a() / 255.f), false, true, g_pSecondFont);
							left_spacing_y += left_table_grenade.table_content.at(i).font_size + 2.f;
						}
						else if (std::strstr(left_table_grenade.table_content.at(i).content.c_str(), "_wi_"))
						{
							std::size_t pos = left_table_grenade.table_content.at(i).content.find("_wi_");
							std::string res_str = left_table_grenade.table_content.at(i).content.substr(0, pos);

							auto sz = m_BigIcons->CalcTextSizeA(left_table_grenade.table_content.at(i).font_size, FLT_MAX, 0.0f, res_str.c_str());
							Render::Get().RenderText(res_str.c_str(), ImVec2(box.left - 3 - left_spacing_x - sz.x, box.top + left_spacing_y), left_table_grenade.table_content.at(i).font_size, Color(left_table_grenade.table_content.at(i).clr.r() / 255.f, left_table_grenade.table_content.at(i).clr.g() / 255.f, left_table_grenade.table_content.at(i).clr.b() / 255.f, left_table_grenade.table_content.at(i).clr.a() / 255.f), false, true, m_BigIcons);
							left_spacing_y += left_table_grenade.table_content.at(i).font_size + 2.f;

						}
						else if (std::strstr(left_table_grenade.table_content.at(i).content.c_str(), "_w_"))
						{
							std::size_t pos = left_table_grenade.table_content.at(i).content.find("_w_");
							std::string res_str = left_table_grenade.table_content.at(i).content.substr(0, pos);

							auto sz = g_pSecondFont->CalcTextSizeA(left_table_grenade.table_content.at(i).font_size, FLT_MAX, 0.0f, res_str.c_str());
							Render::Get().RenderText(res_str.c_str(), ImVec2(box.left - 3 - left_spacing_x - sz.x, box.top + left_spacing_y), left_table_grenade.table_content.at(i).font_size, Color(left_table_grenade.table_content.at(i).clr.r() / 255.f, left_table_grenade.table_content.at(i).clr.g() / 255.f, left_table_grenade.table_content.at(i).clr.b() / 255.f, left_table_grenade.table_content.at(i).clr.a() / 255.f), false, true, g_pSecondFont);
							left_spacing_y += left_table_grenade.table_content.at(i).font_size + 2.f;
						}
					}
				}
			}

			float top_spacing_y = 2.f;

			if (top_table_grenade.table_content.size())
			{
				for (int i = 0; i < top_table_grenade.table_content.size(); i++)
				{
					if (top_table_grenade.table_content.at(i).content != "" && top_table_grenade.table_content.at(i).enabled)
					{
						if (!std::strstr(top_table_grenade.table_content.at(i).content.c_str(), "_wi_") && !std::strstr(top_table_grenade.table_content.at(i).content.c_str(), "_w_"))
						{
							auto sz = g_pSecondFont->CalcTextSizeA(top_table_grenade.table_content.at(i).font_size, FLT_MAX, 0.0f, top_table_grenade.table_content.at(i).content.c_str());
							Render::Get().RenderText(top_table_grenade.table_content.at(i).content.c_str(), ImVec2(box.left + (box.right - box.left) * 0.5f - sz.x * 0.5f, box.top - top_table_grenade.table_content.at(i).font_size - top_spacing_y), top_table_grenade.table_content.at(i).font_size, Color(top_table_grenade.table_content.at(i).clr.r() / 255.f, top_table_grenade.table_content.at(i).clr.g() / 255.f, top_table_grenade.table_content.at(i).clr.b() / 255.f, top_table_grenade.table_content.at(i).clr.a() / 255.f), false, true, g_pSecondFont);
							top_spacing_y += top_table_grenade.table_content.at(i).font_size + 2.f;
						}
						else if (std::strstr(top_table_grenade.table_content.at(i).content.c_str(), "_wi_"))
						{
							std::size_t pos = top_table_grenade.table_content.at(i).content.find("_wi_");
							std::string res_str = top_table_grenade.table_content.at(i).content.substr(0, pos);

							auto sz = m_BigIcons->CalcTextSizeA(top_table_grenade.table_content.at(i).font_size, FLT_MAX, 0.0f, res_str.c_str());
							Render::Get().RenderText(res_str.c_str(), ImVec2(box.left + (box.right - box.left) * 0.5f - sz.x * 0.5f, box.top - top_table_grenade.table_content.at(i).font_size - top_spacing_y), top_table_grenade.table_content.at(i).font_size, Color(top_table_grenade.table_content.at(i).clr.r() / 255.f, top_table_grenade.table_content.at(i).clr.g() / 255.f, top_table_grenade.table_content.at(i).clr.b() / 255.f, top_table_grenade.table_content.at(i).clr.a() / 255.f), true, true, m_BigIcons);
							top_spacing_y += top_table_grenade.table_content.at(i).font_size + 2.f;

						}
						else if (std::strstr(top_table_grenade.table_content.at(i).content.c_str(), "_w_"))
						{
							std::size_t pos = top_table_grenade.table_content.at(i).content.find("_w_");
							std::string res_str = top_table_grenade.table_content.at(i).content.substr(0, pos);

							auto sz = g_pSecondFont->CalcTextSizeA(top_table_grenade.table_content.at(i).font_size, FLT_MAX, 0.0f, res_str.c_str());
							Render::Get().RenderText(res_str.c_str(), ImVec2(box.left + (box.right - box.left) * 0.5f - sz.x * 0.5f, box.top - top_table_grenade.table_content.at(i).font_size - top_spacing_y), top_table_grenade.table_content.at(i).font_size, Color(top_table_grenade.table_content.at(i).clr.r() / 255.f, top_table_grenade.table_content.at(i).clr.g() / 255.f, top_table_grenade.table_content.at(i).clr.b() / 255.f, top_table_grenade.table_content.at(i).clr.a() / 255.f), false, true, g_pSecondFont);
							top_spacing_y += top_table_grenade.table_content.at(i).font_size + 2.f;
						}
					}
				}
			}
			float down_spacing_y = 2.f;

			if (down_table_grenade.table_content.size())
			{
				for (int i = 0; i < down_table_grenade.table_content.size(); i++)
				{
					if (down_table_grenade.table_content.at(i).content != "" && down_table_grenade.table_content.at(i).enabled)
					{
						if (!std::strstr(down_table_grenade.table_content.at(i).content.c_str(), "_wi_") && !std::strstr(down_table_grenade.table_content.at(i).content.c_str(), "_w_"))
						{
							auto sz = g_pSecondFont->CalcTextSizeA(down_table_grenade.table_content.at(i).font_size, FLT_MAX, 0.0f, down_table_grenade.table_content.at(i).content.c_str());
							Render::Get().RenderText(down_table_grenade.table_content.at(i).content.c_str(), ImVec2(box.left + (box.right - box.left) * 0.5f - sz.x * 0.5f, box.bottom + down_spacing_y), down_table_grenade.table_content.at(i).font_size, Color(down_table_grenade.table_content.at(i).clr.r() / 255.f, down_table_grenade.table_content.at(i).clr.g() / 255.f, down_table_grenade.table_content.at(i).clr.b() / 255.f, down_table_grenade.table_content.at(i).clr.a() / 255.f), false, true, g_pSecondFont);
							down_spacing_y += down_table_grenade.table_content.at(i).font_size + 2.f;
						}
						else if (std::strstr(down_table_grenade.table_content.at(i).content.c_str(), "_wi_"))
						{
							std::size_t pos = down_table_grenade.table_content.at(i).content.find("_wi_");
							std::string res_str = down_table_grenade.table_content.at(i).content.substr(0, pos);

							auto sz = m_BigIcons->CalcTextSizeA(down_table_grenade.table_content.at(i).font_size, FLT_MAX, 0.0f, res_str.c_str());
							Render::Get().RenderText(res_str.c_str(), ImVec2(box.left + (box.right - box.left) * 0.5f - sz.x * 0.5f, box.bottom + down_spacing_y), down_table_grenade.table_content.at(i).font_size, Color(down_table_grenade.table_content.at(i).clr.r() / 255.f, down_table_grenade.table_content.at(i).clr.g() / 255.f, down_table_grenade.table_content.at(i).clr.b() / 255.f, down_table_grenade.table_content.at(i).clr.a() / 255.f), true, true, m_BigIcons);
							down_spacing_y += down_table_grenade.table_content.at(i).font_size + 2.f;

						}
						else if (std::strstr(down_table_grenade.table_content.at(i).content.c_str(), "_w_"))
						{
							std::size_t pos = down_table_grenade.table_content.at(i).content.find("_w_");
							std::string res_str = down_table_grenade.table_content.at(i).content.substr(0, pos);

							auto sz = g_pSecondFont->CalcTextSizeA(down_table_grenade.table_content.at(i).font_size, FLT_MAX, 0.0f, res_str.c_str());
							Render::Get().RenderText(res_str.c_str(), ImVec2(box.left + (box.right - box.left) * 0.5f - sz.x * 0.5f, box.bottom + down_spacing_y), down_table_grenade.table_content.at(i).font_size, Color(down_table_grenade.table_content.at(i).clr.r() / 255.f, down_table_grenade.table_content.at(i).clr.g() / 255.f, down_table_grenade.table_content.at(i).clr.b() / 255.f, down_table_grenade.table_content.at(i).clr.a() / 255.f), false, true, g_pSecondFont);
							down_spacing_y += down_table_grenade.table_content.at(i).font_size + 2.f;
						}
					}
				}
			}
		}
	}
}

void visuals::dropped_weapons(C_BaseEntity* entity)
{
	if (!g_Options.dropped_weapon) return;
	auto weapon = entity;
	auto owner = weapon->get_owner_entity();
	if (owner != -1)
		return;

	C_BaseCombatWeapon* weapon_str = static_cast<C_BaseCombatWeapon*>(weapon);

	if (!weapon_str)
		return;

	auto data = weapon_str->GetWeaponData();

	if (!data)
		return;

	auto grenade_origin = entity->m_vecOrigin();
	auto grenade_position = Vector(0, 0, 0);

	if (!Math::WorldToScreen(grenade_origin, grenade_position))
		return;

	RECT box = get_bbox(entity);

	dropped_weapon_name_struct.content = weapon_str->get_weapon();
	dropped_weapon_name_struct.content.append("_w_");
	dropped_weapon_icon_struct.content = weapon_str->get_weapon_icon();
	dropped_weapon_icon_struct.content.append("_wi_");

	if (entity->GetClientClass()->m_ClassID != classids::get("CBaseCSGrenadeProjectile") && entity->GetClientClass()->m_ClassID != classids::get("CSmokeGrenadeProjectile") && entity->GetClientClass()->m_ClassID != classids::get("CSensorGrenadeProjectile") && entity->GetClientClass()->m_ClassID != classids::get("CMolotovProjectile") && entity->GetClientClass()->m_ClassID != classids::get("CDecoyProjectile"))
	dropped_weapon_ammo_struct.content = data->m_iMaxClip1;
	dropped_weapon_ammo_struct.content.append("_w_");

	dropped_weapon_name_struct.clr = Color(g_Options.color_dropped_weapon_text);
	dropped_weapon_icon_struct.clr = Color(g_Options.color_dropped_weapon_icon);
	dropped_weapon_ammo_struct.clr = Color(g_Options.color_dropped_weapon_ammo);

	dropped_weapon_name_struct.font_size = g_Options.dropped_weapon_text_size;
	dropped_weapon_icon_struct.font_size = g_Options.dropped_weapon_icon_size;
	dropped_weapon_ammo_struct.font_size = g_Options.dropped_weapon_ammo_size;

	dropped_weapon_name_struct.enabled = g_Options.dropped_weapon_text;
	dropped_weapon_icon_struct.enabled = g_Options.dropped_weapon_icon;
	dropped_weapon_ammo_struct.enabled = g_Options.dropped_weapon_ammo;

	right_table_weapon.table_content.clear();
	//right_table_weapon.table_content.push_back(dropped_weapon_name_struct);
	//right_table_weapon.table_content.push_back(dropped_weapon_icon_struct);
	right_table_weapon.table_content.push_back(dropped_weapon_ammo_struct);

	left_table_weapon.table_content.clear();
	//left_table_weapon.table_content.push_back(dropped_weapon_name_struct);
	//left_table_weapon.table_content.push_back(dropped_weapon_icon_struct);
	//left_table_weapon.table_content.push_back(dropped_weapon_ammo_struct);

	top_table_weapon.table_content.clear();
	top_table_weapon.table_content.push_back(dropped_weapon_name_struct);
	//top_table_weapon.table_content.push_back(dropped_weapon_icon_struct);
	//top_table_weapon.table_content.push_back(dropped_weapon_ammo_struct);

	down_table_weapon.table_content.clear();
	//down_table_weapon.table_content.push_back(dropped_weapon_name_struct);
	down_table_weapon.table_content.push_back(dropped_weapon_icon_struct);
	//down_table_weapon.table_content.push_back(dropped_weapon_ammo_struct);

	if (g_Options.dropped_weapon_box)
	{
		Render::Get().RenderBox(box.left, box.top, box.right, box.bottom, Color::Black, 2.f);
		Render::Get().RenderBox(box.left, box.top, box.right, box.bottom, Color(g_Options.color_dropped_weapon_box), 1.f);
	}

	float right_spacing_x = 0;
	float right_spacing_y = 0;

	for (int i = 0; i < right_table_weapon.table_content.size(); i++)
	{
		if (right_table_weapon.table_content.at(i).content != "" && right_table_weapon.table_content.at(i).enabled)
		{
			if (!std::strstr(right_table_weapon.table_content.at(i).content.c_str(), "_wi_") && !std::strstr(right_table_weapon.table_content.at(i).content.c_str(), "_w_"))
			{
				Render::Get().RenderText(right_table_weapon.table_content.at(i).content.c_str(), ImVec2(box.right + 3 + right_spacing_x, box.top + right_spacing_y), right_table_weapon.table_content.at(i).font_size, Color(right_table_weapon.table_content.at(i).clr.r() / 255.f, right_table_weapon.table_content.at(i).clr.g() / 255.f, right_table_weapon.table_content.at(i).clr.b() / 255.f, right_table_weapon.table_content.at(i).clr.a() / 255.f), false, true, g_pSecondFont);
				right_spacing_y += right_table_weapon.table_content.at(i).font_size + 2.f;
			}
			else if (std::strstr(right_table_weapon.table_content.at(i).content.c_str(), "_wi_"))
			{
				std::size_t pos = right_table_weapon.table_content.at(i).content.find("_wi_");
				std::string res_str = right_table_weapon.table_content.at(i).content.substr(0, pos);
				Render::Get().RenderText(res_str.c_str(), ImVec2(box.right + 3 + right_spacing_x, box.top + right_spacing_y), right_table_weapon.table_content.at(i).font_size, Color(right_table_weapon.table_content.at(i).clr.r() / 255.f, right_table_weapon.table_content.at(i).clr.g() / 255.f, right_table_weapon.table_content.at(i).clr.b() / 255.f, right_table_weapon.table_content.at(i).clr.a() / 255.f), false, true, weapon_icons);
				right_spacing_y += right_table_weapon.table_content.at(i).font_size + 2.f;
			}
			else if (std::strstr(right_table_weapon.table_content.at(i).content.c_str(), "_w_"))
			{
				std::size_t pos = right_table_weapon.table_content.at(i).content.find("_w_");
				std::string res_str = right_table_weapon.table_content.at(i).content.substr(0, pos);
				Render::Get().RenderText(res_str.c_str(), ImVec2(box.right + 3 + right_spacing_x, box.top + right_spacing_y), right_table_weapon.table_content.at(i).font_size, Color(right_table_weapon.table_content.at(i).clr.r() / 255.f, right_table_weapon.table_content.at(i).clr.g() / 255.f, right_table_weapon.table_content.at(i).clr.b() / 255.f, right_table_weapon.table_content.at(i).clr.a() / 255.f), false, true, g_pSecondFont);
				right_spacing_y += right_table_weapon.table_content.at(i).font_size + 2.f;
			}
		}
	}

	float left_spacing_x = 0;
	float left_spacing_y = 0;

	for (int i = 0; i < left_table_weapon.table_content.size(); i++)
	{
		if (left_table_weapon.table_content.at(i).content != "" && left_table_weapon.table_content.at(i).enabled)
		{
			if (!std::strstr(left_table_weapon.table_content.at(i).content.c_str(), "_wi_") && !std::strstr(left_table_weapon.table_content.at(i).content.c_str(), "_w_"))
			{
				auto sz = g_pSecondFont->CalcTextSizeA(left_table_weapon.table_content.at(i).font_size, FLT_MAX, 0.0f, left_table_weapon.table_content.at(i).content.c_str());
				Render::Get().RenderText(left_table_weapon.table_content.at(i).content.c_str(), ImVec2(box.left - 3 - left_spacing_x - sz.x, box.top + left_spacing_y), left_table_weapon.table_content.at(i).font_size, Color(left_table_weapon.table_content.at(i).clr.r() / 255.f, left_table_weapon.table_content.at(i).clr.g() / 255.f, left_table_weapon.table_content.at(i).clr.b() / 255.f, left_table_weapon.table_content.at(i).clr.a() / 255.f), false, true, g_pSecondFont);
				left_spacing_y += left_table_weapon.table_content.at(i).font_size + 2.f;
			}
			else if (std::strstr(left_table_weapon.table_content.at(i).content.c_str(), "_wi_"))
			{
				std::size_t pos = left_table_weapon.table_content.at(i).content.find("_wi_");
				std::string res_str = left_table_weapon.table_content.at(i).content.substr(0, pos);

				auto sz = weapon_icons->CalcTextSizeA(left_table_weapon.table_content.at(i).font_size, FLT_MAX, 0.0f, res_str.c_str());
				Render::Get().RenderText(res_str.c_str(), ImVec2(box.left - 3 - left_spacing_x - sz.x, box.top + left_spacing_y), left_table_weapon.table_content.at(i).font_size, Color(left_table_weapon.table_content.at(i).clr.r() / 255.f, left_table_weapon.table_content.at(i).clr.g() / 255.f, left_table_weapon.table_content.at(i).clr.b() / 255.f, left_table_weapon.table_content.at(i).clr.a() / 255.f), false, true, weapon_icons);
				left_spacing_y += left_table_weapon.table_content.at(i).font_size + 2.f;

			}
			else if (std::strstr(left_table_weapon.table_content.at(i).content.c_str(), "_w_"))
			{
				std::size_t pos = left_table_weapon.table_content.at(i).content.find("_w_");
				std::string res_str = left_table_weapon.table_content.at(i).content.substr(0, pos);

				auto sz = g_pSecondFont->CalcTextSizeA(left_table_weapon.table_content.at(i).font_size, FLT_MAX, 0.0f, res_str.c_str());
				Render::Get().RenderText(res_str.c_str(), ImVec2(box.left - 3 - left_spacing_x - sz.x, box.top + left_spacing_y), left_table_weapon.table_content.at(i).font_size, Color(left_table_weapon.table_content.at(i).clr.r() / 255.f, left_table_weapon.table_content.at(i).clr.g() / 255.f, left_table_weapon.table_content.at(i).clr.b() / 255.f, left_table_weapon.table_content.at(i).clr.a() / 255.f), false, true, g_pSecondFont);
				left_spacing_y += left_table_weapon.table_content.at(i).font_size + 2.f;
			}
		}
	}

	float top_spacing_y = 2.f;

	for (int i = 0; i < top_table_weapon.table_content.size(); i++)
	{
		if (top_table_weapon.table_content.at(i).content != "" && top_table_weapon.table_content.at(i).enabled)
		{
			if (!std::strstr(top_table_weapon.table_content.at(i).content.c_str(), "_wi_") && !std::strstr(top_table_weapon.table_content.at(i).content.c_str(), "_w_"))
			{
				auto sz = g_pSecondFont->CalcTextSizeA(top_table_weapon.table_content.at(i).font_size, FLT_MAX, 0.0f, top_table_weapon.table_content.at(i).content.c_str());
				Render::Get().RenderText(top_table_weapon.table_content.at(i).content.c_str(), ImVec2(box.left + (box.right - box.left) * 0.5f - sz.x * 0.5f, box.top - top_table_weapon.table_content.at(i).font_size - top_spacing_y), top_table_weapon.table_content.at(i).font_size, Color(top_table_weapon.table_content.at(i).clr.r() / 255.f, top_table_weapon.table_content.at(i).clr.g() / 255.f, top_table_weapon.table_content.at(i).clr.b() / 255.f, top_table_weapon.table_content.at(i).clr.a() / 255.f), false, true, g_pSecondFont);
				top_spacing_y += top_table_weapon.table_content.at(i).font_size + 2.f;
			}
			else if (std::strstr(top_table_weapon.table_content.at(i).content.c_str(), "_wi_"))
			{
				std::size_t pos = top_table_weapon.table_content.at(i).content.find("_wi_");
				std::string res_str = top_table_weapon.table_content.at(i).content.substr(0, pos);

				auto sz = weapon_icons->CalcTextSizeA(top_table_weapon.table_content.at(i).font_size, FLT_MAX, 0.0f, res_str.c_str());
				Render::Get().RenderText(res_str.c_str(), ImVec2(box.left + (box.right - box.left) * 0.5f - sz.x * 0.5f, box.top - top_table_weapon.table_content.at(i).font_size - top_spacing_y), top_table_weapon.table_content.at(i).font_size, Color(top_table_weapon.table_content.at(i).clr.r() / 255.f, top_table_weapon.table_content.at(i).clr.g() / 255.f, top_table_weapon.table_content.at(i).clr.b() / 255.f, top_table_weapon.table_content.at(i).clr.a() / 255.f), true, true, weapon_icons);
				top_spacing_y += top_table_weapon.table_content.at(i).font_size + 2.f;

			}
			else if (std::strstr(top_table_weapon.table_content.at(i).content.c_str(), "_w_"))
			{
				std::size_t pos = top_table_weapon.table_content.at(i).content.find("_w_");
				std::string res_str = top_table_weapon.table_content.at(i).content.substr(0, pos);

				auto sz = g_pSecondFont->CalcTextSizeA(top_table_weapon.table_content.at(i).font_size, FLT_MAX, 0.0f, res_str.c_str());
				Render::Get().RenderText(res_str.c_str(), ImVec2(box.left + (box.right - box.left) * 0.5f - sz.x * 0.5f, box.top - top_table_weapon.table_content.at(i).font_size - top_spacing_y), top_table_weapon.table_content.at(i).font_size, Color(top_table_weapon.table_content.at(i).clr.r() / 255.f, top_table_weapon.table_content.at(i).clr.g() / 255.f, top_table_weapon.table_content.at(i).clr.b() / 255.f, top_table_weapon.table_content.at(i).clr.a() / 255.f), false, true, g_pSecondFont);
				top_spacing_y += top_table_weapon.table_content.at(i).font_size + 2.f;
			}
		}
	}

	float down_spacing_y = 2.f;

	for (int i = 0; i < down_table_weapon.table_content.size(); i++)
	{
		if (down_table_weapon.table_content.at(i).content != "" && down_table_weapon.table_content.at(i).enabled)
		{
			if (!std::strstr(down_table_weapon.table_content.at(i).content.c_str(), "_wi_") && !std::strstr(down_table_weapon.table_content.at(i).content.c_str(), "_w_"))
			{
				auto sz = g_pSecondFont->CalcTextSizeA(down_table_weapon.table_content.at(i).font_size, FLT_MAX, 0.0f, down_table_weapon.table_content.at(i).content.c_str());
				Render::Get().RenderText(down_table_weapon.table_content.at(i).content.c_str(), ImVec2(box.left + (box.right - box.left) * 0.5f - sz.x * 0.5f, box.bottom + down_spacing_y), down_table_weapon.table_content.at(i).font_size, Color(down_table_weapon.table_content.at(i).clr.r() / 255.f, down_table_weapon.table_content.at(i).clr.g() / 255.f, down_table_weapon.table_content.at(i).clr.b() / 255.f, down_table_weapon.table_content.at(i).clr.a() / 255.f), false, true, g_pSecondFont);
				down_spacing_y += down_table_weapon.table_content.at(i).font_size + 2.f;
			}
			else if (std::strstr(down_table_weapon.table_content.at(i).content.c_str(), "_wi_"))
			{
				std::size_t pos = down_table_weapon.table_content.at(i).content.find("_wi_");
				std::string res_str = down_table_weapon.table_content.at(i).content.substr(0, pos);

				auto sz = weapon_icons->CalcTextSizeA(down_table_weapon.table_content.at(i).font_size, FLT_MAX, 0.0f, res_str.c_str());
				Render::Get().RenderText(res_str.c_str(), ImVec2(box.left + (box.right - box.left) * 0.5f - sz.x * 0.5f, box.bottom + down_spacing_y), down_table_weapon.table_content.at(i).font_size, Color(down_table_weapon.table_content.at(i).clr.r() / 255.f, down_table_weapon.table_content.at(i).clr.g() / 255.f, down_table_weapon.table_content.at(i).clr.b() / 255.f, down_table_weapon.table_content.at(i).clr.a() / 255.f), true, true, weapon_icons);
				down_spacing_y += down_table_weapon.table_content.at(i).font_size + 2.f;

			}
			else if (std::strstr(down_table_weapon.table_content.at(i).content.c_str(), "_w_"))
			{
				std::size_t pos = down_table_weapon.table_content.at(i).content.find("_w_");
				std::string res_str = down_table_weapon.table_content.at(i).content.substr(0, pos);

				auto sz = g_pSecondFont->CalcTextSizeA(down_table_weapon.table_content.at(i).font_size, FLT_MAX, 0.0f, res_str.c_str());
				Render::Get().RenderText(res_str.c_str(), ImVec2(box.left + (box.right - box.left) * 0.5f - sz.x * 0.5f, box.bottom + down_spacing_y), down_table_weapon.table_content.at(i).font_size, Color(down_table_weapon.table_content.at(i).clr.r() / 255.f, down_table_weapon.table_content.at(i).clr.g() / 255.f, down_table_weapon.table_content.at(i).clr.b() / 255.f, down_table_weapon.table_content.at(i).clr.a() / 255.f), false, true, g_pSecondFont);
				down_spacing_y += down_table_weapon.table_content.at(i).font_size + 2.f;
			}
		}
	}

}