cmake_minimum_required(VERSION 4.2)

project(ProjectsHttpServer)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED)

add_executable(ProjectsHttpServer src/main.cpp
        src/server/triehttprouter.cpp
        src/server/triehttprouter.h
        src/server/variantvalue.cpp
        src/server/variantvalue.h)


target_link_libraries(ProjectsHttpServer Boost::boost)
