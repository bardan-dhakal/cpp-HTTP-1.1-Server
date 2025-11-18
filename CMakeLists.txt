cmake_minimum_required(VERSION 3.10)
project(HTTP_Server)


#Specifying C++ Standard
set(CMAKE_CXX_STANDARD 17)

#Threading Library
find_package(Threads REQUIRED)

# Create the executable target with your source files
add_executable(HTTP_Server 
    src/main.cpp
    src/server.cpp
    src/request_parser.cpp
    src/request_builder.cpp
    src/file_handler.cpp
    src/connection_handler.cpp
    src/util.cpp
)

# Tell the compiler where to find header files
target_include_directories(HTTP_Server PRIVATE include)

#Linking threading library
target_link_libraries(HTTP_Server PRIVATE Threads::Threads)

if (WIN32)
    target_link_libraries(HTTP_Server PRIVATE ws2_32)
endif()

if (UNIX)
    target_link_libraries(HTTP_Server PRIVATE pthread)
endif()

if(UNIX)
    target_compile_options(HTTP_Server PRIVATE
    -Wall
    -Wextra
    -O2
    -g
    )
endif()