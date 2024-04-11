/*********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  ServSocket.c
 *    Description:  This file is a server socket function file.
 *                 
 *        Version:  1.0.0(2024年03月10日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月10日 21时28分18秒"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include "Logger.h"
#include "Sql.h"
#include "ServSocket.h"

// 接收客户端数据的函数
int recivData(int *servport, log_t *log, db_t *db) {

	int						rv = -1;
	int						servfd = -1, clifd = -1, newfd = -1;
	int						on = 1;
	struct sockaddr_in		servaddr;
	struct sockaddr_in 		cliaddr;
	socklen_t 				cliaddrlen;
	char 					recivbuf[DATABUFSIZE];
    char					result[RESULTSIZE];
    char					devnamebuf[DEVNUM];
	char					*devname = NULL;
	
	// epoll所需的变量
	int						epfd = -1;
	int						evns = -1;
	struct epoll_event 		ev;
	struct epoll_event 		events[MAXEVENTS];
	
	
	// 创建服务端socekt
	if( (servfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		return -1;
	}
	
	// 当服务端程序退出时，立即释放端口
	if( (rv = setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0 ) {
		return -2;
	} 
	
	// 设置服务端监听的IP地址与端口
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(*servport);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	// 绑定端口号
	if( (rv = bind(servfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) < 0 ) {
		return -3;	
	}
	
	// 开始监听
	if( (rv = listen(servfd, BACKLOG)) < 0 ) {
		return -4;
	}

    // 创建epoll实例
    if ( (epfd = epoll_create(MAXEVENTS)) < 0 ) {
    	return -5;
    }
    
    // 将服务端套接字加入epoll，监听其可读事件
    ev.data.fd = servfd;
    ev.events = EPOLLIN; 
    if( (rv = epoll_ctl(epfd, EPOLL_CTL_ADD, servfd, &ev)) < 0 ) {
    	return -6;
    }
	
	// 开始接收数据
	while(1) {
	
		// 等待epoll事件 
		if ( (evns = epoll_wait(epfd, events, MAXEVENTS, -1)) < 0 ) {
			LOG(log, LOG_INFO, "Wait epoll events failure, program will try after 2 seconds\n");
			sleep(2);
			continue;
		}
		
		// 遍历发生的事件，获取发生事件对应的文件描述符
		for(int i = 0; i < evns; i++) {
			newfd = events[i].data.fd;
			
			// 如果是服务端描述符，则有新客户端连接
			if( newfd == servfd ) {
				// 接受新客户端连接
				if( (clifd = accept(servfd, (struct sockaddr *)&cliaddr, &cliaddrlen)) < 0 ) {
					LOG(log, LOG_INFO, "Accept new client failure, program will continue running\n");
					continue;	
				}
				// 将客户端套接字加入epoll，监听其可读事件
				ev.data.fd = clifd;
				ev.events = EPOLLIN;
				if( epoll_ctl(epfd, EPOLL_CTL_ADD, clifd, &ev) < 0 ) {
					LOG(log, LOG_INFO, "Add new client into epoll failure, program will continue running\n");
					close(clifd);
					continue;
				}
				LOG(log, LOG_INFO, "Accept new client[%s:%d] with fd[%d]\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), clifd);	
			}
			
			// 如果是客户端描述符，则有数据
			else {
				memset(recivbuf, 0, sizeof(recivbuf));
				// 读客户端发来的数据
				if( (rv = read(newfd, recivbuf, sizeof(recivbuf))) < 0 ) {
					LOG(log, LOG_INFO, "Read data from client[%s:%d] faliure, program will continue running\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
					close(newfd);
					epoll_ctl(epfd, EPOLL_CTL_DEL, newfd, &ev);
					continue;
				}
				// 客户端主动关闭连接	
				if( 0 == rv ) {
					LOG(log, LOG_INFO, "CLient[%s:%d] closed by some reason\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
					close(newfd);
					epoll_ctl(epfd, EPOLL_CTL_DEL, newfd, &ev);
					continue;
				}
				// 如果收到"over"，则关闭客户端
				if( strcmp(recivbuf, "over") == 0 ) {
					LOG(log, LOG_INFO, "Recieve data from CLient[%s:%d] success\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port)); 
					close(newfd);
					epoll_ctl(epfd, EPOLL_CTL_DEL, newfd, &ev);
					continue;
				}
				// 如果收到温度数据，先检查本地数据库是否为该客户端设备创建表
				memset(devnamebuf, 0, sizeof(devnamebuf));
				strncpy(devnamebuf, recivbuf, 5);
				db -> tablename = devnamebuf;
				rv = dbIfTableExist(db, result);
				if( rv < 0 ) {
					LOG(log, LOG_INFO, "Cannot judge if table %s exist, program will continue running\n", devnamebuf);
					close(newfd);
					epoll_ctl(epfd, EPOLL_CTL_DEL, newfd, &ev);
					continue;
				}
				// 如果没有，则先为该设备创建表
				if( atoi(result) != 1 ) {
					rv = dbCreateTable(db);
					if( rv < 0 ) {
						LOG(log, LOG_INFO, "Create new table for %s faliure, program will continue running\n", devnamebuf);
						close(newfd);
						epoll_ctl(epfd, EPOLL_CTL_DEL, newfd, &ev);
						continue;
					}
				}
				// 将收到的数据存入数据库对应的表中
				rv = dbInsertData(db, recivbuf);
				if( rv < 0 ) {
					LOG(log, LOG_INFO, "Insert data into table %s faliure, program will continue running\n", devnamebuf);
				}
			}//else
		} //for(int i = 0; i < evns; i++)	
	} //while(1)

	// 关闭服务端
	close(epfd);
	close(servfd);

	return 1;
}
