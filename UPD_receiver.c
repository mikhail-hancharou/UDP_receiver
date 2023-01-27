// UPD_receiver.cpp : Defines the entry point for the application.
//
//arpa / inet.h contains the declaration of char* inet_ntoa(struct in_addr in).
//If you don't include this header your compiler will use implicit declaration int inet_ntoa(). 
//Wrong declaration can easily lead to segfault, especially if you are on system where sizeof(int)!=sizeof(void*).
//If you are using gcc you can add - Wall flag.gcc will warn you about using functions without explicit declaration.

#include "UPD_receiver.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "errno.h"
#include "sqlite3.h"
#include <netinet/in.h>
#include <arpa/inet.h> //
#include <stdbool.h>

bool SendEcho(int sock)
{
	struct sockaddr_in sendAddr;
	sendAddr.sin_family = AF_INET;
	sendAddr.sin_port = htons(8000);
	sendAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	puts("Print message or \"q\" to quit: ");
	char buf[1024];
	fgets(buf, sizeof(buf), stdin);

	if (buf[0] == 'q')
	{
		return false;
	}

	sendto(sock, buf, sizeof(buf), 0, (struct sockaddr*)&sendAddr, sizeof(sendAddr));
	return true;
}

void RecvEcho(int sock)
{
	struct sockaddr_in recvAddr;
	char buf[1024];
	int bytes_read;
	if ( (bytes_read = recvfrom(sock, buf, 1024, 0, (struct sockaddr*)&recvAddr, &recvAddr)) < 0)
	{
		printf("Failed to get data. Error %d", errno);
	}
	else
	{
		buf[bytes_read] = '\0';
		printf("[%s]: %s",inet_ntoa(recvAddr.sin_addr), buf);
	}
	fflush(stdout);
}


int main()
{
	int sock;
	struct sockaddr_in addr_in;
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);// 0
	if (sock < 0)
	{
		printf("Failed to create socket. Error %d", errno);
		fflush(stdout);
		return 1;
	}

	char buf[1024];
	int bytes_read;

	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(8000);
	addr_in.sin_addr.s_addr = htons(INADDR_ANY);
	if (bind(sock, (struct sockaddr *)&addr_in, sizeof(addr_in)) < 0)
	{
		printf("Failed to bind socket. Error %d", errno);
		fflush(stdout);
		return 1;
	}

	while (1)
	{
		if (!SendEcho(sock))
		{
			break;
		}
		RecvEcho(sock);
	}

	/*while (1)
	{
		bytes_read = recvfrom(sock, buf, 1024, 0, null, null);
		if (bytes_read < 0)
		{
			printf("failed to read data. error %d", errno);
		}
		else
		{
			buf[bytes_read] = '\0';
			printf(buf);
		}
		fflush(stdout);
	}*/


	return 0;
}