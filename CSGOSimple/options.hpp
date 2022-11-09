#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include "valve_sdk/Misc/Color.hpp"
#include <array>

struct C_ChamsSettings
{
	int32_t m_iMainMaterial = 0;

	bool m_bRenderChams = false;
	bool m_aModifiers[4] = { false, false, false };

	Color m_aModifiersColors[4] = { { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 } };
	Color m_Color = Color(255, 255, 255, 255);
};

struct legitbot_s 
{
	bool legit_nearest = false;
	bool legit_psilent_enable = false;
	bool legit_teamattack = false;
	bool legit_checks = false;
	bool legit_aimlock = false;
	bool legit_autofire = false;
	bool legit_dynamic_fov = false;
	bool legit_autowall = false;

	bool legit_hitbox[3] = { false, false, false };
	int legit_rcs_x = 100;
	int legit_rcs_y = 100;
	int legit_psilent_type = 0;

	float legit_fov = 6.f;
	float legit_smooth = 8.f;
	float legit_fov_after = 7.f;
	float legit_smooth_after = 10.f;
	float legit_psilent_fov = 0.f;
	float legit_delay_after_kill = 0.f;
	float legit_delay_shot = 0.f;
	float legit_mindamage = 0.f;
};

struct triggerbot_s 
{
	bool enable = false;
	bool legit_teamattack = false;
	bool legit_checks = false;
	bool only_in_scope = false;

	int hitgroup = 0;
	int trigger_chance = 100;
	int minimal_damage = 10;
};

struct c_esp_settings
{
	bool enable = false;
	bool only_visible = false;
	bool dormant = false;

	bool esp_box = false;
	//int box_style = 0;
	bool outline_box = false;
	bool filled_box = false;
	bool gradient_fade = false;

	bool esp_names = false;
	float esp_names_size = 16.f;

	bool esp_health = false;
	bool esp_health_custom = false;
	int  esp_health_pos = 0;

	bool esp_armor = false;
	bool esp_armor_custom = false;
	int  esp_armor_pos = 1;

	bool oof = false;
	int oof_style = 1;

	bool sound_arrows = false;
	int sound_arrows_style = 1;
	bool sound_arrows_only_visible = false;

	bool esp_health_text = false;
	float esp_health_size = 16.f;

	bool esp_armor_text = false;
	float esp_armor_size = 16.f;

	bool esp_ammo_text = false;
	float esp_ammo_size = 16.f;

	bool esp_flashed = false;
	float esp_flashed_size = 16.f;

	bool esp_scoped = false;
	float esp_scoped_size = 16.f;

	bool esp_defuser = false;
	float esp_defuser_size = 16.f;

	bool esp_reloading = false;
	float esp_reloading_size = 16.f;

	bool esp_weapon_ico = false;
	float esp_weapon_ico_size = 16.f;

	bool esp_weapon = false;
	float esp_weapon_size = 16.f;

	bool esp_money = false;
	float esp_money_size = 16.f;

	bool esp_fakeduck = false;
	float esp_fakeduck_size = 16.f;

	bool skeleton = false;

	bool sound_esp = false;

	float box_clr_vis[3] = { 0.f,1.f,0.f };
	float box_clr_invis[3] = { 0.f,0.f,1.f };

	float health_bar_clr[3] = { 0.f,1.f,0.f };

	float oof_clr[3] = { 1.f,1.f,1.f };
	float sound_arrow_clr[3] = { 1.f,1.f,1.f };
	

	float skeleton_clr[3] = { 1.f,1.f,1.f };

	float sound_clr[3] = { 0.f,0.f,1.f};

	float names_clr[3] = { 0.f,1.f,0.f };
	float flashed_clr[3] = { 0.f,1.f,0.f };
	float health_clr[3] = { 0.f,1.f,0.f};
	float ammo_clr[3] = { 0.f,1.f,0.f };
	float reloading_clr[3] = { 0.f,1.f,0.f };
	float armor_clr[3] = { 0.f,1.f,0.f};
	float money_clr[3] = { 0.f,1.f,0.f,};
	float weapon_ico_clr[3] = { 0.f,1.f,0.f};
	float weapon_clr[3] = { 0.f,1.f,0.f};
	float scoped_clr[3] = { 0.f,1.f,0.f };
	float defuser_clr[3] = { 0.f,1.f,0.f };
	float fakeduck_clr[3] = { 0.f,1.f,0.f };
};

class Options
{
public:
		bool trigger_only_in_scope = false;
		int trigget_key = 0;
		int trigger_mode = 0;

		bool legit_enable = false;
		bool legit_autoselectweapon = true;
		bool legit_resize_fov =false;
		bool legit_onlyscope =false;
		int fov_draw_type =0;
		bool visuals_draw_fov =false;

		std::array<legitbot_s, 34> legitbot_for_weapon = { };
		std::array<triggerbot_s, 34> trigger_for_weapon = { };
		std::array <c_esp_settings, 3 > esp_settings = { };
		std::array < C_ChamsSettings, 10 > m_aChamsSettings = { };

		C_ChamsSettings grenade_chams;
		C_ChamsSettings weapon_chams;

		bool m_bDrawRagdolls = false;
		float fov_color[3] = { 0.5f, 0.5f, 0.5f };
		// 
		// GLOW
		// 
		bool glow_enabled =false;
		bool glow_enemies_only =false;
		bool glow_players =false;
		bool glow_chickens =false;
		bool glow_c4_carrier =false;
		bool glow_planted_c4 =false;
		bool glow_defuse_kits =false;
		bool glow_weapons =false;
		//
		// MISC
		//
		bool edge_jump =false;

		int edge_jump_key =false;
		int edge_jump_mode =false;

