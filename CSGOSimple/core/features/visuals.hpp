#pragma once

#include "../../singleton.hpp"
#include "../../options.hpp"
#include "../../render.hpp"
#include "../../helpers/math.hpp"
#include "../../valve_sdk/csgostructs.hpp"
#include <deque>

struct damage_indicator 
{
	damage_indicator(int p_hp, float p_time, Vector p_bone) 
	{
		hp = p_hp;
		time = p_time;
		bone = p_bone;
	}
	float alpha = 255.f;
	int hp;
	float time;
	Vector bone;
};

struct sound_arrow_data
{
	sound_arrow_data(C_BasePlayer* p_player, float p_time)
	{
		player = p_player;
		time = p_time;
	}
	C_BasePlayer* player;
	float time;
};

struct hit_arrow_data
{
	hit_arrow_data(C_BasePlayer* p_player, float p_time, int p_hp, std::string p_weapon)
	{
		player = p_player;
		time = p_time;
		hp = p_hp;
		weapon = p_weapon;
	}

	C_BasePlayer* player;
	float time;
	int hp;
	std::string weapon;
};

inline std::deque<damage_indicator> damage_indicator_log;
inline std::deque<hit_arrow_data> hit_arrow_log;
inline std::deque<damage_indicator> hit_markers_log;

inline std::deque<sound_arrow_data> sound_arrow_log_team;
inline std::deque<sound_arrow_data> sound_arrow_log_enemy;

struct visuals_data_t
{
	RECT bbox;
	int height = 0;
	int width = 0;
	float prev_health = 100;
	float prev_ammo = 20;
	bool left_dormancy = false;
};

struct dormant_player_t
{
	float receive_time = 0.0f;
	float dormant = 0.0f;
	Vector origin = Vector(0, 0, 0);
	Vector last_origin = Vector(0, 0, 0);
};

enum bar_pos 
{
	left_1 = 0,
	left_2 = 1,
	right_1 = 2,
	right_2 = 3,
	top_1 = 4,
	top_2 = 5,
	bottom_1 = 6,
	bottom_2 = 7
};

struct movable_struct_t
{
	std::string content;
	int coll_num = 1;
	int num_in_coll = 0;
	int font_size = 16.f;
	bool enabled = false;
	Color clr;
	movable_struct_t() {};

	movable_struct_t(char* content, int num_in_coll, int coll_num, int font_size)
	{
		this->content = content;
		this->coll_num = coll_num;
		this->num_in_coll = num_in_coll;
		this->font_size = font_size;
	}

	movable_struct_t(const char* content)
	{
		this->content = content;
	}
};

class c_table_of_structs
{
public:
	std::deque<movable_struct_t> table_content;
};

inline movable_struct_t name_text_struct = movable_struct_t("umbrella");
inline movable_struct_t health_text_struct = movable_struct_t("100");
inline movable_struct_t armor_text_struct = movable_struct_t("100");
inline movable_struct_t flashed_text_struct = movable_struct_t("FLASHED");
inline movable_struct_t scoped_text_struct = movable_struct_t("SCOPED");
inline movable_struct_t money_text_struct = movable_struct_t("300$");
inline movable_struct_t ammo_text_struct = movable_struct_t("30");
inline movable_struct_t defuser_kit_text_struct = movable_struct_t("KIT");
inline movable_struct_t fakeduck_text_struct = movable_struct_t("FD");
inline movable_struct_t weapon_icon_text_struct = movable_struct_t("d");
inline movable_struct_t weapon_text_struct = movable_struct_t("Glock-18");
inline movable_struct_t reloading_text_struct = movable_struct_t("RELOADING");

inline movable_struct_t grenade_name_struct = movable_struct_t("RELOADING");
inline movable_struct_t grenade_icon_struct = movable_struct_t("RELOADING");

inline movable_struct_t dropped_weapon_name_struct = movable_struct_t("RELOADING");
inline movable_struct_t dropped_weapon_icon_struct = movable_struct_t("RELOADING");
inline movable_struct_t dropped_weapon_ammo_struct = movable_struct_t("RELOADING");

inline c_table_of_structs left_table;
inline c_table_of_structs right_table;
inline c_table_of_structs top_table;
inline c_table_of_structs down_table;

inline c_table_of_structs left_table_grenade;
inline c_table_of_structs right_table_grenade;
inline c_table_of_structs top_table_grenade;
inline c_table_of_structs down_table_grenade;

inline c_table_of_structs left_table_weapon;
inline c_table_of_structs right_table_weapon;
inline c_table_of_structs top_table_weapon;
inline c_table_of_structs down_table_weapon;

namespace visuals
{
	inline std::array < visuals_data_t, 65 > player_data;
	inline std::array < dormant_player_t, 65 > dormant_players;
	inline CUtlVector < SndInfo_t > current_sound_data;
	inline CUtlVector < SndInfo_t > cached_sound_data;

	void draw();

	void render_box(c_esp_settings settings, C_BasePlayer* player, bool visible);
	void render_health(c_esp_settings settings, C_BasePlayer* player, bool visible);
	void render_oof_arrows(c_esp_settings settings, C_BasePlayer* player);


	void render_left_table(c_esp_settings settings, C_BasePlayer* player, bool visible);
	void render_right_table(c_esp_settings settings, C_BasePlayer* player, bool visible);
	void render_top_table(c_esp_settings settings, C_BasePlayer* player, bool visible);
	void render_bottom_table(c_esp_settings settings, C_BasePlayer* player, bool visible);

	void manage_flags(C_BasePlayer* player);

	void render_sounds();

	void skeleton(c_esp_settings settings, C_BasePlayer* player, bool visible);

	bool force_dormant(int index);
	void cache_dormant_players();
	bool is_sound_valid(SndInfo_t sound);
	void reset_data();

	void no_smoke();
	void no_flash();
	void disable_post_processing();
	void remove_panorama_blur();
	void force_sniper_crosshair();

	void draw_damage_indicator(C_BasePlayer* local_player);
	void on_events_damage_indicator(IGameEvent* event);
	void draw_hit_marker2(C_BasePlayer* local_player);
	void draw_hit_marker(C_BasePlayer* local_player);
	void on_events_hit_marker(IGameEvent* event);

	void clear_hit_markers();
	inline void clear_arrows()
	{
		sound_arrow_log_enemy.clear();
		sound_arrow_log_team.clear();
		hit_arrow_log.clear();
	}

	void render_hit_arrows();
	void render_sound_arrows();

	void add_hit_arrow(hit_arrow_data data);
	void add_sound_arrow_enemy(sound_arrow_data data);
	void add_sound_arrow_team(sound_arrow_data data);

	void render_dopped_throwed();
	void grenades();

	void grenade_projectiles(C_BaseEntity* entity);
	void dropped_weapons(C_BaseEntity* entity);

	
}