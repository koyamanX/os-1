
add_library(sys STATIC
	write.S
	open.S
	mkdir.S
	mknod.S
	dup.S
	read.S
	fork.S
	exec.S
	_exit.S
	close.S
	link.S
	truncate.S
	stat.S
	fstat.S
	_sbrk.S
	sbrk.c
	brk.S
	waitpid.S
	)

target_include_directories(sys PUBLIC
	${PROJECT_SOURCE_DIR}/libc/include
	# TODO:
	${PROJECT_SOURCE_DIR}/include)
