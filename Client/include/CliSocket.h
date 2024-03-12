/********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  CliSocket.h
 *    Description:  This file is a client socket function declare file.
 *
 *        Version:  1.0.0(2024年03月06日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月06日 20时23分12秒"
 *                 
 ********************************************************************************/
#ifndef CLISOCKET_H
#define CLISOCKET_H

#define ENDMSG "over"

int sendData(char **servip, int *servport, db_t *db, char sendbuf[DATABUFSIZE]);

#endif
