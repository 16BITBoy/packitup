/*
 * main.c
 *
 *  Created on: 23/08/2010
 *      Author: adrian
 */

#include <stdlib.h>
#include <stdio.h>
#include "fdhandler.h"
int main()
{
	DATA *data;

	data = (DATA *) load_chk_file("/home/adrian/archivo1", 0, 1);

	if(data == NULL)
	{
		printf("Something went wrong while reading from file.");
		exit(1);
	}
	printf("The content of the file is:\n%s\n", (char *)data->data);
	printf("Data size is: %d bytes\n", data->size);
	printf("Saving data to another file...");
	if(save_chk_file("/home/adrian/archivo2", data, 3) != data->size)
	{
		printf("Something went wrong while writting to file.");
		exit(1);
	}

	printf("All went fine.\n");
	return 0;
}
