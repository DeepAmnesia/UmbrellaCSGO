#include "misc.h"
#include <chrono>
#include "../../menu.hpp"
#include <mutex>

#define CheckIfNonValidNumber(x) (fpclassify(x) == FP_INFINITE || fpclassify(x) == FP_NAN || fpclassify(x) == FP_SUBNORMAL)

ImGuiWindowFlags findow_flag_closed2 = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
ImGuiWindowFlags findow_flag2 = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;

void misc::fog_controller() {

	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	for (int i = 1; i <= g_EntityList->GetHighestEntityIndex(); i++) {
		const auto entity = reinterpret_cast<c_fog_controller*>(g_EntityList->GetClientEntity(i));

		if (!entity)
			continue;

		if (entity->GetClientClass()->m_ClassID != classids::get("CFogController"))
			continue;

		Color color( g_Options.fog_color );

		entity->get_fog_enable() = g_Options.fog_enable;
		entity->get_fog_start() = g_Options.fog_start;
		entity->get_fog_end() = g_Options.fog_end;
		entity->get_fog_max_density() =g_Options.fog_density;
		entity->get_fog_blend() = true;
		entity->get_fog_color_primary() = Utils::rgb_to_int(color.b(), color.g(), color.r());
		entity->get_fog_color_secondary() = Utils::rgb_to_int(color.b(), color.g(), color.r());
	}
}

std::string mode_name(int mode)
{
	switch (mode)
	{
	case 0:
		return "[Toggle] ";
		break;
	case 1:
		return "[Hold] ";
		break;
	case 2:
		return "[Force Disable] ";
		break;
	case 3:
		return "[Always on] ";
		break;
	}
	return "[None] ";
}

void misc::binds_list()
{
	if (!g_Options.misc_binds_list) return;

	int specs = 0;
	const int modes = 0;
	std::string spect = "";
	std::string mode = "";
	int index = 1;

	if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected())
	{
		if (Utils::key_bind(g_Options.third_person_key, g_Options.third_person_mode))
		{
			spect += mode_name(g_Options.third_person_mode);
			spect.append("Third person");
			spect += u8"\n";
			specs++;
		}
		if (Utils::key_bind(g_Options.edge_jump_key, g_Options.edge_jump_mode))
		{
			spect += mode_name(g_Options.edge_jump_mode);
			spect.append("Edge Jump");
			spect += u8"\n";
			specs++;
		}
	}

	if (Menu::Get().IsVisible() || specs > 0)
	{
		if (ImGui::Begin("Binds", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize)) {

			if (specs > 0)
				spect += "\n";

			if (modes > 0)
				mode += "\n";

			ImGui::SetWindowSize(ImVec2(200.f, 29.f + ImGui::CalcTextSize(spect.c_str()).y));
			ImGui::Text(spect.c_str());

			index++;
		}
		ImGui::End();
	}
}

float CSGO_Armor(float flDamage, int ArmorValue)
{
	float flArmorRatio = 0.5f;
	float flArmorBonus = 0.5f;
	if (ArmorValue > 0) {
		float flNew = flDamage * flArmorRatio;
		float flArmor = (flDamage - flNew) * flArmorBonus;

		if (flArmor > static_cast<float>(ArmorValue)) {
			flArmor = static_cast<float>(ArmorValue) * (1.f / flArmorBonus);
			flNew = flDamage - flArmor;
		}

		flDamage = flNew;
	}
	return flDamage;
}

