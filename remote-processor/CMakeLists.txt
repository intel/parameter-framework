add_library(remote-processor SHARED
        Socket.cpp
        ListeningSocket.cpp
        ConnectionSocket.cpp
        Message.cpp
        RequestMessage.cpp
        AnswerMessage.cpp
        RemoteProcessorServer.cpp
        RemoteProcessorServerBuilder.cpp)

include(FindThreads)
# TODO: use the "prefer pthread" option; check if we are using pthread (what to
# do else ?)
target_link_libraries(remote-processor ${CMAKE_THREAD_LIBS_INIT})
