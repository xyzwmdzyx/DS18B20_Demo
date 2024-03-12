/********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  CliSql.h
 *    Description:  This file is a sql system declare file.
 *
 *        Version:  1.0.0(2024年03月03日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月03日 17时22分53秒"
 *                 
 ********************************************************************************/
#ifndef CLIDBSQL_H
#define CLIDBSQL_H

#define SQLBUFSIZE		128
#define DATABUFSIZE		128
#define	COUNTSIZE 		336
#define TABLECOLUMN send_data TEXT

// 数据库结构体
typedef struct {
	char	*dbname;
	char	*tablename;
} db_t;

// 创建数据库的函数
int dbInit(db_t *db);

// 创建表的函数
int dbCreateTable(db_t *db);

// 向数据库插入数据的函数
int dbInsertData(db_t *db, char databuf[DATABUFSIZE]);

// 从数据库查询第一条数据的函数
int dbSelectData(db_t *db, char databuf[DATABUFSIZE]);

// 从数据库删除第一条数据的函数
int dbDeleteData(db_t *db);

// 查询一张表有多少条数据的函数
int dbCountData(db_t *db, char count[COUNTSIZE]);

#endif
