#include "../hooks.hpp"
#include "../../features/misc.h"

long __stdcall Hooks::EndScene::hook(IDirect3DDevice9* pDevice)
{
	DWORD colorwrite, srgbwrite;
	IDirect3DVertexDeclaration9* vert_dec = nullptr;
	IDirect3DVertexShader9* vert_shader = nullptr;
	DWORD dwOld_D3DRS_COLORWRITEENABLE = NULL;
	pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
	pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);

	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

	pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &dwOld_D3DRS_COLORWRITEENABLE);
	pDevice->GetVertexDeclaration(&vert_dec);
	pDevice->GetVertexShader(&vert_shader);
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
	pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	auto esp_drawlist = Render::Get().RenderScene();

	Menu::Get().Render();
	radar::render(g_LocalPlayer);
	misc::binds_list();
	misc::bomb_timer();
	misc::spectator_list();
	misc::watermark();
	if (g_cheat.update_radar)
		radar::OnMapLoad(g_EngineClient->GetLevelNameShort(), pDevice);
	ImGui::Render(esp_drawlist);

	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, dwOld_D3DRS_COLORWRITEENABLE);
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
	pDevice->SetVertexDeclaration(vert_dec);
	pDevice->SetVertexShader(vert_shader);

	return o_EndScene(pDevice);
}