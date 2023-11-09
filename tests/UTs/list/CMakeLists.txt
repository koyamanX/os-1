add_executable(list main.c ${CMAKE_SOURCE_DIR}/kernel/lib/list.c)
target_include_directories(list PUBLIC
    ${CMAKE_SOURCE_DIR}/tests/UTs/test_framework
    ${CMAKE_SOURCE_DIR}/include
)
target_link_directories(list PUBLIC 
    ${CMAKE_SOURCE_DIR}/tests/UTs/test_framework
    #${CMAKE_SOURCE_DIR}/kernel/libs
)
target_link_libraries(list PUBLIC
    test_framework
    lib
)
add_test(NAME list COMMAND list)
install(TARGETS list DESTINATION ${CMAKE_INSTALL_PREFIX}/tests/UTs/list)