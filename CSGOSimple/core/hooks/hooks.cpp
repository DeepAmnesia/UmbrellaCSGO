#include <intrin.h>
#include <stdexcept>
#include "../features/game_chams.h"
#include "hooks.hpp" 

#pragma intrinsic(_ReturnAddress)  
#define FIRSTPERSON_TO_THIRDPERSON_VERTICAL_TOLERANCE_MIN 4.0f
#define FIRSTPERSON_TO_THIRDPERSON_VERTICAL_TOLERANCE_MAX 10.0f
#define HOOK(address, function, original) MH_CreateHook(address, function, reinterpret_cast<void**>(&(original))); Utils::ConsolePrint("hooked %s\n", #function);\

inline std::add_pointer_t<bool __fastcall(registers)> o_is_using_static_prop_debug_modes;

namespace Hooks 
{
	float_t __fastcall hk_aspect_ratio(LPVOID pEcx, uint32_t, int32_t X, int32_t Y)
	{
		if (g_Options.do_saspect_ratio)
			return g_Options.aspect_ratio_value;
		else
			return m_GetScreenSizeAspectRatio(pEcx, X, Y);
	}

	void __fastcall hk_get_color_modulation(LPVOID pEcx, uint32_t, float_t* r, float_t* g, float_t* b) {
		const IMaterial* get_material = ( IMaterial* )(pEcx);
		if (!get_material || get_material->IsErrorMaterial())
			return m_GetColorModulation(pEcx, r, g, b);

	
		m_GetColorModulation(pEcx, r, g, b);

		const auto get_group = get_material->GetTextureGroupName();

		static float world_color[3] = { 0.8f, 0.8f, 0.8f };
		static float props_color[3] = { 0.8f, 0.8f, 0.8f };
		static float sky_color[3] = { 0.8f, 0.8f, 0.8f };

		if (g_Options.misc_world_color)
		{
			if (strstr(get_group, "World textures"))
			{
				Color clr = Color(g_Options.color_walls);
				Utils::modern_clamp_world(world_color[0], clr.r() / 255.f, g_GlobalVars->frametime / 24.f);
				Utils::modern_clamp_world(world_color[1], clr.g() / 255.f, g_GlobalVars->frametime / 24.f);
				Utils::modern_clamp_world(world_color[2], clr.b() / 255.f, g_GlobalVars->frametime / 24.f);

				*r = world_color[0],
				*g = world_color[1],
				*b = world_color[2];
			}
			else if (strstr(get_group, "StaticProp textures"))
			{
				Color clr = Color(g_Options.color_props);
				Utils::modern_clamp_world(props_color[0], clr.r() / 255.f, g_GlobalVars->frametime / 24.f);
				Utils::modern_clamp_world(props_color[1], clr.g() / 255.f, g_GlobalVars->frametime / 24.f);
				Utils::modern_clamp_world(props_color[2], clr.b() / 255.f, g_GlobalVars->frametime / 24.f);

				*r = props_color[0],
				*g = props_color[1],
				*b = props_color[2];
			}
			else if (strstr(get_group, "SkyBox textures"))
			{
				Color clr = Color(g_Options.color_sky);
				Utils::modern_clamp_world(sky_color[0], clr.r() / 255.f, g_GlobalVars->frametime / 24.f);
				Utils::modern_clamp_world(sky_color[1], clr.g() / 255.f, g_GlobalVars->frametime / 24.f);
				Utils::modern_clamp_world(sky_color[2], clr.b() / 255.f, g_GlobalVars->frametime / 24.f);

				*r = sky_color[0],
				*g = sky_color[1],
				*b = sky_color[2];
			}
		}
		else
		{
			if (strstr(get_group, "World textures"))
			{
				Utils::modern_clamp_world(world_color[0], 0.8f, g_GlobalVars->frametime / 24.f);
				Utils::modern_clamp_world(world_color[1], 0.8f, g_GlobalVars->frametime / 24.f);
				Utils::modern_clamp_world(world_color[2], 0.8f, g_GlobalVars->frametime / 24.f);

				*r = world_color[0],
				*g = world_color[1],
				*b = world_color[2];
			}
			else if (strstr(get_group, "StaticProp textures"))
			{
				Utils::modern_clamp_world(props_color[0], 0.8f, g_GlobalVars->frametime / 24.f);
				Utils::modern_clamp_world(props_color[1], 0.8f, g_GlobalVars->frametime / 24.f);
				Utils::modern_clamp_world(props_color[2], 0.8f, g_GlobalVars->frametime / 24.f);

				*r = props_color[0],
				*g = props_color[1],
				*b = props_color[2];
			}
			else if (strstr(get_group, "SkyBox textures"))
			{
				Utils::modern_clamp_world(sky_color[0], 0.8f, g_GlobalVars->frametime / 24.f);
				Utils::modern_clamp_world(sky_color[1], 0.8f, g_GlobalVars->frametime / 24.f);
				Utils::modern_clamp_world(sky_color[2], 0.8f, g_GlobalVars->frametime / 24.f);

				*r = sky_color[0],
				*g = sky_color[1],
				*b = sky_color[2];
			}
		}
	}

