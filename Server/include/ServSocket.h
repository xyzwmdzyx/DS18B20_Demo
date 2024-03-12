/********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  ServSocket.h
 *    Description:  This file is a server socekt function declare file.
 *
 *        Version:  1.0.0(2024年03月10日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月10日 21时28分37秒"
 *                 
 ********************************************************************************/
#ifndef SERVSOCKET_H
#define SERVSOCEKT_H

#define BACKLOG 100
#define MAXEVENTS 1024
#define DEVNUM 	1024

// 接收客户端数据的函数
int recivData(int *servport, log_t *log, db_t *db);

#endif
