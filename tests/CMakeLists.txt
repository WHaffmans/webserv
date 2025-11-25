# Test executable
add_executable(webserv_tests
    test_main.cpp
)

# Link with Google Test
if(TARGET gtest_main)
    target_link_libraries(webserv_tests
        gtest_main
        webserv_lib
    )
else()
    target_link_libraries(webserv_tests
        ${GTEST_LIBRARIES}
        ${GTEST_MAIN_LIBRARIES}
        webserv_lib
    )
    target_include_directories(webserv_tests PRIVATE ${GTEST_INCLUDE_DIRS})
endif()

target_include_directories(webserv_tests PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/webserv
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(webserv_tests)
