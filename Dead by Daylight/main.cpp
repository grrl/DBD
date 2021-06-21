#include <iostream>
#include <Windows.h>
#include "driver.h"
#include "macro.h"
#include "w2s.h"
#include "fvector.h"

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
int main() {

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

	//else {
		//std::cout << _xor_("Driver Loaded! \n") << std::endl;
	//}

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

	uworld = Kernel::KeReadVirtualMemory<QWORD>(Kernel::GameModule + uworld_offset);

	std::cout << "uworld " << uworld << std::endl;

	/*uintptr_t Actors = 
	D.RPM<uintptr_t>(
	D.RPM<uintptr_t>(
	D.RPM<uintptr_t>(
	D.RPM<uintptr_t>(BaseAddress + Offsets::Uworld) + Offsets::PersistentLevel) + Offsets::ActorCluster) + Offsets::Actors);


	*/

	persistentlevel = Kernel::KeReadVirtualMemory<uintptr_t>(uworld + persistentlevel_offset);

	if(persistentlevel == NULL)
		system("pause");

	std::cout << "persistentlevel: " << persistentlevel << std::endl;

	actorcluster = Kernel::KeReadVirtualMemory<uintptr_t>(persistentlevel + actor_cluster_offset);

	std::cout << "actorcluster: " << actorcluster << std::endl;

	//actors = Kernel::KeReadVirtualMemory<uintptr_t>(actorcluster + actors_offset);

	actors = Kernel::KeReadVirtualMemory<uintptr_t>(persistentlevel + actors_offset);

	if (actors == NULL)
		system("pause");

	std::cout << "actors: " << actors << std::endl;

	actor_count = Kernel::KeReadVirtualMemory<int>(persistentlevel + actor_count_offset);

	if (actor_count == NULL)
		system("pause");

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

	//std::cout << "playercontroller " << playercontroller << std::endl;

	//auto playercontrollerlocal = Kernel::KeReadVirtualMemory<uintptr_t>(playercontroller);

	std::cout << "playercontrollerlocal " << playercontroller << std::endl;

	auto PlayerCamera = Kernel::KeReadVirtualMemory<uintptr_t>(playercontroller + 0x2D0);
	std::cout << "PlayerCamera " << PlayerCamera << std::endl;

	//auto PlayerCamera = Kernel::KeReadVirtualMemory<uintptr_t>(playercontroller + PlayerCameraManager);

	//std::cout << "PlayerCamera " << PlayerCamera << std::endl;

	//auto camera_cache = Kernel::KeReadVirtualMemory<FCameraCacheEntry>(PlayerCamera + 0x1A70);
	//uintptr_t cameracahce = Kernel::KeReadVirtualMemory<uintptr_t>(PlayerCamera + 0x06E0 + 0x10);

	//std::cout << "cameracahce " << cameracahce << std::endl;

	auto CameraCacheEntry = Kernel::KeReadVirtualMemory<FCameraCacheEntry>(PlayerCamera + 0x1A80);

	std::cout << "cameracachenentry " << CameraCacheEntry.TimeStamp << std::endl;
	std::cout << "cameracachenentryfov " << CameraCacheEntry.POV.FOV << std::endl;


	for (int i = 0; i < actor_count; i++)
	{
		Vector3 Pos{};

		auto AActors = Kernel::KeReadVirtualMemory<uintptr_t>(actors + (i * 0x8));

		std::cout << "actor " << i << AActors << std::endl;

		auto AActors_component = Kernel::KeReadVirtualMemory<uintptr_t>(AActors + rootcomponent);

		std::cout << "actorcomponent " << AActors_component << std::endl;

		FVector ActorsPosition = Kernel::KeReadVirtualMemory<FVector>(AActors_component + relativelocation);


		uint32_t ActorID = Kernel::KeReadVirtualMemory<uintptr_t>(AActors + 0x18);

		std::cout << "actorid " << ActorID << std::endl;

		std::cout << "x " << ActorsPosition.X << "y " << ActorsPosition.Y << "z " << ActorsPosition.Z << std::endl;

		FVector loc = WorldToScreen(CameraCacheEntry.POV, ActorsPosition);

		//std::cout << "w2s locx " << loc.X << " w2sy " << loc.Y << std::endl;

		//if (WorldToScreen(ActorsPosition, &Pos, LocalPlayer))
		//	hDrawTextOutlined(ImVec2(Pos.x, Pos.y), std::to_string(ActorID).c_str(), 14, Vector4(1, 1, 1, 1));
	}



	system("pause");
}