#pragma once

#define NOMINMAX
#include <Windows.h>

#include "Misc/Enums.hpp"
#include "Misc/vfunc.hpp"

#include "Math/VMatrix.hpp"
#include "Math/QAngle.hpp"
#include "Math/Vector.hpp"
#include "Misc/Studio.hpp"

#include "Interfaces/IAppSystem.hpp"
#include "Interfaces/IBaseClientDll.hpp"
#include "Interfaces/IClientEntity.hpp"
#include "Interfaces/IClientEntityList.hpp"
#include "Interfaces/IClientMode.hpp"
#include "Interfaces/IConVar.hpp"
#include "Interfaces/ICvar.hpp"
#include "Interfaces/IEngineTrace.hpp"
#include "Interfaces/IVEngineClient.hpp"
#include "Interfaces/IVDebugOverlay.hpp"
#include "Interfaces/ISurface.hpp"
#include "Interfaces/CInput.hpp"
#include "Interfaces/IVModelInfoClient.hpp"
#include "Interfaces/IVModelRender.hpp"
#include "Interfaces/IRenderView.hpp"
#include "Interfaces/IGameEventmanager.hpp"
#include "Interfaces/IMaterialSystem.hpp"
#include "Interfaces/IMoveHelper.hpp"
#include "Interfaces/IMDLCache.hpp"
#include "Interfaces/IPrediction.hpp"
#include "Interfaces/IPanel.hpp"
#include "Interfaces/IEngineSound.hpp"
#include "Interfaces/IViewRender.hpp"
#include "Interfaces/CClientState.hpp"
#include "Interfaces/IPhysics.hpp"
#include "Interfaces/IInputSystem.hpp"
#include "interfaces/IRefCounted.hpp"
#include "interfaces/IGameTypes.h"
#include "interfaces/IFileSystem.hpp"
#include "interfaces/IServer.h"
#include "interfaces/IMemAlloc.hpp"

#include "Misc/Convar.hpp"
#include "Misc/CUserCmd.hpp"
#include "Misc/glow_outline_effect.hpp"
#include "Misc/datamap.hpp"

#include "../helpers/classids.h"
#include "misc/citemsystem.h"
#include "netvars.hpp"
#include <vector>
#include <deque>

struct soung_esp_info
{
    Vector pos;
    Color clr;
    float time;
    float alpha = 1.f;
};

inline std::uint8_t* PatternScan_zaebal(void* module, const char* signature)
{
    static auto pattern_to_byte = [](const char* pattern) {
        auto bytes = std::vector<int>{};
        auto start = const_cast<char*>(pattern);
        auto end = const_cast<char*>(pattern) + strlen(pattern);

        for (auto current = start; current < end; ++current) {
            if (*current == '?') {
                ++current;
                if (*current == '?')
                    ++current;
                bytes.push_back(-1);
            }
            else {
                bytes.push_back(strtoul(current, &current, 16));
            }
        }
        return bytes;
    };

    auto dosHeader = (PIMAGE_DOS_HEADER)module;
    auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

    auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
    auto patternBytes = pattern_to_byte(signature);
    auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

    auto s = patternBytes.size();
    auto d = patternBytes.data();

    for (auto i = 0ul; i < sizeOfImage - s; ++i) {
        bool found = true;
        for (auto j = 0ul; j < s; ++j) {
            if (scanBytes[i + j] != d[j] && d[j] != -1) {
                found = false;
                break;
            }
        }
        if (found) {
            return &scanBytes[i];
        }
    }
    return nullptr;
}

struct CEconItemDefinition {
    get_virtual_fn(0, get_definition_index(), short(__thiscall*)(void*));
    get_virtual_fn(2, get_item_base_name(), const char* (__thiscall*)(void*));
    get_virtual_fn(3, get_item_type_name(), const char* (__thiscall*)(void*));
    get_virtual_fn(4, get_item_desc(), const char* (__thiscall*)(void*));
    get_virtual_fn(5, get_inventory_image(), const char* (__thiscall*)(void*));
    get_virtual_fn(6, get_base_player_display_model(), const char* (__thiscall*)(void*));
    get_virtual_fn(7, get_world_display_model(), const char* (__thiscall*)(void*));
    get_virtual_fn(12, get_rarity(), char(__thiscall*)(void*));
    get_virtual_fn(44, get_num_supported_sticker_slots(), int(__thiscall*)(void*));
    get_static_offset(const char*, get_weapon_name, 0x1DC);
    get_static_offset(int, get_loadout_slot, 0x268);
};

