/*********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  DSServer.c
 *    Description:  This file is IoTDemo server file.
 *                 
 *        Version:  1.0.0(2024年03月09日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月09日 20时26分21秒"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "ServLogger.h"
#include "ServParse.h"
#include "ServSql.h"
#include "ServSocket.h"

#define CONFIG_DEBUG

int main(int argc, char *argv[]) {

	// 接收各种函数返回值相关变量
	int			rv = -1;

	// 日志系统相关变量
	log_t		log;
	
	// 数据库系统相关变量
	db_t		db;
	
	// 使用 Socket 的相关变量
	int			servport = 0;
	int			servfd = -1;
	char		recivbuf[DATABUFSIZE];
	
	/****** 第一步：在本地初始化日志文件与数据库 ******/
	
	// 初始化日志文件
	// 等级为INFO，输出到日志文件，且其路径为：“../log/Servlog.txt”
	log.loglevel = LOG_INFO;
	log.logtype = LOG_FILE;
	log.logpath = "../log/Servlog.txt";
	
	rv = logInit (&log);
	if( rv < 0 ) {
		printf("Init log system failure, program will run without any log\n");
		#ifdef CONFIG_DEBUG
		if(rv == -1) {
			printf("Create log file using function fopen() failure: %s\n", strerror(errno));
		}
		#endif
	}
	else {
		printf("Init log system success\n");
		if( log.logtype != STD_OUT ) {
			LOG(&log, LOG_INFO, "Init log system success\n");
		}
	}
	
	// 初始化数据库，名为“ServDB”，其路径为“../data/ServDB.db”
	db.dbname = "../data/ServDB.db";
	db.tablename = "\0";
	
	rv = dbInit(&db);
	if( rv < 0 ) {
		printf("Create database failure, program will exit after 2 seconds\n");
		#ifdef CONFIG_DEBUG
		if( rv == -1 ) {
			LOG(&log, LOG_DEBUG, "Create database using function sqlite3_open() failure: %s\n", strerror(errno));
		}
		#endif
		return 0;
	}
	else {
		printf("Init database system success\n");
		if( log.logtype != STD_OUT ) {
		LOG(&log, LOG_INFO, "Init database system success\n");
			}
	}
	
	/****** 第二步：进行命令行参数解析，并给servport赋值 ******/
	
	// 解析命令行参数，并赋值
	rv = parameter_parse(argc, argv, &servport);
	if( rv < 0 ) {
		printf("Parsing parameter failure, program will exit after 2 seconds. Please use -h(--help) to get help informations\n");
		sleep(2);
		#ifdef CONFIG_DEBUG
		if( rv == -1 ) {
			LOG(&log, LOG_DEBUG, "Get server IP using function getopt_long() failure: %s\n", strerror(errno));
		}
		if( rv == -2 ) {
			LOG(&log, LOG_DEBUG, "Get server port using function getopt_long() failure: %s\n", strerror(errno));
		}
		if( rv == -3 ) {
			LOG(&log, LOG_DEBUG, "Get sampling using function getopt_long() interval time failure: %s\n", strerror(errno));
		}
		#endif
		return 0;
	}
	else if( 0 == rv ) {
		return 0;
	}
	else {
		printf("Parsing parameter success\n");
		if( log.logtype != STD_OUT ) {
		LOG(&log, LOG_INFO, "Parsing parameter success\n");
		}
	}
	
	// 打印一些提示信息
	printf("server Port: %d\n", servport);
	if( log.logtype != STD_OUT ) {
		LOG(&log, LOG_INFO, "server Port: %d\n", servport);
		}
		
	printf("Program will run in the background after 2 seconds\n");
	if( log.logtype != STD_OUT ) {
		LOG(&log, LOG_INFO, "Program will run in the background after 2 seconds\n");
		}
	sleep(2);
	
	/****** 第三步：把程序丢到后台运行 ******/
	
	// 不修改工作目录，但是关闭标准输入、输出、出错
	// daemon(1, 0);
	
	/****** 第四步：接收客户端发来的数据，并存储在本地数据库中 ******/
	rv = recivData(&servport, &log, &db);
	if( rv < 0 ) {
		LOG(&log, LOG_INFO, "Server socket init faliure, program will exit after 2 seconds\n");
		sleep(2);
		#ifdef DEBUG
		if( rv == -1 ) {
			LOG(&log, LOG_INFO, "Create server socket using funciton socket() failure: %s\n", strerror(errno));
		}
		if( rv == -2 ) {
			LOG(&log, LOG_INFO, "Set sockopt using function setsockopt() failure: %s\n", strerror(errno));
		}
		if( rv == -3 ) {
			LOG(&log, LOG_INFO, "bind socket address and port using function bind() faliure: %s\n", strerror(errno));
		}
		if( rv == -4 ) {
			LOG(&log, LOG_INFO, "listen port using function listen() failure: %s\n", strerror(errno));
		}
		if( rv == -5 ) {
			LOG(&log, LOG_INFO, "Create epoll suing function epoll_create() failure: %s\n", strerror(errno));
		}
		if( rv == -6 ) {
			LOG(&log, LOG_INFO. "Add server socket fd into epoll using function epoll_ctl() faliure: %s\n", strerror(errno));
		} 
		#endif
		return 0;
	}

	return 0;
}
