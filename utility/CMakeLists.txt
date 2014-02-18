add_library(pfw_utility STATIC
    Tokenizer.cpp
    Utility.cpp
    NaiveTokenizer.cpp)

# '-fPIC' needed for linking against shared libraries (e.g. libparameter)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