struct CEconPaintKitDefinition {
    get_static_offset(int, get_paint_kit, NULL);
    get_static_offset(int, get_rarity_value, 0x78);
    get_static_offset(const char*, get_skin_name, 0x4);
    get_static_offset(const char*, get_hud_name, 0x24);
};

struct CEconStickerDefinition {
    get_static_offset(int, get_sticker_id, NULL);
    get_static_offset(const char*, get_hud_name, 0x28);
    get_static_offset(const char*, get_inventory_image, 0x58);
};

struct CEconMusicDefinition {
    get_static_offset(int, get_kit_id, NULL);
    get_static_offset(const char*, get_hud_name, 0x8);
    get_static_offset(const char*, get_inventory_image, 0x14);
};

struct CCStrike15ItemSchema {
    get_virtual_fn(3, get_item_definition_count(), int(__thiscall*)(void*));
    get_virtual_fn(42, get_item_definition_by_name(const char* name), CEconItemDefinition* (__thiscall*)(void*, const char*), name);

    uint8_t* get_item_definition_by_map() {
        return *reinterpret_cast<uint8_t**>(uintptr_t(this) + *reinterpret_cast<uint8_t**>(PatternScan_zaebal(GetModuleHandleA("client.dll"), "8B 87 ? ? ? ? 83 7C 88 ? ? 7C 0A 8B 44 88 04 5F 5E 5D C2 04 00 8B 87") + 0x2) - 0x4);
    }

    CEconItemDefinition* get_item_definition_by_map_index(int i) {

        if (i < 0 || i >= get_item_definition_count())
            return nullptr;

        return *reinterpret_cast<CEconItemDefinition**>(get_item_definition_by_map() + 0x4 * (i * 0x3) + sizeof(void*));
    }
};

struct CCStrike15ItemSystem {
    get_virtual_fn(0, get_item_schema_interface(), CCStrike15ItemSchema* (__thiscall*)(void*));
};

 
struct IDirect3DDevice9;


enum CollisionGroup : int
{
    COLLISION_GROUP_NONE = 0,
    COLLISION_GROUP_DEBRIS,
    COLLISION_GROUP_DEBRIS_TRIGGER,
    COLLISION_GROUP_INTERACTIVE_DEBRIS,
    COLLISION_GROUP_INTERACTIVE,
    COLLISION_GROUP_PLAYER,
    COLLISION_GROUP_BREAKABLE_GLASS,
    COLLISION_GROUP_VEHICLE,
    COLLISION_GROUP_PLAYER_MOVEMENT,
    COLLISION_GROUP_NPC,
    COLLISION_GROUP_IN_VEHICLE,
    COLLISION_GROUP_WEAPON,
    COLLISION_GROUP_VEHICLE_CLIP,
    COLLISION_GROUP_PROJECTILE,
    COLLISION_GROUP_DOOR_BLOCKER,
    COLLISION_GROUP_PASSABLE_DOOR,
    COLLISION_GROUP_DISSOLVING,
    COLLISION_GROUP_PUSHAWAY,
    COLLISION_GROUP_NPC_ACTOR,
    COLLISION_GROUP_NPC_SCRIPTED,
    LAST_SHARED_COLLISION_GROUP
};

