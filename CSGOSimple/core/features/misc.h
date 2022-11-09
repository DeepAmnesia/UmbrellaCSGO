#pragma once

#include "../../valve_sdk/sdk.hpp"
#include "../../helpers/math.hpp"
#include "../../valve_sdk/csgostructs.hpp"
#include "../../render.hpp"
#include "../../options.hpp"

namespace misc
{
	void auto_strafe(CUserCmd* cmd);
	void spectator_list();
	void binds_list();
	void watermark();
	void anti_afk_kick(CUserCmd* cmd);
	void bomb_timer();
	void skybox_changer();
	void set_sky_box(C_BasePlayer* local_player, const std::string sky_box_name);
	void ragdoll();
	void custom_shadows();
	void fog_controller();
}