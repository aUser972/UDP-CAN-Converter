cmake_minimum_required(VERSION 3.1)
project(ConvExample)
set(CMAKE_C_STANDARD 99)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3 -fno-stack-protector")

file(GLOB SOURCES "main.c")

add_executable(${PROJECT_NAME} ${SOURCES})
find_package(Threads REQUIRED)

include_directories("../Src")
target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libconverter.a)
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
