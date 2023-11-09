#!/bin/bash

apt update && \
	DEBIAN_FRONTEND=noninteractive apt install -y --no-install-recommends \
	gcc-riscv64-unknown-elf libc6-dev-riscv64-cross gcc-riscv64-linux-gnu \
	cmake bash build-essential git ca-certificates texinfo qemu-user qemu-user-binfmt

git clone https://github.com/riscvarchive/riscv-newlib.git /tmp/riscv-newlib
pushd /tmp/riscv-newlib/
	mkdir _build
	pushd _build
		../configure --disable-multilib --target=riscv64-unknown-elf --prefix=/usr/lib/ CFLAGS_FOR_TARGET='-march=rv64g'
		make -j$(nproc)
		make install
	popd
popd

rm -rf /tmp/riscv-newlib
