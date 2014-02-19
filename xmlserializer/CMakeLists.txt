add_library(xmlserializer SHARED
    XmlElement.cpp
    XmlSerializingContext.cpp
    XmlDocSource.cpp
    XmlDocSink.cpp
    XmlMemoryDocSink.cpp
    XmlMemoryDocSource.cpp
    XmlStringDocSink.cpp
    XmlFileDocSink.cpp
    XmlFileDocSource.cpp
    XmlStringDocSource.cpp)

include(FindLibXml2)
if(NOT LIBXML2_FOUND)
    message(SEND_ERROR "
    libxml2 NOT found.  The parameter-framework wont compile.
    Please install the development package (e.g. libxml2-dev on debian-based
    Linux distributions).")
endif(NOT LIBXML2_FOUND)
# TODO: check for the "XInclude" feature in libxml2 (how to ?)

include_directories(${LIBXML2_INCLUDE_DIR})
target_link_libraries(xmlserializer ${LIBXML2_LIBRARIES})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBXML2_DEFINITIONS}")

install(TARGETS xmlserializer LIBRARY DESTINATION lib)
install(FILES
    XmlSink.h
    XmlSource.h
    XmlElement.h
    XmlSerializingContext.h
    DESTINATION "include/xmlserializer")
