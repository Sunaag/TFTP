/*
Steps:

1. Create the socket.
2. Bind the socket to the particular port.
3. Receive
4. Close
 */

#include "common.h"
#include "struct.h"

#define SERVER_BUFFER		25
#define SERVER_IP			"127.0.0.1"
#define SERVER_PORT			28000
#define SERVER_QUEUE_SIZE	20

int main()
{
	wr_packet rrq_packet, wrq_packet;
	d_packet data_packet;
	a_packet ack_packet;
	e_packet error_packet;

	//
	data_packet.opcode = 3;
	data_packet.block = 0;

	//
	error_packet.opcode = 5;
	error_packet.block = 0;
	error_packet.byte = 0;

	//
	ack_packet.opcode = 4;
	ack_packet.block = 0;

	int sockfd, fd, fs;
	int cli_len, count;
	short int opcode;
	struct sockaddr_in serv_addr, cli_addr;
	char server_buffer[25], buff[512];

	/* Create the TCP socket */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Socket");
		exit(1);
	}
	memset(&serv_addr, 0, sizeof(serv_addr));
	memset(&cli_addr, 0, sizeof(cli_addr));

	/* Bind */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serv_addr.sin_port = htons(SERVER_PORT);

	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
	{
		perror("Bind");
		exit(2);
	}
	while(1)
	{
		/* Prompt the user */
		printf("\nServer is waiting\n");

		/* Receive the data */
		cli_len = sizeof(struct sockaddr_in);
		recvfrom(sockfd, &rrq_packet, sizeof(rrq_packet), 0, (struct sockaddr *)&cli_addr, &cli_len);
		if(rrq_packet.opcode == 1)
		{
			/* Print the message */
			printf("%s\n", rrq_packet.file_name);

			fd = open(rrq_packet.file_name, O_RDONLY);


			if(fd == -1)
			{
				opcode = 5;
			}
			else
			{

				opcode = 3;	
			}
			sendto(sockfd, &opcode,sizeof(opcode), 0,(struct sockaddr *)&cli_addr, sizeof(cli_addr) );

			if(opcode == 3)
			{

				while ((count = read(fd, data_packet.data, 512)) > 0)
				{
					//	strcpy(data_packet.data, buff);		
					//	printf("data: %s\n", data_packet.data);
					data_packet.block++;
					sendto(sockfd, &data_packet.data,count, 0,(struct sockaddr *)&cli_addr, sizeof(cli_addr) );

					recvfrom(sockfd, &ack_packet, sizeof(ack_packet), 0, (struct sockaddr *)&cli_addr, &cli_len);

					printf("\nack received with block: %d\n", ack_packet.block);
				}


			}
			if(opcode == 5)
			{
				error_packet.block++;
				strcpy(error_packet.error_msg, "file not found");
				sendto(sockfd, &error_packet,sizeof(error_packet), 0,(struct sockaddr *)&cli_addr, sizeof(cli_addr) );
			}
		}

		if(rrq_packet.opcode == 2)
		{
			/* Print the message */
			printf("%s\n", rrq_packet.file_name);

			fd = open(rrq_packet.file_name,	O_EXCL | O_CREAT | O_WRONLY, 0644);


			if(fd == -1)
			{
				if(errno == EEXIST)
				{
					opcode = 5;
				}
			}
			else
			{

				opcode = 3;	
			}
			sendto(sockfd, &opcode,sizeof(opcode), 0,(struct sockaddr *)&cli_addr, sizeof(cli_addr) );
			if(opcode == 5)
			{
				error_packet.block++;
				strcpy(error_packet.error_msg, "file already exist");
				sendto(sockfd, &error_packet,sizeof(error_packet), 0,(struct sockaddr *)&cli_addr, sizeof(cli_addr) );

			}

			if(opcode == 3)
			{
			while(1)
			{
		int new = recvfrom(sockfd, &data_packet.data, sizeof(data_packet.data), 0, (struct sockaddr *)&cli_addr, &cli_len);
		printf("new: %d\n", new);
				ack_packet.block++;
				sendto(sockfd, &ack_packet,sizeof(ack_packet), 0,(struct sockaddr *)&cli_addr, sizeof(cli_addr) );
			printf("data: %s\n", data_packet.data);	
				if(write(fd, data_packet.data, new) < 512)
				{
					break;
				}

				memset(data_packet.data, 0, 512);
			}
			}


		}

	}
	/* Close the fds */
	close(sockfd);
}


