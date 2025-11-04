cmake_minimum_required(VERSION 3.10)
   project(MyProject)
   
   file(GLOB SOURCES "*.cpp")
   add_executable(HTTP_Server ${SOURCES})