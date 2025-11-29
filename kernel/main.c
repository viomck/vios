#include <efi.h>
#include <efilib.h>
#include <boot/boot.h>
#include <kmain.h>

EFI_STATUS EFIAPI efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE * systemTable)
{
    InitializeLib(imageHandle, systemTable);
    Boot(systemTable, imageHandle);
    KMain();

    for (;;) {}
    return EFI_SUCCESS;
}
