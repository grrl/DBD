#pragma once
#include <Windows.h>
#include "macro.h"
#include "structs.h"
#define ctl_write    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0601, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define ctl_read    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0602, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define ctl_base    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0603, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

namespace Kernel
{
	QWORD GameModule;
	QWORD GNamesAddress;
	HANDLE hKernelDriver;
	LPCSTR WINDOWNAME = "DeadByDaylight  ";
	DWORD ProcessID = 0;

	template <typename Type>
	Type KeReadVirtualMemory(QWORD OffAddr)
	{
		info_t Input_Output_Data;
		Input_Output_Data.pid = ProcessID;
		Input_Output_Data.address = OffAddr;
		Type Return_Value;
		Input_Output_Data.value = &Return_Value;
		Input_Output_Data.size = sizeof(Type);
		DWORD Readed_Bytes_Amount;
		DeviceIoControl(hKernelDriver, ctl_read, &Input_Output_Data, sizeof Input_Output_Data, &Input_Output_Data, sizeof Input_Output_Data, &Readed_Bytes_Amount, nullptr);
		return *(Type*)&Return_Value;
	}

	template <typename Type>
	void KeWriteVirtualMemory(QWORD WriteAddr, Type WriteVal)
	{
		info_t Input_Output_Data;
		Input_Output_Data.pid = ProcessID;
		Input_Output_Data.address = WriteAddr;
		Input_Output_Data.value = &WriteVal;
		Input_Output_Data.size = sizeof(Type);
		DWORD Readed_Bytes_Amount;
		DeviceIoControl(hKernelDriver, ctl_write, &Input_Output_Data, sizeof Input_Output_Data, &Input_Output_Data, sizeof Input_Output_Data, &Readed_Bytes_Amount, nullptr);
	}
};
