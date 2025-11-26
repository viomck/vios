#include <efi.h>
#include <efilib.h>
#include <boot/boot.h>
#include <gfx.h>
#include <stdint.h>
#include <font.h>

EFI_STATUS EFIAPI efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE * systemTable)
{
    InitializeLib(imageHandle, systemTable);
    Boot(systemTable, imageHandle);

    FontRenderStr("hi :D", 4, 0, 0);

    #ifdef FLAG_KERNELPANIC
    GfxSetScreenWidthAndHeight(0x1, 0x2);
    #endif

    for (;;) {}
    return EFI_SUCCESS;
}
