#pragma once
#include "legit_autowall.h"

namespace trigger
{
	extern int weapon_index_data[34];
	extern const char* weapon_data[34];
    void run(CUserCmd* cmd); 
	int get_weapon_select(C_BasePlayer* local_player);
	bool did_hit_player();
	
}