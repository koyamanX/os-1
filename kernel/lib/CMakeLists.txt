
add_library(lib STATIC
	dirname.c
	basename.c
	memcpy.c
	memmove.c
	strlen.c
	memset.c
	strncmp.c
	strcmp.c
	strtok.c
	strcpy.c
	)

target_include_directories(lib PUBLIC
	${PROJECT_SOURCE_DIR}/include)
