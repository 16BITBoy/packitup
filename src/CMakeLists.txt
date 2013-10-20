cmake_minimum_required(VERSION 2.6)

project(src CXX)
set(CMAKE_BUILD_TYPE Debug)
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost 1.49.0 COMPONENTS system filesystem)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    if(QTCREATOR)
        include_directories(SYSTEM "/usr/include/c++/4.6/")
        message("Regenerating project for QtCreator")
    endif()
     

    ADD_LIBRARY(piu fdhandler/fdhandler.cpp piu/piu.cpp)

    ADD_EXECUTABLE(piuview cli-frontend/view.cpp)
    ADD_EXECUTABLE(devtest EXCLUDE_FROM_ALL devtests.cpp)

    ADD_DEPENDENCIES(devtest piu)
    ADD_DEPENDENCIES(piuview piu)

    
    target_link_libraries(devtest ${Boost_LIBRARIES} piu)
    target_link_libraries(piuview ${Boost_LIBRARIES} piu)
    target_link_libraries(piu ${Boost_LIBRARIES})
endif()
    
