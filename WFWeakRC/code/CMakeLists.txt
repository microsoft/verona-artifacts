cmake_minimum_required(VERSION 3.8)
project(rc-tests C CXX)

add_library(rc_lib INTERFACE)

include(FetchContent)

set(SNMALLOC_HEADER_ONLY_LIBRARY ON)

FetchContent_Declare(
  snmalloc
  GIT_REPOSITORY https://github.com/microsoft/snmalloc
  GIT_TAG        4e88b426219163c1fc1626bc9733e1f423cc741e
)

FetchContent_MakeAvailable(snmalloc)

target_include_directories(rc_lib INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(rc_lib INTERFACE snmalloc)

if(NOT MSVC)
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_link_libraries(rc_lib INTERFACE atomic)
  endif()
endif()

if(NOT MSVC)
  find_package(Threads REQUIRED COMPONENTS rc_lib)
  target_link_libraries(rc_lib INTERFACE ${CMAKE_THREAD_LIBS_INIT})
endif()

# ASAN must be added first, or it gets upset.
add_library(rc_test_lib INTERFACE)
target_link_libraries(rc_test_lib INTERFACE rc_lib)
if (NOT MSVC)
  target_compile_options(rc_test_lib INTERFACE -g -fsanitize=${SANITIZER} -fno-omit-frame-pointer)
  target_link_libraries(rc_test_lib INTERFACE -fsanitize=${SANITIZER})
endif()

add_library(rc_perf_lib INTERFACE)
target_link_libraries(rc_perf_lib INTERFACE rc_lib)
target_compile_definitions(rc_perf_lib INTERFACE REPEATS=60)
target_compile_definitions(rc_perf_lib INTERFACE CORES=72)

set(CMAKE_CXX_STANDARD 17)

add_library(rc_tests_wf test.cc)
target_link_libraries(rc_tests_wf rc_test_lib)

add_library(rc_tests_classic test.cc)
target_link_libraries(rc_tests_classic rc_test_lib)
target_compile_definitions(rc_tests_classic PRIVATE CLASSIC)

add_library(rc_tests_opt test.cc)
target_link_libraries(rc_tests_opt rc_test_lib)
target_compile_definitions(rc_tests_opt PRIVATE OPTIMISED)


add_library(rc_perf_wf test.cc)
target_link_libraries(rc_perf_wf rc_perf_lib)

add_library(rc_perf_classic test.cc)
target_link_libraries(rc_perf_classic rc_perf_lib)
target_compile_definitions(rc_perf_classic PRIVATE CLASSIC)

add_library(rc_perf_opt test.cc)
target_link_libraries(rc_perf_opt rc_perf_lib)
target_compile_definitions(rc_perf_opt PRIVATE OPTIMISED)

add_executable(rc_tests main.cc)
target_link_libraries(rc_tests rc_tests_wf rc_tests_classic rc_tests_opt)

add_executable(rc_perf main.cc ${snmalloc_SOURCE_DIR}/src/snmalloc/override/new.cc)
target_link_libraries(rc_perf rc_perf_wf rc_perf_classic rc_perf_opt)

enable_testing()
add_test(NAME rc_tests COMMAND rc_tests)