	bool __fastcall hk_is_using_static_prop_debug_modes(registers) {
		return  g_Options.misc_world_color;
	}

	void __fastcall hkDrawModelExecute_Model(LPVOID pEcx, uint32_t, IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix)
	{
		if (g_MdlRender->IsForcedMaterialOverride() &&
			!strstr(pInfo.pModel->szName, "arms") &&
			!strstr(pInfo.pModel->szName, "weapons/v_"))
			return m_DrawModelExecute_Model(pEcx, pCtx, pState, pInfo, aMatrix);

		return g_Chams->OnModelRender(pCtx, pState, pInfo, aMatrix);
	}

	void __fastcall hkDrawModelExecute_Studio(LPVOID pEcx, uint32_t, LPVOID pResults, DrawModelInfo_t* pInfo, matrix3x4_t* pBoneToWorld, float_t* flpFlexWeights, float_t* flpFlexDelayedWeights, Vector& vrModelOrigin, int32_t iFlags)
	{
		if (g_cheat.m_bDrawModel && pInfo->m_pStudioHdr)
		{
			if (strstr(pInfo->m_pStudioHdr->szName, "weapons\\w_pist_elite.mdl"))
			{
				g_StudioRender->ForcedMaterialOverride(NULL);
				return Hooks::m_DrawModelExecute_Studio(g_StudioRender, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
			}
		}

		return Hooks::m_DrawModelExecute_Studio(g_StudioRender, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
	}

	void __fastcall hkModifyEyePosition(LPVOID pEcx, uint32_t, Vector& vecInputEyePos)
	{
		auto* m_animation_state = static_cast<C_CSGOPlayerAnimationState*>(pEcx);
		if (!m_animation_state)
			return Hooks::m_ModifyEyePosition(pEcx, vecInputEyePos);

		// https://github.com/perilouswithadollarsign/cstrike15_src/blob/29e4c1fda9698d5cebcdaf1a0de4b829fa149bf8/game/shared/cstrike15/csgo_playeranimstate.cpp#L450

		if (!m_animation_state->m_landing && m_animation_state->m_anim_duck_amount == 0.0f)
		{
			m_animation_state->m_smooth_height_valid = false;
			m_animation_state->m_camera_smooth_height = 0x7F7FFFFF;
			return;
		}

		using bone_lookup_fn = int(__thiscall*)(void*, const char*);
		static auto lookup_bone = reinterpret_cast<bone_lookup_fn>(Utils::PatternScan(GetModuleHandleA("client.dll"),
			"55 8B EC 53 56 8B F1 57 83 BE ? ? ? ? ? 75 14 8B 46 04 8D 4E 04 FF 50 20 85 C0 74 07 8B CE E8 ? ? ? ? 8B 8E ? ? ? ? 85 C9 0F 84"));

		auto head_pos = m_animation_state->m_base_player->get_cached_bone_data()[lookup_bone(
			m_animation_state->m_base_player, "head_0")].at(3);
		head_pos.z += 1.7f;

		if (head_pos.z > vecInputEyePos.z)
			return;

		const float_t flLerp = Math::RemapValClamped(abs(vecInputEyePos.z - head_pos.z),
			FIRSTPERSON_TO_THIRDPERSON_VERTICAL_TOLERANCE_MIN,
			FIRSTPERSON_TO_THIRDPERSON_VERTICAL_TOLERANCE_MAX,
			0.0f, 1.0f);

		vecInputEyePos.z = Math::Lerp(flLerp, vecInputEyePos.z, head_pos.z);
	}

	void Initialize()
	{
		const char* game_modules[]{ "client.dll", "engine.dll", "server.dll", "studiorender.dll", "materialsystem.dll", "shaderapidx9.dll", "vstdlib.dll", "vguimatsurface.dll"};
		auto patch = 0x69690004C201B0;
		auto signature = "55 8B EC 56 8B F1 33 C0 57 8B 7D 08";

		for (auto current_module : game_modules)
			WriteProcessMemory(GetCurrentProcess(), (LPVOID)Utils::PatternScan(GetModuleHandleA(current_module), signature), &patch, 7, 0);

		g_cheat.get_line_goes_through_smoke = Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0");

		g_cheat.item_schema_map_offset = Utils::PatternScan(GetModuleHandleA("client.dll"), "8B 87 ? ? ? ? 83 7C 88 ? ? 7C 0A 8B 44 88 04 5F 5E 5D C2 04 00 8B 87") + 0x2;
		g_cheat.item_system = Utils::PatternScan(GetModuleHandleA("client.dll"), "A1 ? ? ? ? 85 C0 75 53");
		g_cheat.smoke_count = Utils::PatternScan(GetModuleHandleA("client.dll"), "8B 15 ? ? ? ? 0F 57 C0 56") + 0x2;
		g_cheat.set_abs_angles = Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8");
		g_cheat.set_abs_origin = Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8");
		g_cheat.m_IsBreakableEntity = Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 51 56 8B F1 85 F6 74 68");
		g_cheat.m_TraceFilterSimple = Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F0 83 EC 7C 56 52") + 0x3D;
		g_cheat.m_TraceFilterSkipTwoEntities = Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 81 EC BC 00 00 00 56 8B F1 8B 86") + 550;
		
		g_cheat.is_using_static_prop_debug_modes = Utils::PatternScan(GetModuleHandleA("engine.dll"), "E8 ? ? ? ? 84 C0 8B 45 08");
		g_cheat.unknown_file_sys_ret_addr = Utils::PatternScan(GetModuleHandleA("panorama.dll"), "C7 43 ? ? ? ? ? 83 7C 24 ? ? 7C 25 8B 54 24 48");
		g_cheat.element_fn = Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39");
		g_cheat.element_this = Utils::PatternScan(GetModuleHandleA("client.dll"), "B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08") + 0x1;
		g_cheat.custom_sky_box = Utils::PatternScan(GetModuleHandleA("engine.dll"), "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45");
		g_cheat.material = Utils::PatternScan(GetModuleHandleA("materialsystem.dll"), "C7 06 ? ? ? ? C7 46 ? ? ? ? ? C7 46 ? ? ? ? ? 89 4D E8") + 0x2;

		g_cheat.m_KeyValuesFindKey = Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 EC 1C 53 8B D9 85 DB");
		g_cheat.m_KeyValuesGetString = Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 C0 81 EC ? ? ? ? 53 8B 5D 08");
		g_cheat.m_KeyValuesSetString = Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC A1 ? ? ? ? 53 56 57 8B F9 8B 08 8B 01");
		g_cheat.m_KeyValuesLoadFromBuffer = Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89 4C 24 04");

		g_cheat.m_SetupBonesForAttachmentQueries = Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 EC 14 83 3D ? ? ? ? ? 53");
		g_cheat.m_DisableRenderTargetAllocationForever = Utils::PatternScan(GetModuleHandleA("client.dll"), "80 B9 ? ? ? ? ? 74 0F") + 0x2;
		g_cheat.m_CreateModel = Utils::PatternScan(GetModuleHandleA("client.dll"), "53 8B D9 56 57 8D 4B 04 C7 03 ? ? ? ? E8 ? ? ? ? 6A");
		g_cheat.m_SetMergedMDL = Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 57 8B F9 8B 0D ? ? ? ? 85 C9 75");

		static const auto EndScene_fn = reinterpret_cast<void*>(get_virtual(g_D3DDevice9, index::EndScene));
		static const auto Reset_fn = reinterpret_cast<void*>(get_virtual(g_D3DDevice9, index::Reset));
		static const auto LockCursor_fn = reinterpret_cast<void*>(get_virtual(g_VGuiSurface, index::LockCursor));
		static const auto CreateMove_fn = reinterpret_cast<void*>(get_virtual(g_ClientMode, index::CreateMove));
		static const auto EmitSound_fn = reinterpret_cast<void*>(get_virtual(g_EngineSound, index::EmitSound1));
		static const auto PaintTreverse_fn = reinterpret_cast<void*>(get_virtual(g_VGuiPanel, index::PaintTraverse));
		static const auto DoPostScreenEffect_fn = reinterpret_cast<void*>(get_virtual(g_ClientMode, index::DoPostScreenSpaceEffects));
		static const auto OverrideView_fn = reinterpret_cast<void*>(get_virtual(g_ClientMode, index::OverrideView));
		ConVar* SvCheats_con = g_CVar->FindVar("sv_cheats");
		static const auto SvCheats_fn = reinterpret_cast<void*>(get_virtual(SvCheats_con, index::SvCheatsGetBool));
		static const auto FrameStageNotify_fn = reinterpret_cast<void*>(get_virtual(g_CHLClient, index::FrameStageNotify));
		static const auto FireEvent_fn = reinterpret_cast<void*>(get_virtual(g_GameEvents, index::FireEvent));
		static const LPVOID get_color_modulation = Utils::PatternScan(GetModuleHandleA("materialsystem.dll"), "55 8B EC 83 EC ? 56 8B F1 8A 46");
		static const auto is_using_static_prop_debug_modes = g_cheat.is_using_static_prop_debug_modes;
		static const auto pGetAspectRatio = reinterpret_cast<void*>(get_virtual(g_EngineClient, 101));
		static const auto pDrawModelExecute_Model = (LPVOID)((*(uintptr_t**)(g_MdlRender))[21]);
		static const auto pDrawModelExecute_Studio = (LPVOID)((*(uintptr_t**)(g_StudioRender))[29]);

		if (MH_Initialize() != MH_OK)
			throw std::runtime_error("Failed to initialize MH_Initialize.");

		if (MH_CreateHook(EndScene_fn, &EndScene::hook, reinterpret_cast<void**>(&o_EndScene)) != MH_OK)
			throw std::runtime_error("Failed to initialize EndScene. (outdated index?)");

		if (MH_CreateHook(CreateMove_fn, &CreateMove::hook, reinterpret_cast<void**>(&o_CreateMove)) != MH_OK)
			throw std::runtime_error("Failed to initialize CreateMove. (outdated index?)");

		if (MH_CreateHook(EmitSound_fn, &EmitSound::hook, reinterpret_cast<void**>(&o_EmitSound)) != MH_OK)
			throw std::runtime_error("Failed to initialize Emit Sound. (outdated index?)");

		if (MH_CreateHook(PaintTreverse_fn, &PaintTreverse::hook, reinterpret_cast<void**>(&o_PaintTreverse)) != MH_OK)
			throw std::runtime_error("Failed to initialize Paint Treverse. (outdated index?)");

		if (MH_CreateHook(DoPostScreenEffect_fn, &DoPostScreenEffects::hook, reinterpret_cast<void**>(&o_DoPostScreenEffects)) != MH_OK)
			throw std::runtime_error("Failed to initialize doPostScreenEffect. (outdated index?)");

		if (MH_CreateHook(OverrideView_fn, &OverrideView::hook, reinterpret_cast<void**>(&o_OverrideView)) != MH_OK)
			throw std::runtime_error("Failed to initialize Override View. (outdated index?)");

		if (MH_CreateHook(SvCheats_fn, &SvCheats::hook, reinterpret_cast<void**>(&o_SvCheats)) != MH_OK)
			throw std::runtime_error("Failed to initialize CVar. (outdated index?)");

		if (MH_CreateHook(LockCursor_fn, &LockCursor::hook, reinterpret_cast<void**>(&o_LockCursor)) != MH_OK)
			throw std::runtime_error("Failed to initialize Lock Cursor. (outdated index?)");

		if (MH_CreateHook(Reset_fn, &Reset::hook, reinterpret_cast<void**>(&o_Reset)) != MH_OK)
			throw std::runtime_error("Failed to initialize Reset. (outdated index?)");

		if (MH_CreateHook(FrameStageNotify_fn, &FrameStageNotify::hook, reinterpret_cast<void**>(&o_FrameStageNotify)) != MH_OK)
			throw std::runtime_error("Failed to initialize FrameStageNotify. (outdated index?)");

		if (MH_CreateHook(FireEvent_fn, &FireEvent::hook, reinterpret_cast<void**>(&o_FireEvent)) != MH_OK)
			throw std::runtime_error("Failed to initialize FireEvent. (outdated index?)");

		if (MH_CreateHook(pGetAspectRatio, &hk_aspect_ratio, reinterpret_cast<void**>(&m_GetScreenSizeAspectRatio)) != MH_OK)
			throw std::runtime_error("Failed to initialize AspectRatio. (outdated index?)");

		if (MH_CreateHook(get_color_modulation, &hk_get_color_modulation, reinterpret_cast<void**>(&m_GetColorModulation)) != MH_OK)
			throw std::runtime_error("Failed to initialize GetColorModulation. (outdated index?)");

		if (MH_CreateHook(is_using_static_prop_debug_modes, &hk_is_using_static_prop_debug_modes, reinterpret_cast<void**>(&o_is_using_static_prop_debug_modes)) != MH_OK)
			throw std::runtime_error("Failed to initialize StaticPropDebugModes. (outdated index?)");

		if (MH_CreateHook(pDrawModelExecute_Model, &hkDrawModelExecute_Model, reinterpret_cast<void**>(&m_DrawModelExecute_Model)) != MH_OK)
			throw std::runtime_error("Failed to initialize DME MODEL. (outdated index?)");

		if (MH_CreateHook(pDrawModelExecute_Studio, &hkDrawModelExecute_Studio, reinterpret_cast<void**>(&m_DrawModelExecute_Studio)) != MH_OK)
			throw std::runtime_error("Failed to initialize DME STUDIO. (outdated index?)");

	//	if (MH_CreateHook(modify_eye_position, &hkModifyEyePosition, reinterpret_cast<void**>(&m_ModifyEyePosition)) != MH_OK)
	//		throw std::runtime_error("Failed to initialize hkModifyEyePosition. (outdated index?)");

		if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
			throw std::runtime_error("Failed to enable Hooks.");

		CreateDirectoryA("C:/umbrella_csgo/", NULL);
		CreateDirectoryA("C:/umbrella_csgo/configs", NULL);
	}

	void Shutdown()
	{
		Glow::Get().Shutdown();
		MH_DisableHook(MH_ALL_HOOKS);
		MH_Uninitialize();
	}
}