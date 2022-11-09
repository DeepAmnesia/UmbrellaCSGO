#include "csgostructs.hpp"
#include "../Helpers/Math.hpp"
#include "../Helpers/Utils.hpp"


//increase it if valve added some funcs to baseentity :lillulmoa:

constexpr auto VALVE_ADDED_FUNCS = 0ull;
const char* C_BaseCombatWeapon::get_weapon()
{
	if (!this)
		return "";

	switch (get_idx())
	{
	case WEAPON_DEAGLE:
		return "Desert Eagle";
	case WEAPON_ELITE:
		return "Dual Berettas";
	case WEAPON_FIVESEVEN:
		return "Five-SeveN";
	case WEAPON_GLOCK:
		return "Glock-18";
	case WEAPON_AK47:
		return "AK-47";
	case WEAPON_AUG:
		return "AUG";
	case WEAPON_AWP:
		return "AWP";
	case WEAPON_FAMAS:
		return "FAMAS";
	case WEAPON_G3SG1:
		return "G3SG1";
	case WEAPON_GALILAR:
		return "Galil AR";
	case WEAPON_M249:
		return "M249";
	case WEAPON_M4A1:
		return "M4A4";
	case WEAPON_MAC10:
		return "MAC-10";
	case WEAPON_P90:
		return "P90";
	case WEAPON_MP5:
		return "MP5-SD";
	case WEAPON_UMP45:
		return "UMP-45";
	case WEAPON_XM1014:
		return "XM1014";
	case WEAPON_BIZON:
		return "PP-Bizon";
	case WEAPON_MAG7:
		return "MAG-7";
	case WEAPON_NEGEV:
		return "Negev";
	case WEAPON_SAWEDOFF:
		return "Sawed-Off";
	case WEAPON_TEC9:
		return "Tec-9";
	case WEAPON_TASER:
		return "ZEUS";
	case WEAPON_HKP2000:
		return "P2000";
	case WEAPON_MP7:
		return "MP7";
	case WEAPON_MP9:
		return "MP9";
	case WEAPON_NOVA:
		return "Nova";
	case WEAPON_P250:
		return "P250";
	case WEAPON_SCAR20:
		return "SCAR-20";
	case WEAPON_SG556:
		return "SG 553";
	case WEAPON_SSG08:
		return "SSG 08";
	case WEAPON_KNIFEGG:
		return "Golden Knife";
	case WEAPON_KNIFE:
		return "Knife";
	case WEAPON_FLASHBANG:
		return "Flashbang";
	case WEAPON_HEGRENADE:
		return "Hegrenade";
	case WEAPON_SMOKEGRENADE:
		return "Smoke";
	case WEAPON_MOLOTOV:
		return "Molotov";
	case WEAPON_DECOY:
		return "Decoy";
	case WEAPON_INCGRENADE:
		return "Inc Grenade";
	case WEAPON_C4:
		return "C4";
	case WEAPON_KNIFE_T:
		return "Knife";
	case WEAPON_M4A1_SILENCER:
		return "M4A1-S";
	case WEAPON_USP_SILENCER:
		return "USP-S";
	case WEAPON_CZ75A:
		return "CZ75-Auto";
	case WEAPON_REVOLVER:
		return "Revolver R8";
	case WEAPON_KNIFE_BAYONET:
		return "Bayonet";
	case WEAPON_KNIFE_CSS:
		return "CSS";
	case WEAPON_KNIFE_FLIP:
		return "Flip";
	case WEAPON_KNIFE_GUT:
		return "Gut";
	case WEAPON_KNIFE_KARAMBIT:
		return "Karambit";
	case WEAPON_KNIFE_M9_BAYONET:
		return "M9 Bayonet";
	case WEAPON_KNIFE_TACTICAL:
		return "Tactical";
	case WEAPON_KNIFE_FALCHION:
		return "Falchion";
	case WEAPON_KNIFE_SURVIVAL_BOWIE:
		return "Bowie";
	case WEAPON_KNIFE_BUTTERFLY:
		return "Butterfly";
	case WEAPON_KNIFE_PUSH:
		return "Daggers";
	case WEAPON_KNIFE_URSUS:
		return "Ursus";
	case WEAPON_KNIFE_GYPSY_JACKKNIFE:
		return "Jack Knife";
	case WEAPON_KNIFE_STILETTO:
		return "Stiletto";
	case WEAPON_KNIFE_WIDOWMAKER:
		return "Widowmaker";
	case WEAPON_KNIFE_SKELETON:
		return "Skeleton Knife";
	case WEAPON_KNIFE_OUTDOOR:
		return "Outdoor Knife";
	case WEAPON_KNIFE_CORD:
		return "Cord Knife";
	case WEAPON_KNIFE_CANIS:
		return "Canis Knife";
	}
	return "";
}

