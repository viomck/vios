#include <efi.h>
#include <efilib.h>
#include <boot/boot.h>
#include <gfx.h>
#include <stdint.h>
#include <font.h>
#include <mem.h>
#include <std.h>
#include <panic.h>

const static int viosscale = 4;

void WriteStartupStep(const char * step)
{
    const static int stepscale = 2;
    int width = GfxGetScreenWidth();
    int height = GfxGetScreenHeight();
    FontRenderStrUnwrapped(
        step, 
        stepscale, 
        width / 2 - (8 * stepscale * ConstStrLen(step) / 2), 
        height / 2 + 16  // 16 for a little visual buffer
    );
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE * systemTable)
{
    InitializeLib(imageHandle, systemTable);
    Boot(systemTable, imageHandle);

    int width = GfxGetScreenWidth();
    int height = GfxGetScreenHeight();

    // my favorite color :)
    GfxFullScreenGradientToColor(0xFF, 0xAA, 0xFF);
    FontRenderStrUnwrapped(
        "VIOS :)", 
        viosscale, 
        width / 2 - (8 * viosscale * 7 / 2), 
        height / 2 - (16 * viosscale)
    );

    #ifdef FLAG_KERNELPANIC
    GfxSetScreenWidthAndHeight(0x1, 0x2);
    #endif

    WriteStartupStep("Initializing memory");
    MemInit();

    uint64_t * allocated = NULL;
    MemAlloc(64, &allocated);
    *allocated = 0xDEADCAFEBEEFABBA;

    uint16_t * blah = NULL; MemAlloc(16, &blah); *blah = 0x1234;
    // FontRenderHex(blah, 8, 0, 0);
    uint8_t * blah2 = NULL; MemAlloc(8, &blah2); *blah2 = 0xAB;
    MemFree(&blah);
    uint8_t * blah3 = NULL; MemAlloc(8, &blah3); *blah3 = 0xCD;

    PanicSetData1((unsigned long) allocated);
    PanicSetData2(*allocated);
    PanicSetData3((unsigned long) &allocated);
    PanicSetData4((unsigned long) blah);
    PanicSetData5((unsigned long) blah2);
    PanicSetData6((unsigned long) blah3);
    Panic("D1=ptr,D2=val,D3=ptr->ptr,D4=blahptr,D5=blah2ptr,D6=blah3ptr");

    for (;;) {}
    return EFI_SUCCESS;
}
