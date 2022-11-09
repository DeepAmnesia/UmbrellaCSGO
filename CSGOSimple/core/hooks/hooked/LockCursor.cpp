#include "../hooks.hpp"

void __stdcall Hooks::LockCursor::hook() 
{
	if (Menu::Get().IsVisible()) {
		g_VGuiSurface->UnlockCursor();
		g_InputSystem->ResetInputState();
		return;
	}
	o_LockCursor(g_VGuiSurface);
}