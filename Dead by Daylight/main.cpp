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
const static int rootcomponent = 0x140; // 0x140
const static int relativelocation = 0x134; // 0x134
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
void entityloop();
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

FTransform GetBoneIndex(QWORD dwbonemesh, int indexnum)
{
	QWORD bonearray = Kernel::KeReadVirtualMemory<QWORD>(dwbonemesh + 0x1E0);
	return Kernel::KeReadVirtualMemory<FTransform>(bonearray + (indexnum * 0x30));
}

FVector GetBoneWithRotation(QWORD dwbonemesh, int boneindexnum)
{
	FTransform bone = GetBoneIndex(dwbonemesh, boneindexnum);
	FTransform ComponentToWorld = Kernel::KeReadVirtualMemory<FTransform>(dwbonemesh + 0x164);
	D3DMATRIX Matrix;
	Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());

	return FVector(Matrix._41, Matrix._42, Matrix._43);
}

bool isReverse(uintptr_t SkillCheck) {
	float rotation = Kernel::KeReadVirtualMemory<float>(SkillCheck + 0x2E0);
	if (rotation < 0.0f) {
		return true;
	}
	return false;
}

float Successzone(uintptr_t SkillCheck) {
	if (isReverse(SkillCheck)) {
		return 1.0f - Kernel::KeReadVirtualMemory<float>(SkillCheck + 0x2D4);
	}
	else {
		return Kernel::KeReadVirtualMemory<float>(SkillCheck + 0x2D0);
	}
}

