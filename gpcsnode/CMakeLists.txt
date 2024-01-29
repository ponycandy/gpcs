# CMakeList.txt: gpcskernel 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)


project ("gpcsnode")

add_library(session STATIC 
    src/session.cpp
          src/session.cpp
)

add_library(gpcsnode STATIC 
    src/gpcsnode.cpp
)

target_include_directories(session
   PUBLIC 
      ${PROJECT_SOURCE_DIR}/include
)

target_include_directories(gpcsnode
   PUBLIC 
      ${PROJECT_SOURCE_DIR}/include
)




set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost REQUIRED COMPONENTS filesystem thread)

include_directories(${Boost_INCLUDE_DIRS}) 

target_link_libraries(session ${Boost_LIBRARIES} boost_thread)

target_link_libraries(gpcsnode ${Boost_LIBRARIES} session boost_thread)
