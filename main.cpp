#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include "libft/libft.h"

int main()
{
	int serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in serv_addr;

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(80);

	if (bind(serv_sock, (sockaddr*)&serv_addr, sizeof(sockaddr_in)) == -1)
	{
		std::cerr << "bind() error\n";
		exit(1);
	}

	if (listen(serv_sock, 5) == -1)
	{
		std::cerr << "listen() error\n";
		exit(1);
	}

	fd_set reads, cpy_reads;
	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads);

	int fd_max = serv_sock;
	timeval timeout;
	while (true)
	{
		cpy_reads = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 50000;

		int fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout);
		if (fd_num == -1)
		{
			std::cerr << "select() error\n";
			exit(1);
		}

		if (fd_num == 0)
			continue;

		for (int i = 3;i < fd_max + 1;i++)
		{
			if (FD_ISSET(i, &cpy_reads))
			{
				if (i == serv_sock)
				{
					socklen_t addr_size = sizeof(int);
					sockaddr_in clnt_addr;
					int clnt_sock = accept(serv_sock, (sockaddr*)&clnt_addr, &addr_size);
					FD_SET(clnt_sock, &reads);
					if (fd_max < clnt_sock)
						fd_max = clnt_sock;
				}
				else
				{
					char buf[BUFSIZ];
					int str_len = recv(i, buf, BUFSIZ, 0);
					if (str_len == 0)
					{
						FD_CLR(i, &reads);
						close(i);
					}
					else
						send(i, buf, str_len, 0);
				}
			}
		}
	}
	close(serv_sock);
	return 0;
}