#pragma once

#include "sdk.hpp"
#include <array>
#include "../helpers/utils.hpp"
#include "../helpers/math.hpp"

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

template < typename T >
static T GetVirtual(void* pClass, int nIndex)
{
	return reinterpret_cast<T>((*(int**)pClass)[nIndex]);
}

#define PushVirtual( strFunctionName, nIndex, Type, ...) \
auto strFunctionName { \
return GetVirtual< Type >( this, nIndex )( this, __VA_ARGS__ ); \
}; 

#define NETVAR(type, name, table, netvar)                           \
    type& name##() const {                                          \
        static int _##name = NetvarSys::Get().GetOffset(table, netvar);     \
        return *(type*)((uintptr_t)this + _##name);                 \
    }

#define PNETVAR(type, name, table, netvar)                           \
    type* name##() const {                                          \
        static int _##name = NetvarSys::Get().GetOffset(table, netvar);     \
        return (type*)((uintptr_t)this + _##name);                 \
    }

#define NETPROP(name, table, netvar) static RecvProp* name() \
{ \
	static auto prop_ptr = NetvarSys::Get().GetNetvarProp(table,netvar); \
	return prop_ptr; \
}

#define CUSTOM_OFFSET( name, type, prop, offset ) \
__forceinline type& name( ) \
{\
    return *( type* )( ( uintptr_t )( this ) + offset ); \
}\

inline uint32_t FindInDataMap(datamap_t* pDataMap, const std::string& szPropName)
{
	while (pDataMap)
	{
		for (int i = 0; i < pDataMap->dataNumFields; i++)
		{
			if (!pDataMap->dataDesc[i].fieldName)
				continue;

			if (pDataMap->dataDesc[i].fieldName == szPropName)
				return pDataMap->dataDesc[i].fieldOffset[TD_OFFSET_NORMAL];

			if (pDataMap->dataDesc[i].fieldType != FIELD_EMBEDDED)
				continue;

			if (!pDataMap->dataDesc[i].td)
				continue;

			const uint32_t iOffset = FindInDataMap(pDataMap->dataDesc[i].td, szPropName);
			if (!iOffset)
				continue;

			return iOffset;
		}

		pDataMap = pDataMap->baseMap;
	}

	return 0;
}

