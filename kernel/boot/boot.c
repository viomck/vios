#include <efi.h>
#include <efilib.h>
#include <stdbool.h>
#include <boot/uefi/uefi.h>
#include <boot/uefi/ueficonsole.h>
#include <boot/uefi/uefiboot.h>
#include <boot/uefi/uefigop.h>
#include <boot/bootpanic.h>
#include <gfx.h>

#define MemType(t, print) case (t): Print(print); break;

const static UINT32 DESIRED_WIDTH = 1280;
const static UINT32 DESIRED_HEIGHT = 800;

void PrintMemoryType(UINT32 type) 
{
    switch (type) {
        MemType(0, L"EfiReservedMemoryType");
        MemType(1, L"EfiLoaderCode");
        MemType(2, L"EfiLoaderData");
        MemType(3, L"EfiBootServicesCode");
        MemType(4, L"EfiBootServicesData");
        MemType(5, L"EfiBootServicesData");
        MemType(6, L"EfiRuntimeServicesData");
        MemType(7, L"EfiConventionalMemory");
        MemType(8, L"EfiUnusableMemory");
        MemType(9, L"EfiACPIReclaimMemory");
        MemType(10, L"EfiACPIMemoryNVS");
        MemType(11, L"EfiMemoryMappedIO");
        MemType(12, L"EfiMemoryMappedIOPortSpace");
        MemType(13, L"EfiPalCode");
        MemType(14, L"EfiPersistentMemory");
        default:
            Print(L"Reserved/Other");
    }
}

void PrintMemoryDescriptor(EFI_MEMORY_DESCRIPTOR descriptor) 
{
    Print(L"Type: %d (", descriptor.Type);
    PrintMemoryType(descriptor.Type);
    Print(L") ");
    // Print(L"MPad: %d ", descriptor.Pad);
    Print(L"Phys: %p ", descriptor.PhysicalStart);
    Print(L"Virt: %p ", descriptor.VirtualStart);
    Print(L"Pags: %d ", descriptor.NumberOfPages);
    Print(L"Attr: %d\n", descriptor.Attribute);
}

#ifdef FLAG_MEMMAP
void PrintMemoryMap(
    IN EFI_MEMORY_DESCRIPTOR * memoryMap, 
    IN UINTN * memoryMapSize,
    IN UINTN * descriptorSize
)
{
    UINTN numDescriptors = *memoryMapSize / *descriptorSize;

    EFI_MEMORY_DESCRIPTOR * descriptor = memoryMap;
    for (UINTN i = 0; i < numDescriptors; i++)
    {
        Print(L"#%d - ", i);
        PrintMemoryDescriptor(*descriptor);
        // thank you newworldandy
        descriptor = (EFI_MEMORY_DESCRIPTOR*) ((UINT8*)descriptor + *descriptorSize);
    }

    Print(L"\nMemory map size:  %d\n", *memoryMapSize);
    Print(L"Descriptor size:  %d\n", *descriptorSize);
    Print(L"# of descriptors: %d\n", numDescriptors);

    Print(L"\nHalted due to memory map. Build without --memmap to boot.\n");

    for (;;) {}
}
#endif

void ExitBootMode(EFI_HANDLE imageHandle)
{
    // our first iteration is to grab relevant metadata by giving it a size of
    // 0 (it will throw EFI_BUFFER_TOO_SMALL and set memoryMapSize to its
    // desired value for allocation)
    UINTN memoryMapSize = 0;
    EFI_MEMORY_DESCRIPTOR * memoryMap = NULL;
    UINTN descriptorSize = 0;
    UINTN mapKey = 0;
    UINT32 descriptorVersion = 0;
    UEFIBootGetMemoryMap(EFI_BUFFER_TOO_SMALL, &memoryMapSize, memoryMap, &mapKey, &descriptorSize, &descriptorVersion);
    
    // allocate the memory.  we add a grace size of 2*descriptorSize since this
    // call may add another descriptor (or two depending on alignment 
    // weirdness)
    memoryMapSize += 2 * descriptorSize;
    UEFIBootAllocatePool(EfiLoaderData, memoryMapSize, (VOID **) &memoryMap);

    // get the memory map for real this time, over our recently allocatd memory
    UEFIBootGetMemoryMap(EFI_SUCCESS, &memoryMapSize, memoryMap, &mapKey, &descriptorSize, &descriptorVersion);

    #ifdef FLAG_MEMMAP
    PrintMemoryMap(memoryMap, &memoryMapSize, &descriptorSize);
    #endif

    // exit boot land
    UEFIBootExitBootServices(imageHandle, mapKey);
}

