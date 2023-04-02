# OS-1: A toy UNIX like Operating System Version 1.
[![CI](https://github.com/koyamanX/os-1/actions/workflows/build.yml/badge.svg)](https://github.com/koyamanX/os-1/actions/workflows/build.yml)
[![Doxygen deploy to GitHub pages](https://github.com/koyamanX/os-1/actions/workflows/docs.yml/badge.svg)](https://github.com/koyamanX/os-1/actions/workflows/docs.yml)
[![clang-format Check](https://github.com/koyamanX/os-1/actions/workflows/format.yml/badge.svg)](https://github.com/koyamanX/os-1/actions/workflows/format.yml)

# Overview

OS-1 is a toy UNIX like operating system version 1.

OS-1 is implemented in C and assembly for RISC-V qemu.

Supporting following features.
- Round-robin scheduling
- virtio-blk device dirver
- UART 16550 driver
- minix3 filesystem
- UNIX system call 
- device file

# Implemented system calls
- [x] write
- [x] execev
- [x] open
- [x] mkdir
- [x] mknod
- [x] dup
- [x] read
- [x] fork
- [x] _exit
- [ ] wait
- [ ] sleep
- [ ] pipe
- [ ] fstat
- [ ] chdir
- [ ] kill
- [ ] getpid
- [ ] sbrk
- [ ] uptime
- [ ] unlink
- [ ] link
- [x] close

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
## Help
```bash
./run.sh help
```

# Documentation

[Internal documentaions](https://koyamanx.github.io/os-1/)

# Limitations

- File larger than 8KB is not supported.
- Directory holding over 128 entries is not supported.
- uid/gid is set to user id and group id of root which is 0.
- Timestamp for file(atime,mtime and ctime) is not supported and always be constant 0 (1970/1/1).
- Current working directory is not supported.
- Relative path is not supported.
