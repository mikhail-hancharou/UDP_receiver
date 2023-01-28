// UPD_receiver.cpp : Defines the entry point for the application.
//

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
#include <string.h>
#include <time.h>


bool SendEcho(int sock, char* IP, int PORT)
{
	struct sockaddr_in sendAddr;
	sendAddr.sin_family = AF_INET;
	sendAddr.sin_port = htons(PORT);
	sendAddr.sin_addr.s_addr = inet_addr(IP);
	puts("~Print message or \"q\" to return to main menu: ");
	char buf[1024];
	scanf("%s", buf);
	if (buf[0] == 'q')
	{
		return false;
	}

	sendto(sock, buf, sizeof(buf), 0, (struct sockaddr*)&sendAddr, sizeof(sendAddr));
	return true;
}

void RecvEcho(int sock, sqlite3* db, sqlite3_stmt* stmt)
{
	struct sockaddr_in recvAddr;
	int size = sizeof(recvAddr);
	char buf[1024];
	int bytes_read = recvfrom(sock, buf, 1024, 0, (struct sockaddr*)&recvAddr, &size);
	time_t t = time(NULL);
	if (bytes_read < 0)
	{
		printf("Failed to get data. Error %d", errno);
	}
	else
	{
		buf[bytes_read] = '\0';
		printf("Received message from [%s]: %s\n",inet_ntoa(recvAddr.sin_addr), buf);
		char sql[128];
		char tm[20];
		strftime(tm, sizeof(tm), "%F %T", gmtime(&t));
		sprintf(sql, "%s'%s', %d, '%s');", SQL_INSERT_BASE, inet_ntoa(recvAddr.sin_addr), bytes_read, tm);
		char* err_msg;
		int term_res = sqlite3_exec(db, sql, 0, 0, &err_msg);
		if (term_res != SQLITE_OK)
		{
			printf("SQL error: %s\n", err_msg);
			sqlite3_free(err_msg);
		}
	}
	fflush(stdout);
}


int main()
{
	sqlite3* db;
	sqlite3_stmt* stmt;
	int term_res = sqlite3_open("test.db", &db);
	if (term_res != SQLITE_OK)//":memory:"
	{
		printf("Cannot open db connection: %s\n", sqlite3_errmsg(db));
		return 1;
	}
	//./home/adduser/.vs/UDP_receiver/out/build/linux-debug/test.db
	//char* sql = "CREATE TABLE Packets(Id INTEGER PRIMARY KEY, Ip TEXT, Size INT, TimeIn DATETIME, TimeWrite DATETIME DEFAULT CURRENT_TIMESTAMP);";// InTime TIMESTAMP, WriteTime TIMESTAMP
	//char* sql = "INSERT INTO Packets(Ip) VALUES ('12345678');";
	char* err_msg;
	//term_res = sqlite3_exec(db, sql, 0, 0, &err_msg);
	//if (term_res != SQLITE_OK)//":memory:"
	//{
	//	printf("SQL error: %s\n", err_msg);
	//	sqlite3_free(err_msg);
	//	sqlite3_close(db);
	//	return 1;
	//}

	regex_t reg;
	int compRes = regcomp(&reg, IP_REGEX, REG_EXTENDED);
	char IP[16];
	int PORT;
	while (1)
	{
		printf("Type IP or q to exit: ");
		scanf("%s", IP);
		//fgets(IP, sizeof(IP), stdin);
		int regRes = regexec(&reg, IP, 0, NULL, 0);
		if (regRes == 0)
		{
			printf("IP confirmed [%s]\n", IP);
		}
		else //if (regRes == REG_NOMATCH)
		{
			if (IP[0] == 'q')
			{
				break;
			}
			puts("Incorrect IP. Try again");
			continue;
		}

		char port[6];
		printf("Type PORT: ");
		scanf(" %d", &PORT);
		if (PORT >= 1 && PORT <= 65535)
		{
			printf("Port confirmed [%d]\n", PORT);
		}
		else
		{
			printf("Incorrect PORT. Try again\n");
			continue;
		}

		int sock;
		struct sockaddr_in addr_in;
		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (sock < 0)
		{
			printf("Failed to create socket. Error %d", errno);
			fflush(stdout);
			return 1;
		}

		char buf[1024];
		int bytes_read;

		addr_in.sin_family = AF_INET;
		addr_in.sin_port = htons(PORT);
		addr_in.sin_addr.s_addr = inet_addr(IP);//htons(INADDR_ANY)
		if (bind(sock, (struct sockaddr*)&addr_in, sizeof(addr_in)) < 0)
		{
			printf("Failed to bind socket. Error %d", errno);
			fflush(stdout);
			return 1;
		}

		while (1)
		{
			if (!SendEcho(sock, IP, PORT))
			{
				break;
			}
			RecvEcho(sock, db, stmt);
		}
	}

	sqlite3_close(db);

	/*struct timeval tv;
	tv.tv_sec = 10;
	tv.tv_usec = 0;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	RecvEcho(sock);*/

	return 0;
}