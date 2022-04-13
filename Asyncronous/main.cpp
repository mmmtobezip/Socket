// Asyncronous Server with MySQL extension
// log table (timestamp, delta, id)
// message format "id delta")

#define DB_HOST "127.0.0.1"
#define DB_USER "root"
#define DB_PASS "jhzzz1086"
#define DB_NAME "socket"
#define TcpPort 5000
#define Nsock 10
#include <iostream>
#include <stdlib.h>
#include <WinSock2.h>
#include <stdio.h>
#include <mysql.h>
#include <errno.h>

#include <ctime> 

using namespace std;

// Shared variable for accumalated add from multiple client
int val;
CRITICAL_SECTION cs;

// Socket Variables 
SOCKET sockfd, newfd[Nsock];
HANDLE hThread[Nsock], AccThread;
DWORD	dwThreadId[Nsock], tmp;
int idx;
struct sockaddr_in	cli_addr, my_addr;
int clilen, servlen;

// MySQL Variables
MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;
	
// Thread for individual connection
// Taking the data access request add to the shared variable
// also store the request to the log database table
void ConnProc(DWORD *ThreadArg) {
	SOCKET *sd = (SOCKET *) ThreadArg;
	char buff[1024];
	int n, delta;
	char cmd[50], id[20];
	time_t timer;
	struct tm *ptr;
		
	while (1)  {
		cout << "Waiting for a message \n";
		memset(buff, 0, sizeof(buff));
		n = recv(*sd, buff, sizeof(buff), 0);
		if (n == SOCKET_ERROR) {
			cout << "Receive fail\n";
			break;
		}
		cout << "Received : " << buff << "\n";
		sscanf(buff, "%s%d", id, &delta);
		
// Critical Section
		EnterCriticalSection(&cs);
		val += delta;
		LeaveCriticalSection(&cs);
		
// return message
		sprintf(buff, "%d ", val);
		send(*sd, buff, strlen(buff), 0);
			
// Database log update
		timer = time(NULL);
    	ptr = localtime(&timer); 

		sprintf(cmd, "insert into log values ('%d-%d-%d %d:%d:%d', '%d', '%s');",
		ptr->tm_year+1900, ptr->tm_mon+1, ptr->tm_mday, 
		ptr->tm_hour, ptr->tm_min, ptr->tm_sec, delta, id);
		cout << cmd <<"\n";
		if(mysql_query(conn, cmd)) {
			cout << "Database Update error\n";
		}
	}
}


// Thread handing connection requests
// Waiting for the connection request, Create a new thread exchaning data with an individual client
// Passing a new socket id
void AcceptProc() {
	while (1) {
		cout << "Waiting for connection request in Accept thread\n";
		clilen = sizeof(cli_addr);
		newfd[idx] = accept(sockfd, (struct sockaddr FAR *) &cli_addr, &clilen);
		if (newfd[idx] == SOCKET_ERROR) {
			cout << "Accept error\n";
			exit(0);
		}
		else {
			hThread[idx] = CreateThread(0, 1000,
				(LPTHREAD_START_ROUTINE) ConnProc, (DWORD *) &newfd[idx], 
				0, &dwThreadId[idx]);
			SetThreadPriority(hThread[idx], THREAD_PRIORITY_NORMAL);
			idx++;
		}
	}
}

// Main Thread 
int main(int argc, char *argv[]) {
	WSADATA	gszWSAData;
	int	i, j, n;
	
// MySQL connection establish
	conn = mysql_init(NULL);
	if(!mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0))  {
		 printf("%s\n", mysql_error(conn));
		 return -1;
	 }
	cout << "Connection OK\n";


// Initialize Criticalsection for shared variable access
	InitializeCriticalSection(&cs);
	
	if (WSAStartup(0x0101, &gszWSAData)!=0)
		exit(0);

// Basic socket call
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == INVALID_SOCKET) {
		cout << "Server socket error\n";
		exit(0);
	}
	cout << "Socket call succeed" << sockfd <<"\n";

// Bind to port 5000 receving from any client
	memset((char *) &my_addr, 0x00, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_addr.sin_port = htons(TcpPort);

	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(my_addr)) == SOCKET_ERROR) {
		cout << "Bound  error\n";
		closesocket(sockfd);
		exit(0);
	}

// Create a thread taking connection request
	listen(sockfd, 5);
	AccThread = CreateThread(0, 1000, (LPTHREAD_START_ROUTINE) AcceptProc, 0, 0, &tmp);

	for (i=0; i<idx; i++)
		WaitForSingleObject(hThread[i], -1);
	WaitForSingleObject(AccThread, -1);
	
	mysql_close(conn);
}
