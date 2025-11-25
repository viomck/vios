#include <efi.h>

void UEFIBootGetMemoryMap(
    IN EFI_STATUS expectedStatus,
    IN OUT UINTN * memoryMapSize,
    OUT EFI_MEMORY_DESCRIPTOR * memoryMap,
    OUT UINTN * mapKey,
    OUT UINTN * descriptorSize,
    OUT UINT32 * descriptorVersion
);

void UEFIBootAllocatePool(
    IN EFI_MEMORY_TYPE memoryType,
    IN UINTN size,
    OUT VOID ** buffer
);

void UEFIBootExitBootServices(IN EFI_HANDLE imageHandle, IN UINTN mapKey);

void UEFIBootLocateProtocol(
    IN EFI_GUID * protocolGuid,
    OUT VOID ** protocol
);
