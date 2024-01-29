# CMakeList.txt: gpcskernel 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)


project ("gpcskernel")

add_library(gpcsserver STATIC 
    src/gpcsserver.cpp
     src/session.cpp
)

target_include_directories(gpcsserver
   PUBLIC 
      ${PROJECT_SOURCE_DIR}/include
)

set(EXECUTABLE_SOURCES
    src/gpcskernel.cpp
)




include_directories(/home/nvidia/boost_1_83_0) 


link_directories(/home/nvidia/boost_1_83_0/stage/lib)
add_executable (gpcskernel ${EXECUTABLE_SOURCES})
target_link_libraries(gpcskernel  gpcsserver boost_system boost_system boost_filesystem boost_thread pthread)

