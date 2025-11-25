#include <efi.h>
#include <panic.h>
#include <uefi/uefi.h>

void UEFIBootGetMemoryMap(
    IN EFI_STATUS expectedStatus,
    IN OUT UINTN * memoryMapSize,
    OUT EFI_MEMORY_DESCRIPTOR * memoryMap,
    OUT UINTN * mapKey,
    OUT UINTN * descriptorSize,
    OUT UINT32 * descriptorVersion
)
{
    UEFIAssertStatus(uefi_call_wrapper(
        UEFIGetSystemTable()->BootServices->GetMemoryMap,
        5,
        memoryMapSize,
        memoryMap,
        mapKey,
        descriptorSize,
        descriptorVersion
    ), expectedStatus, L"UEFIBootGetMemoryMap");
}

void UEFIBootAllocatePool(
    IN EFI_MEMORY_TYPE memoryType,
    IN UINTN size,
    OUT VOID ** buffer
)
{
    UEFIAssertStatus(uefi_call_wrapper(
        UEFIGetSystemTable()->BootServices->AllocatePool,
        3,
        memoryType,
        size,
        buffer
    ), EFI_SUCCESS, L"UEFIBootAllocatePool");
}

void UEFIBootExitBootServices(
    IN EFI_HANDLE imageHandle,
    IN UINTN mapKey
)
{
    UEFIAssertStatus(uefi_call_wrapper(
        UEFIGetSystemTable()->BootServices->ExitBootServices,
        2,
        imageHandle,
        mapKey
    ), EFI_SUCCESS, L"UEFIBootExitBootServices");
}

void UEFIBootLocateProtocol(
    IN EFI_GUID * protocolGuid,
    OUT VOID ** protocol
)
{
    UEFIAssertStatus(uefi_call_wrapper(
        UEFIGetSystemTable()->BootServices->LocateProtocol,
        3,
        protocolGuid,
        NULL,
        protocol
    ), EFI_SUCCESS, L"UEFIBootLocateProtocol");
}
