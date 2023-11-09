#!/bin/bash

set -e

BUILD_DIR=_build
MOUNT_DIR=./mnt
INSTALL_ROOT=$BUILD_DIR/root
IMAGE_NAME=rootfs.img
IMAGE_SIZE=512M
KERNEL=$INSTALL_ROOT/kernel
QEMU=qemu-system-riscv64
QEMU_OPTS="-D qemu.log -d in_asm -machine virt -bios none -m 128M -nographic -kernel $KERNEL"
QEMU_OPTS="$QEMU_OPTS -global virtio-mmio.force-legacy=false"
QEMU_OPTS="$QEMU_OPTS -drive file=$IMAGE_NAME,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0"
QEMU_GDB_PORT=tcp::12345
CLEAN_LISTS=*.log
declare -A LOG_LEVELS=( [verbose]=0 [debug]=1 [info]=2 [warn]=3 [release]=15 )
LOG_LEVEL=${LOG_LEVELS["release"]}
TOOLCHAIN_FILE=$(pwd)/riscv.cmake
DOCKER_IMAGE_VERSION=v1.3

usage() {
	cat <<EOF
NAME `basename $0`
USAGE
	`basename $0` [COMMANDS]... [OPTIONS]...
DESCRIPTION
	Build & run automate tool
COMMANDS 
	build
		verbose
		debug
		info
		warn
		release
	build_in_docker
		verbose
		debug
		info
		warn
		release
	create_image
	run
		verbose
		debug
		info
		warn
		release
	test
	format
	docs
	gdb
	clean
	install_deps
EOF
	exit 0
}

build() {
	if [ "$1" = "verbose" ] || [ "$1" = "debug" ] || [ "$1" = "info" ] || [ "$1" = "warn" ] || [ "$1" = "release" ]; then
		LOG_LEVEL=${LOG_LEVELS[$1]}
		shift
	fi
	[ ! -d "$BUILD_DIR" ] && mkdir $BUILD_DIR
	pushd $BUILD_DIR
	[ ! -f "CMakeCache.txt" ] && cmake .. -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE -DLOG_LEVEL=$LOG_LEVEL
	make install -j$(nproc)
	popd
}

clean() {
	[ -d "$BUILD_DIR" ] && rm -rf $BUILD_DIR
	[ -d "$MOUNT_DIR" ] && rm -rf $MOUNT_DIR
	[ -f "$IMAGE_NAME" ] && rm -rf $IMAGE_NAME
	rm -rf $CLEAN_LISTS
}

create_image() {
	[ -d "$BUILD_DIR" ] && build
	[ -f "$IMAGE_NAME" ] && rm -rf $IMAGE_NAME
	fallocate -l $IMAGE_SIZE $IMAGE_NAME
	mkfs.minix -3 $IMAGE_NAME
	[ ! -d "$MOUNT_DIR" ] && mkdir $MOUNT_DIR
	sudo mount $IMAGE_NAME $MOUNT_DIR
	cp kernel/main.c $INSTALL_ROOT/hello.txt
	cp -r $INSTALL_ROOT/* $MOUNT_DIR -r
	riscv64-unknown-elf-strip $MOUNT_DIR/usr/sbin/*
	sync
	sudo umount $MOUNT_DIR
	[ -d "$MOUNT_DIR" ] && rm -rf $MOUNT_DIR
}

install_deps() {
	./install_deps.sh
}

run() {
	clear
	$QEMU $QEMU_OPTS $@
}

test() {
	pushd $BUILD_DIR
	make test
	popd
}

CMD=$1
[ -n "$CMD" ] || usage
if [ "$CMD" = "help" ]; then
	shift
	usage
fi

CMD=$1
if [ "$CMD" == "clean" ]; then
	shift
	clean
	exit 0
fi

CMD=$1
if [ "$CMD" = "build" ]; then
	shift
	build $1
	exit 0
fi

CMD=$1
if [ "$CMD" = "test" ]; then
	shift
	if [ ! -d "$BUILD_DIR" ] ; then
		build $1
	fi
	test
	exit 0
fi

CMD=$1
if [ "$CMD" = "build_in_docker" ]; then
	shift
	docker run --rm -v$(pwd):/work -w /work koyamanx/os1_dev:$DOCKER_IMAGE_VERSION ./run.sh build $1
	exit 0
fi


CMD=$1
if [ "$CMD" = "create_image" ]; then
	shift
	build
	[ -f "$IMAGE_NAME" ] && rm -rf $IMAGE_NAME
	create_image
	exit 0
fi

CMD=$1
if [ "$CMD" = "run" ]; then
	shift
	if [ ! -d "$BUILD_DIR" ] ; then
		build $1
	fi
	[ ! -d "$IMAGE_NAME" ] && create_image
	run
	exit 0
fi

CMD=$1
if [ "$CMD" = "gdb" ]; then
	shift
	[ ! -d "$BUILD_DIR" ] && build
	[ ! -d "$IMAGE_NAME" ] && create_image
	run -S -gdb $QEMU_GDB_PORT
	exit 0
fi

CMD=$1
if [ "$CMD" = "format" ]; then
	shift
	find . -name '*.c' -exec clang-format -i {} \;
	find . -name '*.h' -exec clang-format -i {} \;
	exit 0
fi

CMD=$1
if [ "$CMD" = "docs" ]; then
	shift
	doxygen
	exit 0
fi

CMD=$1
if [ "$CMD" = "install_deps" ]; then
	shift
	install_deps
	exit 0
fi

usage
