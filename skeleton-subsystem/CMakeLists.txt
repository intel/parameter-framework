# has been tested on 2.8 only - might work on older versions
cmake_minimum_required(VERSION 2.8)

project(parameter-framework-plugins-skeleton)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra")

#
# Find PFW libraries and include directories
#
find_path(PFW_CORE_ROOT_DIR NAMES include/parameter/plugin/Subsystem.h)

find_library(PFW_CORE_LIBRARY NAMES parameter
    HINTS ${PFW_CORE_ROOT_DIR}/lib)

find_path(PFW_CORE_INCLUDE_DIR NAMES Subsystem.h
    HINTS ${PFW_CORE_ROOT_DIR}/include/parameter/plugin)
find_path(PFW_XMLSERIALIZER_INCLUDE_DIR NAMES XmlSink.h
    HINTS ${PFW_CORE_ROOT_DIR}/include/xmlserializer)

set(PFW_INCLUDE_DIRS ${PFW_CORE_INCLUDE_DIR} ${PFW_XMLSERIALIZER_INCLUDE_DIR})
set(PFW_LIBRARIES ${PFW_CORE_LIBRARY})

add_library(skeleton-subsystem SHARED
SkeletonSubsystemBuilder.cpp
SkeletonSubsystem.cpp
SkeletonSubsystemObject.cpp)

include_directories(${PFW_INCLUDE_DIRS})

install(TARGETS skeleton-subsystem LIBRARY DESTINATION lib)