namespace Interfaces
{
    void Initialize();
    void Dump();
}
class C_AnimationLayer
{
public:
    void* m_pThis;			 //0x0000
    float			m_animation_time;	 //0x0004
    float			m_fadeout_time;	 //0x0008
    int				m_flags;			 //0x001C
    int				m_activity;		 //0x0010
    int				m_order;			 //0x0014
    int				m_sequence;		 //0x0018
    float			m_prev_cycle;		 //0x001C
    float			m_weight;			 //0x0020
    float			m_weight_delta_rate; //0x0024
    float			m_playback_rate;	 //0x0028
    float			m_cycle;			 //0x002C
    void* m_owner;			 //0x0030
    int				m_bit_flags;		 //0x0034
};

#pragma pack(push, 1)
struct C_AimLayer
{
    float m_flUnknown0;
    float m_flTotalTime;
    float m_flUnknown1;
    float m_flUnknown2;
    float m_flWeight;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct C_AimLayers
{
    C_AimLayer layers[3];
};
#pragma pack(pop)

struct AnimstatePose_t
{
    bool		m_bInitialized;
    int			m_nIndex;
    const char* m_szName;

    AnimstatePose_t()
    {
        m_bInitialized = false;
        m_nIndex = -1;
        m_szName = "";
    }
};
struct procedural_foot_t
{
    Vector m_vecPosAnim;
    Vector m_vecPosAnimLast;
    Vector m_vecPosPlant;
    Vector m_vecPlantVel;
    float m_flLockAmount;
    float m_flLastPlantTime;
};

class C_CSGOPlayerAnimationState
{
public:
    void* m_this;
    bool					m_is_reset;
    bool					m_unknown_client_boolean;
    char					m_aSomePad[2];
    int						m_nTick;
    float					m_flashed_start_time;
    float					m_flashed_end_time;
    C_AimLayers				m_aim_layers;
    int						m_iModelIndex;
    int						m_iUnknownArray[3];
    C_BasePlayer*           m_base_player;
    LPVOID					m_weapon;
    LPVOID					m_weapon_last;
    float					m_last_update_time;
    int						m_nLastUpdateFrame;
    float					m_last_update_increment;
    float					m_eye_yaw;
    float					m_eye_pitch;
    float					m_foot_yaw;
    float					m_foot_yaw_last;
    float					m_move_yaw;
    float					m_move_yaw_ideal;
    float					m_move_yaw_current_to_ideal;
    float					m_time_to_align_lower_body;
    float					m_primary_cycle;
    float					m_move_weight;
    float					m_move_weight_smoothed;
    float					m_anim_duck_amount;
    float					m_duck_additional;
    float					m_recrouch_weight;
    Vector					m_position_current;
    Vector					m_position_last;
    Vector					m_velocity;
    Vector					m_velocity_normalized;
    Vector					m_velocity_normalized_non_zero;
    float					m_velocity_length_xy;
    float					m_velocity_length_z;
    float					m_speed_as_portion_of_run_top_speed;
    float					m_speed_as_portion_of_walk_top_speed;
    float					m_speed_as_portion_of_crouch_top_speed;
    float					m_duration_moving;
    float					m_duration_still;
    bool					m_on_ground;
    bool					m_landing;
    char					m_pad[2];
    float					m_jump_to_fall;
    float					m_duration_in_air;
    float					m_left_ground_height;
    float					m_land_anim_multiplier;
    float					m_walk_to_run_transition;
    bool					m_landed_on_ground_this_frame;
    bool					m_left_the_ground_this_frame;
    float					m_in_air_smooth_value;
    bool					m_on_ladder;
    float					m_ladder_weight;
    float					m_ladder_speed;
    bool					m_walk_to_run_transition_state;
    bool					m_defuse_started;
    bool					m_plant_anim_started;
    bool					m_twitch_anim_started;
    bool					m_adjust_started;
    char					m_activity_modifiers[20];
    float					m_next_twitch_time;
    float					m_time_of_last_known_injury;
    float					m_last_velocity_test_time;
    Vector					m_velocity_last;
    Vector					m_target_acceleration;
    Vector					m_acceleration;
    float					m_acceleration_weight;
    float					m_aim_matrix_transition;
    float					m_aim_matrix_transition_delay;
    bool					m_flashed;
    float					m_strafe_change_weight;
    float					m_strafe_change_target_weight;
    float					m_strafe_change_cycle;
    int						m_nStrafeSequence;
    bool					m_strafe_changing;
    float					m_duration_strafing;
    float					m_foot_lerp;
    bool					m_feet_crossed;
    bool					m_playerIsAccelerating;
    AnimstatePose_t			m_tPoseParamMappings[20];
    float					m_duration_move_weight_is_too_high;
    float					m_static_approach_speed;
    int						m_nPreviousMoveState;
    float					m_stutter_step;
    float					m_action_weight_bias_remainder;
    procedural_foot_t		m_foot_left;
    procedural_foot_t		m_foot_right;
    float					m_camera_smooth_height;
    bool					m_smooth_height_valid;
    float					m_last_time_velocity_over_ten;
    float					m_fuck_valve;
    float					m_aim_yaw_min;
    float					m_aim_yaw_max;
    float					m_aim_pitch_min;
    float					m_aim_pitch_max;
    int						m_nAnimstateModelVersion;

