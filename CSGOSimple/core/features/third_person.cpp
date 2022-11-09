#include "third_person.h"
#include "../../options.hpp"
#include "../../helpers/utils.hpp"
#include "../../helpers/math.hpp"

void TraceHull(const Vector& vec_start, const Vector& vec_end, const unsigned int n_mask, const Vector& extents, trace_t* p_trace)
{
	CTraceFilterWorldAndPropsOnly Filter;

	Ray_t ray;
	ray.Init(vec_start, vec_end);
	ray.m_Extents = extents;
	ray.m_IsRay = false;

	g_EngineTrace->TraceRay(ray, n_mask, &Filter, p_trace);
}

void C_ThirdPerson::Instance()
{
	if (!g_LocalPlayer)
		return;

	if (!g_LocalPlayer->IsAlive())
	{
		g_Input->m_fCameraInThirdPerson = false;
		m_flThirdpersonDistance = 50.0f;

		g_LocalPlayer->m_iObserverMode() = ObserverType_t::OBS_MODE_CHASE;
		return;
	}

	bool bIsEnabled = Utils::key_bind(g_Options.third_person_key, g_Options.third_person_mode);


	if (bIsEnabled)
		m_flThirdpersonDistance = min(max(m_flThirdpersonDistance, 30.0f) + 12.0f, g_Options.misc_thirdperson_dist);
	else
		m_flThirdpersonDistance = max(m_flThirdpersonDistance - 12.0f, 30.0f);

	if (m_flThirdpersonDistance <= (bIsEnabled ? 50.0f : 30.0f))
	{
		g_Input->m_fCameraInThirdPerson = false;
		return;
	}

	Vector vecEyePos = g_LocalPlayer->GetAbsOrigin() + g_LocalPlayer->m_vecViewOffset();
	//if (g_Globals.m_Packet.m_bVisualFakeDuck)
	//	vecEyePos = g_Globals.m_LocalPlayer->GetAbsOrigin() + g_Globals.m_Interfaces.m_GameMovement->GetPlayerViewOffset(false);

	QAngle angViewAngles;
	g_EngineClient->GetViewAngles(&angViewAngles);

	g_Input->m_fCameraInThirdPerson = true;
	g_Input->m_vecCameraOffset = Vector(angViewAngles.x, angViewAngles.y, m_flThirdpersonDistance);

	CGameTrace Trace;
	
	float flExtent = 12.f + (g_Options.camera_fov / 4.8f - 18.f);

	Vector vecForward = Vector(0, 0, 0);
	Math::AngleVectors(QAngle(angViewAngles.x, angViewAngles.y, 0.0f), vecForward);
	TraceHull(vecEyePos, vecEyePos - vecForward * g_Input->m_vecCameraOffset.z, MASK_SOLID & ~CONTENTS_MONSTER, Vector(flExtent, flExtent, flExtent), &Trace);
	g_Input->m_vecCameraOffset.z *= Trace.fraction;

	if (g_Input->m_vecCameraOffset.z < min(30.0f, g_Options.misc_thirdperson_dist))
		g_Input->m_fCameraInThirdPerson = false;
}