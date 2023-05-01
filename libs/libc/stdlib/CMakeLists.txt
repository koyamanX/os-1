
add_library(stdlib STATIC
	exit.c)

target_include_directories(stdlib PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../include
	# TODO:
	${PROJECT_SOURCE_DIR}/include)
