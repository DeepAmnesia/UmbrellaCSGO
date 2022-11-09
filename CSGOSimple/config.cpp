#include "options.hpp"
#include "singleton.hpp"
#include <Windows.h>
#include <iomanip>
#include "config.hpp"

std::unordered_map <std::string, float[4]> colors;

void C_ConfigManager::Setup()
{
	for (int i = 0; i < 34; i++)
	{
		setup_item(&g_Options.legitbot_for_weapon[i].legit_aimlock, false, "legit.legit_aimlock." + std::to_string(i));
		setup_item(&g_Options.legitbot_for_weapon[i].legit_autofire, false, "legit.legit_autofire." + std::to_string(i));
		setup_item(&g_Options.legitbot_for_weapon[i].legit_autowall, false, "legit.legit_autowall." + std::to_string(i));
		setup_item(&g_Options.legitbot_for_weapon[i].legit_checks, false, "legit.legit_checks." + std::to_string(i));
		setup_item(&g_Options.legitbot_for_weapon[i].legit_dynamic_fov, false, "legit.legit_dynamic_fov." + std::to_string(i));
		setup_item(&g_Options.legitbot_for_weapon[i].legit_nearest, false, "legit.legit_nearest." + std::to_string(i));
		setup_item(&g_Options.legitbot_for_weapon[i].legit_psilent_enable, false, "legit.legit_psilent_enable." + std::to_string(i));
		setup_item(&g_Options.legitbot_for_weapon[i].legit_teamattack, false, "legit.legit_teamattack." + std::to_string(i));
		setup_item(&g_Options.legitbot_for_weapon[i].legit_delay_after_kill, 0, "legit.legit_delay_after_kill." + std::to_string(i));
		setup_item(&g_Options.legitbot_for_weapon[i].legit_delay_shot, 0, "legit.legit_delay_shot." + std::to_string(i));
		setup_item(&g_Options.legitbot_for_weapon[i].legit_psilent_type, 0, "legit.legit_psilent_type." + std::to_string(i));
		setup_item(&g_Options.legitbot_for_weapon[i].legit_fov, 6, "legit.legit_fov." + std::to_string(i));
		setup_item(&g_Options.legitbot_for_weapon[i].legit_fov_after, 7, "legit.legit_fov_after." + std::to_string(i));
		setup_item(&g_Options.legitbot_for_weapon[i].legit_mindamage, 0, "legit.legit_mindamage." + std::to_string(i));
		setup_item(&g_Options.legitbot_for_weapon[i].legit_psilent_fov, 0, "legit.legit_psilent_fov." + std::to_string(i));
		setup_item(&g_Options.legitbot_for_weapon[i].legit_rcs_x, 100, "legit.legit_rcs_x." + std::to_string(i));
		setup_item(&g_Options.legitbot_for_weapon[i].legit_rcs_y, 100, "legit.legit_rcs_y." + std::to_string(i));
		setup_item(&g_Options.legitbot_for_weapon[i].legit_smooth, 8, "legit.legit_smooth." + std::to_string(i));
		setup_item(&g_Options.legitbot_for_weapon[i].legit_smooth_after, 10, "legit.legit_smooth_after." + std::to_string(i));

		setup_item(&g_Options.legitbot_for_weapon[i].legit_hitbox[0], false, "legit.legit_hitbox[0]." + std::to_string(i));
		setup_item(&g_Options.legitbot_for_weapon[i].legit_hitbox[1], false, "legit.legit_hitbox[1]." + std::to_string(i));
		setup_item(&g_Options.legitbot_for_weapon[i].legit_hitbox[2], false, "legit.legit_hitbox[2]." + std::to_string(i));
	}

	for (int i = 0; i < 34; i++)
	{
		setup_item(&g_Options.trigger_for_weapon[i].enable, false, "trigger.enable." + std::to_string(i));
		setup_item(&g_Options.trigger_for_weapon[i].hitgroup, 0, "trigger.hitgroup." + std::to_string(i));
		setup_item(&g_Options.trigger_for_weapon[i].minimal_damage, 0, "trigger.min_damage." + std::to_string(i));
		setup_item(&g_Options.trigger_for_weapon[i].legit_checks, false, "trigger.legit_checks." + std::to_string(i));
		setup_item(&g_Options.trigger_for_weapon[i].only_in_scope, false, "trigger.only_in_scope." + std::to_string(i));
		setup_item(&g_Options.trigger_for_weapon[i].trigger_chance, 0, "trigger.chance." + std::to_string(i));
		setup_item(&g_Options.trigger_for_weapon[i].legit_teamattack, false, "legit.legit_teamattack." + std::to_string(i));
	}

	for (int i = 0; i < 3; i++)
	{
		setup_item(&g_Options.esp_settings[i].enable, false, "esp.enable." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].only_visible, false, "esp.only_visible." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].dormant, false, "esp.dormant." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].esp_box, false, "esp.esp_box." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].esp_names, false, "esp.esp_names." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].esp_weapon_ico, false, "esp.esp_weapon_ico." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].esp_weapon, false, "esp.esp_weapon." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].esp_ammo_text, false, "esp.esp_ammo_text." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].esp_health_text, false, "esp.esp_health_text." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].esp_money, false, "esp.esp_money." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].esp_flashed, false, "esp.esp_flashed." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].esp_reloading, false, "esp.esp_reloading." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].esp_scoped, false, "esp.esp_scoped." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].esp_armor_text, false, "esp.esp_armor_text." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].esp_defuser, false, "esp.esp_defuser." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].skeleton, false, "esp.skeleton." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].esp_health, false, "esp.esp_health." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].sound_esp, false, "esp.sound_esp." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].filled_box, false, "esp.filled_box." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].gradient_fade, false, "esp.gradient_fade." + std::to_string(i));

		setup_item(&g_Options.esp_settings[i].sound_arrows, false, "esp.sound_arrows." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].sound_arrows_style, 0, "esp.sound_arrows_style." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].sound_arrows_only_visible, 0, "esp.sound_arrows_only_visible." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].sound_arrow_clr[0], 1.f, "esp.sound_arrows_clr_vis_r." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].sound_arrow_clr[1], 1.f, "esp.sound_arrows_clr_vis_g." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].sound_arrow_clr[2], 1.f, "esp.sound_arrows_clr_vis_b." + std::to_string(i));

		setup_item(&g_Options.esp_settings[i].box_clr_vis[0], 1.f, "misc.box_clr_vis_r." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].box_clr_vis[1], 1.f, "misc.box_clr_vis_g." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].box_clr_vis[2], 1.f, "misc.box_clr_vis_b." + std::to_string(i));

		setup_item(&g_Options.esp_settings[i].box_clr_invis[0], 1.f, "misc.box_clr_invis_r." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].box_clr_invis[1], 1.f, "misc.box_clr_invis_g." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].box_clr_invis[2], 1.f, "misc.box_clr_invis_b." + std::to_string(i));

		setup_item(&g_Options.esp_settings[i].esp_names_size, 16, "misc.esp_names_size." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].names_clr[0], 1.f, "misc.names_clr_r." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].names_clr[1], 1.f, "misc.names_clr_g." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].names_clr[2], 1.f, "misc.names_clr_b." + std::to_string(i));

		setup_item(&g_Options.esp_settings[i].esp_weapon_ico_size, 16, "misc.esp_weapon_ico_size." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].weapon_ico_clr[0], 1.f, "misc.weapon_ico_clr_r." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].weapon_ico_clr[1], 1.f, "misc.weapon_ico_clr_g." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].weapon_ico_clr[2], 1.f, "misc.weapon_ico_clr_b." + std::to_string(i));

		setup_item(&g_Options.esp_settings[i].esp_weapon_size, 16, "misc.esp_weapon_size." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].weapon_clr[0], 1.f, "misc.weapon_clr_r." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].weapon_clr[1], 1.f, "misc.weapon_clr_g." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].weapon_clr[2], 1.f, "misc.weapon_clr_b." + std::to_string(i));

		setup_item(&g_Options.esp_settings[i].esp_ammo_size, 16, "misc.esp_ammo_size." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].ammo_clr[0], 1.f, "misc.ammo_clr_r." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].ammo_clr[1], 1.f, "misc.ammo_clr_g." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].ammo_clr[2], 1.f, "misc.ammo_clr_b." + std::to_string(i));

		setup_item(&g_Options.esp_settings[i].esp_health_size, 16, "misc.esp_health_size." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].health_clr[0], 1.f, "misc.health_clr_r." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].health_clr[1], 1.f, "misc.health_clr_g." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].health_clr[2], 1.f, "misc.health_clr_b." + std::to_string(i));

		setup_item(&g_Options.esp_settings[i].esp_money_size, 16, "misc.esp_money_size." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].money_clr[0], 1.f, "misc.money_clr_r." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].money_clr[1], 1.f, "misc.money_clr_g." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].money_clr[2], 1.f, "misc.money_clr_b." + std::to_string(i));

		setup_item(&g_Options.esp_settings[i].esp_flashed_size, 16, "misc.esp_flashed_size." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].flashed_clr[0], 1.f, "misc.flashed_clr_r." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].flashed_clr[1], 1.f, "misc.flashed_clr_g." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].flashed_clr[2], 1.f, "misc.flashed_clr_b." + std::to_string(i));

		setup_item(&g_Options.esp_settings[i].esp_reloading_size, 16, "misc.esp_reloading_size." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].reloading_clr[0], 1.f, "misc.reloading_clr_r." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].reloading_clr[1], 1.f, "misc.reloading_clr_g." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].reloading_clr[2], 1.f, "misc.reloading_clr_b." + std::to_string(i));

		setup_item(&g_Options.esp_settings[i].esp_scoped_size, 16, "misc.esp_scoped_size." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].scoped_clr[0], 1.f, "misc.scoped_clr_r." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].scoped_clr[1], 1.f, "misc.scoped_clr_g." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].scoped_clr[2], 1.f, "misc.scoped_clr_b." + std::to_string(i));

		setup_item(&g_Options.esp_settings[i].esp_armor_size, 16, "misc.esp_armor_size." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].armor_clr[0], 1.f, "misc.armor_clr_r." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].armor_clr[1], 1.f, "misc.armor_clr_g." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].armor_clr[2], 1.f, "misc.armor_clr_b." + std::to_string(i));

		setup_item(&g_Options.esp_settings[i].esp_defuser_size, 16, "misc.esp_defuser_size." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].defuser_clr[0], 1.f, "misc.defuser_clr_r." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].defuser_clr[1], 1.f, "misc.defuser_clr_g." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].defuser_clr[2], 1.f, "misc.defuser_clr_b." + std::to_string(i));

		setup_item(&g_Options.esp_settings[i].skeleton_clr[0], 1.f, "misc.skeleton_clr_r." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].skeleton_clr[1], 1.f, "misc.skeleton_clr_g." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].skeleton_clr[2], 1.f, "misc.skeleton_clr_b." + std::to_string(i));

		setup_item(&g_Options.esp_settings[i].sound_clr[0], 1.f, "misc.sound_clr_r." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].sound_clr[1], 1.f, "misc.sound_clr_g." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].sound_clr[2], 1.f, "misc.sound_clr_b." + std::to_string(i));

		setup_item(&g_Options.esp_settings[i].oof, false, "esp.oof_enable." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].oof_style, 1, "esp.oof_style." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].oof_clr[0], 1.f, "esp.oof_clr_r." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].oof_clr[1], 1.f, "esp.oof_clr_g." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].oof_clr[2], 1.f, "esp.oof_clr_b." + std::to_string(i));
		setup_item(&g_Options.esp_settings[i].oof_clr[3], 1.f, "esp.oof_clr_a." + std::to_string(i));
	}

	setup_item(&g_Options.legit_enable, false, "legit.legit_enable");
	setup_item(&g_Options.legit_autoselectweapon, false, "legit.legit_autoselectweapon");
	setup_item(&g_Options.legit_onlyscope, false, "legit.legit_onlyscope");
	setup_item(&g_Options.legit_resize_fov, false, "legit.legit_resize_fov");

	setup_item(&g_Options.visuals_draw_fov, false, "legit.visuals_draw_fov");
	setup_item(&g_Options.fov_draw_type, false, "legit.fov_draw_type");

	setup_item(&g_Options.hit_arrow, false, "esp.hit_arrow");
	setup_item(&g_Options.hit_arrow_damage, false, "esp.hit_arrow_damage");
	setup_item(&g_Options.hit_arrow_weapon, false, "esp.hit_arrow_weapon");
	setup_item(&g_Options.hit_arrow_only_visible, false, "esp.hit_arrow_only_visible");
	setup_item(&g_Options.hit_arrow_style, false, "esp.hit_arrow_style");

	setup_item(&g_Options.hit_arrow_clr[0], 1.f, "esp.hit_arrow_style_color_r");
	setup_item(&g_Options.hit_arrow_clr[1], 1.f, "esp.hit_arrow_style_color_g");
	setup_item(&g_Options.hit_arrow_clr[2], 1.f, "esp.hit_arrow_style_color_b");

	setup_item(&g_Options.fov_color[0], 1.f, "legit.fov_color_r");
	setup_item(&g_Options.fov_color[1], 1.f, "legit.fov_color_g");
	setup_item(&g_Options.fov_color[2], 1.f, "legit.fov_color_b");

	setup_item(&g_Options.radar_enable, false, "misc.radar_enable");
	setup_item(&g_Options.radar_enemy_only, false, "misc.radar_enemy_only");
	setup_item(&g_Options.radar_invisible, false, "misc.radar_invisible");
	setup_item(&g_Options.radar_grenades, false, "misc.radar_grenades");
	setup_item(&g_Options.radar_texture, false, "misc.radar_texture");

	setup_item(&g_Options.radar_range, 1890, "misc.radar_range");
	setup_item(&g_Options.radar_size, 250, "misc.radar_size");

	setup_item(&g_Options.color_radar_t_visible[0], 1.f, "misc.color_radar_t_visible_r");
	setup_item(&g_Options.color_radar_t_visible[1], 1.f, "misc.color_radar_t_visible_g");
	setup_item(&g_Options.color_radar_t_visible[2], 1.f, "misc.color_radar_t_visible_b");
	setup_item(&g_Options.color_radar_t_visible[3], 1.f, "misc.color_radar_t_visible_a");

	setup_item(&g_Options.color_radar_t_invisible[0], 1.f, "misc.color_radar_t_invisible_r");
	setup_item(&g_Options.color_radar_t_invisible[1], 1.f, "misc.color_radar_t_invisible_g");
	setup_item(&g_Options.color_radar_t_invisible[2], 1.f, "misc.color_radar_t_invisible_b");
	setup_item(&g_Options.color_radar_t_invisible[3], 1.f, "misc.color_radar_t_invisible_a");

	setup_item(&g_Options.color_radar_ct_visible[0], 1.f, "misc.color_radar_ct_visible_r");
	setup_item(&g_Options.color_radar_ct_visible[1], 1.f, "misc.color_radar_ct_visible_g");
	setup_item(&g_Options.color_radar_ct_visible[2], 1.f, "misc.color_radar_ct_visible_b");
	setup_item(&g_Options.color_radar_ct_visible[3], 1.f, "misc.color_radar_ct_visible_a");

	setup_item(&g_Options.color_radar_ct_invisible[0], 1.f, "misc.color_radar_ct_invisible_r");
	setup_item(&g_Options.color_radar_ct_invisible[1], 1.f, "misc.color_radar_ct_invisible_g");
	setup_item(&g_Options.color_radar_ct_invisible[2], 1.f, "misc.color_radar_ct_invisible_b");
	setup_item(&g_Options.color_radar_ct_invisible[3], 1.f, "misc.color_radar_ct_invisible_a");

	setup_item(&g_Options.color_radar_grenades[0], 1.f, "misc.color_radar_grenades_r");
	setup_item(&g_Options.color_radar_grenades[1], 1.f, "misc.color_radar_grenades_g");
	setup_item(&g_Options.color_radar_grenades[2], 1.f, "misc.color_radar_grenades_b");
	setup_item(&g_Options.color_radar_grenades[3], 1.f, "misc.color_radar_grenades_a");

	setup_item(&g_Options.misc_hitsound, false, "misc.hit_sound");
	setup_item(&g_Options.misc_antiafk_kick, false, "misc.ani_afk_kick");
	setup_item(&g_Options.misc_spectator_list, false, "misc.spectator_list");
	setup_item(&g_Options.misc_binds_list, false, "misc.binds_list");
	setup_item(&g_Options.bomb_timer, false, "misc.bomb_timer");
	setup_item(&g_Options.misc_watermark, false, "misc.watermark");
	setup_item(&g_Options.misc_bhop, false, "misc.bunny_hop");
	setup_item(&g_Options.bhop_chance, 0, "misc.bhop_chance");
	setup_item(&g_Options.max_bhop_limit_min, 0, "misc.bhop_max_hops_min");
	setup_item(&g_Options.max_bhop_limit_max, 0, "misc.bhop_max_hops_max");
	setup_item(&g_Options.strafe_type, 0, "misc.strafe_type");
	setup_item(&g_Options.edge_jump, false, "misc.edge_jump");
	setup_item(&g_Options.edge_jump_key, false, "misc.edge_jump_key");
	setup_item(&g_Options.edge_jump_mode, false, "misc.edge_jump_mode");
	setup_item(&g_Options.remove_scope, false, "misc.remove_scope");
	setup_item(&g_Options.remove_shadows, false, "misc.remove_shadows");
	setup_item(&g_Options.misc_no_smoke, false, "misc.remove_smoke");
	setup_item(&g_Options.misc_no_flash, false, "misc.remove_flash");
	setup_item(&g_Options.misc_no_post_processing, false, "misc.remove_post_processing");
	setup_item(&g_Options.remove_panorama_blur, false, "misc.remove_panorama_blur");
	setup_item(&g_Options.auto_accept, false, "misc.auto_accept");
	setup_item(&g_Options.misc_showranks, false, "misc.reveal_ranks");
	setup_item(&g_Options.misc_force_crosshair, false, "misc.sniper_crosshair");
	setup_item(&g_Options.fov_changer, false, "misc.fov_changer");
	setup_item(&g_Options.camera_fov, 90, "misc.camera_fov");
	setup_item(&g_Options.view_offsets, false, "misc.viewmodel_offsets");
	setup_item(&g_Options.viewmodel_x, 0, "misc.viewmodel_x");
	setup_item(&g_Options.viewmodel_y, 0, "misc.viewmodel_y");
	setup_item(&g_Options.viewmodel_z, 0, "misc.viewmodel_z");
	setup_item(&g_Options.viewmodel_roll, 0, "misc.viewmodel_roll");
	setup_item(&g_Options.do_saspect_ratio, false, "misc.aspect_ratio");
	setup_item(&g_Options.aspect_ratio_value, 1.3f, "misc.aspect_ratio_value");
	setup_item(&g_Options.misc_thirdperson, false, "misc.third_person");
	setup_item(&g_Options.misc_thirdperson_dist, 100, "misc.third_person_distance");
	setup_item(&g_Options.third_person_key, false, "misc.third_person_key");
	setup_item(&g_Options.third_person_mode, false, "misc.third_person_mode");
	setup_item(&g_Options.skybox_num, 0, "misc.skybox");
	setup_item(&g_Options.ragdolls, 0, "misc.ragdolls");
	setup_item(&g_Options.misc_hiteffect, false, "misc.hit_effect");
	setup_item(&g_Options.misc_hiteffect_duration, 0.5f, "misc.hit_effect_duration");

	setup_item(&g_Options.misc_world_color, false, "misc.world_custom_color");

	setup_item(&g_Options.custom_shadows, false, "misc.custom_shadows");
	setup_item(&g_Options.shadows_x, 0.f, "misc.shadows_x");
	setup_item(&g_Options.shadows_y, 0.f, "misc.shadows_y");
	setup_item(&g_Options.shadows_z, 0.f, "misc.shadows_z");

	setup_item(&g_Options.fog_enable, false, "misc.fog_controller");
	setup_item(&g_Options.fog_start, 100.f, "misc.fog_start");
	setup_item(&g_Options.fog_end, 100.f, "misc.fog_end");
	setup_item(&g_Options.fog_density, 0.5f, "misc.fog_density");
	setup_item(&g_Options.fog_color[0], 1.f, "misc.fog_r");
	setup_item(&g_Options.fog_color[1], 1.f, "misc.fog_g");
	setup_item(&g_Options.fog_color[2], 1.f, "misc.fog_b");

	setup_item(&g_Options.color_walls[0], 1.f, "misc.color_walls_r");
	setup_item(&g_Options.color_walls[1], 1.f, "misc.color_walls_g");
	setup_item(&g_Options.color_walls[2], 1.f, "misc.color_walls_b");

	setup_item(&g_Options.color_sky[0], 1.f, "misc.color_sky_r");
	setup_item(&g_Options.color_sky[1], 1.f, "misc.color_sky_g");
	setup_item(&g_Options.color_sky[2], 1.f, "misc.color_sky_b");

	setup_item(&g_Options.color_props[0], 1.f, "misc.color_props_r");
	setup_item(&g_Options.color_props[1], 1.f, "misc.color_props_g");
	setup_item(&g_Options.color_props[2], 1.f, "misc.color_props_b");

	setup_item(&g_Options.misc_post_processing, false, "misc.post_processing");

	setup_item(&g_Options.color_post_processing[0], 1.f, "misc.post_processing_color_r");
	setup_item(&g_Options.color_post_processing[1], 1.f, "misc.post_processing_color_g");
	setup_item(&g_Options.color_post_processing[2], 1.f, "misc.post_processing_color_b");

	setup_item(&g_Options.misc_hit_marker_1, false, "misc.screen_hit_marker");

	setup_item(&g_Options.color_hit_marker_1[0], 1.f, "misc.screen_hit_marker_color_r");
	setup_item(&g_Options.color_hit_marker_1[1], 1.f, "misc.screen_hit_marker_color_g");
	setup_item(&g_Options.color_hit_marker_1[2], 1.f, "misc.screen_hit_marker_color_b");

	setup_item(&g_Options.misc_hit_marker_2, false, "misc.world_hit_marker");

	setup_item(&g_Options.color_hit_marker_2[0], 1.f, "misc.world_hit_marker_color_r");
	setup_item(&g_Options.color_hit_marker_2[1], 1.f, "misc.world_hit_marker_color_g");
	setup_item(&g_Options.color_hit_marker_2[2], 1.f, "misc.world_hit_marker_color_b");

	setup_item(&g_Options.misc_damage_indicator, false, "misc.damage_indicator");

	setup_item(&g_Options.color_damage_indicator[0], 0.f, "misc.damage_indicator_color_r");
	setup_item(&g_Options.color_damage_indicator[1], 1.f, "misc.damage_indicator_color_g");
	setup_item(&g_Options.color_damage_indicator[2], 0.f, "misc.damage_indicator_color_b");

	setup_item(&g_Options.damage_indicator_size, 20.f, "misc.damage_indicator_font_size");
	setup_item(&g_Options.damage_indicator_speed, 1.5f, "misc.damage_indicator_speed");

	setup_item(&g_Options.grenade_prediction, false, "misc.grenade_prediction");
	setup_item(&g_Options.grenade_timers, false, "misc.grenade_timers");

	setup_item(&g_Options.grenade_projectiles, false, "misc.grenade_projectiles");
	setup_item(&g_Options.grenade_box, false, "misc.grenade_box");
	setup_item(&g_Options.grenade_text, false, "misc.grenade_text");
	setup_item(&g_Options.grenade_icon, false, "misc.grenade_icon");

	setup_item(&g_Options.color_grenade_trajectory[0], 0.f, "misc.color_grenade_trajectory_r");
	setup_item(&g_Options.color_grenade_trajectory[1], 1.f, "misc.color_grenade_trajectory_g");
	setup_item(&g_Options.color_grenade_trajectory[2], 0.f, "misc.color_grenade_trajectory_b");

	setup_item(&g_Options.color_grenade_circle[0], 0.f, "misc.color_grenade_circle_r");
	setup_item(&g_Options.color_grenade_circle[1], 1.f, "misc.color_grenade_circle_g");
	setup_item(&g_Options.color_grenade_circle[2], 0.f, "misc.color_grenade_circle_b");

	setup_item(&g_Options.color_grenade_text[0], 0.f, "misc.color_grenade_text_r");
	setup_item(&g_Options.color_grenade_text[1], 1.f, "misc.color_grenade_text_g");
	setup_item(&g_Options.color_grenade_text[2], 0.f, "misc.color_grenade_text_b");

	setup_item(&g_Options.color_grenade_icon[0], 0.f, "misc.color_grenade_icon_r");
	setup_item(&g_Options.color_grenade_icon[1], 1.f, "misc.color_grenade_icon_g");
	setup_item(&g_Options.color_grenade_icon[2], 0.f, "misc.color_grenade_icon_b");

	setup_item(&g_Options.color_grenade_box[0], 0.f, "misc.color_grenade_box_r");
	setup_item(&g_Options.color_grenade_box[1], 1.f, "misc.color_grenade_box_g");
	setup_item(&g_Options.color_grenade_box[2], 0.f, "misc.color_grenade_box_b");

	setup_item(&g_Options.dropped_weapon, false, "misc.dropped_weapon");
	setup_item(&g_Options.dropped_weapon_ammo, false, "misc.dropped_weapon_ammo");
	setup_item(&g_Options.dropped_weapon_box, false, "misc.dropped_weapon_box");
	setup_item(&g_Options.dropped_weapon_icon, false, "misc.dropped_weapon_icon");
	setup_item(&g_Options.dropped_weapon_text, false, "misc.dropped_weapon_text");

	setup_item(&g_Options.color_dropped_weapon_ammo[0], 0.f, "misc.color_dropped_weapon_ammo_r");
	setup_item(&g_Options.color_dropped_weapon_ammo[1], 1.f, "misc.color_dropped_weapon_ammo_g");
	setup_item(&g_Options.color_dropped_weapon_ammo[2], 0.f, "misc.color_dropped_weapon_ammo_b");

	setup_item(&g_Options.color_dropped_weapon_box[0], 0.f, "misc.color_dropped_weapon_box_r");
	setup_item(&g_Options.color_dropped_weapon_box[1], 1.f, "misc.color_dropped_weapon_box_g");
	setup_item(&g_Options.color_dropped_weapon_box[2], 0.f, "misc.color_dropped_weapon_box_b");

	setup_item(&g_Options.color_dropped_weapon_text[0], 0.f, "misc.color_dropped_weapon_text_r");
	setup_item(&g_Options.color_dropped_weapon_text[1], 1.f, "misc.color_dropped_weapon_text_g");
	setup_item(&g_Options.color_dropped_weapon_text[2], 0.f, "misc.color_dropped_weapon_text_b");

	setup_item(&g_Options.color_dropped_weapon_icon[0], 0.f, "misc.color_dropped_weapon_icon_r");
	setup_item(&g_Options.color_dropped_weapon_icon[1], 1.f, "misc.color_dropped_weapon_icon_g");
	setup_item(&g_Options.color_dropped_weapon_icon[2], 0.f, "misc.color_dropped_weapon_icon_b");

	setup_item(&g_Options.dropped_weapon_ammo_size, 16, "misc.dropped_weapon_ammo_size");
	setup_item(&g_Options.dropped_weapon_icon_size, 16, "misc.dropped_weapon_icon_size");
	setup_item(&g_Options.dropped_weapon_text_size, 16, "misc.dropped_weapon_text_size");

	setup_item(&g_Options.grenade_icon_size, 16, "misc.grenade_icon_size");
	setup_item(&g_Options.grenade_text_size, 16, "misc.grenade_text_size");

	setup_item(&g_Options.buy_bot, false, "misc.buy_bot");
	setup_item(&g_Options.primary_buy_bot, 0, "misc.buy_bot_primary");
	setup_item(&g_Options.secondary_buy_bot, 0, "misc.buy_bot_secondary");
	setup_item(&g_Options.buy_bot_active_buy_delay, 0, "misc.buy_bot_active_delay");
	setup_item(&g_Options.buy_bot_first_buy_delay, 0, "misc.buy_bot_first_delay");

	for (int i = 0; i < 9; i++)
	{
		setup_item(&g_Options.equipment_buy_bot[i], false, "misc.equipment_buy_bot." + std::to_string(i));
	}

}

