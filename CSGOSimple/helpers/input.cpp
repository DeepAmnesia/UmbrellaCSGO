#include "input.hpp"

#include "../valve_sdk/sdk.hpp"
#include "../imgui/imgui.h"
#include "../imgui/impl/imgui_impl_win32.h"
#include "../menu.hpp"
#include "utils.hpp"

InputSys::InputSys()
	: m_hTargetWindow(nullptr), m_ulOldWndProc(0)
{
}

InputSys::~InputSys()
{
	if (m_ulOldWndProc)
		SetWindowLongPtr(m_hTargetWindow, GWLP_WNDPROC, m_ulOldWndProc);
	m_ulOldWndProc = 0;
}
void InputSys::Initialize()
{
	D3DDEVICE_CREATION_PARAMETERS params;

	if (FAILED(g_D3DDevice9->GetCreationParameters(&params)))
		throw std::runtime_error("[InputSys] GetCreationParameters failed.");

	m_hTargetWindow = params.hFocusWindow;
	m_ulOldWndProc = SetWindowLongPtr(m_hTargetWindow, GWLP_WNDPROC, (LONG_PTR)WndProc);

	if (!m_ulOldWndProc)
		throw std::runtime_error("[InputSys] SetWindowLongPtr failed.");
}

LRESULT __stdcall InputSys::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	bool bIsHandlePossible = true;
	if (g_EngineClient->IsConnected() && g_EngineClient->IsInGame())
	{
		if (g_EngineClient->Con_IsVisible())
		{
			bIsHandlePossible = false;
			if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP || msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP || msg == WM_XBUTTONDOWN || msg == WM_XBUTTONUP || msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)
				bIsHandlePossible = true;

			if (msg == WM_KEYDOWN || msg == WM_KEYUP)
				if (wParam == VK_INSERT)
					bIsHandlePossible = true;
		}
	}

	if (bIsHandlePossible)
	{
		if (wParam == VK_MENU)
		{
			if (msg == WM_SYSKEYDOWN)
			{
				Utils::toggled_keys[VK_MENU] = !Utils::toggled_keys[VK_MENU];
				Utils::holded_keys[VK_MENU] = true;
			}
			else if (msg == WM_SYSKEYUP)
				Utils::holded_keys[VK_MENU] = false;
		}

		switch (msg)
		{
		case WM_LBUTTONDOWN:
			Utils::toggled_keys[VK_LBUTTON] = !Utils::toggled_keys[VK_LBUTTON];
			Utils::holded_keys[VK_LBUTTON] = true;
			break;
		case WM_LBUTTONUP:
			Utils::holded_keys[VK_LBUTTON] = false;
			break;
		case WM_XBUTTONDOWN:
			if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
			{
				Utils::holded_keys[VK_XBUTTON1] = true;
				Utils::toggled_keys[VK_XBUTTON1] = !Utils::toggled_keys[VK_XBUTTON1];
			}

			if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
			{
				Utils::holded_keys[VK_XBUTTON2] = true;
				Utils::toggled_keys[VK_XBUTTON2] = !Utils::toggled_keys[VK_XBUTTON2];
			}
			break;
		case WM_XBUTTONUP:
			if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
				Utils::holded_keys[VK_XBUTTON1] = false;

			if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
				Utils::holded_keys[VK_XBUTTON2] = false;
			break;
		case WM_MBUTTONDOWN:
			Utils::holded_keys[VK_MBUTTON] = true;
			Utils::toggled_keys[VK_MBUTTON] = !Utils::toggled_keys[VK_MBUTTON];
			break;
		case WM_MBUTTONUP:
			Utils::holded_keys[VK_MBUTTON] = false;
			break;
		case WM_RBUTTONDOWN:
			Utils::toggled_keys[VK_RBUTTON] = !Utils::toggled_keys[VK_RBUTTON];
			Utils::holded_keys[VK_RBUTTON] = true;
			break;
		case WM_RBUTTONUP:
			Utils::holded_keys[VK_RBUTTON] = false;
			break;
		case WM_KEYDOWN:
			Utils::toggled_keys[wParam] = !Utils::toggled_keys[wParam];
			Utils::holded_keys[wParam] = true;
			break;
		case WM_KEYUP:
			Utils::holded_keys[wParam] = false;
			break;
		default: break;
		}
	}

	if (!g_LocalPlayer)
	{
		for (int i = 0; i < 256; i++)
		{
			if (i == VK_INSERT)
				continue;

			Utils::holded_keys[i] = false;
			Utils::toggled_keys[i] = false;
		}
	}

	bool bPressedMovementKeys = false;

	if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP || msg == WM_LBUTTONUP || msg == WM_MOUSEMOVE || msg == WM_MOUSEWHEEL)
		bPressedMovementKeys = true;

	if (ImGui::GetIO().WantTextInput)
		bPressedMovementKeys = false;

	//g_interfaces.m_input_system->enable_input(!menu_open);

	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam) && Menu::Get().IsVisible() && !bPressedMovementKeys)
		return true;

	if (Menu::Get().IsVisible() && bPressedMovementKeys)
		return false;

	return CallWindowProc((WNDPROC)Get().m_ulOldWndProc, hWnd, msg, wParam, lParam);
}

