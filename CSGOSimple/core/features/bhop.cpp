#include "bhop.hpp"
#include "../../options.hpp"
#include "../../valve_sdk/csgostructs.hpp"

void BunnyHop::OnCreateMove(CUserCmd* cmd)
{
	static bool bLastJumped = false;
	static bool bShouldFake = false;
	static int bActualHop = 0;

	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return;

	if (g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER || g_LocalPlayer->m_nMoveType() == MOVETYPE_NOCLIP)
		return;

	if (!bLastJumped && bShouldFake)
	{
		bShouldFake = false;
		cmd->buttons |= IN_JUMP;
	}
	else if (cmd->buttons & IN_JUMP)
	{
		if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
		{
			bActualHop++;
			bLastJumped = true;
			bShouldFake = true;
		}
		else
		{
			if (g_Options.bhop_chance &&
				g_Options.max_bhop_limit_min &&
				(bActualHop > g_Options.max_bhop_limit_min) &&
				(rand() % 100 > g_Options.bhop_chance))
				return;

			if (g_Options.bhop_chance &&
				!g_Options.max_bhop_limit_min &&
				(rand() % 100 > g_Options.bhop_chance))
				return;

			if (g_Options.max_bhop_limit_min &&
				!g_Options.bhop_chance &&
				(bActualHop > g_Options.max_bhop_limit_min))
				return;

			if (g_Options.max_bhop_limit_max &&
				(bActualHop > g_Options.max_bhop_limit_max))
				return;

			cmd->buttons &= ~IN_JUMP;
			bLastJumped = false;
		}
	}
	else
	{
		bActualHop = 0;
		bLastJumped = false;
		bShouldFake = false;
	}
}