void PrintGraphicsModes(EFI_GRAPHICS_OUTPUT_PROTOCOL * gop)
{
    Print(L"Mode: %d\n", gop->Mode->Mode);
    Print(L"Max modes: %d\n", gop->Mode->MaxMode);

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION * info = NULL;
    for (int i = 0; i < gop->Mode->MaxMode; i++)
    {
        UEFIGOPQueryMode(gop, i, &info);
        Print(L"Mode %d - %dx%d - ", 
            i, 
            info->HorizontalResolution, 
            info->VerticalResolution);
        switch (info->PixelFormat)
        {
            case PixelRedGreenBlueReserved8BitPerColor: Print(L"RGB Reserved 8-Bit Per Color"); break;
            case PixelBlueGreenRedReserved8BitPerColor: Print(L"BGR Reserved 8-Bit Per Color"); break;
            case PixelBitMask: Print(L"Bit Mask"); break;
            case PixelBltOnly: Print(L"Blt Only"); break;
            case PixelFormatMax: Print(L"Format Max"); break;
            default: Print(L"Unknown"); break;
        }
        if (i == gop->Mode->Mode)
        {
            Print(L" (active mode)");
        }
        Print(L"\n");
    }
}

void SetDesiredGraphicsMode(EFI_GRAPHICS_OUTPUT_PROTOCOL * gop)
{
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION * info = NULL;
    for (int i = 0; i < gop->Mode->MaxMode; i++)
    {
        UEFIGOPQueryMode(gop, i, &info);
        if (
            info->HorizontalResolution == DESIRED_WIDTH
                && info->VerticalResolution == DESIRED_HEIGHT
                && (info->PixelFormat == PixelRedGreenBlueReserved8BitPerColor 
                    || info->PixelFormat == PixelBlueGreenRedReserved8BitPerColor)
        )
        {
            UEFIGOPSetMode(gop, i);
            GfxSetScreenWidthAndHeight(info->HorizontalResolution, info->VerticalResolution);
            return;
        }
    }

    BootPanic(L"main: SetDesiredGraphicsMode: Could not find a suitable graphics mode");
}

void Boot(EFI_SYSTEM_TABLE * systemTable, EFI_HANDLE imageHandle)
{
    UEFIInit(systemTable);
    UEFIConsoleClear();
    Print(L"Initializing bootloader\n");

    #ifdef FLAG_BOOTPANIC
    BootPanic(L"Panic initiated by --bootpanic build flag.");
    #endif

    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL * gop = NULL;
    Print(L"Obtaining GOP...\n");
    UEFIBootLocateProtocol(&gopGuid, (VOID **) &gop);
    Print(L"Obtaining current GOP mode info...\n");
    UEFIGOPQueryCurrentMode(gop);

    Print(L"Setting desired graphics mode...\n");
    SetDesiredGraphicsMode(gop);
    PrintGraphicsModes(gop);
    
    Print(L"Setting frame buffer start at %p\n", gop->Mode->FrameBufferBase);
    GfxSetFrameBufferStart((uint64_t) gop->Mode->FrameBufferBase);
    Print(L"Setting pixels per scan line to %d\n", gop->Mode->Info->PixelsPerScanLine);
    GfxSetPixelsPerScanLine((uint32_t) gop->Mode->Info->PixelsPerScanLine);

    Print(L"Exiting bootloader to kernel...\n");
    ExitBootMode(imageHandle);
}
