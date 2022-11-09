#include "../hooks.hpp"

long __stdcall Hooks::Reset::hook(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	Menu::Get().OnDeviceLost();

	auto hr = o_Reset(device, pPresentationParameters);

	if (hr >= 0)
		Menu::Get().OnDeviceReset();

	return hr;
}