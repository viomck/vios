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

    for (int x = 0; x < 1280; x++)
    {
        for (int y = 0; y < 800; y++)
        {
            GfxPlot(x, y, 255, 0, 0);
        }
    }

    FontRenderTestGlyphs();

    for (;;) {}
    return EFI_SUCCESS;
}
