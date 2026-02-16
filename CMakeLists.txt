cmake_minimum_required(VERSION 4.1)
project(Dijkstra)

set(CMAKE_CXX_STANDARD 26)

set(SDL3_DIR "D:/develop/SDL3/x86_64-w64-mingw32/lib/cmake/SDL3")

find_package(SDL3 REQUIRED)

include_directories(${SDL3_PATH}/include)
link_directories(${SDL3_PATH}/lib/x64)

add_executable(Dijkstra
	main.cpp
	Graph.cpp
	Renderer.cpp
	Dijkstra.cpp
)
target_link_libraries(Dijkstra PRIVATE SDL3::SDL3)