bool InputSys::ProcessMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
	case WM_XBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_LBUTTONUP:
	case WM_XBUTTONUP:
		return ProcessMouseMessage(msg, wParam, lParam);
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		return ProcessKeybdMessage(msg, wParam, lParam);
	default:
		return false;
	}
}

bool InputSys::ProcessMouseMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	auto key = VK_LBUTTON;
	auto state = KeyState::None;
	switch (msg) {
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
		state = msg == WM_MBUTTONUP ? KeyState::Up : KeyState::Down;
		key = VK_MBUTTON;
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
		state = msg == WM_RBUTTONUP ? KeyState::Up : KeyState::Down;
		key = VK_RBUTTON;
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
		state = msg == WM_LBUTTONUP ? KeyState::Up : KeyState::Down;
		key = VK_LBUTTON;
		break;
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_XBUTTONDBLCLK:
		state = msg == WM_XBUTTONUP ? KeyState::Up : KeyState::Down;
		key = (HIWORD(wParam) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2);
		break;
	default:
		return false;
	}

	if (state == KeyState::Up && m_iKeyMap[key] == KeyState::Down)
		m_iKeyMap[key] = KeyState::Pressed;
	else
		m_iKeyMap[key] = state;
	return true;
}

bool InputSys::ProcessKeybdMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	auto key = wParam;
	auto state = KeyState::None;

	switch (msg) {
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		state = KeyState::Down;
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		state = KeyState::Up;
		break;
	default:
		return false;
	}

	if (state == KeyState::Up && m_iKeyMap[int(key)] == KeyState::Down) {
		m_iKeyMap[int(key)] = KeyState::Pressed;

		auto& hotkey_callback = m_Hotkeys[key];

		if (hotkey_callback)
			hotkey_callback();

	}
	else {
		m_iKeyMap[int(key)] = state;
	}

	return true;
}
KeyState InputSys::GetKeyState(std::uint32_t vk)
{
	return m_iKeyMap[vk];
}
bool InputSys::IsKeyDown(std::uint32_t vk)
{
	return m_iKeyMap[vk] == KeyState::Down;
}
bool InputSys::WasKeyPressed(std::uint32_t vk)
{
	if (m_iKeyMap[vk] == KeyState::Pressed) {
		m_iKeyMap[vk] = KeyState::Up;
		return true;
	}
	return false;
}

void InputSys::RegisterHotkey(std::uint32_t vk, std::function<void(void)> f)
{
	m_Hotkeys[vk] = f;
}
void InputSys::RemoveHotkey(std::uint32_t vk)
{
	m_Hotkeys[vk] = nullptr;
}
