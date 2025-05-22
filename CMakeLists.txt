cmake_minimum_required(VERSION 3.10)
project(untitled1)

set(CMAKE_CXX_STANDARD 17)

set(SDL2_PATH "C:/libs/SDL2")

include_directories(${SDL2_PATH}/include)
link_directories(${SDL2_PATH}/lib)

add_executable(untitled1 main.cpp)

target_link_libraries(untitled1 SDL2main SDL2)

add_custom_command(TARGET untitled1 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_PATH}/lib/SDL2.dll"
        $<TARGET_FILE_DIR:untitled1>)
