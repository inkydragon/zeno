file(GLOB TEST_SOURCE *.cpp)
add_executable(zeno_tests ${TEST_SOURCE})
target_link_libraries(zeno_tests PRIVATE zeno)

# copy dll for test
if (WIN32)
    add_custom_command(TARGET zeno_tests POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_SOURCE_DIR}/zenqt/lib/zeno.dll"
            $<TARGET_FILE_DIR:zeno_tests>)
endif()

enable_testing()
add_test(zeno_tests zeno_tests)
