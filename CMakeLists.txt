cmake_minimum_required(VERSION 3.10)
project(MyProject)

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