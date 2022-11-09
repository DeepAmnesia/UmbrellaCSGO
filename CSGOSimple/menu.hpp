#pragma once

#include <string>
#include "singleton.hpp"
#include "imgui/imgui.h"
#include "helpers/utils.hpp"
#include <d3d9.h>

struct IDirect3DDevice9;

class Menu
    : public Singleton<Menu>
{
public:
    void Initialize();
    void Shutdown();

    void OnDeviceLost();
    void OnDeviceReset();


	int bool_to_int(bool src)
	{
		return src;
	}

    void Render();

    void Toggle();

    bool IsVisible() const { return Utils::toggled_keys[0x2D]; }

private:
    void CreateStyle();

    ImGuiStyle        _style;
};


struct Texture_t
{
	std::byte			pad0[0xC];		// 0x0000
	IDirect3DTexture9* lpRawTexture;	// 0x000C
};


class ITexture
{
private:
	template <typename T, typename ... args_t>
	constexpr T CallVFunc(void* thisptr, std::size_t nIndex, args_t... argList)
	{
		using VirtualFns = T(__thiscall*)(void*, decltype(argList)...);
		return (*static_cast<VirtualFns**>(thisptr))[nIndex](thisptr, argList...);
	}
private:
	std::byte	pad0[0x50];		 // 0x0000
public:
	Texture_t** pTextureHandles; // 0x0050

	int GetActualWidth()
	{
		return CallVFunc<int>(this, 3);
	}

	int GetActualHeight()
	{
		return CallVFunc<int>(this, 4);
	}

	void IncrementReferenceCount()
	{
		CallVFunc<void>(this, 10);
	}

	void DecrementReferenceCount()
	{
		CallVFunc<void>(this, 11);
	}
};

class IRenderToRTHelperObject
{
public:
	virtual void Draw(const matrix3x4_t& matRootToWorld) = 0;
	virtual bool BoundingSphere(Vector& vecCenter, float& flRadius) = 0;
	virtual ITexture* GetEnvCubeMap() = 0;
};

class CCustomMaterialOwner
{
public:
	virtual ~CCustomMaterialOwner() { }
	virtual void SetCustomMaterial(void* pCustomMaterial, int nIndex = 0) = 0;	// either replaces and existing material (releasing the old one), or adds one to the vector
	virtual void OnCustomMaterialsUpdated() {}
	virtual void DuplicateCustomMaterialsToOther(CCustomMaterialOwner* pOther) const = 0;

public:
	CUtlVector<void*> vecCustomMaterials;
}; // Size: 0x0014

struct MDLSquenceLayer_t
{
	int		nSequenceIndex;
	float	flWeight;
};

enum MorphFormatFlags_t
{
	MORPH_POSITION = 0x0001,	// 3D
	MORPH_NORMAL = 0x0002,	// 3D
	MORPH_WRINKLE = 0x0004,	// 1D
	MORPH_SPEED = 0x0008,	// 1D
	MORPH_SIDE = 0x0010,	// 1D
};

#define MAXSTUDIOBONECTRLS			4
#define MAXSTUDIOPOSEPARAM			24
#define MAXSTUDIOSKINS				32		// total textures
#define MAXSTUDIOFLEXCTRL			96		// maximum number of flexcontrollers (input sliders)
#define MAXSTUDIOANIMBLOCKS			256
#define MAXSTUDIOFLEXDESC			1024	// maximum number of low level flexes (actual morph targets)

class CMDL
{
public:
	std::byte	pad[0x3C]; // 0x0000
	MDLHandle_t	hModelHandle; // 0x003C
	std::byte	pad0[0x8]; // 0x003E
	Color		Color; // 0x0046
	std::byte	pad1[0x2]; // 0x004A
	int			nSkin; // 0x004C
	int			nBody; // 0x0050
	int			nSequence; // 0x0054
	int			nLOD; // 0x0058
	float		flPlaybackRate; // 0x005C
	float		flTime; // 0x0060
	float		flCurrentAnimEndTime; // 0x0064
	float		arrFlexControls[MAXSTUDIOFLEXCTRL * 4]; // 0x0068
	Vector		vecViewTarget; // 0x0668
	bool		bWorldSpaceViewTarget; // 0x0674
	bool		bUseSequencePlaybackFPS; // 0x0675
	std::byte	pad2[0x2]; // 0x0676
	void* pProxyData; // 0x0678
	float		flTimeBasisAdjustment; // 0x067C
	std::byte	pad3[0x4]; // 0x0680 --isn't correct after this point iirc
	CUtlVector<int> arrSequenceFollowLoop; // 0x0684
	matrix3x4_t	matModelToWorld; // 0x0698
	bool		bRequestBoneMergeTakeover; // 0x06C8
}; // Size: 0x06C9 // 0x6D0?

class C_MergedMDL : public IRenderToRTHelperObject
{
public:
	virtual ~C_MergedMDL() { }
	virtual void SetMDL(MDLHandle_t hModelHandle, CCustomMaterialOwner* pCustomMaterialOwner = nullptr, void* pProxyData = nullptr) = 0;
	virtual void SetMDL(const char* szModelPath, CCustomMaterialOwner* pCustomMaterialOwner = nullptr, void* pProxyData = nullptr) = 0;

	void SetMergedMDL(const char* szModelPath, CCustomMaterialOwner* pCustomMaterialOwner = nullptr, void* pProxyData = nullptr);

	void SetupBonesForAttachmentQueries();

	void SetSequence(const int nSequence, const bool bUseSequencePlaybackFPS)
	{
		this->RootMDL.nSequence = nSequence;
		this->RootMDL.bUseSequencePlaybackFPS = bUseSequencePlaybackFPS;
		this->RootMDL.flTimeBasisAdjustment = this->RootMDL.flTime;
	}

	void SetSkin(int nSkin)
	{
		this->RootMDL.nSkin = nSkin;
	}

public:
	CMDL RootMDL; // 0x0000
	CUtlVector<CMDL> vecMergedModels; // 0x069C
	float arrPoseParameters[MAXSTUDIOPOSEPARAM]; // 0x06E9
	int	nSequenceLayers; // 0x0749
	MDLSquenceLayer_t sequenceLayers[8]; // 0x074D -> // end: 0x78D
}; // Expected Size: 0x075C