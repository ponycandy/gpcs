# CMakeList.txt: gpcskernel 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)


project ("gpcsnode")

add_library(gpcsnode STATIC 
    src/session.cpp
          src/session.cpp
src/gpcsnode.cpp
)




target_include_directories(gpcsnode
   PUBLIC 
      ${PROJECT_SOURCE_DIR}/include
)




include_directories(/home/nvidia/boost_1_83_0) 
link_directories(/home/nvidia/boost_1_83_0/stage/lib)

target_link_libraries(gpcsnode  boost_thread)
