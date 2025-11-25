#include <efi.h>

void UEFIInit(IN EFI_SYSTEM_TABLE * systemTable);
void UEFIAssertStatus(
    IN EFI_STATUS actual, 
    IN EFI_STATUS expected,
    IN wchar_t * caller
);

EFI_SYSTEM_TABLE * UEFIGetSystemTable();
