cmake_minimum_required(VERSION 3.22)

project(cf_file)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(WORKING_DIRECTORY ${CMAKE_BUILD_DIR})

# set(GTEST_ROOT_DIR /usr/src/googletest/googletest)

if(NOT GTEST_ROOT_DIR)
  # googltest-11.1.0
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/e2239ee6043f73722e7aa812a459f54a28552929.zip
  )
  FetchContent_MakeAvailable(googletest)
else()
  add_subdirectory(${GTEST_ROOT_DIR} googletest EXCLUDE_FROM_ALL)
endif()


enable_testing()

add_executable(file_test FileImpl.cpp FileImplTests.cpp)
target_compile_definitions(file_test PRIVATE CPP_TEST_FLAG)
target_link_libraries(file_test gtest_main)

include(GoogleTest)

gtest_discover_tests(file_test WORKING_DIRECTORY ${CMAKE_BUILD_DIR})
# message(STATUS "source dir: ${CMAKE_SOURCE_DIR}")
