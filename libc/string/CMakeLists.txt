
add_library(string STATIC
	memset.c
	memcpy.c
	strlen.c
	strcmp.c
	strncmp.c
	strtok.c
	strcpy.c)

target_include_directories(string PUBLIC
	${PROJECT_SOURCE_DIR}/libc/include
	# TODO:
	${PROJECT_SOURCE_DIR}/include)
