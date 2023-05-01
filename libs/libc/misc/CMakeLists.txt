
add_library(misc STATIC
	dirname.c
	basename.c)

target_include_directories(misc PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../include
	# TODO:
	${PROJECT_SOURCE_DIR}/include)
