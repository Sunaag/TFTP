#ifndef STRUCT_H
#define STRUCT_H
#include "common.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//commands
char *commands[] = {"connect",
					"get",
					"put",
					"mode",
					"bye"
					};
//read and write
 typedef struct packet1
{
	short int opcode;
	char file_name[20];
	char byte1;
	char mode[20];
	char byte2;

}wr_packet;

//data packet
typedef struct packet2
{
	short int opcode;
	short int block;
	char data[512];
}d_packet;

//ack pocket
typedef struct packet3
{
	short int opcode;
	short int block;
}a_packet;

//error packet
typedef struct packet4
{
	short int opcode;
	short int block;
	char error_msg[20];
	int byte;
}e_packet;

#endif
