/*********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  CliSql.c
 *    Description:  This file is a database system file.
 *                 
 *        Version:  1.0.0(2024年03月03日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月03日 17时21分13秒"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "CliSql.h"

// 定义执行sql语句的回调函数
int callBack(void *NotUsed, int argc, char **argv, char **azColName) {

    for (int i = 0; i < argc; i++) {
    	//printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    	sprintf(NotUsed, "%s\n", argv[i] ? argv[i] : "NULL");
    }
    return 0;
}

// 创建数据库的函数
int dbInit(db_t *db) {

	int			rv = -1;
	sqlite3		*sqdb;
	char 		sql[SQLBUFSIZE];
	char		*err_msg = NULL;
	
	// 创建数据库
	rv = sqlite3_open(db -> dbname, &sqdb);
	if(rv != SQLITE_OK) {
		sqlite3_close(sqdb);
		return -1;
	}

    sqlite3_close(sqdb);
    
	return 1;
}

// 创建表的函数
int dbCreateTable(db_t *db) {

	int			rv = -1;
	sqlite3		*sqdb;
	char 		sql[SQLBUFSIZE];
	char		*err_msg = NULL;
	
	// 打开数据库
	rv = sqlite3_open(db -> dbname, &sqdb);
	if(rv != SQLITE_OK) {
		sqlite3_close(sqdb);
		return -1;
	}
	
	// 创建表的sql语句
	memset(sql, 0, sizeof(sql));
	sprintf(sql, "CREATE TABLE IF NOT EXISTS %s (TABLECOLUMN);", db -> tablename);
	
    // 创建表
    rv = sqlite3_exec(sqdb, sql, NULL, NULL, &err_msg);
    if (rv != SQLITE_OK ) {
        sqlite3_free(err_msg);
        sqlite3_close(sqdb);
        return -2;
    }
	
	return 1;
}

// 向数据库插入数据的函数
int dbInsertData(db_t *db, char databuf[DATABUFSIZE]) {

	int			rv = -1;
	sqlite3		*sqdb;
	char 		sql[SQLBUFSIZE];
	char		*err_msg = NULL;
	
	// 打开数据库
	rv = sqlite3_open(db -> dbname, &sqdb);
	if(rv != SQLITE_OK) {
		sqlite3_close(sqdb);
		return -1;
	}
	
	// 插入数据的sql语句
	memset(sql, 0, sizeof(sql));
	sprintf(sql, "INSERT INTO %s VALUES('%s');", db -> tablename, databuf);
	
	// 插入数据
    rv = sqlite3_exec(sqdb, sql, NULL, NULL, &err_msg);
    if (rv != SQLITE_OK ) {
        sqlite3_free(err_msg);
        sqlite3_close(sqdb);
        return -2;
    }
    
    sqlite3_close(sqdb);
    
    return 1;
}

// 从数据库查询第一条数据的函数
int dbSelectData(db_t *db, char databuf[DATABUFSIZE]) {

	int			rv = -1;
	sqlite3		*sqdb;
	char 		sql[SQLBUFSIZE];
	char		*err_msg = NULL;
	
	// 打开数据库
	rv = sqlite3_open(db -> dbname, &sqdb);
	if(rv != SQLITE_OK) {
		sqlite3_close(sqdb);
		return -1;
	}
	
	// 查询数据的sql语句
	memset(sql, 0, sizeof(sql));
	sprintf(sql, "SELECT * FROM %s LIMIT 1;", db -> tablename);
	
	// 查询数据
    rv = sqlite3_exec(sqdb, sql, callBack, databuf, &err_msg);
    if (rv != SQLITE_OK ) {
        sqlite3_free(err_msg);
        sqlite3_close(sqdb);
        return -2;
    }
    
    sqlite3_close(sqdb);
    
    return 1;
}

// 从数据库删除第一条数据的函数
int dbDeleteData(db_t *db) {

	int			rv = -1;
	sqlite3		*sqdb;
	char 		sql[SQLBUFSIZE];
	char		*err_msg = NULL;
	
	// 打开数据库
	rv = sqlite3_open(db -> dbname, &sqdb);
	if(rv != SQLITE_OK) {
		sqlite3_close(sqdb);
		return -1;
	}
	
	// 删除数据的sql语句
	memset(sql, 0, sizeof(sql));
	sprintf(sql, "DELETE FROM %s LIMIT 1;", db -> tablename);
	
	// 删除数据
    rv = sqlite3_exec(sqdb, sql, NULL, NULL, &err_msg);
    if (rv != SQLITE_OK ) {
        sqlite3_free(err_msg);
        sqlite3_close(sqdb);
        return -2;
    }
    
    sqlite3_close(sqdb);
    
    return 1;
}

// 查询一张表有多少条数据的函数
int dbCountData(db_t *db, char count[COUNTSIZE]) {

	int			rv = -1;
	sqlite3		*sqdb;
	char 		sql[SQLBUFSIZE];
	char		*err_msg = NULL;
	
	// 打开数据库
	rv = sqlite3_open(db -> dbname, &sqdb);
	if(rv != SQLITE_OK) {
		sqlite3_close(sqdb);
		return -1;
	}
	
	// 查询数据条数的sql语句
	memset(sql, 0, sizeof(sql));
	sprintf(sql, "SELECT COUNT(*) FROM %s;", db -> tablename);
	
	// 查询数据条数
    rv = sqlite3_exec(sqdb, sql, callBack, count, &err_msg);
    if (rv != SQLITE_OK ) {
        sqlite3_free(err_msg);
        sqlite3_close(sqdb);
        return -2;
    }
    
    sqlite3_close(sqdb);
    
    return 1;
}
