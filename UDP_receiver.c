// UPD_receiver.cpp : Defines the entry point for the application.
//
//arpa / inet.h contains the declaration of char* inet_ntoa(struct in_addr in).
//If you don't include this header your compiler will use implicit declaration int inet_ntoa(). 
//Wrong declaration can easily lead to segfault, especially if you are on system where sizeof(int)!=sizeof(void*).
//If you are using gcc you can add - Wall flag.gcc will warn you about using functions without explicit declaration.

#include "UDP_receiver.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "errno.h"
#include "sqlite3.h"
#include <netinet/in.h>
#include <arpa/inet.h> //
#include <regex.h>
#include <stdbool.h>

bool SendEcho(int sock)
{
	struct sockaddr_in sendAddr;
	sendAddr.sin_family = AF_INET;
	sendAddr.sin_port = htons(8000);
	sendAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	puts("Print message or \"q\" to quit: ");
	char buf[1024];
	scanf("%s", buf);
	//fgets(buf, sizeof(buf), stdin);

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
		printf("[%s]: %s\n",inet_ntoa(recvAddr.sin_addr), buf);
	}
	fflush(stdout);
}


int main()
{
	regex_t reg;
	int compRes = regcomp(&reg, "^((25[0-5]|(2[0-4]|1[0-9]|[1-9]|)[0-9])\.){3}((25[0-5]|(2[0-4]|1[0-9]|[1-9]|)[0-9]))$", REG_EXTENDED);
	char IP[16];
	while (1)
	{
		printf("Type IP: ");
		scanf("%s", IP);
		//fgets(IP, sizeof(IP), stdin);
		int regRes = regexec(&reg, IP, 0, NULL, 0);
		if (regRes == 0)
		{
			puts("IP confirmed");
		}
		else //if (regRes == REG_NOMATCH)
		{
			puts("Incorrect IP. Try again");
			continue;
		}
		/*else
		{
			puts("Unexpected error");
		}*/

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
		addr_in.sin_addr.s_addr = inet_addr(IP);//htons(INADDR_ANY)
		if (bind(sock, (struct sockaddr*)&addr_in, sizeof(addr_in)) < 0)
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
	}

	/*struct timeval tv;
	tv.tv_sec = 10;
	tv.tv_usec = 0;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	RecvEcho(sock);*/

	return 0;
}