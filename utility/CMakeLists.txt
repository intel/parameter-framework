add_library(pfw_utility STATIC
    Tokenizer.cpp
    Utility.cpp
    NaiveTokenizer.cpp)

# Needed for linking libpfw_utility against shared libraries e.g. libparameter)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