const char* C_BaseCombatWeapon::get_weapon_icon()
{
	if (!this)
		return "";

	switch (get_idx())
	{
	case WEAPON_DEAGLE:
		return "a";
	case WEAPON_ELITE:
		return "b";
	case WEAPON_FIVESEVEN:
		return "c";
	case WEAPON_GLOCK:
		return "d";
	case WEAPON_AK47:
		return "e";
	case WEAPON_AUG:
		return "f";
	case WEAPON_AWP:
		return "g";
	case WEAPON_FAMAS:
		return "h";
	case WEAPON_G3SG1:
		return "j";
	case WEAPON_GALILAR:
		return "k";
	case WEAPON_M249:
		return "i";
	case WEAPON_M4A1:
		return "l";
	case WEAPON_MAC10:
		return "n";
	case WEAPON_P90:
		return "C";
	case WEAPON_MP5:
		return "p";
	case WEAPON_UMP45:
		return "q";
	case WEAPON_XM1014:
		return "r";
	case WEAPON_BIZON:
		return "s";
	case WEAPON_MAG7:
		return "t";
	case WEAPON_NEGEV:
		return "u";
	case WEAPON_SAWEDOFF:
		return "v";
	case WEAPON_TEC9:
		return "w";
	case WEAPON_TASER:
		return "x";
	case WEAPON_HKP2000:
		return "o";
	case WEAPON_MP7:
		return "z";
	case WEAPON_MP9:
		return "A";
	case WEAPON_NOVA:
		return "B";
	case WEAPON_P250:
		return "y";
	case WEAPON_SCAR20:
		return "D";
	case WEAPON_SG556:
		return "E";
	case WEAPON_SSG08:
		return "F";
	case WEAPON_KNIFE_WIDOWMAKER:
	case WEAPON_KNIFE_T:
	case WEAPON_KNIFE_BAYONET:
	case WEAPON_KNIFE_FLIP:
	case WEAPON_KNIFE_GUT:
	case WEAPON_KNIFE_KARAMBIT:
	case WEAPON_KNIFE_M9_BAYONET:
	case WEAPON_KNIFE_TACTICAL:
	case WEAPON_KNIFE_FALCHION:
	case WEAPON_KNIFE_SURVIVAL_BOWIE:
	case WEAPON_KNIFE_BUTTERFLY:
	case WEAPON_KNIFE_PUSH:
	case WEAPON_KNIFE_URSUS:
	case WEAPON_KNIFE_GYPSY_JACKKNIFE:
	case WEAPON_KNIFE_STILETTO:
	case WEAPON_KNIFEGG:
	case WEAPON_KNIFE:
	case WEAPON_KNIFE_CORD:
	case WEAPON_KNIFE_CANIS:
	case WEAPON_KNIFE_CSS:
	case WEAPON_KNIFE_OUTDOOR:
	case WEAPON_KNIFE_SKELETON:
		return "G";
	case WEAPON_FLASHBANG:
		return "L";
	case WEAPON_HEGRENADE:
		return "I";
	case WEAPON_SMOKEGRENADE:
		return "J";
	case WEAPON_MOLOTOV:
		return "K";
	case WEAPON_DECOY:
		return "H";
	case WEAPON_INCGRENADE:
		return "M";
	case WEAPON_C4:
		return "N";
	case WEAPON_M4A1_SILENCER:
		return "m";
	case WEAPON_USP_SILENCER:
		return "P";
	case WEAPON_CZ75A:
		return "Q";
	case WEAPON_REVOLVER:
		return "R";
	}
	return "";
}

void C_BaseEntity::set_abs_origin(const Vector origin)
{
	static auto fn = reinterpret_cast<void(__thiscall*)(void*, const Vector&)>(g_cheat.set_abs_origin);
	return fn(this, origin);
}

void C_BaseEntity::set_abs_angles(const QAngle angles)
{
	static auto fn = reinterpret_cast<void(__thiscall*)(void*, const QAngle&)>(g_cheat.set_abs_angles);
	fn(this, angles);
}

