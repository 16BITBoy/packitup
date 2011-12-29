CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(packitup C)

SET (CMAKE_BUILD_TYPE Debug)

ADD_LIBRARY(piu STATIC fdhandler/fdhandler.c errors/errors.c piu/piu.c)
ADD_LIBRARY(piushared SHARED fdhandler/fdhandler.c errors/errors.c piu/piu.c)
ADD_EXECUTABLE(piuadd fdhandler/fdhandler.c errors/errors.c piu/piu.c frontend/add.c)
ADD_EXECUTABLE(piudel fdhandler/fdhandler.c errors/errors.c piu/piu.c frontend/delete.c)  
ADD_EXECUTABLE(piuview fdhandler/fdhandler.c errors/errors.c piu/piu.c frontend/view.c)
ADD_EXECUTABLE(devtest EXCLUDE_FROM_ALL fdhandler/fdhandler.c errors/errors.c piu/piu.c main.c)
ADD_EXECUTABLE(piuextract fdhandler/fdhandler.c errors/errors.c piu/piu.c frontend/extract.c)
ADD_EXECUTABLE(check fdhandler/fdhandler.c errors/errors.c piu/piu.c tests/check_piu.c)
