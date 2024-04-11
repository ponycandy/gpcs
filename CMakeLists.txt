project(GpcsMiddleWare)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(GPCS_NODE_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/gpcsnode/include)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

if(WIN32)
    set(Boost_INCLUDE_DIRS C:/Boost/include/boost-1_83)
    set(Boost_LIBRARY_DIRS c:/Boost/lib)
    Message("confirm win32 \n")
else()
    set(BOOST_ROOT /home/nvidia/boost_1_83_0/install)
    set(Boost_INCLUDE_DIRS ${BOOST_ROOT}/include)
    set(Boost_LIBRARY_DIRS ${BOOST_ROOT}/lib)
    Message("confirm LINUX \n")
endif()
find_package(Boost REQUIRED COMPONENTS filesystem thread serialization)

    Message(${Boost_THREAD_LIBRARY_DEBUG})

add_subdirectory(gpcsnode)
add_subdirectory(gpcskernel)

add_subdirectory(Demo)

add_compile_options(-fPIC)
