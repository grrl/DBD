#pragma once
#include "vector3.h"


typedef struct _KERNEL_MEMORY
{
	DWORD ProcessId;

	QWORD Address;
	void* Buffer;
	SIZE_T Size;

} KERNEL_MEMORY, *PKERNEL_MEMORY;


typedef struct info_t {
	int pid = 0;
	DWORD_PTR address;
	void* value;
	SIZE_T size;
	void* data;
}info, *p_info;
