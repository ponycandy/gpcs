
cmake_minimum_required (VERSION 3.8)


project ("gpcskernel")
find_package(Boost REQUIRED COMPONENTS serialization thread system chrono)
add_library(gpcsserver STATIC  
    src/gpcsserver.cpp
     src/session.cpp
)
target_link_libraries (gpcsserver ${Boost_THREAD_LIBRARY_DEBUG} ${Boost_CHRONO_LIBRARY_DEBUG})  

include_directories(${Boost_INCLUDE_DIRS}) 

target_include_directories(gpcsserver
   PUBLIC 
      ${PROJECT_SOURCE_DIR}/include
)

set(EXECUTABLE_SOURCES
    src/gpcskernel.cpp
)







add_executable (gpcskernel ${EXECUTABLE_SOURCES} )
target_link_libraries(gpcskernel  gpcsserver ${Boost_THREAD_LIBRARY_DEBUG}  ${Boost_SYSTEM_LIBRARY_DEBUG} ${Boost_SERIALIZATION_LIBRARY_DEBUG})

