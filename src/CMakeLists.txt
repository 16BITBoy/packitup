cmake_minimum_required(VERSION 2.6)

project(src CXX)
set(CMAKE_BUILD_TYPE Debug)
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost 1.49.0 COMPONENTS system filesystem program_options)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS} piu fdhandler misc)
    if(QTCREATOR)
        include_directories(SYSTEM "/usr/include/c++/4.6/")
        message("Regenerating project for QtCreator")
    endif()
     

    ADD_LIBRARY(piu fdhandler/fdhandler.cpp piu/piu.cpp piu/piu_impl.cpp piu/operations.hpp piu/exceptions.hpp piu/fileformat.hpp fdhandler/fdhandler.hpp misc/pathutils.h)

    ADD_EXECUTABLE(piuview cli-frontend/view.cpp)
    ADD_EXECUTABLE(piuadd cli-frontend/add.cpp)
    ADD_EXECUTABLE(piuextract cli-frontend/extract.cpp)
    ADD_EXECUTABLE(devtest EXCLUDE_FROM_ALL devtests.cpp)

    ADD_DEPENDENCIES(devtest piu)
    ADD_DEPENDENCIES(piuview piu)
    ADD_DEPENDENCIES(piuadd piu)
    ADD_DEPENDENCIES(piuextract piu)

    
    target_link_libraries(piu ${Boost_LIBRARIES})
    target_link_libraries(devtest ${Boost_LIBRARIES} piu)
    target_link_libraries(piuview piu)
    target_link_libraries(piuadd piu)
    target_link_libraries(piuextract piu)


endif()
    
