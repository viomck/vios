# vios
making an operating system from scratch for fun

# how to run
1. don't
1. be on linux (tested on ubuntu)
1. install qemu-system and mtools
1. `git clone --recursive https://github.com/viomck/vios && cd vios`
    - if you already cloned without --recursive, run `git submodule update --init --recursive`
1. cd kernel
1. ./run.sh
    - override QEMU's path with the `QEMU` env var
    - override QEMU's working directory (where the image gets stored
      temporarily with the OVMF files)