void C_ConfigManager::add_item(void* pointer, const char* name, const std::string& type) {
	items.push_back(new C_ConfigItem(std::string(name), pointer, type));
}

void C_ConfigManager::setup_item(int* pointer, int value, const std::string& name)
{
	add_item(pointer, name.c_str(), _S("int"));
	*pointer = value;
}
void C_ConfigManager::setup_item(float* pointer, float value, const std::string& name)
{
	add_item(pointer, name.c_str(), _S("float"));
	*pointer = value;
}

void C_ConfigManager::setup_item(bool* pointer, bool value, const std::string& name)
{
	add_item(pointer, name.c_str(), _S("bool"));
	*pointer = value;
}

void C_ConfigManager::setup_item(Color* pointer, Color value, const std::string& name)
{
	colors[name][0] = (float)value.r() / 255.0f;
	colors[name][1] = (float)value.g() / 255.0f;
	colors[name][2] = (float)value.b() / 255.0f;
	colors[name][3] = (float)value.a() / 255.0f;

	add_item(pointer, name.c_str(), _S("Color"));
	*pointer = value;
}

void C_ConfigManager::setup_item(std::vector< int >* pointer, int size, const std::string& name)
{
	add_item(pointer, name.c_str(), _S("vector<int>"));
	pointer->clear();

	for (int i = 0; i < size; i++)
		pointer->push_back(false);
}
void C_ConfigManager::setup_item(std::vector< std::string >* pointer, const std::string& name)
{
	add_item(pointer, name.c_str(), _S("vector<string>"));
}

