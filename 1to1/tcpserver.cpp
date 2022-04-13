// TCP server Example
// 1:1 Communication

#define TcpPort 5000
#include <iostream>
#include <stdlib.h>
#include <WinSock2.h>

using namespace std;

int main(int argc, char *argv[]) {
	WSADATA	gszWSAData;
	SOCKET sockfd, newfd;
	int     clilen, servlen;
	int	i, j, n;
	struct sockaddr_in	cli_addr, my_addr;
	char buff[1024];

	if (WSAStartup(0x0101, &gszWSAData) !=0)
		exit(0);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == INVALID_SOCKET) {
		cout << "Client socket error\n";
		exit(0);
	 }
	cout << " Socket call succeed " << sockfd <<"\n";

	memset((char *) &my_addr, 0x00, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_addr.sin_port = htons(TcpPort);

	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(my_addr)) == SOCKET_ERROR) {
		closesocket(sockfd);
		exit(0);
	}

	listen(sockfd, 5);
	
	clilen = sizeof(cli_addr);

	newfd =  accept(sockfd, (struct sockaddr FAR *) &cli_addr, &clilen);
	if (newfd == SOCKET_ERROR) {
		cout << "Accept error\n";
		exit(0);
	}
	cout<< "Accepted\n";
    
	for(;;)  {
		int n;
		memset(buff, 0, sizeof(buff));
		n = recv(newfd, buff, 100, 0);
		if (n == SOCKET_ERROR) {
			cout << "Receive fail\n";
		}
		cout << "Received : " << buff << "\n";
		if (! strncmp(buff, "End", 3)) {
			closesocket(newfd);
			closesocket(sockfd);
			exit(0);
		}
		cout << "toSend : ";
		gets(buff);
		send(newfd, buff, strlen(buff), 0);
	}
}
