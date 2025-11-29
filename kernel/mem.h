#include <efi.h>

void MemSetMemoryMapPtr(
    EFI_MEMORY_DESCRIPTOR * memoryMapPtr, 
    uint64_t mapSize, 
    uint64_t descriptorSize
);
void MemInit();

#define MemAlloc(size, out) _MemAlloc(size, (void **) out)
void _MemAlloc(uint64_t size, void ** out);
#define MemFree(memory) _MemFree((void **) memory)
void _MemFree(void ** memory);
