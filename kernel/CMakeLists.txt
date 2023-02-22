set(KERNEL_SOURCE
	bio.c
	fs.c
	main.c
	panic.c
	printk.c
	proc.c
	sched.c
	timer.c
	trap.c
	uart.c
	virtio.c
	vm.c
	init.S
	start.S
	swtch.S)

set(KERNEL_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/kernel.ldS)
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -fno-omit-frame-pointer -mno-relax -mcmodel=medany -fno-stack-protector -fno-pie -no-pie")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -fno-omit-frame-pointer -mno-relax -mcmodel=medany -fno-stack-protector -fno-pie -no-pie")

add_executable(kernel ${KERNEL_SOURCE})

target_include_directories(kernel
	PUBLIC ${PROJECT_SOURCE_DIR}/lib/libc/include
	PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(kernel libc)

target_link_options(kernel PRIVATE LINKER:-T ${KERNEL_LINKER_SCRIPT} -nostdlib -nodefaultlibs -fno-pie -no-pie -z max-page-size=4096)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kernel DESTINATION ${CMAKE_INSTALL_PREFIX})
