set(USER_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/user.ldS")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -T ${USER_LINKER_SCRIPT} -static -nostdlib -nodefaultlibs -fno-pie -no-pie -z max-page-size=4096")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -fno-pie -no-pie")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -fno-pie -no-pie")

add_executable(init init.c crt0.S)
target_include_directories(init
	PUBLIC ${PROJECT_SOURCE_DIR}/libc/include
	PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(init libc)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/init" DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/sbin/")

add_executable(sh sh.c crt0.S)
target_include_directories(sh
	PUBLIC ${PROJECT_SOURCE_DIR}/libc/include
	PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(sh libc)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/sh" DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/sbin/")

add_executable(hello hello.c crt0.S)
target_include_directories(hello
	PUBLIC ${PROJECT_SOURCE_DIR}/libc/include
	PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(hello libc)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/hello" DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/sbin/")
