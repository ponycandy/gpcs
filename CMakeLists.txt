project(GpcsMiddleWare)


if(WIN32)
    set(BOOST_ROOT c:/Boost)
    set(Boost_LIBRARY_DIRS c:/Boost/lib)
else()
    set(Boost_INCLUDE_DIRS /home/nvidia/boost_1_83_0)
    set(Boost_LIBRARY_DIRS /home/nvidia/boost_1_83_0/stage/lib)
endif()

add_subdirectory(gpcsnode)
add_subdirectory(gpcskernel)

add_subdirectory(Demo)
add_compile_options(-fPIC)

