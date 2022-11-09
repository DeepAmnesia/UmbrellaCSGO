#include "model.h"
#include "../features/game_chams.h"
#include "../../helpers/math.hpp"

std::array < Vector4D, 6 > aWhiteArray =
{
	Vector4D(0.4f, 0.4f, 0.4f, 1.0f),
	Vector4D(0.4f, 0.4f, 0.4f, 1.0f),
	Vector4D(0.4f, 0.4f, 0.4f, 1.0f),
	Vector4D(0.4f, 0.4f, 0.4f, 1.0f),
	Vector4D(0.4f, 0.4f, 0.4f, 1.0f),
	Vector4D(0.4f, 0.4f, 0.4f, 1.0f),
};

std::array < float_t, 3 > aColorModulation =
{
	1.0f,
	1.0f,
	1.0f
};

void C_DrawModel::Instance()
{
	if (!Menu::Get().IsVisible())
		return;

	if (!m_PreviewTexture)
	{
		g_MatSystem->BeginRenderTargetAllocation();

		m_PreviewTexture = g_MatSystem->CreateNamedRenderTargetTextureEx(
			"Preview",
			300, 400,
			RT_SIZE_NO_CHANGE,
			g_MatSystem->GetBackBufferFormat(),
			MATERIAL_RT_DEPTH_SHARED,
			TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT,
			0x00000001
		);

		g_MatSystem->FinishRenderTargetAllocation();
	}

	if (!m_CubemapTexture)
		m_CubemapTexture =g_MatSystem->FindTexture("editor/cubemap.hdr", TEXTURE_GROUP_CUBE_MAP);

	auto CreateModel = reinterpret_cast<void(__thiscall*)(void*)>(g_cheat.m_CreateModel);
	if (!m_PreviewModel)
	{
		m_PreviewModel = static_cast<C_MergedMDL*>( g_MemAlloc->Alloc(0x75C));
		CreateModel(m_PreviewModel);

		m_PreviewModel->SetMDL("models/player/custom_player/uiplayer/animset_uiplayer.mdl");
		m_PreviewModel->SetMergedMDL("models/player/custom_player/legacy/ctm_st6_variante.mdl");
		m_PreviewModel->SetMergedMDL("models/weapons/w_pist_elite.mdl");

		m_PreviewModel->SetSequence(32, false);
		m_PreviewModel->SetupBonesForAttachmentQueries();
	}

	m_PreviewModel->RootMDL.flTime += g_GlobalVars->frametime / 2.0f;

	m_ViewSetup.iX = 0;
	m_ViewSetup.iY = 0;
	m_ViewSetup.iWidth = 350;
	m_ViewSetup.iHeight = 575;
	m_ViewSetup.bOrtho = false;
	m_ViewSetup.flFOV = 70.f;
	m_ViewSetup.vecOrigin = Vector(-65.0f, 2.0f, 50);
	m_ViewSetup.angView = QAngle(0, 0, 0);
	m_ViewSetup.flNearZ = 7.0f;
	m_ViewSetup.flFarZ = 1000.f;
	m_ViewSetup.bDoBloomAndToneMapping = true;

	CMatRenderContextPtr pRenderContext(g_MatSystem);

	pRenderContext->PushRenderTargetAndViewport();
	pRenderContext->SetRenderTarget(m_PreviewTexture);

	pRenderContext->BindLocalCubemap(m_CubemapTexture);
	pRenderContext->SetLightingOrigin(-65.0f, 2.0f, 50.0f);
	pRenderContext->SetIntRenderingParameter(10, 0);

	Frustum dummyFrustum;
	g_RenderView->Push3DView(pRenderContext, m_ViewSetup, 1 | 2 | 32, m_PreviewTexture, dummyFrustum);

	pRenderContext->ClearColor4ub(false, false, false, false);
	pRenderContext->ClearBuffers(true, true, true);
	pRenderContext->SetAmbientLightCube(aWhiteArray.data());

	//g_interfaces.studio_render->SetAmbientLightColors( aWhiteArray.data( ) );
	//g_interfaces.studio_render->SetLocalLights( 0, nullptr );

	matrix3x4_t matPlayerView = { };
	Math::AngleMatrix(QAngle(0, g_cheat.m_flModelRotation, 0), matPlayerView, Vector(0, 0, 0));

	g_MdlRender->SuppressEngineLighting(true);
	if (m_ChamsSettings.m_bRenderChams)
		g_Chams->ApplyMaterial(m_ChamsSettings.m_iMainMaterial, false, Color(m_ChamsSettings.m_Color), true);

	g_cheat.m_bDrawModel = true;

	m_PreviewModel->Draw(matPlayerView);
	if (m_ChamsSettings.m_bRenderChams)
	{
		if (m_ChamsSettings.m_aModifiers[0])
		{
			g_Chams->ApplyMaterial(2, true, Color(m_ChamsSettings.m_aModifiersColors[0]), true);
			m_PreviewModel->Draw(matPlayerView);
		}

		if (m_ChamsSettings.m_aModifiers[1])
		{
			g_Chams->ApplyMaterial(3, true, Color(m_ChamsSettings.m_aModifiersColors[1]), true);
			m_PreviewModel->Draw(matPlayerView);
		}

		if (m_ChamsSettings.m_aModifiers[2])
		{
			g_Chams->ApplyMaterial(4, true, Color(m_ChamsSettings.m_aModifiersColors[2]), true);
			m_PreviewModel->Draw(matPlayerView);
		}

		if (m_ChamsSettings.m_aModifiers[3])
		{
			g_Chams->ApplyMaterial(5, true, Color(m_ChamsSettings.m_aModifiersColors[3]), true);
			m_PreviewModel->Draw(matPlayerView);
		}
	}

	g_cheat.m_bDrawModel = false;

	g_MdlRender->SuppressEngineLighting(false);

	g_RenderView->PopView(pRenderContext, dummyFrustum);
	pRenderContext->BindLocalCubemap(nullptr);

	pRenderContext->PopRenderTargetAndViewport();
	pRenderContext->Release();
}

void C_MergedMDL::SetupBonesForAttachmentQueries()
{
	return ((void(__thiscall*)(void*))(g_cheat.m_SetupBonesForAttachmentQueries))(this);
}

bool& IMaterialSystem::DisableRenderTargetAllocationForever()
{
	return *reinterpret_cast<bool*>(reinterpret_cast<std::uintptr_t>(this) + g_cheat.m_DisableRenderTargetAllocationForever);
}

void C_MergedMDL::SetMergedMDL(const char* szModelPath, CCustomMaterialOwner* pCustomMaterialOwner, void* pProxyData)
{
	return ((void(__thiscall*)(void*, const char*, CCustomMaterialOwner*, void*, bool))(g_cheat.m_SetMergedMDL))(this, szModelPath, pCustomMaterialOwner, pProxyData, false);
}