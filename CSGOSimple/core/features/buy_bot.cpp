#include "buy_bot.h"
#include <thread>
#include <sstream>

std::string parse_new_string(std::string src, int num)
{
	std::stringstream new_src(src);
	std::string segment;
	std::vector<std::string> seglist;

	while (std::getline(new_src, segment, ';'))
	{
		seglist.push_back(segment);
	}

	return seglist.at(num);
}

void C_BuyBot::OnRoundStart()
{
	switch (g_Options.primary_buy_bot)
	{
	case 1: command += "buy scar20; buy g3sg1;"; commands_value++;  break;
	case 2: command += "buy ssg08;"; commands_value++;  break;
	case 3: command += "buy awp;"; commands_value++;  break;
	case 4: command += "buy ak47; buy m4a1;"; commands_value++;  break;
	}

	switch (g_Options.secondary_buy_bot)
	{
	case 1: command += "buy fn57; buy tec9;"; commands_value++;  break;
	case 2: command += "buy elite;";  commands_value++;  break;
	case 3: command += "buy deagle;"; commands_value++;  break;
	case 4: command += "buy usp_silencer; buy glock;";  commands_value++;  break;
	case 5: command += "buy p250;";  commands_value++;  break;
	}

	if (g_Options.equipment_buy_bot[0])
	{
		command += "buy incgrenade; buy molotov;";
		commands_value++;
	}

	if (g_Options.equipment_buy_bot[1])
	{
		command += "buy smokegrenade;";
		commands_value++;
	}
	
	if (g_Options.equipment_buy_bot[2])
	{
		command += "buy hegrenade;";
		commands_value++;
	}
	
	if (g_Options.equipment_buy_bot[3])
	{
		command += "buy flashbang;";
		commands_value++;
	}

	if (g_Options.equipment_buy_bot[4])
	{
		command += "buy taser;";
		commands_value++;
	}

	if (g_Options.equipment_buy_bot[5])
	{
		command += "buy heavyarmor;";
		commands_value++;
	}

	if (g_Options.equipment_buy_bot[6])
	{
		command += "buy vesthelm;";
		commands_value++;
	}

	if (g_Options.equipment_buy_bot[7])
	{
		command += "buy defuser;";
		commands_value++;
	}

	need_to_work = true;
	
	next_time = g_GlobalVars->curtime + g_Options.buy_bot_first_buy_delay / 1000.f;
}

void C_BuyBot::OnCreateMove()
{
	if (g_Options.buy_bot)
	{
		if (need_to_work && g_LocalPlayer && g_LocalPlayer->IsAlive())
		{
			if (g_GlobalVars->curtime >= next_time)
			{
				if (commands_passed <= commands_value)
				{
					g_EngineClient->ExecuteClientCmd(parse_new_string(command.c_str(), commands_passed).c_str());
					next_time = g_GlobalVars->curtime + g_Options.buy_bot_active_buy_delay / 1000.f;
					commands_passed++;
				}
				else
				{
					command = "";
					commands_value = 0;
					commands_passed = 0;
					need_to_work = false;
				}
			}
		}
	}
	else
	{
		command = "";
		commands_value = 0;
		commands_passed = 0;
		need_to_work = false;
	}
}