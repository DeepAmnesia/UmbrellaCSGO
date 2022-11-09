#pragma once
#include <intrin.h> 
#include <d3d9.h>

#include "../../valve_sdk/csgostructs.hpp"
#include "minhook/minhook.h" 

#include "../../render.hpp"
#include "../../menu.hpp"
#include "../../options.hpp"
#include "../../helpers/input.hpp"
#include "../../helpers/utils.hpp"

#include "../../core/features/visuals.hpp"
#include "../../core/features/glow.hpp"
#include "../../core/features/bhop.hpp"
#include "../../core/features/Radar.h"

#include "../../valve_sdk/misc/Recv.hpp"
#include "../../helpers/vfunc_hook.hpp"

#define registers uintptr_t ecx, uintptr_t edx

typedef void(__thiscall* DrawModelExecute_Model_t)(LPVOID, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);
typedef float_t(__thiscall* GetScreenSizeAspectRatio_t)(LPVOID, int32_t, int32_t);
typedef void(__thiscall* GetColorModulation_t)(LPVOID, float_t*, float_t*, float_t*);
typedef void(__thiscall* DrawModelExecute_Studio_t)(LPVOID, LPVOID, DrawModelInfo_t*, matrix3x4_t*, float_t*, float_t*, Vector&, int32_t);
typedef void(__thiscall* ModifyEyePosition_t)(LPVOID, Vector&);

namespace index
{
	constexpr auto EmitSound1 = 5;
	constexpr auto EmitSound2 = 6;
	constexpr auto EndScene = 42;
	constexpr auto Reset = 16;
	constexpr auto PaintTraverse = 41;
	constexpr auto CreateMove = 24;
	constexpr auto PlaySound = 82;
	constexpr auto FrameStageNotify = 37;
	constexpr auto DrawModelExecute = 21;
	constexpr auto DoPostScreenSpaceEffects = 44;
	constexpr auto SvCheatsGetBool = 13;
	constexpr auto OverrideView = 18;
	constexpr auto LockCursor = 67;
	constexpr auto FireEvent = 9;
}

namespace Hooks
{
	void Initialize();
	void Shutdown();

	inline unsigned int get_virtual(void* _class, unsigned int index) { return static_cast<unsigned int>((*static_cast<int**>(_class))[index]); }

	namespace CreateMove
	{
		using fn = bool(__thiscall*)(IClientMode*, float, CUserCmd*);
		bool __stdcall hook(float input_sample_frametime, CUserCmd* cmd);
	}
	inline CreateMove::fn o_CreateMove = nullptr; //

	namespace EndScene
	{
		using fn = long(__stdcall*)(IDirect3DDevice9*);
		long __stdcall hook(IDirect3DDevice9* device);
	}
	inline EndScene::fn o_EndScene = nullptr; //

	namespace Reset
	{
		using fn = long(__stdcall*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
		long __stdcall hook(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters);
	}
	inline Reset::fn o_Reset = nullptr; //

	namespace PaintTreverse
	{
		using fn = void(__thiscall*)(IPanel*, vgui::VPANEL, bool, bool);
		void __stdcall hook(vgui::VPANEL panel, bool forceRepaint, bool allowForce);
	}
	inline PaintTreverse::fn o_PaintTreverse = nullptr; //

	namespace DoPostScreenEffects
	{
		using fn = int(__thiscall*)(IClientMode*, int);
		int  __stdcall hook(int a1);
	}
	inline DoPostScreenEffects::fn o_DoPostScreenEffects = nullptr;

	namespace LockCursor
	{
		using fn = void(__thiscall*)(ISurface*);
		void __stdcall hook();
	}
	inline LockCursor::fn o_LockCursor = nullptr; //

	namespace OverrideView
	{
		using fn = void(__thiscall*)(IClientMode*, CViewSetup*);
		void __stdcall hook(CViewSetup* vsView);
	}
	inline OverrideView::fn o_OverrideView = nullptr;

	namespace EmitSound
	{
		using fn = void(__thiscall*)(void*, IRecipientFilter&, int, int, const char*, unsigned int, const char*, float, int, float, int, int, const Vector*, const Vector*, void*, bool, float, int, int);
		void __stdcall hook(IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSoundEntry, unsigned int nSoundEntryHash, const char* pSample, float flVolume, int nSeed, float flAttenuation, int iFlags, int iPitch, const Vector* pOrigin, const Vector* pDirection, void* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, int unk);
	}
	inline EmitSound::fn o_EmitSound = nullptr; //

	namespace SvCheats
	{
		using fn = bool(__thiscall*)(PVOID);
		bool __fastcall hook(PVOID pConVar, void* edx);
	}
	inline SvCheats::fn o_SvCheats = nullptr;

	namespace FrameStageNotify
	{
		using fn = void(__thiscall*)(IBaseClientDLL*, ClientFrameStage_t);
		void __stdcall hook(ClientFrameStage_t stage);
	}
	inline FrameStageNotify::fn o_FrameStageNotify = nullptr;

	namespace FireEvent
	{
		using fn = bool(__thiscall*)(IGameEventManager2*, IGameEvent*);
		bool __stdcall hook(IGameEvent* pEvent);
	}
	inline FireEvent::fn o_FireEvent = nullptr;

	inline DrawModelExecute_Model_t m_DrawModelExecute_Model = NULL;
	inline GetScreenSizeAspectRatio_t m_GetScreenSizeAspectRatio = NULL;
	inline GetColorModulation_t m_GetColorModulation = NULL;
	inline DrawModelExecute_Studio_t m_DrawModelExecute_Studio = NULL;
	inline ModifyEyePosition_t m_ModifyEyePosition = nullptr;
}