# has been tested on 2.8 only - might work on older versions
cmake_minimum_required(VERSION 2.8)

# linking policy (see cmake --help-policy CMP0003)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

project(parameter-framework)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra") # -Wno-unused-parameter

add_subdirectory(xmlserializer)
add_subdirectory(parameter)
add_subdirectory(utility)
add_subdirectory(remote-processor)

add_subdirectory(remote-process)
add_subdirectory(test/test-platform)
