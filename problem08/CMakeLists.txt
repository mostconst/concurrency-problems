cmake_minimum_required(VERSION 3.14)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

project(leibnitz_pi VERSION ${PROJECT_VESRION})

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

option(WITH_GTEST "Whether to build Google test" ON)

add_executable(calc_pi main.cpp)
add_library(leibnitz_pi leibnitz_pi.cpp)

set_target_properties(calc_pi leibnitz_pi PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(calc_pi PRIVATE
    leibnitz_pi
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

    add_executable(test_leibnitz_pi test_leibnitz_pi.cpp)

    set_target_properties(test_leibnitz_pi PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
    )

    target_link_libraries(test_leibnitz_pi
        GTest::gtest_main
        leibnitz_pi
    )
endif()

if (MSVC)
    target_compile_options(calc_pi PRIVATE
        /W4
    )
    target_compile_options(leibnitz_pi PRIVATE
        /W4
    )
    if(WITH_GTEST)
        target_compile_options(test_leibnitz_pi PRIVATE
            /W4
        )
    endif()
else ()
    target_compile_options(calc_pi PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(leibnitz_pi PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    if(WITH_GTEST)
        target_compile_options(test_leibnitz_pi PRIVATE
            -Wall -Wextra -pedantic -Werror
        )
    endif()
endif()

function(invalid_arg_test arg)
  add_test(NAME InvalidArg_${arg} COMMAND calc_pi ${arg})
  set_tests_properties(InvalidArg_${arg}
    PROPERTIES PASS_REGULAR_EXPRESSION "Usage:"
    )
endfunction()

if(WITH_GTEST)
    enable_testing()
    include(GoogleTest)
    gtest_discover_tests(test_leibnitz_pi)

    add_test(NAME SimplePositive COMMAND calc_pi 4)
    set_tests_properties(SimplePositive
      PROPERTIES PASS_REGULAR_EXPRESSION "3.14159265"
      )

    add_test(NAME InvalidArg_empty COMMAND calc_pi)
    set_tests_properties(InvalidArg_empty
      PROPERTIES PASS_REGULAR_EXPRESSION "Usage:"
      )

    add_test(NAME InvalidArg_extra_args COMMAND calc_pi 2 4)
    set_tests_properties(InvalidArg_extra_args
    PROPERTIES PASS_REGULAR_EXPRESSION "Usage:"
    )

    invalid_arg_test(0)
    invalid_arg_test(-1)
    invalid_arg_test("two")
    invalid_arg_test("2a")

    add_test(NAME Usage1 COMMAND calc_pi)
    set_tests_properties(Usage1
    PROPERTIES PASS_REGULAR_EXPRESSION "Usage:"
    )

endif()
