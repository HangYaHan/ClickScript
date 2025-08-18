cmake_minimum_required(VERSION 3.10.0)
project(ISC VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
file(GLOB_RECURSE src "src/*.cpp")

include_directories(${PROJECT_SOURCE_DIR}/include)

find_package(Threads REQUIRED)

add_executable(ISC ${src})

target_link_libraries(ISC 
    Threads::Threads 
    user32 
    gdi32
)