void misc::bomb_timer()
{
	if (!g_Options.bomb_timer)
		return;

	static float timer;
	static std::string timer_text;
	static std::string site = "A";
	static float damage = 0;
	static float rest = 0;

	if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected())
	{	
		if (g_cheat.bomb_begin_defuse && !g_cheat.bomb_is_planting)
		{
			timer = g_cheat.bomb_timer_stamp - g_GlobalVars->curtime;
			timer_text = "Bomb is defusing";
		}
		else if (!g_cheat.bomb_begin_defuse && g_cheat.bomb_is_planting)
		{
			timer = g_cheat.plant_time - g_GlobalVars->curtime;
			timer_text = "Bomb is planting";
		}
		else
		{
			timer_text = "Defuse info";
			timer = 0;

			for (int i = 1; i <= g_EntityList->GetHighestEntityIndex(); i++) 
			{
				const auto entity = reinterpret_cast<c_planted_c4*>(g_EntityList->GetClientEntity(i));

				if (!entity || entity->IsDormant())
					continue;

				if (entity->GetClientClass()->m_ClassID != classids::get("CPlantedC4"))
					continue;

				if (entity->get_bomb_defused())
					continue;

				timer = entity->get_bomb_timer();

				if (timer == 0.f)
					continue;

				site = entity->m_nBombSite() == 0 ? "A" : "B";
				g_cheat.global_bomb_site = site;

				if (timer <= 40.f && timer > 10.f)
					timer_text = "Can be defused";
				else if (timer <= 10.f && timer > 5.f)
					timer_text = "Only with kits";
				else if (timer <= 5.f && timer > 0)
					timer_text = "Run!";
				else if (timer <= 0.f)
					timer_text = "Defuse info";

				float flDistance = g_LocalPlayer->m_vecOrigin().DistTo(entity->m_vecOrigin());

				float a = 450.7f;
				float b = 75.68f;
				float c = 789.2f;
				float d = ((flDistance - b) / c);
				float flDamage = a * exp(-d * d);

				damage = max((int)floorf(CSGO_Armor(flDamage, g_LocalPlayer->m_ArmorValue())), 0);
				rest = max(g_LocalPlayer->m_iHealth() - damage, 0);
			}
		}
	}
	else
	{
		timer_text = "Defuse info";
		timer = 0;
	}

	if (g_cheat.clear_bomb_data)
	{
		g_cheat.clear_bomb_data = false;
		timer_text = "Defuse info";
		timer = 0;
	}

	if (timer > 0 || Menu::Get().IsVisible())
	{
		if (ImGui::Begin("BombTimer", NULL, Menu::Get().IsVisible() ? findow_flag2 : findow_flag_closed2)) 
		{
			ImGui::SetWindowSize(ImVec2(200.f, 100.f));
			ImGui::Text("Time: ");
			ImGui::SameLine();
			ImGui::Text(std::to_string(static_cast<int>(timer)).c_str());
			ImGui::Text(timer_text.c_str());
			if (!g_cheat.bomb_is_planting)
			{
				ImGui::Text(site.c_str());
				ImGui::Text("Damage: "); ImGui::SameLine(); ImGui::Text(std::to_string(damage).c_str());
				ImGui::Text("Health: "); ImGui::SameLine(); ImGui::Text(std::to_string(rest).c_str());
			}
		}
		ImGui::End();
	}
}

void misc::set_sky_box(C_BasePlayer* local_player, const std::string sky_box_name) 
{
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (!local_player)
		return;

	const auto o_set_sky_box = reinterpret_cast<void(__fastcall*)(const char*)>(g_cheat.custom_sky_box);
	o_set_sky_box(sky_box_name.c_str());
}

void misc::ragdoll() 
{

	if (!g_Options.ragdolls)
		return;

	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	for (int i = 1; i <= g_EntityList->GetHighestEntityIndex(); i++) {
		const auto ragdoll = reinterpret_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));

		if (!ragdoll || ragdoll->IsDormant())
			continue;

		if (ragdoll->GetClientClass()->m_ClassID != classids::get("CCSRagdoll"))
			continue;

		Utils::ConsolePrint("checks passed\n");

		ragdoll->get_force() *= 1000.f;
		ragdoll->get_ragdoll_velocity() *= 1000.f;
	}
}

void misc::skybox_changer()
{
	static auto skyname_cvar = g_CVar->FindVar("sv_skyname");

	std::string skybox = "";
	switch (g_Options.skybox_num)
	{
	case 1:
		skybox = "cs_tibet";
		break;
	case 2:
		skybox = "cs_baggage_skybox_";
		break;
	case 3:
		skybox = "italy";
		break;
	case 4:
		skybox = "jungle";
		break;
	case 5:
		skybox = "office";
		break;
	case 6:
		skybox = "sky_cs15_daylight01_hdr";
		break;
	case 7:
		skybox = "sky_cs15_daylight02_hdr";
		break;
	case 8:
		skybox = "vertigoblue_hdr";
		break;
	case 9:
		skybox = "vertigo";
		break;
	case 10:
		skybox = "sky_day02_05_hdr";
		break;
	case 11:
		skybox = "nukeblank";
		break;
	case 12:
		skybox = "sky_venice";
		break;
	case 13:
		skybox = "sky_cs15_daylight03_hdr";
		break;
	case 14:
		skybox = "sky_cs15_daylight04_hdr";
		break;
	case 15:
		skybox = "sky_csgo_cloudy01";
		break;
	case 16:
		skybox = "sky_csgo_night02";
		break;
	case 17:
		skybox = "sky_csgo_night02b";
		break;
	case 18:
		skybox = "sky_csgo_night_flat";
		break;
	case 19:
		skybox = "sky_dust";
		break;
	case 20:
		skybox = "vietnam";
		break;
	}

	static auto r_3dsky = g_CVar->FindVar("r_3dsky");
	r_3dsky->SetValue(false);
	if (g_Options.skybox_num <= 0)
		r_3dsky->SetValue(true);

	return set_sky_box(g_LocalPlayer, g_Options.skybox_num > 0 ? skybox : skyname_cvar->GetString());
}

