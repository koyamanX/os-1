
add_library(sys STATIC
	sys_write.c
	sys_execev.c
	sys_open.c
	sys_mkdir.c
	sys_mknod.c
	sys_dup.c
	sys_read.c
	sys_fork.c
	sys_exit.c
	sys_close.c
	sys_link.c
	sys_truncate.c
	sys_stat.c
	sys_fstat.c
	sys_sbrk.c
	sys_brk.c
	sys_waitpid.c
	)

target_include_directories(sys PUBLIC
	${PROJECT_SOURCE_DIR}/include
	${NEWLIB_INCLUDES})
