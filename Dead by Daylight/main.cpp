#include <iostream>
#include <Windows.h>
#include "driver.h"
#include "macro.h"

HWND hGameWnd;
const static int uworld_offset = 0x8CFAF00;
const static int gnames_offset = 0x8B2B940;
const static int persistentlevel_offset = 0x38;
const static int actors_offset = 0xa0;
const static int actor_count_offset = 0xa8;
QWORD uworld;
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

	system("pause");
}