void rotate_movement(CUserCmd* cmd, float y)
{
	QAngle viewangles;
	g_EngineClient->GetViewAngles(&viewangles);

	float rotation = DEG2RAD(viewangles.y - y);

	float cos_rot = cos(rotation);
	float sin_rot = sin(rotation);

	float new_forwardmove = cos_rot * cmd->forwardmove - sin_rot * cmd->sidemove;
	float new_sidemove = sin_rot * cmd->forwardmove + cos_rot * cmd->sidemove;

	cmd->forwardmove = new_forwardmove;
	cmd->sidemove = new_sidemove;
}


void misc::anti_afk_kick(CUserCmd* cmd)
{
	if (g_Options.misc_antiafk_kick && cmd->command_number % 2)
		cmd->buttons |= 1 << 27;
}

void misc::auto_strafe(CUserCmd* cmd)
{
	if (!g_Options.strafe_type)
		return;

	if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
		return;

	if (g_LocalPlayer->m_nMoveType() == MOVETYPE_NOCLIP)
		return;

	static auto cl_sidespeed = g_CVar->FindVar("cl_sidespeed");
	auto side_speed = cl_sidespeed->GetFloat();
	static auto old_y = cmd->viewangles.y;

	if (g_Options.strafe_type == 1)
	{
		QAngle engine_angles;
		g_EngineClient->GetViewAngles(&engine_angles);

		auto velocity = g_LocalPlayer->m_vecVelocity();

		if (5850.0f / velocity.Length2D() > side_speed)
			cmd->forwardmove = side_speed;
		else
			cmd->forwardmove = 5850.0f / velocity.Length2D();
	
		cmd->sidemove = cmd->command_number % 2 ? side_speed : -side_speed;

		auto y_velocity = Math::CalcAngle(Vector(0.0f, 0.0f, 0.0f), velocity).y;
		auto ideal_rotation = Math::clamp(RAD2DEG(atan2(15.0f, velocity.Length2D())), 0.0f, 45.0f);

		auto y_rotation = fabs(y_velocity - engine_angles.y) + (cmd->command_number % 2 ? ideal_rotation : -ideal_rotation);
		auto ideal_y_rotation = y_rotation < 5.0f ? y_velocity : engine_angles.y;

		rotate_movement(cmd, ideal_y_rotation);
	}

	if (g_Options.strafe_type == 2)
	{
		static auto old_y = 0.0f;

		auto get_velocity_degree = [](float velocity)
		{
			auto tmp = RAD2DEG(atan(15.0f / velocity));

			if (CheckIfNonValidNumber(tmp) || tmp > 90.0f)
				return 90.0f;

			else if (tmp < 0.0f)
				return 0.0f;
			else
				return tmp;
		};


		auto velocity = g_LocalPlayer->m_vecVelocity();
		velocity.z = 0.0f;

		auto forwardmove = cmd->forwardmove;
		auto sidemove = cmd->sidemove;

		if (velocity.Length2D() < 15.0f && !forwardmove && !sidemove)
			return;

		static auto flip = false;
		flip = !flip;

		auto turn_direction_modifier = flip ? 1.0f : -1.0f;
		auto viewangles = cmd->viewangles;

		if (forwardmove || sidemove)
		{
			cmd->forwardmove = 0.0f;
			cmd->sidemove = 0.0f;

			auto turn_angle = atan2(-sidemove, forwardmove);
			viewangles.y += turn_angle * M_RADPI;
		}
		else if (forwardmove)
			cmd->forwardmove = 0.0f;

		auto strafe_angle = RAD2DEG(atan(15.0f / velocity.Length2D()));

		if (strafe_angle > 90.0f)
			strafe_angle = 90.0f;
		else if (strafe_angle < 0.0f)
			strafe_angle = 0.0f;

		auto temp = Vector(0.0f, viewangles.y - old_y, 0.0f);
		temp.y = Math::normalize_yaw(temp.y);

		auto y_delta = temp.y;
		old_y = viewangles.y;

		auto abs_y_delta = fabs(y_delta);

		if (abs_y_delta <= strafe_angle || abs_y_delta >= 15.0f)
		{
			QAngle velocity_angles;
			Math::VectorAngles(velocity, velocity_angles);

			temp = Vector(0.0f, viewangles.y - velocity_angles.y, 0.0f);
			temp.y = Math::normalize_yaw(temp.y);

			auto velocityangle_ydelta = temp.y;
			auto velocity_degree = get_velocity_degree(velocity.Length2D());

			if (velocityangle_ydelta <= velocity_degree || velocity.Length2D() <= 15.0f)
			{
				if (-velocity_degree <= velocityangle_ydelta || velocity.Length2D() <= 15.0f)
				{
					viewangles.y += strafe_angle * turn_direction_modifier;
					cmd->sidemove = side_speed * turn_direction_modifier;
				}
				else
				{
					viewangles.y = velocity_angles.y - velocity_degree;
					cmd->sidemove = side_speed;
				}
			}
			else
			{
				viewangles.y = velocity_angles.y + velocity_degree;
				cmd->sidemove = -side_speed;
			}
		}
		else if (y_delta > 0.0f)
			cmd->sidemove = -side_speed;
		else if (y_delta < 0.0f)
			cmd->sidemove = side_speed;

		auto move = Vector(cmd->forwardmove, cmd->sidemove, 0.0f);
		auto speed = move.Length();

		QAngle angles_move;
		Math::VectorAngles(move, angles_move);

		auto normalized_x = fmod(cmd->viewangles.x + 180.0f, 360.0f) - 180.0f;
		auto normalized_y = fmod(cmd->viewangles.y + 180.0f, 360.0f) - 180.0f;

		auto y = DEG2RAD(normalized_y - viewangles.y + angles_move.y);

		if (normalized_x >= 90.0f || normalized_x <= -90.0f || cmd->viewangles.x >= 90.0f && cmd->viewangles.x <= 200.0f || cmd->viewangles.x <= -90.0f && cmd->viewangles.x <= 200.0f)
			cmd->forwardmove = -cos(y) * speed;
		else
			cmd->forwardmove = cos(y) * speed;

		cmd->sidemove = sin(y) * speed;
	}
}

