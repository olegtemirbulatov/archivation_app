#cmake_minimum_required(VERSION 2.6)
project(archivation_app)
set(SOURCES src/readandwrite.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})
#include_directories("headers/" "more_headers/")
