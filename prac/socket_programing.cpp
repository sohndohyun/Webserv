/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket_programing.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinkim <jinkim@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/15 19:08:19 by jinkim            #+#    #+#             */
/*   Updated: 2021/03/16 04:13:17 by jinkim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#define MAXLINE 1024
#define PORTNUM 3600
#define SOCK_SETSIZE 1021

void nonblock(int sockfd)
{
    int opts;
    opts = fcntl(sockfd, F_GETFL);
    if(opts < 0)
    {
        perror("fcntl(F_GETFL)\n");
        exit(1);
    }
    opts = (opts | O_NONBLOCK);
    if(fcntl(sockfd, F_SETFL, opts) < 0)
    {
        perror("fcntl(F_SETFL)\n");
        exit(1);
    }
}

int main(int argc, char **argv)
{
	int listen_fd, client_fd;
	socklen_t addrlen;
	int fd_num;
	int maxfd = 0;
	int sockfd;
	int readn;
	int i= 0;
	char buf[MAXLINE];
	fd_set readfds, tmp_readfds, writefds, tmp_writefds;

	struct timeval timeout;
	struct sockaddr_in server_addr, client_addr;

	char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

	if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket error");
		return 1;
	}
	//printf("[listen_fd : %d]\n", listen_fd);
	int flag = fcntl(listen_fd, F_GETFL, 0);
	fcntl(listen_fd, F_SETFL, flag | O_NONBLOCK);
	memset((void *)&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORTNUM);

	if(bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("bind error");
		return 1;
	}
	if(listen(listen_fd, 5) == -1)
	{
		perror("listen error");
		return 1;
	}

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_SET(listen_fd, &readfds);
	FD_SET(listen_fd, &writefds);

	maxfd = listen_fd;
	while(1)
	{

		tmp_readfds = readfds;
		tmp_writefds = writefds;

		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		printf("Select Wait %d\n", maxfd);
		fd_num = select(maxfd + 1 , &tmp_readfds, &tmp_writefds, (fd_set *)0, &timeout);
		//printf("[fd_num : %d]\n", fd_num);

		if (FD_ISSET(listen_fd, &tmp_readfds))
		{
			addrlen = sizeof(client_addr);
			client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addrlen);
			//printf("[client_fd : %d]\n", client_fd);
			fcntl(client_fd, F_SETFL, O_NONBLOCK);

			FD_SET(client_fd,&readfds);

			if (client_fd > maxfd)
				maxfd = client_fd;
			printf("Accept OK\n");
			read(client_fd, buf, MAXLINE-1);
			printf("[%s]\n", buf);
			write(client_fd , hello , strlen(hello));

			continue;
		}

		for (i = 0; i <= maxfd; i++)
		{
			sockfd = i;
			if (FD_ISSET(sockfd, &tmp_readfds))
			{
				if (sockfd != listen_fd)
				{
					readn = read(sockfd, buf, MAXLINE-1);
					buf[readn] = '\0';
					printf("%s",buf);
					write(client_fd , hello , strlen(hello));
					if (readn < 0)
					{
						if (errno != EAGAIN)
    					{
							printf("close\n");
							close(sockfd);
							FD_CLR(sockfd, &readfds);
							fd_num--;
							if (fd_num <= 0)
								break;
    					}
						if (FD_ISSET(sockfd, &tmp_writefds))
							write(client_fd , hello , strlen(hello));
					}
				}
			}
		}
	}
}
