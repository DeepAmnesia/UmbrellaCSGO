#include "game_chams.h"
#include "../../valve_sdk/interfaces/KeyValues.hpp"
#include "../hooks/hooks.hpp"

void C_Chams::AddOwnMaterial(IMaterial* material, char* name)
{
	names.push_back(name);
	materials.push_back(material);
}

void C_Chams::OnModelRender(IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix)
{
	bool bIsPlayer = strstr(pInfo.pModel->szName, "models/player");
	const bool bIsArm = strstr(pInfo.pModel->szName, "arms");

	C_BaseEntity* pBaseEntity = reinterpret_cast <C_BaseEntity*> (g_EntityList->GetClientEntity(pInfo.entity_index));
	
	if (pBaseEntity)
	{
		if (pBaseEntity->IsPlayer())
			bIsPlayer = true;
	}
	if (bIsPlayer)
	{
		C_BasePlayer* pPlayer = (C_BasePlayer*)(pBaseEntity);
		if (!pPlayer)
		{
			Hooks::m_DrawModelExecute_Model(g_MdlRender, pCtx, pState, pInfo, aMatrix);
			return g_MdlRender->ForcedMaterialOverride(nullptr);
		}

		bool bIsValid = (pPlayer->IsAlive() && !pPlayer->IsDormant() && pPlayer->IsPlayer()) || (pPlayer->GetClientClass()->m_ClassID == classids::get("CCSRagdoll") && g_Options.m_bDrawRagdolls);
		if (!bIsValid)
		{
			Hooks::m_DrawModelExecute_Model(g_MdlRender, pCtx, pState, pInfo, aMatrix);
			return g_MdlRender->ForcedMaterialOverride(nullptr);
		}

		C_ChamsSettings aVisible = C_ChamsSettings();
		C_ChamsSettings aInvisible = C_ChamsSettings();

		if (pPlayer == g_LocalPlayer)
			aVisible = g_Options.m_aChamsSettings[4];
		else if (pPlayer->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
		{
			aVisible = g_Options.m_aChamsSettings[2];
			aInvisible = g_Options.m_aChamsSettings[3];
		}
		else
		{
			aVisible = g_Options.m_aChamsSettings[0];
			aInvisible = g_Options.m_aChamsSettings[1];
		}

		if (aInvisible.m_bRenderChams)
			this->DrawModel(aInvisible, pCtx, pState, pInfo, aMatrix, true, true);

		if (aVisible.m_bRenderChams)
			this->DrawModel(aVisible, pCtx, pState, pInfo, aMatrix, false, false);
	}
	else if (bIsArm && g_Options.m_aChamsSettings[5].m_bRenderChams)
		this->DrawModel(g_Options.m_aChamsSettings[5], pCtx, pState, pInfo, aMatrix, false, false);

	Hooks::m_DrawModelExecute_Model(g_MdlRender, pCtx, pState, pInfo, aMatrix);
	return g_MdlRender->ForcedMaterialOverride(nullptr);
}

void C_Chams::DrawModel(C_ChamsSettings Settings, IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix, bool bForceNull, bool bXQZ)
{
	this->ApplyMaterial(Settings.m_iMainMaterial, bXQZ, Settings.m_Color);
	Hooks::m_DrawModelExecute_Model(g_MdlRender, pCtx, pState, pInfo, aMatrix);

	if (Settings.m_aModifiers[0])
	{
		this->ApplyMaterial(2, bXQZ, Settings.m_aModifiersColors[0]);
		Hooks::m_DrawModelExecute_Model(g_MdlRender, pCtx, pState, pInfo, aMatrix);
	}

	if (Settings.m_aModifiers[1])
	{
		this->ApplyMaterial(3, bXQZ, Settings.m_aModifiersColors[1]);
		Hooks::m_DrawModelExecute_Model(g_MdlRender, pCtx, pState, pInfo, aMatrix);
	}

	if (Settings.m_aModifiers[2])
	{
		this->ApplyMaterial(4, bXQZ, Settings.m_aModifiersColors[2]);
		Hooks::m_DrawModelExecute_Model(g_MdlRender, pCtx, pState, pInfo, aMatrix);
	}

	if (Settings.m_aModifiers[3])
	{
		this->ApplyMaterial(5, bXQZ, Settings.m_aModifiersColors[3]);
		Hooks::m_DrawModelExecute_Model(g_MdlRender, pCtx, pState, pInfo, aMatrix);
	}

	if (!bForceNull)
		return;

	const float aWhite[3]
		=
	{
		1.0f,
		1.0f,
		1.0f
	};

	g_RenderView->SetBlend(1.0f);
	g_RenderView->SetColorModulation(aWhite);

	return g_MdlRender->ForcedMaterialOverride(NULL);
}

#define MACRO_CONCAT( x, y ) x##y
#define CREATE_MATERIAL( var, name, content ) C_KeyValues* MACRO_CONCAT( KV, var ) = new C_KeyValues( name ); \
    MACRO_CONCAT( KV, var )->LoadFromBuffer( MACRO_CONCAT( KV, var ), name, content ); \
    var = g_MatSystem->CreateMaterial( name, MACRO_CONCAT( KV, var ) ); \
	var->IncrementReferenceCount( );\

void C_Chams::CreateMaterials()
{
	CREATE_MATERIAL(generated_material, "u_custom", "UnlitGeneric{ }");

	CREATE_MATERIAL(m_pGlowMaterial, "u_glow", R"#( "VertexLitGeneric" 
        {
          "$additive" "1"
          "$envmap" "models/effects/cube_white"
          "$envmaptint" "[1 1 1]"
          "$envmapfresnel" "1"
          "$envmapfresnelminmaxexp" "[0 1 2]"
        })#");

	CREATE_MATERIAL(m_pFlatMaterial, "u_flat", "UnlitGeneric { }");

	this->m_pRegularMaterial = g_MatSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL);
	this->m_pGhostMaterial = g_MatSystem->FindMaterial("dev/glow_armsrace", TEXTURE_GROUP_OTHER);
	this->m_pGlassMaterial = g_MatSystem->FindMaterial("models/inventory_items/trophy_majors/gloss", TEXTURE_GROUP_MODEL);
	this->m_pPulseMaterial = g_MatSystem->FindMaterial("models/inventory_items/dogtags/dogtags_outline", TEXTURE_GROUP_MODEL);

	materials.push_back(m_pFlatMaterial);
	materials.push_back(m_pRegularMaterial);
	materials.push_back(m_pGlowMaterial);
	materials.push_back(m_pGhostMaterial);

	names.push_back("Flat");
	names.push_back("Regular");
	//names.push_back("Glow");
	//names.push_back("Ghost");
}

