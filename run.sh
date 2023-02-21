#!/bin/bash

mkdir _build
pushd _build
cmake ..
make install -j$(nproc)
popd

fallocate -l 512M rootfs.img
mkfs.minix -3 rootfs.img
sudo mount rootfs.img ./mnt
cp -r _build/mnt/* ./mnt -r
sync
sudo umount ./mnt

qemu-system-riscv64 -D qemu.log -d in_asm -machine virt -bios none -kernel _build/mnt/kernel -m 128M -nographic -global virtio-mmio.force-legacy=false -drive file=rootfs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0
