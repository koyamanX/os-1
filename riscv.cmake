set(TOOLCHAIN_PREFIX "riscv64-linux-gnu-")
#set(TOOLCHAIN_SUFFIX "-12")
set(CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}gcc${TOOLCHAIN_SUFFIX}")
set(CMAKE_ASM_COMPILER "${TOOLCHAIN_PREFIX}gcc${TOOLCHAIN_SUFFIX}")
set(CMAKE_LINKER "${TOOLCHAIN_PREFIX}gcc${TOOLCHAIN_SUFFIX}")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -Werror -O0 -ggdb -g3 -gdwarf-2 -march=rv64g -ffreestanding -fno-common -nostdlib \
						-fno-omit-frame-pointer -mno-relax -mcmodel=medany -fno-stack-protector -fno-pie -no-pie")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -Wall -Werror -O0 -ggdb -g3 -gdwarf-2 -march=rv64g -ffreestanding -fno-common -nostdlib \
						-fno-omit-frame-pointer -mno-relax -mcmodel=medany -fno-stack-protector -fno-pie -no-pie")