    int32_t SelectSequenceFromActivityModifier(int32_t iActivity);
    bool IsLayerSequenceFinished(C_AnimationLayer* pAnimationLayer, float_t flTime);
    void SetLayerSequence(C_AnimationLayer* pAnimationLayer, int32_t iActivity);
    void SetLayerRate(C_AnimationLayer* pAnimationLayer, float_t flRate);
    void SetLayerWeight(C_AnimationLayer* pAnimationLayer, float_t flWeight);
    void IncrementLayerCycle(C_AnimationLayer* Layer, bool bIsLoop);
    void SetLayerCycle(C_AnimationLayer* pAnimationLayer, float_t flCycle);
};

inline IVEngineClient*       g_EngineClient   = nullptr;
inline IBaseClientDLL*       g_CHLClient      = nullptr;
inline IClientEntityList*    g_EntityList     = nullptr;
inline CGlobalVarsBase*      g_GlobalVars     = nullptr;
inline IEngineTrace*         g_EngineTrace    = nullptr;
inline ICvar*                g_CVar           = nullptr;
inline IPanel*               g_VGuiPanel      = nullptr;
inline IClientMode*          g_ClientMode     = nullptr;
inline IVDebugOverlay*       g_DebugOverlay   = nullptr;
inline ISurface*             g_VGuiSurface    = nullptr;
inline CInput*               g_Input          = nullptr;
inline IVModelInfoClient*    g_MdlInfo        = nullptr;
inline IVModelRender*        g_MdlRender      = nullptr;
inline IVRenderView*         g_RenderView     = nullptr;
inline IMaterialSystem*      g_MatSystem      = nullptr;
inline IGameEventManager2*   g_GameEvents     = nullptr;
inline IMoveHelper*          g_MoveHelper     = nullptr;
inline IMDLCache*            g_MdlCache       = nullptr;
inline IPrediction*          g_Prediction     = nullptr;
inline CGameMovement*        g_GameMovement   = nullptr;
inline IEngineSound*         g_EngineSound    = nullptr;
inline CGlowObjectManager*   g_GlowObjManager = nullptr;
inline IViewRender*          g_ViewRender     = nullptr;
inline IDirect3DDevice9*     g_D3DDevice9     = nullptr;
inline CClientState*         g_ClientState    = nullptr;
inline IPhysicsSurfaceProps* g_PhysSurface    = nullptr;
inline IInputSystem*         g_InputSystem    = nullptr;
inline IWeaponSystem*        g_WeaponSystem   = nullptr;
inline IGameTypes*           g_GameTypes      = nullptr;
inline IFileSystem*          g_FileSystem     = nullptr;
inline IServerGameDLL*       g_Server         = nullptr;
inline IStudioRender*        g_StudioRender   = nullptr;
inline IMemAlloc*            g_MemAlloc       = nullptr;
template<typename... Args>
void ConMsg(const char* pMsg, Args... args)
{
    static auto import = (void(*)(const char*, ...))GetProcAddress(GetModuleHandleW(L"tier0.dll"), "?ConMsg@@YAXPBDZZ");
    return import(pMsg, args...);
}
template<typename... Args>
void ConColorMsg(const Color& clr, const char* pMsg, Args... args)
{
    static auto import = (void(*)(const Color&, const char*, ...))GetProcAddress(GetModuleHandleW(L"tier0.dll"), "?ConColorMsg@@YAXABVColor@@PBDZZ");
    return import(clr, pMsg, args...);
}

#include "Misc/EHandle.hpp"

class C_LocalPlayer
{
    friend bool operator==(const C_LocalPlayer& lhs, void* rhs);
public:
    C_LocalPlayer() : m_local(nullptr) {}