		bool auto_accept =false;
		bool bomb_timer =false;
		int skybox_num =0;
		int ragdolls =0;
		bool misc_no_smoke =false;
		bool misc_no_flash =false;
		bool misc_no_post_processing =false;
		bool remove_panorama_blur =false;
		bool remove_scope =false;
		bool remove_shadows =false;

		bool fov_changer =false;
		float camera_fov =90.f;

		bool view_offsets =false;
		float viewmodel_x =0.f;
		float viewmodel_y =0.f;
		float viewmodel_z =0.f;
		float viewmodel_roll =0.f;

		int strafe_type =0;
		bool misc_bhop =false;
		int bhop_chance =0;
		int max_bhop_limit_max =0;
		int max_bhop_limit_min= 0;
		
		bool misc_spectator_list =false;
		bool misc_binds_list =false;

		bool misc_thirdperson =false;
		bool misc_showranks =true;
		bool misc_force_crosshair =true;
		bool misc_antiafk_kick =false;

		int third_person_key = 0;
		int third_person_mode = 0;

		bool grenade_projectiles = false;
		bool grenade_prediction =false;
		bool grenade_timers =false;
		bool grenade_box = false;
		bool grenade_text = false;
		bool grenade_icon = false;

		float grenade_text_size = 16.f;
		float grenade_icon_size = 16.f;

		bool dropped_weapon = false;
		bool dropped_weapon_box = false;
		bool dropped_weapon_text = false;
		bool dropped_weapon_icon = false;
		bool dropped_weapon_ammo = false;

		float dropped_weapon_text_size = 16.f;
		float dropped_weapon_icon_size = 16.f;
		float dropped_weapon_ammo_size = 16.f;

		bool do_saspect_ratio =false;
		float aspect_ratio_value =1.3f;

		bool misc_watermark =true;
		float misc_thirdperson_dist =50.f;

		bool misc_world_color =true;

		bool misc_damage_indicator =true;
		float damage_indicator_size =20.f;
		float damage_indicator_speed =1.2f;

		bool misc_hit_marker_1 =true;
		bool misc_hit_marker_2 =true;

		int misc_hitsound = 0;

		bool misc_hiteffect =false;
		float misc_hiteffect_duration =0.5f;

		bool misc_post_processing =false;
		bool custom_shadows =false;

		float shadows_x =0.f;
		float shadows_y =0.f;
		float shadows_z =0.f;

		char custom_hit_sound[128];
		//
		// RADAR
		//
		bool radar_enable =false;
		bool radar_enemy_only =false;
		bool radar_invisible =false;
		bool radar_texture =false;
		bool radar_grenades =false;
		int radar_range =1890;
		int radar_size =250;

		bool fog_enable = false;
		float fog_start = 100;
		float fog_end = 100;
		float fog_density = 0;

		float fog_color[3] = { 0.5f, 0.5f, 0.5f };

		float color_radar_ct_visible[3] = { 0.5f, 0.5f, 0.5f };
		float color_radar_ct_invisible[3] = { 0.5f, 0.5f, 0.5f };
		float color_radar_t_visible[3] = { 0.5f, 0.5f, 0.5f };
		float color_radar_t_invisible[3] = { 0.5f, 0.5f, 0.5f };
		float color_radar_grenades[3] = { 0.5f, 0.5f, 0.5f };

		bool hit_arrow = false;
		int hit_arrow_style = 1;
		float hit_arrow_clr[3] = { 1.f,1.f,1.f };
		bool hit_arrow_damage = false;
		bool hit_arrow_weapon = false;
		bool hit_arrow_only_visible = false;

		bool safe_mode = false;

		bool buy_bot = false;
		int primary_buy_bot = 0;
		int secondary_buy_bot = 0;
		bool equipment_buy_bot[9] = {};
		int buy_bot_active_buy_delay = 0;
		int buy_bot_first_buy_delay = 0;
		// 
		// COLORS
		// 
		float color_grenade_trajectory[3] = { 0.5f, 0.5f, 0.5f };
		float color_grenade_circle[3] = { 0.5f, 0.5f, 0.5f };
		float color_grenade_box[3] = { 0.5f, 0.5f, 0.5f };
		float color_grenade_text[3] = { 0.5f, 0.5f, 0.5f };
		float color_grenade_icon[3] = { 0.5f, 0.5f, 0.5f };

		float color_dropped_weapon_box[3] = { 0.5f, 0.5f, 0.5f };
		float color_dropped_weapon_text[3] = { 0.5f, 0.5f, 0.5f };
		float color_dropped_weapon_icon[3] = { 0.5f, 0.5f, 0.5f };
		float color_dropped_weapon_ammo[3] = { 0.5f, 0.5f, 0.5f };

		float color_walls[3] = { 0.5f, 0.5f, 0.5f };
		float color_props[3] = { 0.5f, 0.5f, 0.5f };
		float color_sky[3] = { 0.5f, 0.5f, 0.5f };

		Color color_glow_ally= Color(0, 128, 255);
		Color color_glow_enemy= Color(255, 0, 0);
		Color color_glow_chickens= Color(0, 128, 0);
		Color color_glow_c4_carrier= Color(255, 255, 0);
		Color color_glow_planted_c4= Color(128, 0 ,128);
		Color color_glow_defuse= Color(255, 255 ,255);
		Color color_glow_weapons= Color(255, 128, 0);

		float color_damage_indicator[3] = { 0.f, 1.f, 0.f };
		float color_hit_marker_1[3] = { 1.f, 1.f, 1.f };
		float color_hit_marker_2[3] = {1.f, 1.f, 1.f};

		float color_post_processing[3] = { 1.f, 1.f, 1.f };
};

inline Options g_Options;
inline bool   g_Unload;