void misc::spectator_list()
{
	if (!g_Options.misc_spectator_list) return;
 
	std::string specs = "";
	std::string modes = "";

	if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected()) {
		C_BasePlayer* pLocalEntity = g_LocalPlayer;

		if (g_LocalPlayer && !g_LocalPlayer->IsAlive())
			pLocalEntity = g_LocalPlayer->m_hObserverTarget();

		if (pLocalEntity) {
			for (int i = 0; i < g_EngineClient->GetMaxClients(); i++) {
				C_BasePlayer* pBaseEntity = C_BasePlayer::GetPlayerByIndex(i);
				if (!pBaseEntity)										     continue;
				if (pBaseEntity->m_iHealth() > 0)							 continue;
				if (pBaseEntity == pLocalEntity)							 continue;
				if (pBaseEntity->IsDormant())								 continue;
				if (pBaseEntity->m_hObserverTarget() != pLocalEntity)		 continue;
				player_info_t pInfo;
				g_EngineClient->GetPlayerInfo(pBaseEntity->EntIndex(), &pInfo);
				if (pInfo.ishltv) continue;

				std::string mode = "unknown";
				switch (pBaseEntity->m_iObserverMode())
				{
				case OBS_MODE_CHASE:
					mode = ("3rd Person");
					break;
				case OBS_MODE_DEATHCAM:
					mode = ("DeathCams");
					break;
				case OBS_MODE_FIXED:
					mode = ("Fixed");
					break;
				case OBS_MODE_FREEZECAM:
					mode = ("FreezeCam");
					break;
				case OBS_MODE_IN_EYE:
					mode = ("1st Person");
					break;
				case OBS_MODE_NONE:
					mode = ("None");
					break;
				case OBS_MODE_ROAMING:
					mode = ("Roaming");
					break;
				}

				specs += pInfo.szName;
				specs += "\n";

				modes += mode;
				modes += "\n";
			}
		}
	}
	if (specs.size() > 0) specs += "\n";

	if (Menu::Get().IsVisible() || specs.size() > 0)
	{
		if (ImGui::Begin("Spectator List", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize)) {

			ImVec2 pWsize = ImGui::CalcTextSize(specs.c_str());
			ImGui::SetWindowSize(ImVec2(200, 35 + pWsize.y));
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 2.f);

			ImGui::BeginGroup();
			ImGui::Columns(2, nullptr, false);

			ImGui::Text(specs.c_str());
			ImGui::NextColumn();
			ImGui::Text(modes.c_str());

			ImGui::EndGroup();
		}
		ImGui::End();
	}
}

