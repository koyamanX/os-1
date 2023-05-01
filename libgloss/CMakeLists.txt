add_library(gloss STATIC
	crt0.S
	nanosleep.c
	sbrk.c
	syscalls.c
	times.c
	brk.S
	close.S
	crt0.S
	dup.S
	execv.S
	_exit.S
	fork.S
	fstat.S
	link.S
	mkdir.S
	mknod.S
	open.S
	read.S
	_sbrk.S
	stat.S
	truncate.S
	waitpid.S
	write.S
)

target_include_directories(gloss PUBLIC
	${NEWLIB_INCLUDE}
	${PROJECT_SOURCE_DIR}/include
)