bool C_BaseEntity::IsLoot() {
	return GetClientClass()->m_ClassID == ClassId_CPhysPropAmmoBox ||
		GetClientClass()->m_ClassID == ClassId_CPhysPropLootCrate ||
		GetClientClass()->m_ClassID == ClassId_CPhysPropRadarJammer ||
		GetClientClass()->m_ClassID == ClassId_CPhysPropWeaponUpgrade ||
		GetClientClass()->m_ClassID == ClassId_CDrone ||
		GetClientClass()->m_ClassID == ClassId_CDronegun ||
		GetClientClass()->m_ClassID == ClassId_CItem_Healthshot ||
		GetClientClass()->m_ClassID == ClassId_CItemCash || 
		GetClientClass()->m_ClassID == ClassId_CBumpMine;
}

bool C_BaseEntity::IsWeapon()
{
	//index: 160
	//ref: "CNewParticleEffect::DrawModel"
	//sig: 8B 80 ? ? ? ? FF D0 84 C0 74 6F 8B 4D A4
	return CallVFunction<bool(__thiscall*)(C_BaseEntity*)>(this, 166 + VALVE_ADDED_FUNCS)(this);
}


bool C_BaseEntity::IsPlantedC4()
{
	return GetClientClass()->m_ClassID == classids::get("CPlantedC4");
}

bool C_BaseEntity::IsDefuseKit()
{
	return GetClientClass()->m_ClassID == ClassId_CBaseAnimating;
}

bool C_BaseCombatWeapon::HasBullets()
{
	return !IsReloading() && m_iClip1() > 0;
}

bool C_BaseCombatWeapon::CanFire()
{
	auto owner = this->m_hOwnerEntity().Get();
	if (!owner)
		return false;

	if (IsReloading() || m_iClip1() <= 0)
		return false;

	auto flServerTime = owner->m_nTickBase() * g_GlobalVars->interval_per_tick;

	if (owner->m_flNextAttack() > flServerTime)
		return false;


	return m_flNextPrimaryAttack() <= flServerTime;
}


float c_planted_c4::get_bomb_timer() const
{
	const float get_value = this->get_c4_blow() - g_GlobalVars->curtime;
	return get_value < 0 ? 0.f : get_value;
}

bool C_BaseCombatWeapon::IsGrenade()
{
	return get_weapon_type() == WT_Grenade;
}

bool C_BaseCombatWeapon::IsGun()
{
	switch (GetWeaponData()->m_iWeaponType)
	{
	case WEAPONTYPE_C4:
		return false;
	case WEAPONTYPE_GRENADE:
		return false;
	case WEAPONTYPE_KNIFE:
		return false;
	case WEAPONTYPE_UNKNOWN:
		return false;
	default:
		return true;
	}
}

bool C_BaseCombatWeapon::IsKnife()
{
	if (this->m_Item().m_iItemDefinitionIndex() == WEAPON_TASER) return false;
	return GetWpnType() == WEAPONTYPE_KNIFE;
}

