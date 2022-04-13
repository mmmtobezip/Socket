
/**
 *  Name : tcpcA.cpp
 *  Date : 2022-04-13
 *  Author : Jihyun Park(mmmszip@gmail.com)
 *
 *  TCP client Example
 *  1:1 Communication
 */


#include <stdio.h>
#include <winsock.h>
#include <iostream>
#define TcpPort 5000
#define SERV_HOST_ADDR "127.0.0.1"

using namespace std;

//Main program
//Usage : tcpcA id
int main(int argc, char *argv[]) {
	char buff[100];
	SOCKET	sockfd;
	struct 	sockaddr_in serv_addr;
	int		i=0, flag;
	WSADATA  wsaData;
                 
    WSAStartup(0x0101, &wsaData);

// Basic socket call for TCP connection
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf(" Socket error\n");
		shutdown(sockfd, 2);
		exit(0);
	 }
    cout << "Socket call success\n";
    
// Connect call to the server
   	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR);
	serv_addr.sin_port = htons(TcpPort);

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		cout << " Connect error\n";
		shutdown(sockfd, 2);
		exit(0);
	 }
	cout << "Connection pass\n";

// Main loop taking user input and sending to the server
	while (1) {
		memset(buff, 0, sizeof(buff));
		cout << "Number to send : ";
		int tt;
		cin >> tt;
		flag = 0;
		sprintf(buff, "%s %d", argv[1], tt);
		send(sockfd, buff, strlen(buff), flag);
		memset(buff, 0, sizeof(buff));
		recv(sockfd, buff, 100, 0);
		cout << "Recv : "<< buff << "\n";
	}
	closesocket(sockfd);
 }

