
add_library(stdio STATIC
	fprintf.c
	stdio.c)

target_include_directories(stdio PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../include
	# TODO:
	${PROJECT_SOURCE_DIR}/include)