bool C_BaseCombatWeapon::IsRifle()
{
	switch (GetWeaponData()->m_iWeaponType)
	{
	case WEAPONTYPE_RIFLE:
		return true;
	case WEAPONTYPE_SUBMACHINEGUN:
		return true;
	case WEAPONTYPE_SHOTGUN:
		return true;
	case WEAPONTYPE_MACHINEGUN:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::IsPistol()
{
	switch (GetWeaponData()->m_iWeaponType)
	{
	case WEAPONTYPE_PISTOL:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::IsSniper()
{
	switch (GetWeaponData()->m_iWeaponType)
	{
	case WEAPONTYPE_SNIPER_RIFLE:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::IsReloading()
{
	static auto inReload = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client.dll"), "C6 87 ? ? ? ? ? 8B 06 8B CE FF 90") + 2);
	return *(bool*)((uintptr_t)this + inReload);
}

float C_BaseCombatWeapon::GetInaccuracy()
{
	return CallVFunction<float(__thiscall*)(void*)>(this, 483 + VALVE_ADDED_FUNCS)(this);
}

float C_BaseCombatWeapon::GetSpread()
{
	return CallVFunction<float(__thiscall*)(void*)>(this, 453 + VALVE_ADDED_FUNCS)(this);
}

void C_BaseCombatWeapon::UpdateAccuracyPenalty()
{
	CallVFunction<void(__thiscall*)(void*)>(this, 484 + VALVE_ADDED_FUNCS)(this);
}

CUtlVector<IRefCounted*>& C_BaseCombatWeapon::m_CustomMaterials()
{	static auto inReload = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client.dll"), "83 BE ? ? ? ? ? 7F 67") + 2) - 12;
	return *(CUtlVector<IRefCounted*>*)((uintptr_t)this + inReload);
}

bool* C_BaseCombatWeapon::m_bCustomMaterialInitialized()
{
	static auto currentCommand = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client.dll"), "C6 86 ? ? ? ? ? FF 50 04") + 2);
	return (bool*)((uintptr_t)this + currentCommand);
}

CUserCmd*& C_BasePlayer::m_pCurrentCommand()
{
	static auto currentCommand = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client.dll"), "89 BE ? ? ? ? E8 ? ? ? ? 85 FF") + 2);
	return *(CUserCmd**)((uintptr_t)this + currentCommand);
}

int C_BasePlayer::GetNumAnimOverlays()
{
	return *(int*)((DWORD)this + 0x298C);
}

AnimationLayer *C_BasePlayer::GetAnimOverlays()
{
	return *(AnimationLayer**)((DWORD)this + 0x2980);
}

AnimationLayer *C_BasePlayer::GetAnimOverlay(int i)
{
	if (i < 15)
		return &GetAnimOverlays()[i];
	return nullptr;
}

int C_BasePlayer::GetSequenceActivity(int sequence)
{
	auto hdr = g_MdlInfo->GetStudiomodel(this->GetModel());

	if (!hdr)
		return -1;

	// sig for stuidohdr_t version: 53 56 8B F1 8B DA 85 F6 74 55
	// sig for C_BaseAnimating version: 55 8B EC 83 7D 08 FF 56 8B F1 74 3D
	// c_csplayer vfunc 242, follow calls to find the function.
	// Thanks @Kron1Q for merge request
	static auto get_sequence_activity = reinterpret_cast<int(__fastcall*)(void*, studiohdr_t*, int)>(Utils::PatternScan(GetModuleHandle(L"client.dll"), "55 8B EC 53 8B 5D 08 56 8B F1 83"));

	return get_sequence_activity(this, hdr, sequence);
}

CCSGOPlayerAnimState *C_BasePlayer::GetPlayerAnimState()
{
	return *(CCSGOPlayerAnimState**)((DWORD)this + 0x3914);
}

void C_BasePlayer::UpdateAnimationState(CCSGOPlayerAnimState *state, QAngle angle)
{
	static auto UpdateAnimState = Utils::PatternScan(
		GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24");

	if (!UpdateAnimState)
		return;

	__asm {
		push 0
	}

	__asm
	{
		mov ecx, state

		movss xmm1, dword ptr[angle + 4]
		movss xmm2, dword ptr[angle]

		call UpdateAnimState
	}
}

void C_BasePlayer::ResetAnimationState(CCSGOPlayerAnimState *state)
{
	using ResetAnimState_t = void(__thiscall*)(CCSGOPlayerAnimState*);
	static auto ResetAnimState = (ResetAnimState_t)Utils::PatternScan(GetModuleHandleA("client.dll"), "56 6A 01 68 ? ? ? ? 8B F1");
	if (!ResetAnimState)
		return;

	ResetAnimState(state);
}

void C_BasePlayer::CreateAnimationState(CCSGOPlayerAnimState *state)
{
	using CreateAnimState_t = void(__thiscall*)(CCSGOPlayerAnimState*, C_BasePlayer*);
	static auto CreateAnimState = (CreateAnimState_t)Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46");
	if (!CreateAnimState)
		return;

	CreateAnimState(state, this);
}

Vector C_BasePlayer::GetEyePos()
{
	return  m_vecOrigin() + m_vecViewOffset();
}

player_info_t C_BasePlayer::GetPlayerInfo()
{
	player_info_t info;
	g_EngineClient->GetPlayerInfo(EntIndex(), &info);
	return info;
}

bool C_BasePlayer::IsAlive()
{
	return m_lifeState() == LIFE_ALIVE;
}

bool C_BasePlayer::IsFlashed()
{
	static auto m_flFlashMaxAlpha = NetvarSys::Get().GetOffset("DT_CSPlayer", "m_flFlashMaxAlpha");
	return *(float*)((uintptr_t)this + m_flFlashMaxAlpha - 0x8) > 200.0;
}

bool C_BasePlayer::HasC4()
{
	static auto fnHasC4
		= reinterpret_cast<bool(__thiscall*)(void*)>(
			Utils::PatternScan(GetModuleHandleW(L"client.dll"), "56 8B F1 85 F6 74 31")
			);

	return fnHasC4(this);
}

Vector C_BasePlayer::GetHitboxPos(int hitbox_id)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];

	if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f)) {
		auto studio_model = g_MdlInfo->GetStudiomodel(GetModel());
		if (studio_model) {
			auto hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_id);
			if (hitbox) {
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

mstudiobbox_t* C_BasePlayer::GetHitbox(int hitbox_id)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];

	if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f)) {
		auto studio_model = g_MdlInfo->GetStudiomodel(GetModel());
		if (studio_model) {
			auto hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_id);
			if (hitbox) {
				return hitbox;
			}
		}
	}
	return nullptr;
}

bool C_BasePlayer::GetHitboxPos(int hitbox, Vector &output)
{
	if (hitbox >= HITBOX_MAX)
		return false;

	const model_t *model = this->GetModel();
	if (!model)
		return false;

	studiohdr_t *studioHdr = g_MdlInfo->GetStudiomodel(model);
	if (!studioHdr)
		return false;

	matrix3x4_t matrix[MAXSTUDIOBONES];
	if (!this->SetupBones(matrix, MAXSTUDIOBONES, 0x100, 0))
		return false;

	mstudiobbox_t *studioBox = studioHdr->GetHitboxSet(0)->GetHitbox(hitbox);
	if (!studioBox)
		return false;

	Vector min, max;

	Math::VectorTransform(studioBox->bbmin, matrix[studioBox->bone], min);
	Math::VectorTransform(studioBox->bbmax, matrix[studioBox->bone], max);

	output = (min + max) * 0.5f;

	return true;
}

Vector C_BasePlayer::GetBonePos(int bone)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];
	if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, 0.0f)) {
		return boneMatrix[bone].at(3);
	}
	return Vector{};
}

