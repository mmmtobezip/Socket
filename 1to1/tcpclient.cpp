// TCP client Example
// 1:1 Communication

#include <stdio.h>
#include <winsock.h>
#include <iostream>
#define TcpPort 5000
#define SERV_HOST_ADDR "127.0.0.1"

using namespace std;

int main(int argc, char *argv[]) {
	char buff[100];
	SOCKET	sockfd;
	struct 	sockaddr_in serv_addr;
	int		i=0, flag;
	WSADATA  wsaData;
                 
    WSAStartup(0x0101, &wsaData);

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR);
	serv_addr.sin_port = htons(TcpPort);

    
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf(" Socket error\n");
		shutdown(sockfd, 2);
		exit(0);
	 }
    cout << "Socket call success\n";

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		cout << " Connect error\n";
		shutdown(sockfd, 2);
		exit(0);
	 }
	cout << "Connection pass\n";

	while (1) {
		memset(buff, 0, sizeof(buff));
		cout << "String to send : ";
		gets(buff); 
		flag = 0;
		send(sockfd, buff, strlen(buff), flag);
		if (! strncmp(buff, "End", 3))
			break;
		recv(sockfd, buff, 100, 0);
		cout << "Recv : "<< buff << "\n";
	}
	closesocket(sockfd);
 }

