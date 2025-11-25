#include <efi.h>
#include <efilib.h>
#include <boot/uefi/ueficonsole.h>

const static long UNSET_PANIC_DATA = 0xDEADBEEFCAFE1337;

unsigned long g_panicData1 = UNSET_PANIC_DATA;
unsigned long g_panicData2 = UNSET_PANIC_DATA;
unsigned long g_panicData3 = UNSET_PANIC_DATA;
unsigned long g_panicData4 = UNSET_PANIC_DATA;

void BootPanicSetData1(unsigned long data) { g_panicData1 = data; }
void BootPanicSetData2(unsigned long data) { g_panicData2 = data; }
void BootPanicSetData3(unsigned long data) { g_panicData3 = data; }
void BootPanicSetData4(unsigned long data) { g_panicData4 = data; }

void PrintPanicData(unsigned long data)
{
    if (data == UNSET_PANIC_DATA)
    {
        Print(L"<unset>\n");
        return;
    }

    Print(L"%lx (%ld)\n", data, data);
}

void BootPanic(wchar_t * message)
{   
    UEFIConsoleSetColors(EFI_WHITE | EFI_BACKGROUND_RED);
    UEFIConsoleClear();
    Print(L"\n");
    Print(L"\n");
    Print(L"\n");
    Print(L"                  ___ |[]|P_a_n_i_c\n");
    Print(L"                 |___||__|###|____}\n");
    Print(L"                  O-O--O-O+++--O-O\n");
    Print(L"\n");
    Print(L" /$$$$$$$   /$$$$$$  /$$   /$$ /$$$$$$  /$$$$$$  /$$\n");
    Print(L"| $$__  $$ /$$__  $$| $$$ | $$|_  $$_/ /$$__  $$| $$\n");
    Print(L"| $$  \\ $$| $$  \\ $$| $$$$| $$  | $$  | $$  \\__/| $$\n");
    Print(L"| $$$$$$$/| $$$$$$$$| $$ $$ $$  | $$  | $$      | $$\n");
    Print(L"| $$____/ | $$__  $$| $$  $$$$  | $$  | $$      |__/\n");
    Print(L"| $$      | $$  | $$| $$\\  $$$  | $$  | $$    $$    \n");
    Print(L"| $$      | $$  | $$| $$ \\  $$ /$$$$$$|  $$$$$$/ /$$\n");
    Print(L"|__/      |__/  |__/|__/  \\__/|______/ \\______/ |__/\n");
    Print(L"\n");
    Print(L"Bootloader Panic\n");
    Print(L"--------------------\n");
    Print(L"\n");
    Print(L"Unfortunately for you, the panic train has arrived at the station.\n");
    Print(L"\n");
    Print(L"--------------------\n");
    Print(L"\n");
    Print(L"Reason: %s\n", message);
    Print(L"\n");
    Print(L"Data 1: "); PrintPanicData(g_panicData1);
    Print(L"Data 2: "); PrintPanicData(g_panicData2);
    Print(L"Data 3: "); PrintPanicData(g_panicData3);
    Print(L"Data 4: "); PrintPanicData(g_panicData4);
    Print(L"\n");
    Print(L"Did you know you get a free subscription with Amazon Prime?\n");
    Print(L"twitch.tv/viboof\n");
    for (;;) {}
}
