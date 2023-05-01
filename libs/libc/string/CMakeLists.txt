
add_library(string STATIC
	memset.c
	memcpy.c
	strlen.c
	strcmp.c
	strncmp.c
	strtok.c
	strcpy.c
	memmove.c)

target_include_directories(string PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../include
	# TODO:
	${PROJECT_SOURCE_DIR}/include)
