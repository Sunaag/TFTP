/*
Steps:

1. Create the socket.
2. Bind the socket to the particular port(OPTIONAL).
3. Send
4. Close
 */

#include "common.h"
#include "struct.h"
#define SERVER_BUFFER		25
#define SERVER_IP			"127.0.0.1"
#define SERVER_PORT			29000
#define SERVER_QUEUE_SIZE	20


//driver
int main()
{
	//packet declarations
	wr_packet rrq_packet, wrq_packet;
	d_packet data_packet;
	a_packet ack_packet;
	e_packet error_packet;


	//assigning the values to packets
	//1.read paket
	rrq_packet.opcode = 1;
	rrq_packet.byte1 = 0;
	rrq_packet.byte2 = 0;
	char *mode1 = "net ascii";
	strcpy(rrq_packet.mode, mode1);

	//2.write packet
	wrq_packet.opcode = 2;
	wrq_packet.byte1 = 0;
	wrq_packet.byte2 = 0;
	strcpy(wrq_packet.mode, "net ascii");


	//3.data packet
	data_packet.opcode = 3;
	data_packet.block = 0;

	//4.ack packet
	ack_packet.opcode = 4;
	ack_packet.block = 0;

	//5.error packet
	error_packet.opcode = 5;
	error_packet.block = 0;
	error_packet.byte = 0;



	//initializing the variables
	int sockfd, status, count, fd;
	struct sockaddr_in serv_addr, cli_addr;
	char client_buffer[30], command[30], buff[512];
	int cli_len = sizeof(struct sockaddr_in);
	short int opcode;

	//prompt the user to enter the first comamnd and it must be connect
	printf("\n<tftp>");
	scanf("%[^:\n]s", command);
	while(1)
	{

		if(strncmp(command, commands[0], 7) == 0)
		{
			if(strcmp(command+8, SERVER_IP) == 0)
			{

				break;
			}
			else
			{
				printf("enter proper ip address\n");
			}
		}
		else
		{
			printf("first establish the connection\n");
		}

		printf("\n<tftp>");
		scanf("\n%[^:\n]s", command);
	}

	/* Create the TCP socket */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Socket");
		exit(1);
	}
	memset(&serv_addr, 0, sizeof(serv_addr));
	/* Bind : OPTIONAL */

	/* Populate the server details */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serv_addr.sin_port = htons(SERVER_PORT);
	while(1)
	{

		ack_packet.block = 0;
		/* Read the info from the user */
		printf("\n<tftp>");
		scanf("\n%[^\n]s", client_buffer);

		if(strncmp(client_buffer, commands[1], 3) == 0)
		{
			strcpy(rrq_packet.file_name, client_buffer + 4);
			printf("\nfile name : %s\n", rrq_packet.file_name);
			printf("mode : %s\n", rrq_packet.mode);


			/* Receive the data */
			if (status = sendto(sockfd, &rrq_packet, sizeof(rrq_packet), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)))
			{
				printf("Message sent SUCCESSFULLY\n");
			}
			else
			{
				printf("Message FAILED to send\n");
			}

			recvfrom(sockfd, &opcode, sizeof(opcode), 0, (struct sockaddr *)&serv_addr, &cli_len );


			if(opcode == 3)
			{
				//create and write into that file
				fd = open(client_buffer+4, O_CREAT|O_WRONLY, 0644);

				if(fd == -1)
				{
					perror("open");
					exit(1);
				}


				while(1)
				{

					int new =	recvfrom(sockfd, &data_packet.data, sizeof(data_packet.data), 0, (struct sockaddr *)&serv_addr, &cli_len );

					printf("data received with block: %d\n", ++data_packet.block);
					ack_packet.block++;
					if (status = sendto(sockfd, &ack_packet, sizeof(ack_packet), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)))
					{
						printf("acknowledgment sent SUCCESSFULLY\n");
					}
					else
					{
						printf("acknowledgement FAILED to send\n");
					}

					if( write(fd, data_packet.data, new) < 512)
					{
						break;
					}
					memset(data_packet.data,0, 512);
				}
			}
			else if(opcode == 5)
			{

				recvfrom(sockfd, &error_packet, sizeof(error_packet), 0, (struct sockaddr *)&serv_addr, &cli_len );
				printf("%s\n", error_packet.error_msg);
			}
		}
		else if(strncmp(client_buffer, commands[2], 3) == 0)
		{
			strcpy(wrq_packet.file_name, client_buffer + 4);
			printf("\nfile name : %s\n", wrq_packet.file_name);
			printf("mode : %s\n", wrq_packet.mode);

			if (status = sendto(sockfd, &wrq_packet, sizeof(wrq_packet), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)))
			{
				printf("Message sent SUCCESSFULLY\n");
			}
			else
			{
				printf("Message FAILED to send\n");
			}

			recvfrom(sockfd, &opcode, sizeof(opcode), 0, (struct sockaddr *)&serv_addr, &cli_len );
			if(opcode == 5)
			{

				recvfrom(sockfd, &error_packet, sizeof(error_packet), 0, (struct sockaddr *)&serv_addr, &cli_len );
				printf("%s\n", error_packet.error_msg);
			}

			if(opcode == 3)
			{
				fd = open(client_buffer+4, O_RDONLY);

				if(fd == -1)
				{
					perror("open");
					exit(1);
				}

				while((count = read(fd, data_packet.data, 512))  > 0)
				{
					if (status = sendto(sockfd, &data_packet.data, count, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)))
					{
						printf("\ndata sent SUCCESSFULLY\n");
					}
					else
					{
						printf("Message FAILED to send\n");
					}

					recvfrom(sockfd, &ack_packet, sizeof(ack_packet), 0, (struct sockaddr *)&serv_addr, &cli_len );

					printf("acknowledgement %d received\n", ack_packet.block);
				}
			}

		}


		else if(strncmp(client_buffer, commands[4], 3) == 0)
		{
			return 0;
		}
		else
		{
			printf("please enter proper command\n");
		}
	}
	/* Close the fds */
	close(sockfd);
}


