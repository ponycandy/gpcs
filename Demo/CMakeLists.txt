# CMakeList.txt: gpcskernel 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)


project ("Demo")

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost REQUIRED COMPONENTS filesystem thread serialization)
include_directories(${Boost_INCLUDE_DIRS}) 

add_executable (Demo  main.cpp)

target_include_directories(Demo  PUBLIC  gpcsnode/include)


target_include_directories(Demo ${Boost_LIBRARIES} gpcsnode  boost_thread boost_serialization)





