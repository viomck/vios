#!/bin/bash
set -e

: "${QEMU:=qemu-system-x86_64}"
: "${QEMU_RUNDIR:=/tmp/viosqemuworkingdir}"
echo "QEMU: $QEMU"
echo "QEMU Working Directory: $QEMU_RUNDIR"

ARGS="$@"

has_flag() {
    flag=$1

    if [[ "$ARGS" == *"--$flag"* ]]; then
        return 0  # 0 = true in bash
    fi

    return 1  # 1 = false in bash
}

cleanup() {
    rm -f part.img
    rm -f uefi.img
    rm -f main.efi
    rm -f main.so
    rm -rf EFI
    rm -rf ../gnu-efi/x86_64
    rm -rf ../res/bdf2c/bdf2c
    rm -rf ../res/bdf2c/bdf2c.o
    rm -rf gen/genfont.c
    rm -rf gen/font.h
    rm -rf "$QEMU_RUNDIR/ovmf"
    rm -rf "$QEMU_RUNDIR/uefi.img"
    find . -maxdepth 10 -name "*.o" -exec rm {} \;
}

cleanup

# ------------------------------ BUILD VGA FONTS ------------------------------
#
echo "making bdf2c..."
cd ../res/bdf2c
# there are compiler warnings, but they don't stop bdf2c from functioning, so
# we edit the Makefile lol
cp Makefile Makefile.bak
sed -i 's/-Werror -W -Wall//' Makefile
make
mv Makefile.bak Makefile

echo "generating gen/font.h..."
./bdf2c -C ../../kernel/gen/font.h
echo "generating gen/genfont.c..."
./bdf2c -b < ../u_vga16.bdf > ../../kernel/gen/genfont.c
cd ../../kernel
printf "\n\nunsigned char GenFontGetBitmap(int index) { return __font_bitmap__[index]; }" >> gen/genfont.c
# -----------------------------------------------------------------------------

# ------------------------------- BUILD GNU-EFI -------------------------------
echo "making gnu-efi..."
cd ../gnu-efi
make
cd ../kernel
# -----------------------------------------------------------------------------

# ------------------------- BUILD THE EFI APPLICATION -------------------------
CCFLAGS="-I. -I../gnu-efi/inc -fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args -fno-builtin"
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
build gen/genfont
build font
build gfx
build kmain
build main
build mem
build panic
build print
build std

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
KERNELDIR="$PWD"

monitorarg="-monitor stdio"

if has_flag "memmap"; then
    monitorarg="-serial stdio"
fi

mkdir -p "$QEMU_RUNDIR/ovmf"
cp ../ovmf/* "$QEMU_RUNDIR/ovmf"
cp uefi.img "$QEMU_RUNDIR"
cd $QEMU_RUNDIR
echo "running qemu..." ; "$QEMU" \
	-m 1024M \
	-drive if=pflash,unit=0,format=raw,readonly=on,file=ovmf/codex86.fd \
	-drive if=pflash,unit=1,format=raw,file=ovmf/varsx86.fd \
	-drive file=uefi.img,format=raw,if=ide \
    $monitorarg
cd $KERNELDIR

cleanup
