#include <Windows.h>
#include <imgui.h>
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "d3d9.h"

bool aim;

void SendKey() {
	INPUT input;
	WORD vkey = VK_LEFT; // see link below
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = vkey;
	input.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
	SendInput(1, &input, sizeof(INPUT));
}

void SendKeySpace() {
	INPUT input;
	WORD vkey = VK_SPACE; // see link below
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = vkey;
	input.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
	SendInput(1, &input, sizeof(INPUT));
}

void ChangeClickability(bool canclick, HWND ownd)
{
	long style = GetWindowLong(
		ownd, GWL_EXSTYLE);
	if (canclick) {
		style &= ~WS_EX_LAYERED;
		SetWindowLong(ownd,
			GWL_EXSTYLE, style);
		SetForegroundWindow(ownd);
		windowstate = 1;
	}
	else {
		style |= WS_EX_LAYERED;
		SetWindowLong(ownd,
			GWL_EXSTYLE, style);
		windowstate = 0;
	}
}

void menu() {

	if (windowstate != 1) //You might need to remove this check and call it every time to get keyboard input working. Mouse input works anyway.
	{
		ChangeClickability(true, ohwnd);
		SendKey();
	}

	ImGui::SetNextWindowSize(ImVec2(140, 100));
	if (ImGui::Begin("dbd", nullptr,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		/*ImGuiWindowFlags_AlwaysAutoResize |*/
		ImGuiWindowFlags_NoSavedSettings
	))
	{
		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Aimb##aim", &aim);
		ImGui::End();
	}
}