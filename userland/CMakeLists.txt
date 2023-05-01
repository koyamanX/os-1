set(USER_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/user.ldS")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -T ${USER_LINKER_SCRIPT}")

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
	PUBLIC ${NEWLIB}/include
	PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(hello ${NEWLIB_LIBS})
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/hello" DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/sbin/")

add_executable(cat cat.c crt0.S)
target_include_directories(cat
	PUBLIC ${NEWLIB}/include
	PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(cat ${NEWLIB_LIBS})
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/cat" DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/sbin/")

add_executable(ls ls.c crt0.S)
target_include_directories(ls
	PUBLIC ${NEWLIB}/include
	PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(ls ${NEWLIB_LIBS})
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ls" DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/sbin/")