void C_ConfigManager::setup_item(std::string* pointer, const std::string& value, const std::string& name)
{
	add_item(pointer, name.c_str(), _S("string"));
	*pointer = value;
}

void C_ConfigManager::save(std::string config)
{

	json allJson;

	for (auto it : items)
	{
		json j;

		j[_S("name")] = it->name;
		j[_S("type")] = it->type;

		if (!it->type.compare(_S("int")))
			j[_S("value")] = (int)*(int*)it->pointer; //-V206
		else if (!it->type.compare(_S("float")))
			j[_S("value")] = (float)*(float*)it->pointer;
		else if (!it->type.compare(_S("bool")))
			j[_S("value")] = (bool)*(bool*)it->pointer;
		else if (!it->type.compare(_S("Color")))
		{
			auto c = *(Color*)(it->pointer);

			std::vector<int> a = { c.r(), c.g(), c.b(), c.a() };
			json ja;

			for (auto& i : a)
				ja.push_back(i);

			j[_S("value")] = ja.dump();
		}
		else if (!it->type.compare(_S("vector<int>")))
		{
			auto& ptr = *(std::vector<int>*)(it->pointer);
			json ja;

			for (auto& i : ptr)
				ja.push_back(i);

			j[_S("value")] = ja.dump();
		}
		else if (!it->type.compare(_S("vector<string>")))
		{
			auto& ptr = *(std::vector<std::string>*)(it->pointer);
			json ja;

			for (auto& i : ptr)
				ja.push_back(i);

			j[_S("value")] = ja.dump();
		}
		else if (!it->type.compare(_S("string")))
			j[_S("value")] = (std::string) * (std::string*)it->pointer;

		allJson.push_back(j);
	}

	std::string data;

	Base64 base64;
	base64.encode(allJson.dump(), &data);

	std::ofstream ofs;
	std::string res = "C:\\umbrella_csgo\\configs\\";
	res.append(config);
	res.append(".cfg");
	ofs.open(res + '\0', std::ios::out | std::ios::trunc);

	ofs << std::setw(4) << data << std::endl;
	ofs.close();
}

