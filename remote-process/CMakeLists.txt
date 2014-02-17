add_executable(remote-process main.cpp) # TODO: WIN32 ?

# TODO: separate remote-processor's includes in half (public/private)
# And use only public headers here
include_directories("${PROJECT_SOURCE_DIR}/remote-processor")
include_directories("${PROJECT_SOURCE_DIR}/utility")

target_link_libraries(remote-process remote-processor pfw_utility)

install(TARGETS remote-process RUNTIME DESTINATION bin)
