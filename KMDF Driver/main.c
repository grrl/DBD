#include <ntdef.h>
#include <ntifs.h>
#include "macro.h"
#include "imports.h"
#include "structs.h"
#include "enum.h"


#include <ntifs.h>
#include <ntstrsafe.h>

#define ctl_write    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0601, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define ctl_read    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0602, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define ctl_base    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0603, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

//io
NTSTATUS unsupported_io(PDEVICE_OBJECT device_obj, PIRP irp);
NTSTATUS create_io(PDEVICE_OBJECT device_obj, PIRP irp);
NTSTATUS close_io(PDEVICE_OBJECT device_obj, PIRP irp);
NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject);

// memory
void read_mem(int pid, void* addr, void* value, size_t size);
void write_mem(int pid, void* addr, void* value, size_t size);

UNICODE_STRING  sym_link, dev_name;
PDEVICE_OBJECT  dev_obj;

NTKERNELAPI NTSTATUS IoCreateDriver(PUNICODE_STRING DriverName, PDRIVER_INITIALIZE InitializationFunction);
NTKERNELAPI NTSTATUS ObReferenceObjectByName(PUNICODE_STRING ObjectName, ULONG Attributes, PACCESS_STATE PassedAccessState, ACCESS_MASK DesiredAccess, POBJECT_TYPE ObjectType, KPROCESSOR_MODE AccessMode, PVOID ParseContext, PVOID* Object);
NTKERNELAPI NTSTATUS MmCopyVirtualMemory(PEPROCESS SourceProcess, PVOID SourceAddress, PEPROCESS TargetProcess, PVOID TargetAddress, SIZE_T BufferSize, KPROCESSOR_MODE PreviousMode, PSIZE_T ReturnSize);
NTKERNELAPI PVOID PsGetProcessSectionBaseAddress(PEPROCESS Process);


NTSTATUS ctl_io(PDEVICE_OBJECT device_obj, PIRP irp)
{
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = sizeof(info);

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	p_info buffer = (p_info)irp->AssociatedIrp.SystemBuffer;

	size_t size = 0;

	if (stack)
	{
		if (buffer && sizeof(*buffer) >= sizeof(info))
		{

			if (stack->Parameters.DeviceIoControl.IoControlCode == ctl_read)
			{
				if (buffer->address < 0x7FFFFFFFFFFF)
				{
					read_mem(buffer->pid, (void*)buffer->address, buffer->value, buffer->size);
				}
				else
				{
					buffer->value = 0;
				}
			}
			else if (stack->Parameters.DeviceIoControl.IoControlCode == ctl_write)
			{
				write_mem(buffer->pid, (void*)buffer->address, buffer->value, buffer->size);
			}
			else if (stack->Parameters.DeviceIoControl.IoControlCode == ctl_base)
			{
				PEPROCESS pe;
				PsLookupProcessByProcessId((HANDLE)buffer->pid, &pe);
				buffer->data = PsGetProcessSectionBaseAddress(pe);
				ObfDereferenceObject(pe);
			}
		}
	}

	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return irp->IoStatus.Status;
}

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	DbgPrintEx(0, 0, "Unload routine called.\n");

	//PsRemoveLoadImageNotifyRoutine(ImageLoadCallback);
	IoDeleteSymbolicLink(&sym_link);
	IoDeleteDevice(pDriverObject->DeviceObject);
}

// real main
/*
NTSTATUS driver_initialize(PDRIVER_OBJECT driver_obj, PUNICODE_STRING registery_path)
{
	auto  status = STATUS_SUCCESS;
	RtlInitUnicodeString(&dev_name, L"\\Device\\NEET");
	status = IoCreateDevice(driver_obj, 0, &dev_name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &dev_obj);
	if (status != STATUS_SUCCESS) {
		return status;
	}
	RtlInitUnicodeString(&sym_link, L"\\DosDevices\\NEET");
	status = IoCreateSymbolicLink(&sym_link, &dev_name);
	if (status != STATUS_SUCCESS) {
		return status;
	}
	dev_obj->Flags |= DO_BUFFERED_IO;
	for (int t = 0; t <= IRP_MJ_MAXIMUM_FUNCTION; t++)
		driver_obj->MajorFunction[t] = unsupported_io;
	driver_obj->MajorFunction[IRP_MJ_CREATE] = create_io;
	driver_obj->MajorFunction[IRP_MJ_CLOSE] = close_io;
	driver_obj->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ctl_io;
	driver_obj->DriverUnload = UnloadDriver;
	dev_obj->Flags &= ~DO_DEVICE_INITIALIZING;
	return status;
}
*/

NTSTATUS DriverEntry(PDRIVER_OBJECT driver_obj, PUNICODE_STRING registery_path)
{
	/*
	auto        status = STATUS_SUCCESS;
	UNICODE_STRING  drv_name;
	RtlInitUnicodeString(&drv_name, L"\\Driver\\NEET");
	status = IoCreateDriver(&drv_name, &driver_initialize);
	return STATUS_SUCCESS;
	*/
	auto status = STATUS_SUCCESS;
	RtlInitUnicodeString(&dev_name, L"\\Device\\NEET");
	status = IoCreateDevice(driver_obj, 0, &dev_name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &dev_obj);

	if (status != STATUS_SUCCESS) {
		return status;
	}

	RtlInitUnicodeString(&sym_link, L"\\DosDevices\\NEET");
	status = IoCreateSymbolicLink(&sym_link, &dev_name);

	if (status != STATUS_SUCCESS) {
		return status;
	}

	dev_obj->Flags |= DO_BUFFERED_IO;

	for (int t = 0; t <= IRP_MJ_MAXIMUM_FUNCTION; t++)
		driver_obj->MajorFunction[t] = unsupported_io;

	driver_obj->MajorFunction[IRP_MJ_CREATE] = create_io;
	driver_obj->MajorFunction[IRP_MJ_CLOSE] = close_io;
	driver_obj->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ctl_io;
	driver_obj->DriverUnload = UnloadDriver;

	dev_obj->Flags &= ~DO_DEVICE_INITIALIZING;

	return status;
}

NTSTATUS unsupported_io(PDEVICE_OBJECT device_obj, PIRP irp) {
	irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return irp->IoStatus.Status;
}

NTSTATUS create_io(PDEVICE_OBJECT device_obj, PIRP irp) {
	UNREFERENCED_PARAMETER(device_obj);

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return irp->IoStatus.Status;
}

NTSTATUS close_io(PDEVICE_OBJECT device_obj, PIRP irp) {
	UNREFERENCED_PARAMETER(device_obj);
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return irp->IoStatus.Status;
}

void write_mem(int pid, void* addr, void* value, size_t size) {
	PEPROCESS pe;
	SIZE_T bytes;
	PsLookupProcessByProcessId((HANDLE)pid, &pe);
	MmCopyVirtualMemory(PsGetCurrentProcess(), value, pe, addr, size, KernelMode, &bytes);
	ObfDereferenceObject(pe);
}

void read_mem(int pid, void* addr, void* value, size_t size) {
	PEPROCESS pe;
	SIZE_T bytes;
	PsLookupProcessByProcessId((HANDLE)pid, &pe);
	ProbeForRead(addr, size, 1);
	MmCopyVirtualMemory(pe, addr, PsGetCurrentProcess(), value, size, KernelMode, &bytes);
	ObfDereferenceObject(pe);
}
