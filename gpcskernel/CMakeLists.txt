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




set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost REQUIRED COMPONENTS filesystem thread)

include_directories(${Boost_INCLUDE_DIRS}) 
add_executable (gpcskernel ${EXECUTABLE_SOURCES})
target_link_libraries(gpcskernel ${Boost_LIBRARIES} gpcsserver boost_thread)