bool C_BasePlayer::CanSeePlayer(C_BasePlayer* player, int hitbox)
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;

	auto endpos = player->GetHitboxPos(hitbox);

	ray.Init(GetEyePos(), endpos);
	g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	return tr.hit_entity == player || tr.fraction > 0.97f;
}

bool C_BasePlayer::CanSeePlayer(C_BasePlayer* player, const Vector& pos)
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;

	ray.Init(GetEyePos(), pos);
	g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	return tr.hit_entity == player || tr.fraction > 0.97f;
}

void C_BasePlayer::UpdateClientSideAnimation()
{
	return CallVFunction<void(__thiscall*)(void*)>(this, 224 + VALVE_ADDED_FUNCS)(this);
}

void C_BasePlayer::InvalidateBoneCache()
{
	static DWORD addr = (DWORD)Utils::PatternScan(GetModuleHandleA("client.dll"), "80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81");

	*(int*)((uintptr_t)this + 0xA30) = g_GlobalVars->framecount; //we'll skip occlusion checks now
	*(int*)((uintptr_t)this + 0xA28) = 0;//clear occlusion flags

	unsigned long g_iModelBoneCounter = **(unsigned long**)(addr + 10);
	*(unsigned int*)((DWORD)this + 0x2924) = 0xFF7FFFFF; // m_flLastBoneSetupTime = -FLT_MAX;
	*(unsigned int*)((DWORD)this + 0x2690) = (g_iModelBoneCounter - 1); // m_iMostRecentModelBoneCounter = g_iModelBoneCounter - 1;
}

int C_BasePlayer::m_nMoveType()
{
	return *(int*)((uintptr_t)this + 0x25C);
}

QAngle* C_BasePlayer::GetVAngles()
{
	static auto deadflag = NetvarSys::Get().GetOffset("DT_BasePlayer", "deadflag");
	return (QAngle*)((uintptr_t)this + deadflag + 0x4);
}

void C_BaseAttributableItem::SetGloveModelIndex(int modelIndex)
{
	return CallVFunction<void(__thiscall*)(void*, int)>(this, 75)(this, modelIndex);
}

void C_BaseViewModel::SendViewModelMatchingSequence(int sequence)
{
	return CallVFunction<void(__thiscall*)(void*, int)>(this, 246 + VALVE_ADDED_FUNCS)(this, sequence);
}

float_t C_BasePlayer::m_flSpawnTime()
{
	return *(float_t*)((uintptr_t)this + 0xA370);
}
