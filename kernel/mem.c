#include <efi.h>
#include <panic.h>
#include <stdbool.h>
#include <font.h>

const static uint16_t MAGIC_ALLOCATED_HEADER = 0xA10C;

EFI_MEMORY_DESCRIPTOR * g_memoryMapPtr = NULL;
uint64_t g_mapSize, g_descriptorSize;

struct Allocation
{
    uint64_t size;
    struct Allocation * last;
    struct Allocation * next;
    uint8_t checksum;
};

bool testAllocChecksum(struct Allocation * alloc)
{
    int numOnes = 0;
    for (uint8_t byt = 0; byt < sizeof(struct Allocation); byt++)
    {
        for (uint8_t bit = 0; bit < 8; bit++)
        {
            if (*(uint8_t *) ((uint64_t) alloc + byt) >> bit & 1)
            {
                numOnes++;
            }
        }
    }
    FontRenderHex(numOnes, 2, 500, 500);
    // for (uint64_t i = 0; i < 100000000; i++) { __asm__("nop"); }
    return numOnes % 8 == 0;
}

void writeAllocChecksum(struct Allocation * alloc)
{
    for (
        alloc->checksum = 0; 
        alloc->checksum <= 255 && !testAllocChecksum(alloc);
        alloc->checksum++
    ){
        FontRenderHex(alloc->checksum, 8, 0, 0);
    }

    if (/*still*/ !testAllocChecksum(alloc))
    {
        PanicSetData1(alloc->size);
        PanicSetData2((unsigned long) alloc->last);
        PanicSetData3((unsigned long) alloc->next);
        PanicSetData4((unsigned long) alloc);
        Panic("writeAllocChecksum: Could not compute checksum! D1=size,D2=last,D3=next,D4=ptr");
    }
}

void verifyAlloc(struct Allocation * alloc)
{
    if (!testAllocChecksum(alloc))
    {
        PanicSetData1(alloc->size);
        PanicSetData2((unsigned long) alloc->last);
        PanicSetData3((unsigned long) alloc->next);
        PanicSetData4(alloc->checksum);
        PanicSetData5((unsigned long) alloc);
        Panic("verifyAlloc: Could not verify Allocation checksum! D1=size,D2=last,D3=next,D4=checksum,D5=ptr");
    }
}

void getEndOfAlloc(struct Allocation * alloc, void ** out)
{
    FontRenderStr("getEndOfAlloc", 2, 0, 0);
    verifyAlloc(alloc);
    *out = (void *) ((uint64_t) alloc + sizeof(struct Allocation) + alloc->size);
}

uint64_t startOfDesc(EFI_MEMORY_DESCRIPTOR * desc)
{
    // on some systems, at least on qemu, 0x0 is the start of a valid 
    // EfiConventionalMemory, but since 0x0 is NULL, we'll save infinite
    // headache by just pretending it starts at 0x1
    if (desc->PhysicalStart == 0x0)
    {
        return 0x1;
    }

    return desc->PhysicalStart;
}

void MemSetMemoryMapPtr(
    EFI_MEMORY_DESCRIPTOR * memoryMapPtr, 
    uint64_t mapSize, 
    uint64_t descriptorSize
)
{
    if (g_memoryMapPtr != NULL || g_mapSize != 0 || g_descriptorSize != 0)
    {
        Panic("MemSetMemoryMapPtr: Already initialized!");
    }
    g_memoryMapPtr = memoryMapPtr;
    g_mapSize = mapSize;
    g_descriptorSize = descriptorSize;
}

bool hasExistingAllocations(EFI_MEMORY_DESCRIPTOR * descriptor)
{
    return *((uint16_t *) startOfDesc(descriptor)) == MAGIC_ALLOCATED_HEADER;
}

uint64_t bytesToPages(uint64_t bytes)
{
    return 1 + bytes / 4096;
}

void maybeGetAllocPtrForDesc(
    EFI_MEMORY_DESCRIPTOR * descriptor, 
    uint64_t size, 
    struct Allocation ** out
)
{
    *out = NULL;

    if (!hasExistingAllocations(descriptor))
    {
        *out = (void *) startOfDesc(descriptor);
        **((uint64_t **) out) = 0;
        return;
    }

    *out = (struct Allocation *) (startOfDesc(descriptor) + 2);
    FontRenderStr("maybeGetAllocPtrForDesc", 2, 0, 0);
    void * endOfAlloc = NULL;

    for (; (*out)->next != NULL; *out = (*out)->next)
    {
        verifyAlloc(*out);
        // if the gap between the end of this allocation and the start of the next
        // can fit size + sizeof(Allocation) + alloc->size
        getEndOfAlloc(*out, &endOfAlloc);

        uint64_t gap = (uint64_t) (*out)->next - (uint64_t) endOfAlloc;
        FontRenderHex(gap, 2, 0, 500);

        if (gap >= sizeof(struct Allocation) + size)
        {
            return;
        }
    }

    // do this in case the first alloc had null next, and such the loop never ran
    getEndOfAlloc(*out, &endOfAlloc);

    // couldn't find any gaps, give it the last alloc as long as it fits
    int usedPages = bytesToPages((uint64_t) endOfAlloc - descriptor->PhysicalStart);
    if (descriptor->NumberOfPages - usedPages >= bytesToPages(size))
    {
        // return last
        return;
    }

    // couldn't find anything sorry
    *out = NULL;
}

