    ////////////////////////////////////////////////////////////
//
// FDHandler - A simple file data handler
// Copyright (C) 2011 Adrián Pérez Heredia (aperezhrd@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

/*
 * fdhandler.c
 *
 *  Created on: 23/08/2010
 *      Author: adrian
 */

#include "fdhandler.h"
#ifdef __WINDOWS__
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#endif

/* Loads the entire file into memory and closes it */
DATA *loadfile(char *path)
{
#ifdef __WINDOWS__
	//Win32API
#else
	int fd;
	int size;
	int bytesread;
	DATA *data;
	void *dataptr = NULL;

	fd = open(path, O_RDONLY);

    if(fd < 0)
        return NULL;

	size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	dataptr = malloc(size);
	bytesread = read(fd, dataptr, size);
	if(bytesread < 0)
		return NULL;

	close(fd);

    data = (DATA *) malloc(sizeof(DATA));
	data->data = dataptr;
	data->size = size;
	return data;
#endif
}

/* Opens a file descriptor with read and write mode.
 * you can close it by using the syscall close()
*/
int fileopen(char *path, int create)
{
    int fd;
    if(create){
        fd = open(path, O_RDWR | O_CREAT);
        chmod(path, S_IRUSR | S_IWUSR | S_IRGRP);
        return fd;
    }

    return open(path, O_RDWR);
}

/* Loads a chunk of data from a file. 
 * Loads 'size' bytes from 'offset'
*/
DATA *loadchkfile(int fd, unsigned long offset, unsigned long size)
{
	int bytesread;
	int filesize;
	DATA *data;
	void *dataptr = NULL;

    /* Extra checking, but good ;) */    
    if(fd < 0)
        return NULL;    

	filesize = lseek(fd, 0, SEEK_END);
	lseek(fd, offset, SEEK_SET);

	if(offset + size > filesize)
		return NULL;

	dataptr = malloc(size);
	bytesread = read(fd, dataptr, size);
    if(bytesread < 0)
		return NULL;
    
	data = (DATA *) malloc(sizeof(DATA));
	data->data = dataptr;
	data->size = size;
	return data;
}

/* Saves 'data' in the file specified by 'path'.
 * Overwrites all the previous data in 'path'.
*/
long savetofile(char *path, DATA *data)
{
#ifdef __WINDOWS__
	//Win32API
#else
	int fd;
	long byteswritten;

	fd = fileopen(path, FCREATE);
    if(fd < 0){
        printf("No se pudo crear un nuevo descriptor de fichero\n");
        printf("Error al abrir o crear un fichero en: %s\n", path);
        perror("file descriptor");
        return -1;
    }
	byteswritten = write(fd, data->data, data->size);
    if(byteswritten == -1){
        perror("Error al escribir el fichero a disco");
        printf("Se intentó copiar el fichero en la siguiente ubicación: %s\n", path);
    }
    close(fd);
	return byteswritten;
#endif
}
/* Saves 'data' in the file which corresponds to 
 * the file descriptor provided. 
*/
int savetofd(int fd, DATA *data)
{
#ifdef __WINDOWS__
	//Win32API
#else
	int byteswritten;

	byteswritten = write(fd, data->data, data->size);
	return byteswritten;
#endif
}
/* Appends 'data' to the end of the file.
*/
int appendtofd(int fd, DATA *data)
{
#ifdef __WINDOWS__
	//Win32API
#else
	int byteswritten;

	if(fd < 0){
		return -1;
	}

	lseek(fd, 0, SEEK_END);
	byteswritten = write(fd, data->data, data->size);
	return byteswritten;
#endif
}

void freedata(DATA *data){
    free(data->data);
    free(data);
}
