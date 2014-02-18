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
# TODO: check for the "XInclude" feature in libxml2 (how to ?)
include_directories(${LIBXML2_INCLUDE_DIR})
target_link_libraries(xmlserializer ${LIBXML2_LIBRARIES})

install(TARGETS xmlserializer LIBRARY DESTINATION lib)
