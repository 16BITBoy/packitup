CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(piutests C)

SET (CMAKE_BUILD_TYPE Debug)

ADD_EXECUTABLE(piutests main.c fdhandler/fdhandler.c errors/errors.c piu/piu.c)


