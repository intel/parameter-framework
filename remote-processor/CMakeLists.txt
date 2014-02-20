add_library(remote-processor SHARED
        Socket.cpp
        ListeningSocket.cpp
        ConnectionSocket.cpp
        Message.cpp
        RequestMessage.cpp
        AnswerMessage.cpp
        RemoteProcessorServer.cpp
        RemoteProcessorServerBuilder.cpp)

set(CMAKE_THREAD_PREFER_PTHREAD 1)
include(FindThreads)
if(NOT CMAKE_USE_PTHREADS_INIT)
    message(SEND_ERROR "
    pthread library not found! Please install the POSIX thread library and
    headers.")
endif(NOT CMAKE_USE_PTHREADS_INIT)
target_link_libraries(remote-processor ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS remote-processor LIBRARY DESTINATION lib)
