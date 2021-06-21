#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <dwmapi.h>
#include <ctime>
#include "driver.h"
#include "macro.h"
#include "w2s.h"
#include "fvector.h"
#include "d3d9.h"
#include "draw.h"
#include "menu.h"
#include <imgui.h>
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3dx9.h>
#include <d3d9.h>
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")
HWND hGameWnd;
const static int uworld_offset = 0x8CFAF00;
const static int gnames_offset = 0x8B2B940;
const static int persistentlevel_offset = 0x38;
const static int actors_offset = 0xa0;
const static int actor_count_offset = 0xa8;
const static int actor_cluster_offset = 0xE0; // 0xE0
const static uintptr_t rootcomponent = 0x140; // 0x140
const static uintptr_t relativelocation = 0x134; // 0x134
//const static int actors_offset = 0x30;

uintptr_t uworld;
uintptr_t persistentlevel;
uintptr_t actorcluster;
uintptr_t actors;
uintptr_t gameinstance;
int actor_count;

char lWindowName[256] = "Overlay";
char tWindowName[256] = "DeadByDaylight  "; // put Game window name here

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
MARGINS pMargin;
MSG Message;
HANDLE hnd;
DWORD id;
int entityloop();
int render();

LRESULT CALLBACK Proc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam) && show_menu == true)
	{
		return 1l;
	}
	switch (Message)
	{
	case WM_PAINT: // we need to paint? lets paint!
		render();
		break;
	case WM_CREATE:
		return DwmExtendFrameIntoClientArea(hWnd, &pMargin); // extension of window frame into client area
		break;
	case WM_DESTROY:
		PostQuitMessage(0); // We need to use this to exit a message loop
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam); // Making sure all messages are processed
		break;
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);
	printf("Debugging Window:\n");

	Kernel::hKernelDriver = CreateFileA(("\\\\.\\NEET"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

	if (Kernel::hKernelDriver == INVALID_HANDLE_VALUE)
	{
		std::cout << "Driver not loaded Exit in 2 Secs..." << std::endl;
		Sleep(2000);
		exit(0);
	}

	std::cout << "handleok: " << Kernel::hKernelDriver << std::endl;

	while (hGameWnd == NULL)
	{
		hGameWnd = FindWindow(0, Kernel::WINDOWNAME);
		Sleep(1000);
	}
	std::cout << "hwnd: " << hGameWnd << std::endl;

	if (hGameWnd == NULL)
	{
		std::cout << "Window %s Not FOUND Exiting......" << std::endl;

		Sleep(1000);
		exit(0);
	}

	GetWindowThreadProcessId(hGameWnd, &Kernel::ProcessID);

	if (Kernel::ProcessID == NULL)
		exit(0);

	std::cout << "processid: " << Kernel::ProcessID << std::endl;

	info_t Input_Output_Data;
	Input_Output_Data.pid = Kernel::ProcessID;
	DWORD Readed_Bytes_Amount;

	DeviceIoControl(Kernel::hKernelDriver, ctl_base, &Input_Output_Data, sizeof Input_Output_Data, &Input_Output_Data, sizeof Input_Output_Data, &Readed_Bytes_Amount, nullptr);
	Kernel::GameModule = (QWORD)Input_Output_Data.data;

	//DumpEXE(GameModule);

	std::cout << ("ProcessID: ") << Kernel::ProcessID << std::endl;
	//std::cout << ("GameBase: 0x") << std::uppercase << std::hex << Kernel::GameModule << std::endl;
	std::cout << "GameBase: " << Kernel::GameModule << std::endl;


	WNDCLASSEXA OverlayWnd; // contains window class information
	OverlayWnd.cbSize = sizeof(WNDCLASSEXA); // size of struct, basically checking for version or check
	OverlayWnd.style = CS_HREDRAW | CS_VREDRAW;  // Style, redraw method type
	OverlayWnd.lpfnWndProc = Proc; // Pointer to the window procedure
	OverlayWnd.cbClsExtra = 0; // window class struct extra bytes
	OverlayWnd.cbWndExtra = 0; // window instance extra bytes
	OverlayWnd.hInstance = hInstance; // handle to the instance that contains the window procedure for the class
	OverlayWnd.hIcon = LoadIcon(NULL, IDI_APPLICATION); // basic window icon set
	OverlayWnd.hIconSm = LoadIcon(NULL, IDI_APPLICATION); // basic window icon set
	OverlayWnd.hCursor = LoadCursor(NULL, IDC_ARROW); // basic window cursor icon set
	OverlayWnd.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0)); // handle to the class background brush
	OverlayWnd.lpszMenuName = lWindowName;
	OverlayWnd.lpszClassName = lWindowName;


	// registers a window class for the use in call to this createwindowex func
	if (!RegisterClassExA(&OverlayWnd))
	{
		::UnregisterClass(OverlayWnd.lpszClassName, OverlayWnd.hInstance);

		exit(1);
	}

	TargetWnd = FindWindowA(0, tWindowName);

	if (TargetWnd)
	{
		GetWindowRect(TargetWnd, &WindowRect);
		windowWidth = WindowRect.right - WindowRect.left;
		windowHeight = WindowRect.bottom - WindowRect.top;

		std::cout << "width: " << windowWidth << " " << "height: " << windowHeight << std::endl;
		pMargin = { 0, 0, windowHeight, windowWidth };
		hWnd = CreateWindowExA(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, lWindowName, lWindowName, WS_POPUP, 1, 1, windowWidth, windowHeight, 0, 0, 0, 0);
	}

	SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
	SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);

	ShowWindow(hWnd, /*SW_SHOW*/SW_SHOWDEFAULT);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.DisplaySize.x = clientWidth/*windowWidth*/;
	io.DisplaySize.y = clientHeight/*windowHeight*/;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hWnd);
	D3D9Init(hWnd);


	for (;;)
	{

		if (GetAsyncKeyState(VK_END))
			break;


		if (PeekMessage(&Message, hWnd, 0, 0, PM_REMOVE))
		{

			TranslateMessage(&Message);

			DispatchMessage(&Message);
		}

		TargetWnd = FindWindowA(0, tWindowName);


		if (!TargetWnd)
			exit(0);

		GetWindowRect(TargetWnd, &WindowRect);
		GetClientRect(TargetWnd, &ClientRect);

		windowWidth = WindowRect.right - WindowRect.left;
		windowHeight = WindowRect.bottom - WindowRect.top;


		DWORD dwStyle = GetWindowLong(TargetWnd, GWL_STYLE);

		if (dwStyle & WS_BORDER)
		{
			windowHeight = WindowRect.bottom - WindowRect.top;
			windowWidth = WindowRect.right - WindowRect.left;
			clientHeight = ClientRect.bottom - ClientRect.top;
			clientWidth = ClientRect.right - ClientRect.left;
			borderHeight = (windowHeight - ClientRect.bottom);
			borderWidth = (windowWidth - ClientRect.right) / 2; //only want one side
			borderHeight -= borderWidth; //remove bottom from width, bottom is the same size as either side so we subtract that

			WindowRect.left += borderWidth;
			WindowRect.top += borderHeight;
		}

		MoveWindow(hWnd, WindowRect.left, WindowRect.top, clientWidth, clientHeight, true);

	} // End of Panic Loop

	ImGui::DestroyContext();
	exit(0);

	return 0;
}


