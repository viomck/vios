#include <efi.h>
#include <boot/uefi/uefi.h>

void UEFIGOPSetMode(
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL * gop,
    IN UINT32 mode
)
{
    UEFIAssertStatus(uefi_call_wrapper(
        gop->SetMode,
        2,
        gop,
        mode
    ), EFI_SUCCESS, L"UEFIGOPSetMode");
}

EFI_STATUS queryMode(
    IN  EFI_GRAPHICS_OUTPUT_PROTOCOL * gop,
    IN  UINT32 mode,
    OUT EFI_GRAPHICS_OUTPUT_MODE_INFORMATION ** info
)
{
    UINTN sizeOfInfo = 0;

    return uefi_call_wrapper(
        gop->QueryMode,
        4,
        gop,
        mode,
        &sizeOfInfo,
        info
    );
}

// loads the gop->Mode
void UEFIGOPQueryCurrentMode(IN EFI_GRAPHICS_OUTPUT_PROTOCOL * gop)
{
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION * info = NULL;
    EFI_STATUS status = queryMode(gop, gop->Mode == NULL ? 0 : gop->Mode->Mode, &info);

    // hack: sometimes we need to set the mode to get the mode, i guess
    if (status == EFI_NOT_STARTED)
    {
        UEFIGOPSetMode(gop, 0);
    } 
    else
    {
        UEFIAssertStatus(status, EFI_SUCCESS, L"UEFIGOPQueryCurrentMode");
    }
}

void UEFIGOPQueryMode(
    IN  EFI_GRAPHICS_OUTPUT_PROTOCOL * gop,
    IN  UINT32 mode,
    OUT EFI_GRAPHICS_OUTPUT_MODE_INFORMATION ** info
)
{
    UEFIAssertStatus(queryMode(gop, mode, info), EFI_SUCCESS, L"UEFIGOPQueryMode");
}