void misc::custom_shadows()
{
	static auto override_mode = g_CVar->FindVar("cl_csm_rot_override");
	static auto cl_csm_rot_x = g_CVar->FindVar("cl_csm_rot_x");
	static auto cl_csm_rot_y = g_CVar->FindVar("cl_csm_rot_y");
	static auto cl_csm_rot_z = g_CVar->FindVar("cl_csm_rot_z");

	static float x = cl_csm_rot_x->GetFloat();
	static float y = cl_csm_rot_y->GetFloat();
	static float z = cl_csm_rot_z->GetFloat();

	if (g_Options.custom_shadows && !g_Options.remove_shadows)
	{
		override_mode->SetValue(1);
		Utils::modern_clamp_shadows(x, g_Options.shadows_x, g_GlobalVars->frametime * 48.f);
		Utils::modern_clamp_shadows(y, g_Options.shadows_y, g_GlobalVars->frametime * 48.f);
		Utils::modern_clamp_shadows(z, g_Options.shadows_z, g_GlobalVars->frametime * 48.f);

		cl_csm_rot_x->SetValue(x);
		cl_csm_rot_y->SetValue(y);
		cl_csm_rot_z->SetValue(z);

	}
	else
	{
		override_mode->SetValue(0);
		Utils::modern_clamp_shadows(x, 0, g_GlobalVars->frametime * 48.f);
		Utils::modern_clamp_shadows(y, 0, g_GlobalVars->frametime * 48.f);
		Utils::modern_clamp_shadows(z, 0, g_GlobalVars->frametime * 48.f);
	}

}
int get_fps()
{
	using namespace std::chrono;
	static int count = 0;
	static auto last = high_resolution_clock::now();
	auto now = high_resolution_clock::now();
	static int fps = 0;

	count++;

	if (duration_cast<milliseconds>(now - last).count() > 1000) {
		fps = count;
		count = 0;
		last = now;
	}

	return fps;
}

void misc::watermark() 
{
	if (!g_Options.misc_watermark)
		return;

	time_t lt;
	struct tm* t_m;
	lt = time(NULL);
	t_m = localtime(&lt);

	int time_h = t_m->tm_hour;
	int time_m = t_m->tm_min;
	int time_s = t_m->tm_sec;
	int w, h;
	g_EngineClient->GetScreenSize(w, h);
	std::string time;

	if (time_h < 10)
		time += "0";

	time += std::to_string(time_h) + ":";

	if (time_m < 10)
		time += "0";

	time += std::to_string(time_m) + ":";

	if (time_s < 10)
		time += "0";

	time += std::to_string(time_s);

	std::string delay = "0";
	std::string tickrate = "0";

	if (g_EngineClient->IsInGame())
	{
		auto nci = g_EngineClient->GetNetChannelInfo();
		auto latency = nci && g_EngineClient->IsPlayingDemo() ? nci->GetAvgLatency(0) : 0.0f;

		static auto cl_updaterate = g_CVar->FindVar("cl_updaterate");
		latency -= 0.5f / cl_updaterate->GetFloat();

		if (0.0f > latency)
			delay = std::to_string((int)(0.0f * 1000.0f));
		else
			delay = std::to_string((int)(latency* 1000.0f));

		tickrate = std::to_string((int)(1.0f / g_GlobalVars->interval_per_tick));
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(7, 5));
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.f);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5.f, 5.f));
	ImGuiStyle* Style = &ImGui::GetStyle();
	if (ImGui::Begin("Watermark", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {

		ImVec2 p = ImGui::GetCursorScreenPos();
		ImGui::SetCursorPosY(3);
		ImGui::SetCursorPosX(10);
	
		ImGui::Text("Umbrella");
	
		ImGui::SameLine();
		ImGui::Text(" ");
		ImGui::SameLine();
		ImGui::Text(time.c_str());
		ImGui::SameLine();
		ImGui::TextColored(ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f }, " FPS: %03d", get_fps());
		if (g_EngineClient->IsInGame())
		{
			ImGui::SameLine();
			ImGui::Text(" Ping");
			ImGui::SameLine();
			ImGui::Text(delay.c_str());
			ImGui::SameLine();
			ImGui::Text(" Rate");
			ImGui::SameLine();
			ImGui::Text(tickrate.c_str());
		}
		ImGui::SetWindowPos(ImVec2(w - ImGui::GetWindowSize().x - 7.f, 7.f));
	}
	ImGui::End();
	ImGui::PopStyleVar(3);
}