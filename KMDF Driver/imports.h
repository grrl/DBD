#pragma once

NTKERNELAPI PVOID PsGetProcessSectionBaseAddress(__in PEPROCESS Process);

NTSTATUS ZwQuerySystemInformation(ULONG InfoClass, PVOID Buffer, ULONG Length, PULONG ReturnLength);

NTKERNELAPI NTSTATUS MmCopyVirtualMemory(
	IN PEPROCESS		SourceProcess,
	IN PVOID			SourceAddress,
	IN PEPROCESS		TargetProcess,
	IN PVOID			TargetAddress,
	IN SIZE_T			BufferSize,
	IN KPROCESSOR_MODE  PreviousMode,
	OUT PSIZE_T			ReturnSize
);
