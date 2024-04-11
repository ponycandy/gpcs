# CMakeList.txt: gpcskernel 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)


project ("gpcsnode")

add_library(gpcsnode  STATIC
    src/session.cpp
    src/gpcsnode.cpp
)



target_include_directories(gpcsnode
   PUBLIC 
   ${Boost_INCLUDE_DIRS}
      ${PROJECT_SOURCE_DIR}/include
)
link_directories(${Boost_LIBRARY_DIRS})

# target_link_libraries(gpcsnode  boost_thread)
# message(${Boost_THREAD_LIBRARY_DEBUG})
target_link_libraries(gpcsnode  ${Boost_THREAD_LIBRARY_DEBUG}
    ${Boost_CHRONO_LIBRARY_DEBUG} ${Boost_SERIALIZATION_LIBRARY_DEBUG})