    operator bool() const { return *m_local != nullptr; }
    operator C_BasePlayer*() const { return *m_local; }

    C_BasePlayer* operator->() { return *m_local; }

private:
    C_BasePlayer** m_local;
};

inline C_LocalPlayer g_LocalPlayer;



class C_GlobalVariables
{
public:
    int cur_cfg_idx = 0;
    Vector origin;
    CUserCmd* cmd;
    bool* m_pbSendPacket;
    bool in_third_person = false;
    bool reset_third_person = false;

    bool config_autosave = true;
    std::string cfg_name = "";
    std::string global_bomb_site = "A";
    bool clear_bomb_data = false;
    uint8_t* get_line_goes_through_smoke = nullptr; //get_signature(get_line_goes_through_smoke, XORSTR("client.dll"), XORSTR("55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0"), NULL);
    uint8_t* item_system = nullptr; //get_signature(item_system, XORSTR("client.dll"), XORSTR("A1 ? ? ? ? 85 C0 75 53"), NULL);
    uint8_t* item_schema_map_offset = nullptr;//get_signature(item_schema_map_offset, XORSTR("client.dll"), XORSTR("8B 87 ? ? ? ? 83 7C 88 ? ? 7C 0A 8B 44 88 04 5F 5E 5D C2 04 00 8B 87"), 0x2);
    uint8_t* smoke_count = nullptr;
    uint8_t* set_abs_origin = nullptr;
    uint8_t* set_abs_angles = nullptr;
    uint8_t* m_IsBreakableEntity = nullptr;
    uint8_t* m_TraceFilterSimple = nullptr;
    std::deque<soung_esp_info> sounds_info;
    Vector m_vecShootPosition = Vector(0, 0, 0);
    uint8_t* unknown_file_sys_ret_addr = nullptr;
    uint8_t* element_fn = nullptr;
    uint8_t* element_this = nullptr;
    uint8_t* is_using_static_prop_debug_modes = nullptr;
    uint8_t* custom_sky_box = nullptr;
    uint8_t* m_TraceFilterSkipTwoEntities = nullptr;
    uint8_t* material = nullptr;
    uint8_t* m_KeyValuesFindKey = nullptr;
    uint8_t* m_KeyValuesGetString = nullptr;
    uint8_t* m_KeyValuesLoadFromBuffer = nullptr;
    uint8_t* m_KeyValuesSetString = nullptr;
    uint8_t* m_DisableRenderTargetAllocationForever = nullptr;
    uint8_t* m_SetMergedMDL = nullptr;
    uint8_t* m_SetupBonesForAttachmentQueries = nullptr;
    uint8_t* m_CreateModel = nullptr;

    float m_flModelRotation = 180.f;
    bool m_bDrawModel = false;
    bool bomb_is_planting = false;
    bool bomb_begin_defuse = false;
    bool defuser_has_kits = false;
    float bomb_timer_stamp = 0;
    float plant_time = 0;
    bool update_radar = false;

    [[nodiscard]] CCStrike15ItemSystem* get_item_system() const
    {
        static auto fn = reinterpret_cast<CCStrike15ItemSystem * (__stdcall*)()>(item_system);
        return fn();
    }

    [[nodiscard]] CCStrike15ItemSchema* get_item_schema() const
    {
        return get_item_system()->get_item_schema_interface();
    }

};

inline C_GlobalVariables g_cheat;




