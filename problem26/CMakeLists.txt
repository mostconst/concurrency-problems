cmake_minimum_required(VERSION 3.14)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

project(queue VERSION ${PROJECT_VESRION})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

option(WITH_GTEST "Whether to build Google test" OFF)

set(EXEC_NAME ${CMAKE_PROJECT_NAME})
set(LIB_NAME "${CMAKE_PROJECT_NAME}_lib")
set(TEST_NAME "${CMAKE_PROJECT_NAME}_test")

add_executable(${EXEC_NAME} main.cpp)
add_library(${LIB_NAME} "queue.cpp")

target_link_libraries(${EXEC_NAME} PRIVATE
    ${LIB_NAME}
    Threads::Threads
)

if(WITH_GTEST)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(${TEST_NAME} "test_queue.cpp")

    target_link_libraries(${TEST_NAME}
        GTest::gtest_main
        ${LIB_NAME}
    )
endif()

if (MSVC)
    target_compile_options(${EXEC_NAME} PRIVATE
        /W4
    )
    target_compile_options(${LIB_NAME} PRIVATE
        /W4
    )
    if(WITH_GTEST)
        target_compile_options(${TEST_NAME} PRIVATE
            /W4
        )
    endif()
else ()
    target_compile_options(${EXEC_NAME} PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(${LIB_NAME} PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    if(WITH_GTEST)
        target_compile_options(${TEST_NAME} PRIVATE
            -Wall -Wextra -pedantic -Werror
        )
    endif()
endif()

function(invalid_arg_test arg)
  add_test(NAME InvalidArg_${arg} COMMAND ${EXEC_NAME} ${arg})
  set_tests_properties(InvalidArg_${arg}
    PROPERTIES PASS_REGULAR_EXPRESSION "Usage:"
    )
  set_tests_properties(InvalidArg_${arg}
    PROPERTIES WILL_FAIL TRUE
    )
endfunction()

if(WITH_GTEST)
    enable_testing()
    include(GoogleTest)
    gtest_discover_tests(${TEST_NAME})
endif()
