# CMakeList.txt: gpcskernel 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)


project ("Demo")

#set(Boost_USE_STATIC_LIBS ON) 
#set(Boost_USE_MULTITHREADED ON)  
#set(Boost_USE_STATIC_RUNTIME OFF)
#find_package(Boost REQUIRED COMPONENTS filesystem thread serialization)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

add_executable (Demo  main.cpp)
target_include_directories(Demo
   PUBLIC 
      ${GPCS_NODE_HEADER}
)

target_link_libraries(Demo PUBLIC gpcsnode ${Boost_THREAD_LIBRARY_DEBUG}
    ${Boost_CHRONO_LIBRARY_DEBUG} ${Boost_SERIALIZATION_LIBRARY_DEBUG}
    ${Boost_SYSTEM_LIBRARY_DEBUG} )







