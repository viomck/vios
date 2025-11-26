#!/bin/bash

cd ~/uefios/kernel

ARGS="$@"

has_flag() {
    flag=$1

    if [[ "$ARGS" == *"--$flag"* ]]; then
        return 0  # 0 = true in bash
    fi

    return 1  # 1 = false in bash
}

# ------------------------- BUILD THE EFI APPLICATION -------------------------
CCFLAGS="-I. -I../gnu-efi/inc -fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args"
LDFLAGS="-shared -Bsymbolic -L../gnu-efi/x86_64/lib -L../gnu-efi/x86_64/gnuefi -T../gnu-efi/gnuefi/elf_x86_64_efi.lds ../gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o"
OBJCOPYFLAGS="-j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10"

define_build_flag() {
    flag=$1
    
    if has_flag $flag; then
        defname="FLAG_${flag^^}"
        CCFLAGS="$CCFLAGS -D$defname"
    fi
}

define_build_flag memmap
define_build_flag bootpanic
define_build_flag kernelpanic
define_build_flag slowgfx

linker_files=""

cleanup() {
    rm -f part.img
    rm -f uefi.img
    rm -f main.efi
    rm -f main.so
    rm -rf EFI
    find . -maxdepth 10 -name "*.o" -exec rm {} \;
}

cleanup

build() {
    path="$1"
    cfile="$path.c"
    ofile="$path.o"
    echo "building $cfile -> $ofile..." ; gcc $CCFLAGS -c $cfile -o $ofile
    linker_files="$linker_files $ofile"
}

build boot/uefi/uefi
build boot/uefi/uefiboot
build boot/uefi/ueficonsole
build boot/uefi/uefigop
build boot/boot
build boot/bootpanic
build gen/font
build font
build gfx
build main
build panic

echo "linking $linker_files..." ; ld $LDFLAGS $linker_files -o main.so -lgnuefi -lefi
echo "extracting EFI executable..." ; objcopy $OBJCOPYFLAGS main.so main.efi

if has_flag "buildonly"; then
    cleanup
    exit 0
fi
# -----------------------------------------------------------------------------

# ------------------------- BUILD THE UEFI DISK IMAGE -------------------------
echo "creating zeroed uefi.img..." ; dd if=/dev/zero of=uefi.img bs=512 count=93750
echo "creating gpt partition..." ; parted uefi.img -s -a minimal mklabel gpt
echo "creating FAT16 EFI partition..." ; parted uefi.img -s -a minimal mkpart EFI FAT16 2048s 93716s
echo "making partition bootable..." ; parted uefi.img -s -a minimal toggle 1 boot

echo "creating zeroed part.img for EFI data partition..." ; dd if=/dev/zero of=part.img bs=512 count=91669
echo "formatting part.img as FAT16..." ; mformat -i part.img -h 32 -t 32 -n 64 -c 1
mkdir EFI
mkdir EFI/BOOT
cp main.efi EFI/BOOT/BOOTX64.EFI
echo "copying main.efi into part.img..." ; mcopy -si part.img EFI ::

echo "copying part.img into uefi.img..." ; dd if=part.img of=uefi.img bs=512 count=91669 seek=2048 conv=notrunc
# -----------------------------------------------------------------------------

# --------------------------- RUN THE IMAGE IN QEMU ---------------------------
cd /mnt/c/Users/Violet/Documents/uefios
cp ~/uefios/ovmf/* ovmf
cp ~/uefios/kernel/uefi.img uefi.img
echo "running qemu..." ; /mnt/c/Program\ Files/qemu/qemu-system-x86_64.exe \
	-m 1024M \
	-drive if=pflash,unit=0,format=raw,readonly=on,file=ovmf/codex86.fd \
	-drive if=pflash,unit=1,format=raw,file=ovmf/varsx86.fd \
	-drive file=uefi.img,format=raw,if=ide \
    -serial stdio
cd ~/uefios/kernel

cleanup
