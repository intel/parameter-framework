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
else(NOT LIBXML2_FOUND)
    # libxml2 has been found, but does it support XInclude ?
    include(CheckLibraryExists)
    check_library_exists(xml2 xmlXIncludeProcess "" XML2_XINCLUDE_SUPPORT)
    if(NOT XML2_XINCLUDE_SUPPORT)
        message(SEND_ERROR "
    libxml2 has not been built with support for XInclude.  xmlserializer needs
    that feature; please install a version of libxml2 supporting it.")
    endif(NOT XML2_XINCLUDE_SUPPORT)
endif(NOT LIBXML2_FOUND)

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
