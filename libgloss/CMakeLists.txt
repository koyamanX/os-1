add_library(gloss STATIC
	crt0.S
	nanosleep.c
	syscalls.c
	times.c
	brk.S
	close.S
	crt0.S
	dup.S
	dup.c
	execv.S
	execv.c
	execve.c
	_exit.S
	fork.S
	fstat.S
	link.S
	mkdir.S
	mknod.S
	mknod.c
	open.S
	read.S
	_sbrk.S
	stat.S
	truncate.S
	waitpid.S
	waitpid.c
	wait.c
	write.S
)

target_include_directories(gloss PUBLIC
	${NEWLIB_INCLUDES}
	${PROJECT_SOURCE_DIR}/include
)
