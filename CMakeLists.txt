cmake_minimum_required(VERSION 3.16)
project(URIStreamExample LANGUAGES CXX)

find_package(TagLib 2.3.2 CONFIG REQUIRED)
find_package(CURL REQUIRED)

add_library(taglib_URIStream URIStream.cpp)
target_link_libraries(taglib_URIStream
    PUBLIC
        TagLib::TagLib
        CURL::libcurl
)

add_executable(example example.cpp)
target_link_libraries(example
    PRIVATE
        taglib_URIStream
        TagLib::TagLib
        CURL::libcurl
)

