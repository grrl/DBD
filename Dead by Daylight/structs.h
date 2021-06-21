#pragma once
#include "vector3.h"
#include "fvector.h"

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


struct FMinimalViewInfo
{
public:
	FVector Location;
	FRotator Rotation;
	float FOV;
};

struct FCameraCacheEntry
{
public:
	float TimeStamp;
	//unsigned char UnknownData00[0x4];                                       // 0x0004(0x0004) MISSED OFFSET
	char pad_0x0564[0x4];
	FMinimalViewInfo POV;
};