void entityloop() {


	uworld = Kernel::KeReadVirtualMemory<QWORD>(Kernel::GameModule + uworld_offset);

	std::cout << "uworld " << uworld << std::endl;

	if (uworld == NULL)
		return;

	/*uintptr_t Actors = 
	D.RPM<uintptr_t>(
	D.RPM<uintptr_t>(
	D.RPM<uintptr_t>(
	D.RPM<uintptr_t>(BaseAddress + Offsets::Uworld) + Offsets::PersistentLevel) + Offsets::ActorCluster) + Offsets::Actors);
	*/

	persistentlevel = Kernel::KeReadVirtualMemory<uintptr_t>(uworld + persistentlevel_offset);

	if (persistentlevel == NULL)
		return;

	std::cout << "persistentlevel: " << persistentlevel << std::endl;

	actorcluster = Kernel::KeReadVirtualMemory<uintptr_t>(persistentlevel + actor_cluster_offset);

	std::cout << "actorcluster: " << actorcluster << std::endl;

	//actors = Kernel::KeReadVirtualMemory<uintptr_t>(actorcluster + actors_offset);

	actors = Kernel::KeReadVirtualMemory<uintptr_t>(persistentlevel + actors_offset);

	if (actors == NULL)
		return;

	std::cout << "actors: " << actors << std::endl;

	actor_count = Kernel::KeReadVirtualMemory<int>(persistentlevel + actor_count_offset);

	if (actor_count == NULL)
		return;

	std::cout << "actorcount: " << actor_count << std::endl;

	const static int PlayerCameraManager = 0x02D0;
	const static int PlayerController = 0x38;
	const static int CameraCache = 0x1A80;//0x0E60;//0x1A70;

	gameinstance = Kernel::KeReadVirtualMemory<uintptr_t>(uworld + 0x198);

	std::cout << "gameinstance " << gameinstance << std::endl;

	auto localplayer = Kernel::KeReadVirtualMemory<uintptr_t>(gameinstance + 0x40);

	std::cout << "localplayer " << localplayer << std::endl;

	auto localplayerFinal = Kernel::KeReadVirtualMemory<uintptr_t>(localplayer);

	std::cout << "localplayerFinal " << localplayerFinal << std::endl;

	auto playercontroller = Kernel::KeReadVirtualMemory<uintptr_t>(localplayerFinal + PlayerController);


	std::cout << "playercontrollerlocal " << playercontroller << std::endl;

	auto PlayerCamera = Kernel::KeReadVirtualMemory<uintptr_t>(playercontroller + 0x2D0);
	std::cout << "PlayerCamera " << PlayerCamera << std::endl;

	auto CameraCacheEntry = Kernel::KeReadVirtualMemory<FCameraCacheEntry>(PlayerCamera + 0x1A80);

	std::cout << "cameracachenentry " << CameraCacheEntry.TimeStamp << std::endl;
	std::cout << "cameracachenentryfov " << CameraCacheEntry.POV.FOV << std::endl;


	for (int i = 0; i < actor_count; i++)
	{
		Vector3 Pos{};

		auto AActors = Kernel::KeReadVirtualMemory<uintptr_t>(actors + (i * 0x8));

		//std::cout << "actor " << i << AActors << std::endl;

		auto AActors_component = Kernel::KeReadVirtualMemory<uintptr_t>(AActors + rootcomponent);

		//std::cout << "actorcomponent " << AActors_component << std::endl;

		FVector ActorsPosition = Kernel::KeReadVirtualMemory<FVector>(AActors_component + relativelocation);


		uint32_t ActorID = Kernel::KeReadVirtualMemory<uintptr_t>(AActors + 0x18);

		std::cout << "actorid " << ActorID << std::endl;

		//std::cout << "x " << ActorsPosition.X << "y " << ActorsPosition.Y << "z " << ActorsPosition.Z << std::endl;

		FVector loc = WorldToScreen(CameraCacheEntry.POV, ActorsPosition);

		if (loc.X > 0 && loc.X <= 1920 && loc.Y > 0 && loc.Y <= 1080)
			std::cout << "w2s locx " << loc.X << " w2sy " << loc.Y << std::endl;

		DrawString((char*)"object", loc.X, loc.Y,  255, 255, 0, dx_FontCalibri);


		//if (WorldToScreen(ActorsPosition, &Pos, LocalPlayer))
		//	hDrawTextOutlined(ImVec2(Pos.x, Pos.y), std::to_string(ActorID).c_str(), 14, Vector4(1, 1, 1, 1));
	}


}

int render() {

	dx_Device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	dx_Device->BeginScene();

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (TargetWnd == GetForegroundWindow())
	{
		time_t now = time(0);
		// convert now to string form
		char* dt = ctime(&now);
		// convert now to tm struct for UTC
		//tm *gmtm = gmtime(&now);
		//dt = asctime(gmtm);
		char stamp[256] = "Shionji ";
		strcat(stamp, dt);
		DrawString(stamp, 10, 10, 255, 0, 255, dx_FontCalibri); // Put Main procedure here like ESP etc.

	}

	//w2s.Update();


	if (GetAsyncKeyState(VK_INSERT) & 0x8000) {
		if (show_menu)
			show_menu = false;
		else
			show_menu = true;
		Sleep(200);
	}

	if (!show_menu) {
		//	Circle((int)(clientWidth / 2), (int)(clientHeight / 2), fov, 0, full, true, 32, BLACK(255));
		entityloop();
	}



	if (show_menu)
	{

		menu();

	}
	else if (windowstate != 0)
	{
		ChangeClickability(false, ohwnd);
	}
	ImGui::EndFrame();


	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	dx_Device->EndScene();
	dx_Device->PresentEx(0, 0, 0, 0, 0);

	return 0;

}