void entityloop() {


	uworld = Kernel::KeReadVirtualMemory<QWORD>(Kernel::GameModule + uworld_offset);

	//std::cout << "uworld " << uworld << std::endl;

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

	//std::cout << "persistentlevel: " << persistentlevel << std::endl;

	actorcluster = Kernel::KeReadVirtualMemory<uintptr_t>(persistentlevel + actor_cluster_offset);

	//std::cout << "actorcluster: " << actorcluster << std::endl;

	//actors = Kernel::KeReadVirtualMemory<uintptr_t>(actorcluster + actors_offset);

	actors = Kernel::KeReadVirtualMemory<uintptr_t>(persistentlevel + actors_offset);

	if (actors == NULL)
		return;

	//std::cout << "actors: " << actors << std::endl;

	actor_count = Kernel::KeReadVirtualMemory<int>(persistentlevel + actor_count_offset);

	if (actor_count == NULL)
		return;

	//std::cout << "actorcount: " << actor_count << std::endl;

	const static int PlayerCameraManager = 0x02D0;
	const static int PlayerController = 0x38;
	const static int CameraCache = 0x1A80;//0x0E60;//0x1A70;

	gameinstance = Kernel::KeReadVirtualMemory<uintptr_t>(uworld + 0x198);

	//std::cout << "gameinstance " << gameinstance << std::endl;

	auto localplayer = Kernel::KeReadVirtualMemory<uintptr_t>(gameinstance + 0x40);

	//std::cout << "localplayer " << localplayer << std::endl;

	auto localplayerFinal = Kernel::KeReadVirtualMemory<uintptr_t>(localplayer + 0x0);

	//std::cout << "localplayerFinal " << localplayerFinal << std::endl;

	auto playercontroller = Kernel::KeReadVirtualMemory<uintptr_t>(localplayerFinal + PlayerController);


	//std::cout << "playercontrollerlocal " << playercontroller << std::endl;

	auto PlayerCamera = Kernel::KeReadVirtualMemory<uintptr_t>(playercontroller + 0x2D0);
	//std::cout << "PlayerCamera " << PlayerCamera << std::endl;

	auto CameraCacheEntry = Kernel::KeReadVirtualMemory<FMinimalViewInfo>(PlayerCamera + 0x1A80);

	//std::cout << "cameracachenentry " << CameraCacheEntry.TimeStamp << std::endl;
	//std::cout << "cameracachenentryfov " << CameraCacheEntry.POV.FOV << std::endl;
	/*
	auto pawn = Kernel::KeReadVirtualMemory<uintptr_t>(playercontroller + 0x2B8);

	auto interactionhandler = Kernel::KeReadVirtualMemory<uintptr_t>(actors + 0x0890);

	std::cout << "interactionhandler" << interactionhandler << std::endl;

	auto SkillCheck = Kernel::KeReadVirtualMemory<uintptr_t>(interactionhandler + 0x02D8);

	std::cout << "SkillCheck" << SkillCheck << std::endl;

	bool isdisplayed = Kernel::KeReadVirtualMemory<bool>(SkillCheck + 0x130);

	std::cout << "isdisplayed" << isdisplayed << std::endl;

	if (isdisplayed == false)
		return;


	float progress = Kernel::KeReadVirtualMemory<float>(SkillCheck + 0x134);

	std::cout << "progress" << progress << std::endl;

	float TARGET = Kernel::KeReadVirtualMemory<float>(SkillCheck + 0x184);
	float successzonestart = Kernel::KeReadVirtualMemory<float>(SkillCheck + 0x134 + 0x4);
	float successzoneend = Kernel::KeReadVirtualMemory<float>(SkillCheck + 0x134 + 0x4 + 0x4);
	float bonuszonelength = Kernel::KeReadVirtualMemory<float>(SkillCheck + 0x134 + 0x4 + 0x4 + 0x4);
	float currentprogress = Kernel::KeReadVirtualMemory<float>(SkillCheck + 0x134 + 0x4 + 0x4 + 0x4 + 0x4);

	
		PlayerController->Pawn + 0x0828 = interactionHandler -> ptr
		interactionHandler + 0x0140 = skillCheck -> ptr
		skillCheck + 0x01D0 = bDisplayed -> bool
		 
		skillCheck + 0x01B4 = ProgressRate -> float
		skillCheck + 0x01B8 = SuccessZoneStart -> float
		skillCheck + 0x01BC = SuccessZoneEnd  ->float 
		skillCheck + 0x01C0 = BonusZoneLength -> float
		skillCheck + 0x01D4 = currentProgress -> float
		 
		if(bDisplayed && (currentProgress > SuccessZoneStart && currentProgress < SuccessZoneEnd))
		{
			executeSkillCheck();
		}
	*/

	//if (isdisplayed && (currentprogress > successzonestart && currentprogress < successzoneend))
	//{
	//	SendKeySpace();
	//}
	/*
	if(bDisplayed && (currentProgress > SuccessZoneStart && currentProgress < SuccessZoneEnd))
{
	executeSkillCheck();
}

	*/
	//matrix Matrix 7F310A0

	//latest
	/*	
		DWORD64 InteractionHandler = *(DWORD64*)(Pawn + 0x0950);
DWORD64 SkillCheck = *(DWORD64*)(InteractionHandler + 0x0280);

	SkillCheck:
#define ISDISPLAY 0x130
#define PROGRESS 0x134
#define TARGET 0x184

bool IsDisplayed = *(bool*)(SkillCheck + 0x027C);
float CurrentProgress = *(float*)(SkillCheck + 0x022C);
float BonusProgress = *(float*)(SkillCheck + 0x0264);

	*/


	/*
	std::uint64_t playerstate = read<std::uint64_t>(apawn + 0x250);
	FPlayerStateData playerstateData = read<FPlayerStateData>(playerstate + 0x3D0);
	*/

	//PlayerList : UWORLD -> 0x140 -> 0x710 -> 0x148 -> i * 0x10 // (not 0x8)
    //PlayerCount: UWORLD -> 0x140 -> 0x710 -> 0x150

	/*
	var isDisplayed = Memory.ZwReadBool(processHandle, (IntPtr)USkillCheck.ToInt64() +0x0270);
                if (isDisplayed)
                {
                    var currentProgress = Memory.ZwReadFloat(processHandle,
                        (IntPtr)USkillCheck.ToInt64() + 0x022C);<-- wrong
                    var startSuccessZone = Memory.ZwReadFloat(processHandle,
                        (IntPtr)USkillCheck.ToInt64() + 0x0264);<--- wrong

	*/

	/*
	DWORD64 InteractionHandler = *(DWORD64*)(Pawn + 0x0950);
DWORD64 SkillCheck = *(DWORD64*)(InteractionHandler + 0x0280);

bool IsDisplayed = *(bool*)(SkillCheck + 0x027C);
float CurrentProgress = *(float*)(SkillCheck + 0x022C);
float BonusProgress = *(float*)(SkillCheck + 0x0264);


Please update

	*/

	/*
	Managers::DBD::IsDisplayed = read<bool>(USkillCheck + 0x0288);
		Managers::DBD::currentProgress = read<float>(USkillCheck + 0x028C);
		Managers::DBD::startSuccessZone = read<float>(USkillCheck + 0x02D0);

	*/


	
	for (int i = 0; i < actor_count; i++)
	{

		uint64_t CurrentActor = Kernel::KeReadVirtualMemory<uint64_t>(actors + (static_cast<uint64_t>(i) * 0x8));
		//uint64_t CurrentActor = read<uint64_t>(Managers::EntityList + ActorNum * 0x8);
		if (CurrentActor == (uint64_t)nullptr || CurrentActor == -1 || CurrentActor == NULL)
			return;

		//std::cout << "actor " << i << AActors << std::endl;
		//uint64 AActors_component = Kernel::KeReadVirtualMemory<uint64>(Entity + rootcomponent);
		//std::cout << "actorcomponent " << AActors_component << std::endl;
		//FVector ActorsPosition = Kernel::KeReadVirtualMemory<FVector>(AActors_component + relativelocation);


		uint32_t ActorID = Kernel::KeReadVirtualMemory<uint32_t>(CurrentActor + 0x18);


		//FVector ActorsPosition = Kernel::KeReadVirtualMemory<FVector>(Kernel::KeReadVirtualMemory<uintptr_t>((AActors + rootcomponent) + relativelocation));
		//std::cout << "actorid " << ActorID << " x " << ActorsPosition.X << " y " << ActorsPosition.Y << " z " << ActorsPosition.Z << std::endl;
		//std::cout << "x " << ActorsPosition.X << "y " << ActorsPosition.Y << "z " << ActorsPosition.Z << std::endl;

		//uint64 CurrentActorMesh = Kernel::KeReadVirtualMemory<uint64>(Entity + 0x290);//Kernel::KeReadVirtualMemory<uint64_t>(EntityRootComp + Mesh);

		//FVector pos = GetBoneWithRotation(CurrentActorMesh, 66);

		uint64 EntityRootComp = Kernel::KeReadVirtualMemory<uint64>(CurrentActor + rootcomponent);
		FVector pos = Kernel::KeReadVirtualMemory<FVector>(EntityRootComp + relativelocation);


		std::cout << "pos fov " << CameraCacheEntry.FOV << std::endl;

		//FVector loc = WorldToScreen(CameraCacheEntry.POV, pos);
		FVector PlayerScreenPos = WorldToScreen(CameraCacheEntry, pos);


		std::cout << "PlayerScreenPos " << PlayerScreenPos.X << " " << PlayerScreenPos.Y << std::endl;


		//if (loc.X > 0 && loc.X <= 1920 && loc.Y > 0 && loc.Y <= 1080)
			//std::cout << "w2s locx " << loc.X << " w2sy " << loc.Y << std::endl;

		DrawString((char*)"object", PlayerScreenPos.X, PlayerScreenPos.Y,  255, 0, 255, dx_FontCalibri);


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

/*
if (this->name.find("BP_Slasher_Character_01_C") != std::string::npos) killername = "Trapper";
		if (this->name.find("BP_Slasher_Character_02_C") != std::string::npos) killername = "Wraith";
		if (this->name.find("BP_Slasher_Character_03_C") != std::string::npos) killername = "Hillbilly";
		if (this->name.find("BP_Slasher_Character_04_C") != std::string::npos) killername = "Nurse";
		if (this->name.find("BP_Slasher_Character_05_C") != std::string::npos) killername = "Hag";
		if (this->name.find("BP_Slasher_Character_06_C") != std::string::npos) killername = "Myers";
		if (this->name.find("BP_Slasher_Character_07_C") != std::string::npos) killername = "Doctor";
		if (this->name.find("BP_Slasher_Character_08_C") != std::string::npos) killername = "Huntress";
		if (this->name.find("BP_Slasher_Character_09_C") != std::string::npos) killername = "Leatherface";
		if (this->name.find("BP_Slasher_Character_10_C") != std::string::npos) killername = "Freddy";
		if (this->name.find("BP_Slasher_Character_11_C") != std::string::npos) killername = "Pig";
		if (this->name.find("BP_Slasher_Character_12_C") != std::string::npos) killername = "Clown";
		if (this->name.find("BP_Slasher_Character_13_C") != std::string::npos) killername = "Spirit";
		if (this->name.find("BP_Slasher_Character_14_C") != std::string::npos) killername = "Legion";
		if (this->name.find("BP_Slasher_Character_15_C") != std::string::npos) killername = "Plague";
		if (this->name.find("BP_Slasher_Character_16_C") != std::string::npos) killername = "Ghost Face";
		if (this->name.find("BP_Slasher_Character_17_C") != std::string::npos) killername = "Demogorgon";
		if (this->name.find("BP_Slasher_Character_18_C") != std::string::npos) killername = "Oni";
		if (this->name.find("BP_Slasher_Character_19_C") != std::string::npos) killername = "Deathslinger";
		if (this->name.find("_20_C") != std::string::npos) killername = "Pyramid Head";
		if (this->name.find("BP_Slasher_Character_21_C") != std::string::npos) killername = "Blight";
		if (this->name.find("BP_Slasher_Character_22_C") != std::string::npos) killername = "Twins";
		if (this->name.find("Twin") != std::string::npos) killername = "Victor";
*/
/*
if (this->name.find("BP_CamperFemale01_Character") != std::string::npos) survivorname = "Meg";
		if (this->name.find("BP_CamperFemale02_Character") != std::string::npos) survivorname = "Claudette";
		if (this->name.find("BP_CamperFemale03_Character") != std::string::npos) survivorname = "Nea";
		if (this->name.find("BP_CamperFemale04_Character") != std::string::npos) survivorname = "Laurie";
		if (this->name.find("BP_CamperFemale05_Character") != std::string::npos) survivorname = "Feng";
		if (this->name.find("BP_CamperFemale06_Character") != std::string::npos) survivorname = "Kate";
		if (this->name.find("BP_CamperFemale07_Character") != std::string::npos) survivorname = "Jane";
		if (this->name.find("BP_CamperFemale08_Character") != std::string::npos) survivorname = "Nancy";
		if (this->name.find("BP_CamperFemale09_Character") != std::string::npos) survivorname = "Yui";
		if (this->name.find("BP_CamperFemale10_Character") != std::string::npos) survivorname = "Zarina";
		if (this->name.find("BP_CamperFemale11_Character") != std::string::npos) survivorname = "Cheryl";
		if (this->name.find("BP_CamperFemale12_Character") != std::string::npos) survivorname = "Elodie";
		if (this->name.find("BP_CamperMale01") != std::string::npos) survivorname = "Dwight";
		if (this->name.find("BP_CamperMale02_Character") != std::string::npos) survivorname = "Jake";
		if (this->name.find("BP_CamperMale03_Character") != std::string::npos) survivorname = "Ace";
		if (this->name.find("BP_CamperMale04_Character") != std::string::npos) survivorname = "Bill";
		if (this->name.find("BP_CamperMale05_Character") != std::string::npos) survivorname = "David";
		if (this->name.find("BP_CamperMale06_Character") != std::string::npos) survivorname = "Quentin";
		if (this->name.find("BP_CamperMale07_Character") != std::string::npos) survivorname = "Tapp";
		if (this->name.find("BP_CamperMale08_Character") != std::string::npos) survivorname = "Adam";
		if (this->name.find("BP_CamperMale09_Character") != std::string::npos) survivorname = "Jeff";
		if (this->name.find("BP_CamperMale10_Character") != std::string::npos) survivorname = "Ashley";
		if (this->name.find("BP_CamperMale11_Character") != std::string::npos) survivorname = "Steve";
		if (this->name.find("BP_CamperMale12_Character") != std::string::npos) survivorname = "Felix";

*/