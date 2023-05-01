#!/bin/bash

apt update && \
	DEBIAN_FRONTEND=noninteractive apt install -y --no-install-recommends \
	gcc-riscv64-unknown-elf cmake bash build-essential git ca-certificates texinfo

git clone https://sourceware.org/git/newlib-cygwin.git /tmp/newlib-cygwin
pushd /tmp/newlib-cygwin/
	mkdir _build
	pushd _build
		../configure --disable-multilib --target=riscv64-unknown-elf --prefix=/opt/riscv/newlib
		make -j$(nproc)
		make install
	popd
popd

rm -rf /tmp/newlib-cygwin
