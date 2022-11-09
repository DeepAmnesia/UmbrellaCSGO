#include "radar.h"
#include "../../menu.hpp"
#include "../../options.hpp"

ImGuiWindowFlags findow_flag_closed = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
ImGuiWindowFlags findow_flag = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;

void radar::calc_radar_point(C_BasePlayer* local_player, Vector origin, float& x, float& y) 
{
	Vector angle;

	g_EngineClient->GetViewAngles(&angle);

	float range = ((float)g_Options.radar_range) / 1000.f;

	ImVec2 draw_pos = ImGui::GetWindowPos();
	ImVec2 draw_size = ImGui::GetWindowSize();

	float r_1, r_2;
	float x_1, y_1;

	Vector eye_origin = local_player->get_eye_position();

	r_1 = -(origin.y - eye_origin.y);
	r_2 = origin.x - eye_origin.x;

	float yaw_to_radian = (float)(angle.y - 90.0f) * (float)(3.14159265358979323846 / 180.0F);
	x_1 = (float)(r_2 * (float)cos((double)(yaw_to_radian)) - r_1 * sin((double)(yaw_to_radian))) / 20.f;
	y_1 = (float)(r_2 * (float)sin((double)(yaw_to_radian)) + r_1 * cos((double)(yaw_to_radian))) / 20.f;

	x_1 *= range;
	y_1 *= range;

	float sizX = draw_size.x / 2, sizY = draw_size.y / 2;

	x_1 += sizX;
	y_1 += sizY;

	x_1 += draw_pos.x;
	y_1 += draw_pos.y;

	x = x_1;
	y = y_1;
}

radar::map_info radar::get_map() 
{
	return { Vector(map_origin.x, map_origin.y, 0.f), Vector(float(map_scale * 1000 + 100), float(map_scale * 1000 + 100), 0.f) };
}

void radar::draw_map(C_BasePlayer* local_player) 
{
	if (!g_Options.radar_texture)
		return;

	if (!is_valid())
		return;

	map_info current_map = get_map();

	ImDrawList* draw = ImGui::GetWindowDrawList();

	float h = current_map.size.y, w = current_map.size.x;

	ImVec2 map_start_pos(current_map.pos.x, current_map.pos.y);

	ImVec2 map_point_1(map_start_pos.x + w, map_start_pos.y);
	ImVec2 map_point_2(map_start_pos.x + w, map_start_pos.y - h);
	ImVec2 map_point_3(map_start_pos.x, map_start_pos.y - h);

	calc_radar_point(local_player, Vector(map_start_pos.x, map_start_pos.y, 0), map_start_pos.x, map_start_pos.y);
	calc_radar_point(local_player, Vector(map_point_1.x, map_point_1.y, 0), map_point_1.x, map_point_1.y);
	calc_radar_point(local_player, Vector(map_point_2.x, map_point_2.y, 0), map_point_2.x, map_point_2.y);
	calc_radar_point(local_player, Vector(map_point_3.x, map_point_3.y, 0), map_point_3.x, map_point_3.y);

	ImVec2 uv_a(0, 0);
	ImVec2 uv_c(1, 1);
	ImVec2 uv_b(1, 0);
	ImVec2 uv_d(0, 1);

	draw->PushTextureID(map_image);
	draw->PrimReserve(6, 4);
	draw->PrimQuadUV(map_point_1, map_point_2, map_point_3, map_start_pos, uv_b, uv_c, uv_d, uv_a, 0xFFFFFFFF);
	draw->PopTextureID();
}

void radar::load_map(long map_overview) 
{
	map_origin.x = *(float*)(map_overview + 0x100);
	map_origin.y = *(float*)(map_overview + 0x104);
	map_scale = *(float*)(map_overview + 0x114);
}

void radar::draw_player(C_BasePlayer* local_player) 
{
	if (local_player)
	{
		ImDrawList* draw = ImGui::GetWindowDrawList();

		float x = 0, y = 0;

		for (int i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++) {
			const auto entity = reinterpret_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));

			if (!entity)
				continue;

			calc_radar_point(local_player, entity->m_vecOrigin(), x, y);

			if (g_Options.radar_grenades && is_grenade(entity))
			{
				Color clr = Color(g_Options.color_radar_grenades);
				draw->AddRectFilled(ImVec2((float)x, (float)y), ImVec2((float)x + 4, (float)y + 4), Render::Get().GetU32(clr));
			}
		}

		for (int i = 1; i <= g_EntityList->GetHighestEntityIndex(); i++) {
			const auto entity = reinterpret_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));

			if (!entity)
				continue;

			if (i <= g_GlobalVars->maxClients) {
				if (entity->IsDormant() || entity->GetClientClass()->m_ClassID != ClassId_CCSPlayer)
					continue;

				if (!entity->IsAlive())
					continue;

				if (entity->is_teammate(local_player) && g_Options.radar_enemy_only)
					continue;

				bool is_visible = local_player->is_visible(entity, HITBOX_CHEST);

				if (!g_Options.radar_invisible && !is_visible)
					continue;

				static Color color;

				calc_radar_point(local_player, entity->m_vecOrigin(), x, y);

				if (entity->m_iTeamNum() == 2)
					color = is_visible ? Color(g_Options.color_radar_t_visible) : Color(g_Options.color_radar_t_invisible);
				else if (entity->m_iTeamNum() == 3)
					color = is_visible ? Color(g_Options.color_radar_ct_visible) : Color(g_Options.color_radar_ct_invisible);

				draw->AddRectFilled({ x - 3.f, y - 3.f }, { x + 3.f, y + 3.f }, ImColor(color.r(), color.g(), color.b(), color.a()));
				draw->AddRect({ x - 4.f, y - 4.f }, { x + 4.f, y + 4.f }, ImColor(0, 0, 0));
			}
		}
	}
}

void radar::render(C_BasePlayer* local_player) 
{
	if (!g_Options.radar_enable)
		return;

	ImGui::SetNextWindowSize({ (float)g_Options.radar_size, (float)g_Options.radar_size });
	ImGui::Begin("radar", NULL, Menu::Get().IsVisible() ? findow_flag : findow_flag_closed); {
		ImDrawList* draw = ImGui::GetWindowDrawList();
		ImVec2 draw_pos = ImGui::GetCursorScreenPos(), draw_size = ImGui::GetContentRegionAvail();

		if (g_EngineClient->IsInGame() && local_player && local_player->IsAlive())
		{
			draw_map(local_player);
			draw_player(local_player);
		}

		draw->AddCircleFilled(ImVec2(draw_pos.x + (float)g_Options.radar_size / 2.f, draw_pos.y + (float)g_Options.radar_size / 2.f), 4, ImColor(1.f, 1.f, 1.f, 1.f), 20);
		draw->AddCircle(ImVec2(draw_pos.x + (float)g_Options.radar_size / 2.f, draw_pos.y + (float)g_Options.radar_size / 2.f), 4, ImColor(0.f, 0.f, 0.f, 1.f), 20, 1.f);
	}
	ImGui::End();
}
