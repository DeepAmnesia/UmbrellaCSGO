#pragma once
#include "../../valve_sdk/csgostructs.hpp"
#include "../../options.hpp"



class C_Chams
{
public:
	virtual void CreateMaterials();
	virtual void DrawModel(C_ChamsSettings Settings, IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix, bool bForceNull = false, bool bXQZ = false);
	virtual void OnModelRender(IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix);
	virtual void ApplyMaterial(int32_t iMaterial, bool bIgnoreZ, Color aColor, bool bCustom = false, bool bShotChams = false);
	virtual void AddOwnMaterial(IMaterial* material, char* name);

	std::vector<const char*> names;
	IMaterial* generated_material = nullptr;
private:
	IMaterial* m_pFlatMaterial = nullptr;
	IMaterial* m_pGlowMaterial = nullptr;
	IMaterial* m_pPulseMaterial = nullptr;
	IMaterial* m_pRegularMaterial = nullptr;
	IMaterial* m_pGlassMaterial = nullptr;
	IMaterial* m_pGhostMaterial = nullptr;
	IMaterial* m_pHaloMaterial = nullptr;
	
	std::deque<IMaterial*> materials;
	
};

inline C_Chams* g_Chams = new C_Chams();