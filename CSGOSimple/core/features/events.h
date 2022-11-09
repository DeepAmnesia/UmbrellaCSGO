#pragma once

#include "../hooks/hooks.hpp"
#include "../../config.hpp"
#define MAX_FLOATING_TEXTS 50

class C_HookedEvents : public IGameEventListener2
{
public:
	struct ImpactData
	{
		C_BasePlayer* pEntity;
		Vector ImpactPosition;
		float Time;
	};

	std::vector<ImpactData> impacts;
	void FireGameEvent(IGameEvent* event);
	void RegisterSelf();
	void RemoveSelf();
	int GetEventDebugID(void);
};

inline C_HookedEvents* HookedEvents = new C_HookedEvents();