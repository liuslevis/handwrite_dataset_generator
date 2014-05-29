cmake_minimum_required (VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /FindOpenCV.cmake)
set(CMAKE_CXX_FLAGS "-mmacosx-version-min=10.7")

find_package(OpenCV)
message(STATUS "OpenCV_FOUND: " ${OpenCV_FOUND})

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(main main.cpp)

target_link_libraries(main ${OpenCV_LIBS})