void maybeGetAllocPtr(uint64_t size, bool wantExistingAllocsOnDesc, struct Allocation ** out)
{
    *out = NULL;

    EFI_MEMORY_DESCRIPTOR * desc = g_memoryMapPtr;

    for (int i = 0; i < g_mapSize / g_descriptorSize; i++)
    {
        // if(!wantExistingAllocsOnDesc) for (uint64_t _ = 0; _ < 1000000000; _++) { __asm__("nop"); }
        FontRenderHex(i, 2, 0, 0);
        FontRenderHex(desc->Type, 2, 0, 100);
        FontRenderHex(0xFFFFFFFFFFFFFFFF, 2, 0, 200);
        FontRenderHex(0xFFFFFFFFFFFFFFFF, 2, 0, 300);
        if (desc->Type == EfiConventionalMemory)
        {
            bool allocsOk = !wantExistingAllocsOnDesc || 
                hasExistingAllocations(desc);
            
            FontRenderHex(wantExistingAllocsOnDesc, 2, 0, 200);
            FontRenderHex(hasExistingAllocations(desc), 2, 0, 300);

            if (allocsOk)
            {
                maybeGetAllocPtrForDesc(desc, size, out);

                if (*out != NULL)
                {
                    return;
                }
            }
        }

        desc = (EFI_MEMORY_DESCRIPTOR *) ((uint8_t *) desc + g_descriptorSize);
    }

    *out = NULL;
}

void _MemAlloc(uint64_t size, void ** out)
{
    *out = NULL;

    struct Allocation * allocPtr;

    // prefer a descriptor that alrdy has allocs
    maybeGetAllocPtr(size, true, &allocPtr);

    if (allocPtr == NULL)
    {
        // ok fine have another descriptor
        maybeGetAllocPtr(size, false, &allocPtr);

        if (allocPtr /* still */ == NULL)
        {
            // uh oh
            PanicSetData1(size);
            PanicSetData2((unsigned long) out);
            Panic("MemAlloc: Out of memory. D1=size, D2=out");
        }
    }

    // if its on a descriptor with no allocations
    if (*((uint16_t *) allocPtr) == 0)
    {
        // mark allocated
        *((uint16_t *) allocPtr) = MAGIC_ALLOCATED_HEADER;
        // move past header
        struct Allocation * newAllocPtr = (struct Allocation *) ((uint64_t) allocPtr + 2);
        *newAllocPtr = (struct Allocation) 
            { .size = size, .last = NULL, .next = NULL, .checksum = 0 };
        writeAllocChecksum(newAllocPtr);
        *out = (void *) ((uint64_t) newAllocPtr + sizeof(struct Allocation));
    }
    else {
        verifyAlloc(allocPtr);

        // real existing allocation
        getEndOfAlloc(allocPtr, out);
        *((struct Allocation *) *out) = (struct Allocation)
            {
                .size = size,
                .last = allocPtr,
                .next = allocPtr->next == NULL ? NULL : allocPtr->next,
            };

        writeAllocChecksum((struct Allocation *) *out);
            
        if (allocPtr->next != NULL)
        {
            verifyAlloc(allocPtr->next);
            allocPtr->next->last = (struct Allocation *) *out;
            writeAllocChecksum(allocPtr->next);
        }

        allocPtr->next = (struct Allocation *) *out;
        writeAllocChecksum(allocPtr);

        *out = (void *) ((uint64_t) *out + sizeof(struct Allocation));
    }

    if (*out == NULL)
    {
        PanicSetData1(size);
        PanicSetData2((unsigned long) out);
        Panic("MemAlloc: *init = NULL. This should be unreachable! D1=size, D2=out");
    }
}

void _MemFree(void ** memory)
{
    struct Allocation * alloc = (struct Allocation *) 
        ((uint64_t) *memory - sizeof(struct Allocation));
    FontRenderStr("free", 2, 0, 0);
    verifyAlloc(alloc);

    if (alloc->last != NULL)
    {
        alloc->last->next = alloc->next == NULL ? NULL : alloc->next;
    }

    if (alloc->next != NULL)
    {
        alloc->next->last = alloc->last == NULL ? NULL : alloc->next;
    }

    // clear memory
    for (int i = 0; i < alloc->size; i++)
    {
        *((uint8_t *) ((uint64_t) *memory + i)) = 0;
    }

    // clear alloc
    for (int i = 0; i < sizeof(struct Allocation); i++)
    {
        *((uint8_t *) ((uint64_t) alloc + i)) = 0;
    }
}

void MemInit()
{
    if (g_memoryMapPtr == NULL)
    {
        PanicSetData1((unsigned long) g_memoryMapPtr);
        PanicSetData2(g_mapSize);
        PanicSetData3(g_descriptorSize);
        Panic("MemInit: Missing important information. D1=g_memoryMapPtr,D2=g_mapSize,D3=g_descriptorSize");
    }
}