#define DATAMAP( type, name ) \
__forceinline type& name( ) \
{\
	static uint32_t g_Datamap_##name = 0;\
    if ( !g_Datamap_##name )\
        g_Datamap_##name = FindInDataMap( this->GetPredDescMap( ), #name );\
    return *( type* )( ( uintptr_t )( this ) + g_Datamap_##name );\
}
enum ObserverType_t
{
	OBS_MODE_NONE = 0,	// not in spectator mode
	OBS_MODE_DEATHCAM,	// special mode for death cam animation
	OBS_MODE_FREEZECAM,	// zooms to a target, and freeze-frames on them
	OBS_MODE_FIXED,		// view from a fixed camera position
	OBS_MODE_IN_EYE,	// follow a player in first person view
	OBS_MODE_CHASE,		// follow a player in third person view
	OBS_MODE_ROAMING,	// free roaming

	NUM_OBSERVER_MODES,
};

enum WeaponType : int
{
	WT_Invalid,
	WT_Grenade,
	WT_Knife,
	WT_Misc,
	WT_Pistol,
	WT_Submg,
	WT_Rifle,
	WT_Sniper,
	WT_Shotgun,
	WT_Machinegun,
	WT_Max
};

struct datamap_t;
class AnimationLayer;
class CBasePlayerAnimState;
class CCSGOPlayerAnimState;
class C_BaseEntity;

class C_CSWeaponData
{
public:
	void* pVTable;                       // 0x0000
	char m_strConsoleName;                   // 0x0004
	char pad_0008[12];                   // 0x0008
	int m_iMaxClip1;                       // 0x0014
	int m_iMaxClip2;                       // 0x0018
	int m_iDefaultClip1;                   // 0x001C
	int m_iDefaultClip2;                   // 0x0020
	char pad_0024[8];                    // 0x0024
	char* m_szWorldModel;                  // 0x002C
	char* m_szViewModel;                   // 0x0030
	char* m_szDroppedMode;                 // 0x0034
	char pad_0038[4];                    // 0x0038
	char* m_szShotSound;                   // 0x003C
	char pad_0040[56];                   // 0x0040
	char* m_szEmptySound;                  // 0x0078
	char pad_007C[4];                    // 0x007C
	char* m_szBulletType;                  // 0x0080
	char pad_0084[4];                    // 0x0084
	char* m_szHudName;                     // 0x0088
	char* m_szWeaponName;                  // 0x008C
	char pad_0090[56];                   // 0x0090
	int m_iWeaponType;            // 0x00C8
	char pad_00CC[4];                    // 0x00CC
	int m_iWeaponPrice;                    // 0x00D0
	int m_iKillAward;                      // 0x00D4
	char* m_szAnimationPrefix;             // 0x00D8
	float m_flCycleTime;                   // 0x00DC
	float m_flCycleTimeAlt;                // 0x00E0
	float m_flTimeToIdle;                  // 0x00E4
	float m_flIdleInterval;                // 0x00E8
	bool m_bFullAuto;                      // 0x00EC
	char pad_00ED[3];                    // 0x00ED
	int m_iDamage;                         // 0x00F0
	char valve_are_gays[4];
	float m_flArmorRatio;                  // 0x00F4
	int m_iBullets;                        // 0x00F8
	float m_flPenetration;                 // 0x00FC
	float m_flFlinchVelocityModifierLarge; // 0x0100
	float m_flFlinchVelocityModifierSmall; // 0x0104
	float m_flRange;                       // 0x0108
	float m_flRangeModifier;               // 0x010C
	float m_flThrowVelocity;               // 0x0110
	char pad_0114[12];                   // 0x0114
	bool m_bHasSilencer;                   // 0x0120
	char pad_0121[3];                    // 0x0121
	char* m_pSilencerModel;                // 0x0124
	int m_iCrosshairMinDistance;           // 0x0128
	int m_iCrosshairDeltaDistance;         // 0x012C
	float m_flMaxPlayerSpeed;              // 0x0130
	float m_flMaxPlayerSpeedAlt;           // 0x0134
	float m_flMaxPlayerSpeedMod;           // 0x0138
	float m_flSpread;                      // 0x013C
	float m_flSpreadAlt;                   // 0x0140
	float m_flInaccuracyCrouch;            // 0x0144
	float m_flInaccuracyCrouchAlt;         // 0x0148
	float m_flInaccuracyStand;             // 0x014C
	float m_flInaccuracyStandAlt;          // 0x0150
	float m_flInaccuracyJumpInitial;       // 0x0154
	float m_flInaccuracyJump;              // 0x0158
	float m_flInaccuracyJumpAlt;           // 0x015C
	float m_flInaccuracyLand;              // 0x0160
	float m_flInaccuracyLandAlt;           // 0x0164
	float m_flInaccuracyLadder;            // 0x0168
	float m_flInaccuracyLadderAlt;         // 0x016C
	float m_flInaccuracyFire;              // 0x0170
	float m_flInaccuracyFireAlt;           // 0x0174
	float m_flInaccuracyMove;              // 0x0178
	float m_flInaccuracyMoveAlt;           // 0x017C
	float m_flInaccuracyReload;            // 0x0180
	int m_iRecoilSeed;                     // 0x0184
	float m_flRecoilAngle;                 // 0x0188
	float m_flRecoilAngleAlt;              // 0x018C
	float m_flRecoilAngleVariance;         // 0x0190
	float m_flRecoilAngleVarianceAlt;      // 0x0194
	float m_flRecoilMagnitude;             // 0x0198
	float m_flRecoilMagnitudeAlt;          // 0x019C
	float m_flRecoilMagnitudeVariance;     // 0x01A0
	float m_flRecoilMagnitudeVarianceAlt;  // 0x01A4
	float m_flRecoveryTimeCrouch;          // 0x01A8
	float m_flRecoveryTimeStand;           // 0x01AC
	float m_flRecoveryTimeCrouchFinal;     // 0x01B0
	float m_flRecoveryTimeStandFinal;      // 0x01B4
	char pad_01B8[40];                   // 0x01B8
	char* m_szWeaponClass;                 // 0x01E0
	char pad_01E4[8];                    // 0x01E4
	char* m_szEjectBrassEffect;            // 0x01EC
	char* m_szTracerEffect;                // 0x01F0
	int m_iTracerFrequency;                // 0x01F4
	int m_iTracerFrequencyAlt;             // 0x01F8
	char* m_szMuzzleFlashEffect_1stPerson; // 0x01FC
	char pad_0200[4];                    // 0x0200
	char* m_szMuzzleFlashEffect_3rdPerson; // 0x0204
	char pad_0208[4];                    // 0x0208
	char* m_szMuzzleSmokeEffect;           // 0x020C
	float m_flHeatPerShot;                 // 0x0210
	char* m_szZoomInSound;                 // 0x0214
	char* m_szZoomOutSound;                // 0x0218
	float m_flInaccuracyPitchShift;        // 0x021C
	float m_flInaccuracySoundThreshold;    // 0x0220
	float m_flBotAudibleRange;             // 0x0224
	char pad_0228[12];                   // 0x0228
	bool m_bHasBurstMode;                  // 0x0234
	bool m_bIsRevolver;                    // 0x0235
};

enum CSWeaponType
{
	WEAPONTYPE_UNKNOWN = -1,
	WEAPONTYPE_KNIFE,
	WEAPONTYPE_PISTOL,
	WEAPONTYPE_SUBMACHINEGUN,
	WEAPONTYPE_RIFLE,
	WEAPONTYPE_SHOTGUN,
	WEAPONTYPE_SNIPER_RIFLE,
	WEAPONTYPE_MACHINEGUN,
	WEAPONTYPE_C4,
	WEAPONTYPE_TASER,
	WEAPONTYPE_GRENADE,
	WEAPONTYPE_HEALTHSHOT,
	WEAPONTYPE_STACKABLEITEM,
	WEAPONTYPE_FISTS,
	WEAPONTYPE_BREACHCHARGE,
	WEAPONTYPE_BUMPMINE,
	WEAPONTYPE_TABLET,
	WEAPONTYPE_MELEE
};

class C_BaseEntity;


// Created with ReClass.NET by KN4CK3R
class CHudTexture
{
public:
	char szShortName[64];    //0x0000
	char szTextureFile[64];  //0x0040
	bool bRenderUsingFont;   //0x0080
	bool bPrecached;         //0x0081
	int8_t cCharacterInFont; //0x0082
	uint8_t pad_0083[1];     //0x0083
	uint32_t hFont;          //0x0084
	int32_t iTextureId;      //0x0088
	float afTexCoords[4];    //0x008C
	uint8_t pad_009C[16];    //0x009C
};

class C_EconItemView
{
private:
	using str_32 = char[32];
public:
	NETVAR(int32_t, m_bInitialized, "DT_BaseAttributableItem", "m_bInitialized");
	NETVAR(int16_t, m_iItemDefinitionIndex, "DT_BaseAttributableItem", "m_iItemDefinitionIndex");
	NETVAR(int32_t, m_iEntityLevel, "DT_BaseAttributableItem", "m_iEntityLevel");
	NETVAR(int32_t, m_iAccountID, "DT_BaseAttributableItem", "m_iAccountID");
	NETVAR(int32_t, m_iItemIDLow, "DT_BaseAttributableItem", "m_iItemIDLow");
	NETVAR(int32_t, m_iItemIDHigh, "DT_BaseAttributableItem", "m_iItemIDHigh");
	NETVAR(int32_t, m_iEntityQuality, "DT_BaseAttributableItem", "m_iEntityQuality");
	NETVAR(str_32, m_iCustomName, "DT_BaseAttributableItem", "m_szCustomName");
};

class C_BaseEntity : public IClientEntity
{
public:
	datamap_t * GetDataDescMap() {
		typedef datamap_t*(__thiscall *o_GetPredDescMap)(void*);
		return CallVFunction<o_GetPredDescMap>(this, 15)(this);
	}

	datamap_t *GetPredDescMap() {
		typedef datamap_t*(__thiscall *o_GetPredDescMap)(void*);
		return CallVFunction<o_GetPredDescMap>(this, 17)(this);
	}
	static __forceinline C_BaseEntity* GetEntityByIndex(int index) {
		return static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(index));
	}
	static __forceinline C_BaseEntity* get_entity_from_handle(CBaseHandle h) {
		return static_cast<C_BaseEntity*>(g_EntityList->GetClientEntityFromHandle(h));
	}
	NETVAR(int, m_nBombSite, "DT_PlantedC4", "m_nBombSite");
	NETVAR(int32_t, m_nModelIndex, "DT_BaseEntity", "m_nModelIndex");
	NETVAR(int32_t, m_iTeamNum, "DT_BaseEntity", "m_iTeamNum");
	NETVAR(Vector, m_vecOrigin, "DT_BaseEntity", "m_vecOrigin");
	NETVAR(Vector, m_vecAngles, "DT_BaseEntity", "m_vecAngles");
	NETVAR(bool, m_bShouldGlow, "DT_DynamicProp", "m_bShouldGlow");
	NETVAR(CHandle<C_BasePlayer>, m_hOwnerEntity, "DT_BaseEntity", "m_hOwnerEntity");
	NETVAR(CBaseHandle, get_owner_entity, "DT_BaseEntity","m_hOwnerEntity");
	NETVAR(int32_t, m_iAccountID, "DT_CSPlayer", "m_iAccount");
	NETVAR(bool, m_bSpotted, "DT_BaseEntity", "m_bSpotted");
	NETVAR(float_t, m_flC4Blow, "DT_PlantedC4", "m_flC4Blow");
	NETVAR(int, m_iObserverMode, "DT_BasePlayer", "m_iObserverMode");
	NETVAR(CHandle <C_BaseEntity >, m_hGroundEntity, "DT_BasePlayer", "m_hGroundEntity");
	NETVAR(Vector, m_aimPunchAngleVel, "DT_BasePlayer", "m_aimPunchAngleVel");
	NETVAR(Vector, get_force, "DT_CSRagdoll", "m_vecForce");
	NETVAR(Vector, get_ragdoll_velocity, "DT_CSRagdoll", "m_vecRagdollVelocity");
	const matrix3x4_t& m_rgflCoordinateFrame()
	{
		static auto _m_rgflCoordinateFrame = NetvarSys::Get().GetOffset("DT_BaseEntity", "m_CollisionGroup") - 0x30;
		return *(matrix3x4_t*)((uintptr_t)this + _m_rgflCoordinateFrame);
	}
	DATAMAP(Vector, m_vecAbsVelocity);

	CUtlVector<matrix3x4_t>& get_cached_bone_data()
	{
		return *reinterpret_cast<CUtlVector<matrix3x4_t>*>(reinterpret_cast<uintptr_t>(this) + 0x2914);
	}

	bool IsBreakableEntity()
	{
		const auto szObjectName = this->GetClientClass()->m_pNetworkName;
		if (szObjectName[0] == 'C')
		{
			if (szObjectName[7] == 't' || szObjectName[7] == 'b')
				return true;
		}

		return ((bool(__thiscall*)(C_BaseEntity*))(g_cheat.m_IsBreakableEntity))(this);
	}
	bool is_breakable()
	{
		if (!this)
			return false;

		static auto is_breakable_fn = reinterpret_cast<bool(__thiscall*)(IClientEntity*)>(g_cheat.m_IsBreakableEntity);
		const auto result = is_breakable_fn(this);

		if (!result && GetClientClass() != nullptr &&
			(GetClientClass()->m_ClassID == classids::get("CBaseDoor") ||
				GetClientClass()->m_ClassID == classids::get("CBreakableSurface") ||
				GetClientClass()->m_ClassID == classids::get("CBaseEntity") && GetCollideable() != nullptr &&
				GetCollideable()->GetSolid() == 1))
			return true;

		return result;
	}

	bool chams_breakable()
	{
		const auto szObjectName = this->GetClientClass()->m_pNetworkName;
		if (strstr(szObjectName, "CDynamicProp"))
		{
			return true;
		}
		return false;
	}

	PushVirtual(IsPlayer(), 158, bool(__thiscall*)(void*));
	bool IsLoot();
	bool IsWeapon();
	
	bool IsPlantedC4();
	bool IsDefuseKit();
	//bool isSpotted();

	void set_abs_origin(Vector origin);
	void set_abs_angles(QAngle angles);
	
	CUSTOM_OFFSET(m_nExplodeEffectTickBegin, int32_t, "m_nExplodeEffectTickBegin", 0x29F4);
};

class c_fog_controller : public C_BaseEntity
{
public:
	NETVAR(bool, get_fog_enable, "DT_FogController", "m_fog.enable");
	NETVAR(bool, get_fog_blend, "DT_FogController", "m_fog.blend");
	NETVAR(float, get_fog_start, "DT_FogController", "m_fog.start");
	NETVAR(float, get_fog_end, "DT_FogController", "m_fog.end");
	NETVAR(float, get_fog_max_density, "DT_FogController", "m_fog.maxdensity");
	NETVAR(int, get_fog_color_primary, "DT_FogController", "m_fog.colorPrimary");
	NETVAR(int, get_fog_color_secondary, "DT_FogController", "m_fog.colorSecondary");
};

class C_PlantedC4
{
public:
	NETVAR(bool, m_bBombTicking, "DT_PlantedC4", "m_bBombTicking");
	NETVAR(bool, m_bBombDefused, "DT_PlantedC4", "m_bBombDefused");
	NETVAR(float, m_flC4Blow, "DT_PlantedC4", "m_flC4Blow");
	NETVAR(float, m_flTimerLength, "DT_PlantedC4", "m_flTimerLength");
	NETVAR(float, m_flDefuseLength, "DT_PlantedC4", "m_flDefuseLength");
	NETVAR(float, m_flDefuseCountDown, "DT_PlantedC4", "m_flDefuseCountDown");
	NETVAR(CHandle<C_BasePlayer>, m_hBombDefuser, "DT_PlantedC4", "m_hBombDefuser");
};

class C_BaseAttributableItem : public C_BaseEntity
{
public:
	NETVAR(uint64_t, m_OriginalOwnerXuid, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
	NETVAR(int32_t, m_OriginalOwnerXuidLow, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
	NETVAR(int32_t, m_OriginalOwnerXuidHigh, "DT_BaseAttributableItem", "m_OriginalOwnerXuidHigh");
	NETVAR(int32_t, m_nFallbackStatTrak, "DT_BaseAttributableItem", "m_nFallbackStatTrak");
	NETVAR(int32_t, m_nFallbackPaintKit, "DT_BaseAttributableItem", "m_nFallbackPaintKit");
	NETVAR(int32_t, m_nFallbackSeed, "DT_BaseAttributableItem", "m_nFallbackSeed");
	NETVAR(float_t, m_flFallbackWear, "DT_BaseAttributableItem", "m_flFallbackWear");
	NETVAR(short, get_idx, "DT_BaseAttributableItem", "m_iItemDefinitionIndex");
	NETVAR(C_EconItemView, m_Item2, "DT_BaseAttributableItem", "m_Item");

	C_EconItemView& m_Item()
	{
		// Cheating. It should be this + m_Item netvar but then the netvars inside C_EconItemView wont work properly.
		// A real fix for this requires a rewrite of the netvar manager
		return *(C_EconItemView*)this;
	}
	void SetGloveModelIndex(int modelIndex);

};

class C_BaseWeaponWorldModel : public C_BaseEntity
{
public:
	NETVAR(int32_t, m_nModelIndex, "DT_BaseWeaponWorldModel", "m_nModelIndex");
};

class C_BaseCombatWeapon : public C_BaseAttributableItem
{
public:
	NETVAR(float_t, m_flNextPrimaryAttack, "DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
	NETVAR(float_t, m_flNextSecondaryAttack, "DT_BaseCombatWeapon", "m_flNextSecondaryAttack");
	NETVAR(int32_t, m_iClip1, "DT_BaseCombatWeapon", "m_iClip1");
	NETVAR(int32_t, m_iClip2, "DT_BaseCombatWeapon", "m_iClip2");
	NETVAR(float_t, m_flRecoilIndex, "DT_WeaponCSBase", "m_flRecoilIndex");
	NETVAR(int32_t, m_iViewModelIndex, "DT_BaseCombatWeapon", "m_iViewModelIndex");
	NETVAR(int32_t, m_iWorldModelIndex, "DT_BaseCombatWeapon", "m_iWorldModelIndex");
	NETVAR(int32_t, m_iWorldDroppedModelIndex, "DT_BaseCombatWeapon", "m_iWorldDroppedModelIndex");
	NETVAR(bool, m_bPinPulled, "DT_BaseCSGrenade", "m_bPinPulled");
	NETVAR(float_t, m_fThrowTime, "DT_BaseCSGrenade", "m_fThrowTime");
	NETVAR(float_t, m_flPostponeFireReadyTime, "DT_BaseCombatWeapon", "m_flPostponeFireReadyTime");
	NETVAR(CHandle<C_BaseWeaponWorldModel>, m_hWeaponWorldModel, "DT_BaseCombatWeapon", "m_hWeaponWorldModel");
	NETVAR(float, m_fAccuracyPenalty, "DT_WeaponCSBase", "m_fAccuracyPenalty");
	NETVAR(int, get_zoom_level, "DT_WeaponCSBaseGun", "m_zoomLevel");
	NETVAR(float_t, m_flThrowStrength, "DT_BaseCSGrenade", "m_flThrowStrength");
	PushVirtual(GetWeaponData(), 461, C_CSWeaponData* (__thiscall*)(void*));
	bool HasBullets();
	bool CanFire();
	bool IsGrenade();
	bool IsKnife();
	bool IsReloading();
	bool IsRifle();
	bool IsPistol();
	bool IsSniper();
	bool IsGun();
	float GetInaccuracy();
	float GetSpread();
	void UpdateAccuracyPenalty();
	CUtlVector<IRefCounted*>& m_CustomMaterials();
	bool* m_bCustomMaterialInitialized();
	const char* get_weapon();
	const char* get_weapon_icon();

	inline bool is_empty() const
	{
		return m_iClip1() == 0;
	}

	__forceinline int GetWpnType() 
	{
		if (!this)
			return 0;

		return CallVFunction< int(__thiscall*)(void*) >(this, 455)(this);
	}

	inline int get_weapon_type()
	{
		if (!this)
			return WT_Invalid;

		switch (this->get_idx())
		{
		case WEAPON_DEAGLE:
		case WEAPON_P250:
		case WEAPON_USP_SILENCER:
		case WEAPON_HKP2000:
		case WEAPON_GLOCK:
		case WEAPON_FIVESEVEN:
		case WEAPON_TEC9:
		case WEAPON_ELITE:
		case WEAPON_REVOLVER:
		case WEAPON_CZ75A:
			return WT_Pistol;
		case WEAPON_MP9:
		case WEAPON_MP7:
		case WEAPON_MP5:
		case WEAPON_UMP45:
		case WEAPON_BIZON:
		case WEAPON_P90:
		case WEAPON_MAC10:
			return WT_Submg;
		case WEAPON_KNIFE_BAYONET:
		case WEAPON_KNIFE_CSS:
		case WEAPON_KNIFE_SURVIVAL_BOWIE:
		case WEAPON_KNIFE_BUTTERFLY:
		case WEAPON_KNIFE:
		case WEAPON_KNIFE_FALCHION:
		case WEAPON_KNIFE_FLIP:
		case WEAPON_KNIFE_GUT:
		case WEAPON_KNIFE_KARAMBIT:
		case WEAPON_KNIFE_M9_BAYONET:
		case WEAPON_KNIFE_PUSH:
		case WEAPON_KNIFE_TACTICAL:
		case WEAPON_KNIFE_T:
		case WEAPON_KNIFE_URSUS:
		case WEAPON_KNIFE_GYPSY_JACKKNIFE:
		case WEAPON_KNIFE_STILETTO:
		case WEAPON_KNIFE_WIDOWMAKER:
		case WEAPON_KNIFE_SKELETON:
		case WEAPON_KNIFE_OUTDOOR:
		case WEAPON_KNIFE_CORD:
		case WEAPON_KNIFE_CANIS:
			return WT_Knife;
		case WEAPON_SAWEDOFF:
		case WEAPON_XM1014:
		case WEAPON_MAG7:
		case WEAPON_NOVA:
			return WT_Shotgun;
		case WEAPON_M249:
		case WEAPON_NEGEV:
			return WT_Machinegun;
		case WEAPON_TASER:
		case WEAPON_C4:
			return WT_Misc;
		case WEAPON_HEGRENADE:
		case WEAPON_FLASHBANG:
		case WEAPON_DECOY:
		case WEAPON_SMOKEGRENADE:
		case WEAPON_INCGRENADE:
		case WEAPON_MOLOTOV:
			return WT_Grenade;
		case WEAPON_AK47:
		case WEAPON_M4A1:
		case WEAPON_M4A1_SILENCER:
		case WEAPON_GALILAR:
		case WEAPON_FAMAS:
		case WEAPON_AUG:
		case WEAPON_SG556:
			return WT_Rifle;
		case WEAPON_SCAR20:
		case WEAPON_G3SG1:
		case WEAPON_SSG08:
		case WEAPON_AWP:
			return WT_Sniper;
		default:
			return WT_Knife;
		}
	}

	CUSTOM_OFFSET(m_hThrower, CHandle < C_BasePlayer >, "m_hThrower", 0x29B0);
	CUSTOM_OFFSET(m_flDamage, float_t, "m_flDamage", 0x29A8);
};

class C_BasePlayer : public C_BaseEntity
{
public:
	float m_alpha = 255.f;
	float health_scale = 0.f;
	bool reset_bar = true;
	bool is_visible_real = false;

	static __forceinline C_BasePlayer* GetPlayerByUserId(int id)
	{
		return static_cast<C_BasePlayer*>(GetEntityByIndex(g_EngineClient->GetPlayerForUserID(id)));
	}
	static __forceinline C_BasePlayer* GetPlayerByIndex(int i)
	{
		return static_cast<C_BasePlayer*>(GetEntityByIndex(i));
	}
	
	
	NETVAR(float, get_health_boost_time, "DT_CSPlayer", "m_flHealthShotBoostExpirationTime");
	NETVAR(int, get_survival_team, "DT_CSPlayer", "m_nSurvivalTeam");
	NETVAR(bool, m_bHasDefuser, "DT_CSPlayer", "m_bHasDefuser");
	NETVAR(bool, m_bGunGameImmunity, "DT_CSPlayer", "m_bGunGameImmunity");
	NETVAR(int32_t, m_iShotsFired, "DT_CSPlayer", "m_iShotsFired");
	NETVAR(QAngle, m_angEyeAngles, "DT_CSPlayer", "m_angEyeAngles[0]");
	NETVAR(int, m_ArmorValue, "DT_CSPlayer", "m_ArmorValue");
	NETVAR(bool, m_bHasHeavyArmor, "DT_CSPlayer", "m_bHasHeavyArmor");
	NETVAR(bool, m_bHasHelmet, "DT_CSPlayer", "m_bHasHelmet");
	NETVAR(bool, m_bIsScoped, "DT_CSPlayer", "m_bIsScoped");;
	NETVAR(float, m_flLowerBodyYawTarget, "DT_CSPlayer", "m_flLowerBodyYawTarget");
	NETVAR(int32_t, m_iHealth, "DT_BasePlayer", "m_iHealth");
	NETVAR(int32_t, m_lifeState, "DT_BasePlayer", "m_lifeState");
	NETVAR(int32_t, m_fFlags, "DT_BasePlayer", "m_fFlags");
	NETVAR(int32_t, m_nTickBase, "DT_BasePlayer", "m_nTickBase");
	NETVAR(Vector, m_vecViewOffset, "DT_BasePlayer", "m_vecViewOffset[0]");
	NETVAR(QAngle, m_viewPunchAngle, "DT_BasePlayer", "m_viewPunchAngle");
	NETVAR(QAngle, m_aimPunchAngle, "DT_BasePlayer", "m_aimPunchAngle");
	NETVAR(CHandle<C_BaseViewModel>, m_hViewModel, "DT_BasePlayer", "m_hViewModel[0]");
	NETVAR(Vector, m_vecVelocity, "DT_BasePlayer", "m_vecVelocity[0]");
	NETVAR(float, m_flMaxspeed, "DT_BasePlayer", "m_flMaxspeed");
	NETVAR(CHandle<C_BasePlayer>, m_hObserverTarget, "DT_BasePlayer", "m_hObserverTarget");
	NETVAR(float, m_flFlashMaxAlpha, "DT_CSPlayer", "m_flFlashMaxAlpha");
	NETVAR(int32_t, m_nHitboxSet, "DT_BaseAnimating", "m_nHitboxSet");
	NETVAR(CHandle<C_BaseCombatWeapon>, m_hActiveWeapon, "DT_BaseCombatCharacter", "m_hActiveWeapon");
	NETVAR(int32_t, m_iAccount, "DT_CSPlayer", "m_iAccount");
	NETVAR(float, m_flFlashDuration, "DT_CSPlayer", "m_flFlashDuration");
	NETVAR(float, m_flSimulationTime, "DT_BaseEntity", "m_flSimulationTime");
	NETVAR(float, m_flCycle, "DT_BaseAnimating", "m_flCycle");
	NETVAR(int, m_nSequence, "DT_BaseViewModel", "m_nSequence");
	NETVAR(float, m_flNextAttack, "DT_BaseCombatCharacter", "m_flNextAttack");
	NETVAR(float, get_fall_velocity, "DT_BasePlayer","m_flFallVelocity");
	NETVAR(QAngle, get_punch, "DT_BasePlayer", "m_aimPunchAngle");
	//NETVAR(int, m_iAccount, "DT_CSPlayer", "m_iAccount");
	NETVAR(QAngle, get_view_punch, "DT_BasePlayer", "m_viewPunchAngle");
	NETVAR(float_t, m_flVelocityModifier, "DT_CSPlayer", "m_flVelocityModifier");
	NETVAR(CBaseHandle, m_hVehicle, "DT_BasePlayer", "m_hVehicle");
	NETVAR(int, m_nImpulse, "DT_CSPlayer", "m_nImpulse");
	NETVAR(QAngle, m_angAbsAngles, "DT_BaseEntity", "m_angAbsAngles");
	NETVAR(Vector, m_angAbsOrigin, "DT_BaseEntity", "m_angAbsOrigin");
	NETVAR(float, m_flDuckSpeed, "DT_BasePlayer", "m_flDuckSpeed");
	NETVAR(float, m_flDuckAmount, "DT_BasePlayer", "m_flDuckAmount");
	NETVAR(CHandle<C_BaseViewModel>, get_view_model, "DT_BasePlayer","m_hViewModel[0]");
	std::array<float, 24> &m_flPoseParameter() const {
		static int _m_flPoseParameter = NetvarSys::Get().GetOffset("DT_BaseAnimating", "m_flPoseParameter");
		return *(std::array<float, 24>*)((uintptr_t)this + _m_flPoseParameter);
	}



	bool using_standard_weapons_in_vechile()
	{
		static auto using_standard_weapons_in_vechile_fn = reinterpret_cast<bool(__thiscall*)(void*)>(
			Utils::PatternScan(GetModuleHandleA("client.dll"), "56 57 8B F9 8B 97 ? ? ? ? 83 FA FF 74 41"));
		return using_standard_weapons_in_vechile_fn(this);
	}
	bool physics_run_think(int index)
	{
		static auto physics_run_think_fn = reinterpret_cast<bool(__thiscall*)(void*, int)>(
			Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 EC 10 53 56 57 8B F9 8B 87"));
		return physics_run_think_fn(this, index);
	}
	void pre_think()
	{
		return CallVFunction<void(__thiscall*)(void*)>(this, 318)(this);
	}
	void think()
	{
		return CallVFunction<void(__thiscall*)(void*)>(this, 139)(this);
	}
	PNETVAR(CHandle<C_BaseCombatWeapon>, m_hMyWeapons, "DT_BaseCombatCharacter", "m_hMyWeapons");
	PNETVAR(CHandle<C_BaseAttributableItem>, m_hMyWearables, "DT_BaseCombatCharacter", "m_hMyWearables");
	PNETVAR(char, m_szLastPlaceName, "DT_BasePlayer", "m_szLastPlaceName");
	bool is_teammate(C_BasePlayer* local_player) const
	{
		if (!local_player)
			return false;

		//if (g_GameTypes->GetCurrentGameType() == 6)
		//	return local_player->get_survival_team() == -1
		//	? false
		//	: local_player->get_survival_team() == this->get_survival_team();
		return local_player->m_iTeamNum() == this->m_iTeamNum();
	}
	NETPROP(m_flLowerBodyYawTargetProp, "DT_CSPlayer", "m_flLowerBodyYawTarget");
	CUserCmd*& m_pCurrentCommand();

	/*gladiator v2*/
	void InvalidateBoneCache();
	int GetNumAnimOverlays();
	AnimationLayer *GetAnimOverlays();
	AnimationLayer *GetAnimOverlay(int i);
	int GetSequenceActivity(int sequence);
	CCSGOPlayerAnimState *GetPlayerAnimState();

	static void UpdateAnimationState(CCSGOPlayerAnimState *state, QAngle angle);
	static void ResetAnimationState(CCSGOPlayerAnimState *state);
	void CreateAnimationState(CCSGOPlayerAnimState *state);

	float_t &m_surfaceFriction()
	{
		static unsigned int _m_surfaceFriction = Utils::FindInDataMap(GetPredDescMap(), "m_surfaceFriction");
		return *(float_t*)((uintptr_t)this + _m_surfaceFriction);
	}
	Vector &m_vecBaseVelocity()
	{
		static unsigned int _m_vecBaseVelocity = Utils::FindInDataMap(GetPredDescMap(), "m_vecBaseVelocity");
		return *(Vector*)((uintptr_t)this + _m_vecBaseVelocity);
	}

	float_t &m_flMaxspeed()
	{
		static unsigned int _m_flMaxspeed = Utils::FindInDataMap(GetPredDescMap(), "m_flMaxspeed");
		return *(float_t*)((uintptr_t)this + _m_flMaxspeed);
	}


	Vector get_eye_position()
	{
		/* @note: dont fucking use get_view_origin !!! */
		/* @uc ref: https://www.unknowncheats.me/forum/counterstrike-global-offensive/338731-weapon_shootpos-rebuilt-server-code.html
		 * https://www.unknowncheats.me/forum/counterstrike-global-offensive/277792-getting-eye-position-m_vecviewoffset.html
		*/

		/* first way */
		static auto sig = reinterpret_cast<float* (__thiscall*)(void*, Vector*)>(Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 56 8B 75 08 57 8B F9 56 8B 07 FF 90 ? ? ? ?")); // 8B 07 FF 90 ? ? ? ? 80 BF ? ? ? ? ? 74 10 
		Vector eye_pos;
		sig(this, &eye_pos);
		return eye_pos;

		/* second way*/
		//Vector out_pos;
		//if (this->should_use_new_animstate())
		//	if (this->get_animation_state())
		//		this->modify_eye_pos(&out_pos);
		//return out_pos;
	}
	typedef void(__thiscall* GetShootPosition_t)(LPVOID, Vector*);
	Vector GetShootPosition()
	{
		Vector vecShootPosition = Vector(0, 0, 0);
		if (!this)
			return vecShootPosition;

		CallVFunction < GetShootPosition_t >(this, 285)(this, &vecShootPosition);
		return vecShootPosition;
	}
	Vector        GetEyePos();
	player_info_t GetPlayerInfo();
	bool          IsAlive();
	bool		  IsFlashed();
	bool          HasC4();
	Vector        GetHitboxPos(int hitbox_id);
	mstudiobbox_t * GetHitbox(int hitbox_id);
	bool          GetHitboxPos(int hitbox, Vector &output);
	Vector        GetBonePos(int bone);
	bool          CanSeePlayer(C_BasePlayer* player, int hitbox);
	bool          CanSeePlayer(C_BasePlayer* player, const Vector& pos);
	void UpdateClientSideAnimation();

	int m_nMoveType();
	QAngle * GetVAngles();
	float_t m_flSpawnTime();

	Vector world_space_center()
	{
		const Vector vecOrigin = m_vecOrigin();

		const Vector vecMins = this->GetCollideable()->OBBMins() + vecOrigin;
		const Vector vecMaxs = this->GetCollideable()->OBBMaxs() + vecOrigin;

		Vector vecSize = vecMaxs - vecMins;
		vecSize /= 2.0f;
		vecSize += vecMins;
		return vecSize;
	}

	inline bool get_update(C_BasePlayer* local_player)
	{
		return !this || this == local_player || m_iHealth() <= 0 || IsDormant() || !IsAlive() || m_bGunGameImmunity()
			|| m_fFlags() & FL_FROZEN || GetClientClass()->m_ClassID != classids::get("CCSPlayer");
	}

	inline bool is_visible(C_BasePlayer* player, const int bone)
	{
		if (!this || !player)
			return false;

		CGameTrace tr;
		Ray_t ray;
		CTraceFilter filter;
		filter.pSkip = this;

		const auto endpos = player->get_bone_position(bone);

		ray.Init(get_eye_position(), endpos);
		g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

		return tr.hit_entity == player || tr.fraction > 0.97f;
	}
	inline bool is_visible_entity(C_BaseEntity* player, Vector pos)
	{
		if (!this || !player)
			return false;

		CGameTrace tr;
		Ray_t ray;
		CTraceFilter filter;
		filter.pSkip = this;

		const auto endpos = pos;

		ray.Init(get_eye_position(), endpos);
		g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

		return tr.hit_entity == player || tr.fraction > 0.97f;
	}
	inline bool is_visible_with_smoke_check(C_BasePlayer* player, int bone)
	{
		if (!this || !player)
			return false;

		CGameTrace tr;
		Ray_t ray;
		CTraceFilter filter;
		filter.pSkip = this;

		const auto endpos = player->get_bone_position(bone);

		if (Utils::line_goes_through_smoke(get_eye_position(), endpos))
			return false;

		ray.Init(get_eye_position(), endpos);
		g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

		return tr.hit_entity == player || tr.fraction > 0.97f;
	}

	inline bool is_visible(C_BasePlayer* player, const Vector& pos)
	{
		CGameTrace tr;
		Ray_t ray;
		CTraceFilter filter;
		filter.pSkip = this;

		ray.Init(get_eye_position(), pos);
		g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

		return tr.hit_entity == player || tr.fraction > 0.97f;
	}

	inline Vector get_bone_position(int hitbox_id)
	{
		matrix3x4_t boneMatrix[MAXSTUDIOBONES];
		if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f))
		{
			const auto studio_model = g_MdlInfo->GetStudiomodel(GetModel());
			if (studio_model)
			{
				const auto hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_id);
				if (hitbox)
				{
					auto
						min = Vector{},
						max = Vector{};

					Math::VectorTransform(hitbox->bbmin, boneMatrix[hitbox->bone], min);
					Math::VectorTransform(hitbox->bbmax, boneMatrix[hitbox->bone], max);

					return (min + max) / 2.0f;
				}
			}
		}
		return Vector{};
	}

};


class c_planted_c4 : public C_BaseEntity
{
public:
	NETVAR(bool, get_bomb_ticking, "DT_PlantedC4", "m_bBombTicking");
	NETVAR(bool, get_bomb_defused, "DT_PlantedC4", "m_bBombDefused");
	NETVAR(float, get_c4_blow, "DT_PlantedC4", "m_flC4Blow");
	NETVAR(float, get_timer_length, "DT_PlantedC4", "m_flTimerLength");
	NETVAR(float, get_defuse_length, "DT_PlantedC4", "m_flDefuseLength");
	NETVAR(float, get_defuse_count_down, "DT_PlantedC4", "m_flDefuseCountDown");
	NETVAR(CHandle<C_BaseEntity>, get_bomb_defuser, "DT_PlantedC4", "m_hBombDefuser");

	[[nodiscard]] float get_bomb_timer() const;
};

class C_BaseViewModel : public C_BaseEntity
{
public:
	NETVAR(int32_t, m_nModelIndex, "DT_BaseViewModel", "m_nModelIndex");
	NETVAR(int32_t, m_nViewModelIndex, "DT_BaseViewModel", "m_nViewModelIndex");
	NETVAR(CHandle<C_BaseCombatWeapon>, m_hWeapon, "DT_BaseViewModel", "m_hWeapon");
	NETVAR(CHandle<C_BasePlayer>, m_hOwner, "DT_BaseViewModel", "m_hOwner");
	NETVAR(int, m_nSequence, "DT_BaseViewModel", "m_nSequence");
	NETVAR(int, m_iAnimationParity, "DT_BaseViewModel","m_nAnimationParity");
	void SendViewModelMatchingSequence(int sequence);

	DATAMAP(float, m_flCycle);
	DATAMAP(float, m_flAnimTime);
};

class AnimationLayer
{
public:
	char  pad_0000[20];
	// These should also be present in the padding, don't see the use for it though
	//float	m_flLayerAnimtime;
	//float	m_flLayerFadeOuttime;
	uint32_t m_nOrder; //0x0014
	uint32_t m_nSequence; //0x0018
	float_t m_flPrevCycle; //0x001C
	float_t m_flWeight; //0x0020
	float_t m_flWeightDeltaRate; //0x0024
	float_t m_flPlaybackRate; //0x0028
	float_t m_flCycle; //0x002C
	void *m_pOwner; //0x0030 // player's thisptr
	char  pad_0038[4]; //0x0034
}; //Size: 0x0038

class CCSGOPlayerAnimState
{
public:
	void* pThis;
	char pad2[91];
	void* pBaseEntity; //0x60
	void* pActiveWeapon; //0x64
	void* pLastActiveWeapon; //0x68
	float m_flLastClientSideAnimationUpdateTime; //0x6C
	int m_iLastClientSideAnimationUpdateFramecount; //0x70
	float m_flEyePitch; //0x74
	float m_flEyeYaw; //0x78
	float m_flPitch; //0x7C
	float m_flGoalFeetYaw; //0x80
	float m_flCurrentFeetYaw; //0x84
	float m_flCurrentTorsoYaw; //0x88
	float m_flUnknownVelocityLean; //0x8C //changes when moving/jumping/hitting ground
	float m_flLeanAmount; //0x90
	char pad4[4]; //NaN
	float m_flFeetCycle; //0x98 0 to 1
	float m_flFeetYawRate; //0x9C 0 to 1
	float m_fUnknown2;
	float m_fDuckAmount; //0xA4
	float m_fLandingDuckAdditiveSomething; //0xA8
	float m_fUnknown3; //0xAC
	Vector m_vOrigin; //0xB0, 0xB4, 0xB8
	Vector m_vLastOrigin; //0xBC, 0xC0, 0xC4
	float m_vVelocityX; //0xC8
	float m_vVelocityY; //0xCC
	char pad5[4];
	float m_flUnknownFloat1; //0xD4 Affected by movement and direction
	char pad6[8];
	float m_flUnknownFloat2; //0xE0 //from -1 to 1 when moving and affected by direction
	float m_flUnknownFloat3; //0xE4 //from -1 to 1 when moving and affected by direction
	float m_unknown; //0xE8
	float speed_2d; //0xEC
	float flUpVelocity; //0xF0
	float m_flSpeedNormalized; //0xF4 //from 0 to 1
	float m_flFeetSpeedForwardsOrSideWays; //0xF8 //from 0 to 2. something  is 1 when walking, 2.something when running, 0.653 when crouch walking
	float m_flFeetSpeedUnknownForwardOrSideways; //0xFC //from 0 to 3. something
	float m_flTimeSinceStartedMoving; //0x100
	float m_flTimeSinceStoppedMoving; //0x104
	unsigned char m_bOnGround; //0x108
	unsigned char m_bInHitGroundAnimation; //0x109
	char pad7[10];
	float m_flLastOriginZ; //0x114
	float m_flHeadHeightOrOffsetFromHittingGroundAnimation; //0x118 from 0 to 1, is 1 when standing
	float m_flStopToFullRunningFraction; //0x11C from 0 to 1, doesnt change when walking or crouching, only running
	char pad8[4]; //NaN
	float m_flUnknownFraction; //0x124 affected while jumping and running, or when just jumping, 0 to 1
	char pad9[4]; //NaN
	float m_flUnknown3;
	char pad10[528];
}; //Size=0x344

class DT_CSPlayerResource
{
public:
	PNETVAR(int32_t, m_nActiveCoinRank, "DT_CSPlayerResource", "m_nActiveCoinRank");
	PNETVAR(int32_t, m_nMusicID, "DT_CSPlayerResource", "m_nMusicID");
	PNETVAR(int32_t, m_nPersonaDataPublicLevel, "DT_CSPlayerResource", "m_nPersonaDataPublicLevel");
	PNETVAR(int32_t, m_nPersonaDataPublicCommendsLeader, "DT_CSPlayerResource", "m_nPersonaDataPublicCommendsLeader");
	PNETVAR(int32_t, m_nPersonaDataPublicCommendsTeacher, "DT_CSPlayerResource", "m_nPersonaDataPublicCommendsTeacher");
	PNETVAR(int32_t, m_nPersonaDataPublicCommendsFriendly, "DT_CSPlayerResource", "m_nPersonaDataPublicCommendsFriendly");
	PNETVAR(int32_t, m_iCompetitiveRanking, "DT_CSPlayerResource", "m_iCompetitiveRanking");
	PNETVAR(int32_t, m_iCompetitiveWins, "DT_CSPlayerResource", "m_iCompetitiveWins");
	PNETVAR(int32_t, m_iPlayerVIP, "DT_CSPlayerResource", "m_iPlayerVIP");
	PNETVAR(int32_t, m_iMVPs, "DT_CSPlayerResource", "m_iMVPs");
	PNETVAR(int32_t, m_iScore, "DT_CSPlayerResource", "m_iScore");
};
