/*********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  CliSocket.c
 *    Description:  This file is a client socket function file.
 *                 
 *        Version:  1.0.0(2024年03月06日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月06日 20时22分59秒"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "Sql.h"
#include "CliSocket.h"

int sendData(char **servip, int *servport, db_t *db, char sendbuf[DATABUFSIZE]) {

	int						rv = -1;
	int						clifd = -1;
	int						count = -1;
	char					dbdatabuf[DATABUFSIZE];
	char					countbuf[COUNTSIZE];
	char					nowdatabuf[DATABUFSIZE];
	struct sockaddr_in		servaddr;
	
	// 创建客户端 socket
	if( (clifd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		return -1;
	}
	
	// 设置服务器的 IP 地址与端口
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(*servport);
	inet_aton(*servip, &servaddr.sin_addr);
	
	// 与服务器建立连接	
	if( (rv = connect(clifd, (struct sockaddr *)&servaddr, sizeof(servaddr))) < 0 ) {
		// 返回值 < 0 说明网络故障，则将读取到的数据写入本地数据库
		rv = dbInsertData(db, sendbuf);
		if( rv < 0 ) {
			return -2;
		}
		else {
			close(clifd);
			return 1;
		}
	}
	else {
		// 返回值 = 0 说明网络正常，先将数据库中的数据发送给服务器，再将此次数据发送给服务器
		// 查询本地数据库有多少条数据
		rv = dbCountData(db, countbuf);
		if( rv < 0 ) {
			return -3;
		}
		count = atoi(countbuf);
		// 逐条将数据发送给服务器
		for(int i = 0; i < count; i++) {
			memset(dbdatabuf, 0, sizeof(dbdatabuf));
			rv = dbSelectData(db, dbdatabuf);
			if( rv < 0 ) {
				return -4;
			}
			rv = write(clifd, dbdatabuf, sizeof(dbdatabuf));
			if( rv < 0 ) {
				return -5;
			}
			rv = dbDeleteData(db);
			if( rv < 0 ) {
				return -6;
			}
		}
		// 再将此次数据发送给服务器
		strcpy(nowdatabuf, sendbuf);
		rv = write(clifd, nowdatabuf, sizeof(nowdatabuf));
		if( rv < 0 ) {
			return -7;
		}
		// 给服务器发送确认信息
		rv = write(clifd, ENDMSG, strlen(ENDMSG));
		if( rv < 0 ) {
			return -8;
		}
		close(clifd);
		return 2;
	}
}
