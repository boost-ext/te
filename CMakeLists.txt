#
# Copyright (c) 2018-2019 Kris Jusiak (kris at jusiak dot net)
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
cmake_minimum_required(VERSION 3.10)
project(Boost.TE CXX)

option(ENABLE_MEMCHECK "Run the unit tests and examples under valgrind if it is found." OFF)
option(ENABLE_COVERAGE "Run coverage." OFF)
option(ENABLE_SANITIZERS "Run static analysis." OFF)

add_custom_target(style)
add_custom_command(TARGET style COMMAND find ${CMAKE_CURRENT_LIST_DIR}/example
  ${CMAKE_CURRENT_LIST_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/test -iname
  "*.hpp" -or -iname "*.cpp" | xargs clang-format -i)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Werror -pedantic -pedantic-errors)

if (ENABLE_COVERAGE)
  add_compile_options(--coverage -g -O0)
endif()

if (ENABLE_SANITIZERS)
  add_compile_options(-O0 -g -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak -fsanitize=undefined)
endif()

enable_testing()

include_directories(include)

find_program(MEMORYCHECK_COMMAND valgrind)
if (ENABLE_MEMCHECK AND MEMORYCHECK_COMMAND)
  function(test name)
    string(REPLACE "/" "_" out ${name})
    add_executable(${out} ${CMAKE_CURRENT_LIST_DIR}/${name}.cpp)
    add_custom_command(TARGET ${out} COMMAND ${MEMORYCHECK_COMMAND} --leak-check=full --error-exitcode=1 ./${out})
  endfunction()
else()
  function(test name)
    string(REPLACE "/" "_" out ${name})
    add_executable(${out} ${CMAKE_CURRENT_LIST_DIR}/${name}.cpp)
    add_custom_command(TARGET ${out} COMMAND ./${out})
  endfunction()
endif()

add_subdirectory(example)
add_subdirectory(test)
