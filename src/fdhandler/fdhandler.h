////////////////////////////////////////////////////////////
//
// FDHandler - A simple filesystem handler for load and save data
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
 * fdhandler.h
 *
 *  Created on: 23/08/2010
 *      Author: adrian
 */

#ifndef FDHANDLER_H_
#define FDHANDLER_H_

#define FILE_MUST_EXIST 0
#define CREATE_FILE     1

typedef struct
{
	void *data;
	int size;
}DATA;

DATA *load_file(char * path);
DATA *load_chk_file(int fd, unsigned long offset, unsigned long size);
int save_file(char * path, DATA *data);
int append_to_file(int fd, DATA *data);
int file_open(char *path, int create);
#endif /* FDHANDLER_H_ */
