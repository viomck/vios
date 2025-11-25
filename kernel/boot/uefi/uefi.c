#include <efi.h>
#include <boot/bootpanic.h>

EFI_SYSTEM_TABLE * g_systemTable = NULL;

void UEFIInit(IN EFI_SYSTEM_TABLE * systemTable)
{
    g_systemTable = systemTable;
}

void UEFIAssertStatus(
    IN EFI_STATUS actual, 
    IN EFI_STATUS expected,
    IN wchar_t * caller
)
{
    if (actual != expected)
    {
        BootPanicSetData1(actual);
        BootPanicSetData2(expected);
        BootPanic(caller);
    }
}

EFI_SYSTEM_TABLE * UEFIGetSystemTable()
{
    if (g_systemTable == NULL)
    {
        BootPanic(L"g_systemTable accessed before initialization");
    }
    return g_systemTable;
}
