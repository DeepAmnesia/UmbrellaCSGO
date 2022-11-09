#include "../../valve_sdk/csgostructs.hpp"
#include "../../render.hpp"
#include "../../options.hpp"
#include "../../helpers/math.hpp"
#include <iomanip>
#include <chrono>

class C_BuyBot
{
public:
	virtual void OnRoundStart(); 
	virtual void OnCreateMove();

	std::string command = "";

	bool time_setupped = false;
	bool need_to_work = false;

	int commands_value = 0;
	int commands_passed = 0;

	float next_time{};
};

inline C_BuyBot* g_BuyBot = new C_BuyBot();