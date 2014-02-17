add_executable(test-platform # WIN32 ?
    main.cpp
    TestPlatform.cpp)

# FIXME
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")

include_directories("${PROJECT_SOURCE_DIR}/parameter/include")
include_directories("${PROJECT_SOURCE_DIR}/remote-processor")
include_directories("${PROJECT_SOURCE_DIR}/utility")

target_link_libraries(test-platform parameter remote-processor)

install(TARGETS test-platform RUNTIME DESTINATION bin)