void C_Chams::ApplyMaterial(int32_t iMaterial, bool bIgnoreZ, Color aColor, bool bCustom, bool bShotChams)
{
	IMaterial* pRenderMaterial = materials.at(iMaterial);
	/*switch (iMaterial)
	{
	case 0: pRenderMaterial = m_pFlatMaterial; break;
	case 1: pRenderMaterial = m_pRegularMaterial; break;
	case 2: pRenderMaterial = m_pGlowMaterial; break;
	case 3: pRenderMaterial = m_pGhostMaterial; break;
	case 4: pRenderMaterial = m_pGlassMaterial; break;
	case 5: pRenderMaterial = m_pPulseMaterial; break;
	case 6: pRenderMaterial = m_pHaloMaterial; break;
	}
	*/
	bool bHasBeenFound = false;
	if (!pRenderMaterial)
		return;

	IMaterialVar* pMaterialVar = pRenderMaterial->FindVar("$envmaptint", &bHasBeenFound);
	if (!pMaterialVar)
		return;

	if (bHasBeenFound && iMaterial > 1)
		pMaterialVar->SetVecValue(aColor.r() / 255.0f, aColor.g() / 255.0f, aColor.b() / 255.0f);

	const float aBlendColor[3]
		=
	{
		(float_t)(aColor.r()) / 255.0f,
		(float_t)(aColor.g()) / 255.0f,
		(float_t)(aColor.b()) / 255.0f
	};

	pRenderMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, bIgnoreZ && !bCustom);
	if (bCustom)
	{
		pRenderMaterial->ColorModulate(aColor.r() / 255.0f, aColor.g() / 255.0f, aColor.b() / 255.0f);
		pRenderMaterial->AlphaModulate(aColor.a() / 255.0f);

		return g_StudioRender->ForcedMaterialOverride(pRenderMaterial);
	}

	pRenderMaterial->ColorModulate(1.0f, 1.0f, 1.0f);
	pRenderMaterial->AlphaModulate(1.0f);

	if (bShotChams)
	{
		g_RenderView->SetColorModulation(aBlendColor);
		g_RenderView->SetBlend(aColor.a() / 255.0f);
		return g_MdlRender->ForcedMaterialOverride(pRenderMaterial);
	}

	g_RenderView->SetColorModulation(aBlendColor);
	g_RenderView->SetBlend(aColor.a() / 255.0f);
	return g_MdlRender->ForcedMaterialOverride(pRenderMaterial);
}