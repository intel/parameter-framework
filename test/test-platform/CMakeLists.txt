add_executable(test-platform # WIN32 ?
    main.cpp
    TestPlatform.cpp)

# FIXME: Supress the need for the -Wno-unused-parameter
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")

include_directories(
    "${PROJECT_SOURCE_DIR}/parameter/include"
    "${PROJECT_SOURCE_DIR}/remote-processor"
    "${PROJECT_SOURCE_DIR}/utility")

target_link_libraries(test-platform parameter remote-processor)

install(TARGETS test-platform RUNTIME DESTINATION bin)