void C_ConfigManager::load(std::string config)
{
	static auto find_item = [](std::vector< C_ConfigItem* > items, std::string name) -> C_ConfigItem*
	{
		for (int i = 0; i < (int)items.size(); i++) //-V202
			if (!items[i]->name.compare(name))
				return items[i];

		return nullptr;
	};

	std::string data;

	std::ifstream ifs;
	std::string res = "C:\\umbrella_csgo\\configs\\";
	res.append(config);
	res.append(".cfg");
	ifs.open(res + '\0');

	ifs >> data;
	ifs.close();

	if (data.empty())
		return;

	Base64 base64;

	std::string decoded_data;
	base64.decode(data, &decoded_data);

	std::ofstream ofs;
	ofs.open(res + '\0', std::ios::out | std::ios::trunc);

	ofs << decoded_data;
	ofs.close();

	json allJson;

	std::ifstream ifs_final;
	ifs_final.open(res + '\0');

	ifs_final >> allJson;
	ifs_final.close();

	base64.encode(allJson.dump(), &data);

	std::ofstream final_ofs;
	final_ofs.open(res + '\0', std::ios::out | std::ios::trunc);

	final_ofs << data;
	final_ofs.close();

	for (auto it = allJson.begin(); it != allJson.end(); ++it)
	{
		json j = *it;

		std::string name = j[_S("name")];
		std::string type = j[_S("type")];

		auto item = find_item(items, name);

		if (item)
		{
			if (!type.compare(_S("int")))
				*(int*)item->pointer = j[_S("value")].get<int>();
			else if (!type.compare(_S("float")))
				*(float*)item->pointer = j[_S("value")].get<float>();
			else if (!type.compare(_S("bool")))
				*(bool*)item->pointer = j[_S("value")].get<bool>();
			else if (!type.compare(_S("Color")))
			{
				std::vector<int> a;
				json ja = json::parse(j[_S("value")].get<std::string>().c_str());

				for (json::iterator it = ja.begin(); it != ja.end(); ++it)
					a.push_back(*it);

				colors.erase(item->name);
				*(Color*)item->pointer = Color(a[0], a[1], a[2], a[3]);
			}
			else if (!type.compare(_S("vector<int>")))
			{
				auto ptr = static_cast<std::vector <int>*> (item->pointer);
				ptr->clear();

				json ja = json::parse(j[_S("value")].get<std::string>().c_str());

				for (json::iterator it = ja.begin(); it != ja.end(); ++it)
					ptr->push_back(*it);
			}
			else if (!type.compare(_S("vector<string>")))
			{
				auto ptr = static_cast<std::vector <std::string>*> (item->pointer);
				ptr->clear();

				json ja = json::parse(j[_S("value")].get<std::string>().c_str());

				for (json::iterator it = ja.begin(); it != ja.end(); ++it)
					ptr->push_back(*it);
			}
			else if (!type.compare(_S("string")))
				*(std::string*)item->pointer = j[_S("value")].get<std::string>();
		}

	}
}

void C_ConfigManager::remove(std::string config)
{
	std::string res = "C:\\umbrella_csgo\\configs\\";
	res.append(config);
	res.append(".cfg");
	std::string path = res + '\0';
	std::remove(path.c_str());
}

void C_ConfigManager::config_files()
{
	files.clear();

	std::string pattern("C:\\umbrella_csgo\\configs\\*");

	WIN32_FIND_DATA data;
	HANDLE find;

	if ((find = FindFirstFileA(pattern.c_str(), (LPWIN32_FIND_DATAA)&data)) != INVALID_HANDLE_VALUE) {
		do {
			std::string filename((char*)data.cFileName);
			if (filename.substr(filename.find_last_of(".") + 1) == "cfg")
			{
				std::string result_string = filename.erase(filename.length() - 4, 4);
				files.push_back(result_string);
			}
		} while (FindNextFileA(find, (LPWIN32_FIND_DATAA)&data) != 0);
		FindClose(find);
	}
}

