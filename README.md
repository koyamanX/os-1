# A toy UNIX like Operating System OS-1

# Overview
OS-1 is a toy UNIX like operating system version 1.

OS-1 is implemented in C and assembly for RISC-V qemu.

Supporting following features.
- Round-robin scheduling
- virtio-blk device dirver
- UART 16550 driver
- minix3 filesystem
- UNIX system call 
- device file (not implemented yet)

# Implemented system calls
- [ ] write (partially implemented)
- [ ] open (partially implemented)
- [ ] execev (partially implemented)

# How to use
RISC-V toolchain, qemu for RISC-V and minix3 kernel module is required to run os-1.

## Build
```bash
./run build
```
## Build & Run
```bash
./run.sh run
```
## Clean
```bash
./run.sh clean
```
