cmake_minimum_required(VERSION 3.1)
project(converter)

set(SOURCE_FILES "converter.c")
set(HEADER_FILES "converter.h")
set(THREADS_PREFER_PTHREAD_FLAG ON)


add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})
