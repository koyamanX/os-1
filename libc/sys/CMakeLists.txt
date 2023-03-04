
add_library(sys STATIC
	open.S
	mkdir.S
	mknod.S
	dup.S
	write.S)

target_include_directories(sys PUBLIC
	${PROJECT_SOURCE_DIR}/libc/include
	# TODO:
	${PROJECT_SOURCE_DIR}/include)
