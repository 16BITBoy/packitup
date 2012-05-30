cmake_minimum_required(VERSION 2.6)

project(packitup CXX)
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
    
    #ADD_LIBRARY(piu STATIC fdhandler/fdhandler.c errors/errors.c piu/piu.c)
    #ADD_LIBRARY(piushared SHARED fdhandler/fdhandler.c errors/errors.c piu/piu.c)
    #ADD_EXECUTABLE(piuadd fdhandler/fdhandler.c errors/errors.c piu/piu.c frontend/add.c)
    #ADD_EXECUTABLE(piudel fdhandler/fdhandler.c errors/errors.c piu/piu.c frontend/delete.c)  
    #ADD_EXECUTABLE(piuview fdhandler/fdhandler.c errors/errors.c piu/piu.c frontend/view.c)
    ADD_EXECUTABLE(devtest EXCLUDE_FROM_ALL fdhandler/fdhandler.cpp piu/piu.cpp devtests.cpp)
    #ADD_EXECUTABLE(piuextract fdhandler/fdhandler.c errors/errors.c piu/piu.c frontend/extract.c)
    #ADD_EXECUTABLE(check fdhandler/fdhandler.c errors/errors.c piu/piu.c tests/check_piu.c)
    
    target_link_libraries(devtest ${Boost_LIBRARIES})
endif()
    
