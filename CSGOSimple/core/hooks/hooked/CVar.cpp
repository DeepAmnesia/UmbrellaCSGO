#include "../hooks.hpp"

bool __fastcall Hooks::SvCheats::hook(PVOID pConVar, void* edx)
{
	static const auto dwCAM_Think = Utils::PatternScan(GetModuleHandleW(L"client.dll"), "85 C0 75 30 38 86");

	if (!o_SvCheats)
		return false;

	if (reinterpret_cast<DWORD>(_ReturnAddress()) == reinterpret_cast<DWORD>(dwCAM_Think))
		return true;

	return o_SvCheats(pConVar);
}