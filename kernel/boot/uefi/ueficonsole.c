#include <efi.h>
#include <boot/uefi/uefi.h>

void UEFIConsoleClear()
{
    uefi_call_wrapper(
        UEFIGetSystemTable()->ConOut->ClearScreen,
        1,
        UEFIGetSystemTable()->ConOut
    );
}

void UEFIConsoleSetColors(IN UINTN efiColors)
{
    uefi_call_wrapper(
        UEFIGetSystemTable()->ConOut->SetAttribute,
        2,
        UEFIGetSystemTable()->ConOut,
        efiColors
    );
}
