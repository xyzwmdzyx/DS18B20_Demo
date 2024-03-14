/*********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  DSClient.c
 *    Description:  This file is IoTDemo client file.
 *                 
 *        Version:  1.0.0(2024年02月29日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年02月29日 19时02分51秒"
 *                 
 ********************************************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "Logger.h"
#include "Sql.h"
#include "CliParse.h"
#include "CliTemperature.h"
#include "CliPackage.h"
#include "CliSocket.h"
 
#define CONFIG_DEBUG

int main(int argc, char*argv[]) {

    // 接收各种函数返回值相关变量
	int			rv = -1;

	// 日志系统相关变量
	log_t		log;
	
	// 数据库系统相关变量
	db_t		db;
	
	// 读取 DS18B20 温度数据的相关变量
	time_t 		readtime = 0, start = 0, end = 0, elapsed = 0; // 读取温度的时间间隔；起始时间；结束时间，单位为秒
	int			first = 1;
	float		temper = 0;
	
	// 使用 Socket 向服务器端发送数据的相关变量
	char		*servip = NULL;
	int			servport = 0;
	int			clifd = -1;
	char		sendbuf[DATABUFSIZE];

	/****** 第一步：在本地初始化日志文件与数据库 ******/
	
	// 初始化日志文件
	// 等级为INFO，输出到日志文件，且其路径为：“./log/Clilog.txt”
	log.loglevel = LOG_INFO;
	log.logtype = LOG_FILE;
	log.logpath = "./log/Clilog.txt";
	
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
	
	
	// 初始化数据库，名为“CliDB”；该数据库包含一张表，名为“TempTable”
	// 该表包含一个字段，名为“send_data”，类型为“TEXT”
	db.dbname = "./data/CliDB.db";
	db.tablename = "TempTable";
	
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
		rv = dbCreateTable(&db);
		if( rv < 0 ) {
			printf("Create table failure, program will exit after 2 seconds\n");
			#ifdef CONFIG_DEBUG
			if( rv == -1 ) {
				LOG(&log, LOG_DEBUG, "Open database using function sqlite3_open() failure: %s\n", strerror(errno));
			}
			if( rv == -2 ) {
				LOG(&log, LOG_DEBUG, "Create table using function sqlite3_exec() failure: %s\n", strerror(errno));
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
	}
	
	/****** 第二步：进行命令行参数解析，并给servip, servport, readtime赋值 ******/
	
	// 解析命令行参数，并赋值
	rv = parameter_parse(argc, argv, &servip, &servport, &readtime);
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
	printf("server IP: %s, server Port: %d, readtime: %ld seconds\n", servip, servport, readtime);
	if( log.logtype != STD_OUT ) {
		LOG(&log, LOG_INFO, "server IP: %s, server Port: %d, readtime: %ld seconds\n", servip, servport, readtime);
		}
		
	printf("Program will run in the background after 2 seconds\n");
	if( log.logtype != STD_OUT ) {
		LOG(&log, LOG_INFO, "Program will run in the background after 2 seconds\n");
		}
	sleep(2);
	
	/****** 第三步：把程序丢到后台运行 ******/
	
	// 不修改工作目录，但是关闭标准输入、输出、出错
	// daemon(1, 0);
	
	start = time(NULL);
	
	while(1) {
		/****** 第四步：每隔 readtime 秒，读取一次 DS18B20 温度数据 ******/
		// 获取当前温度
		end = time(NULL);
		elapsed = difftime(end, start);
		if( (elapsed >= readtime) || (first == 1) ) {
			first = 0;
			rv = getTemperature(&temper);
			if( rv < 0 ) {
				LOG(&log, LOG_INFO, "Get DS18B20 temperature failure, program will try again after %ld seconds", readtime);
				#ifdef CONFIG_DEBUG
				if( rv == -1 ) {
					LOG(&log, LOG_DEBUG, "Open folder using function opendir() failure: %s\n", strerror(errno));
				}
				if( rv == -2 ) {
					LOG(&log, LOG_DEBUG, "Can not find DS18B20 chipset\n");
				}
				if( rv == -3 ) {
					LOG(&log, LOG_DEBUG, "Open file using function open() failure: %s\n", strerror(errno));
				}
				if( rv == -4 ) {
					LOG(&log, LOG_DEBUG, "Read data using function read() failure: %s\n", strerror(errno));
				}
				if( rv == -5 ) {
					LOG(&log, LOG_DEBUG, "Can not find string: 't= '\n");
				}
				#endif
				continue;
			}
			else {
				LOG(&log, LOG_INFO, "Get DS18B20 temperature success, temperature = %f\n", temper);
			}
			start = time(NULL);
		/****** 第五步：使用 Socket 向服务器端发送温度数据 ******/
		// 先将所有要发送的内容组装成字符串，用 , 分割
		memset(sendbuf, 0, sizeof(sendbuf));
		packData(&temper, sendbuf);
		LOG(&log, LOG_INFO, "Prepare send data success, data = %s\n", sendbuf);
		// 与服务器建立连接
		// 如果连接失败，则把本次读取的数据存入本地数据库
		// 如果连接成功，则先发送本地数据库的数据，再发送本次读取的数据
		rv = sendData(&servip, &servport, &db, sendbuf);
		if( rv == -1 ) {
			LOG(&log, LOG_INFO, "Create socket fd failure, program will try after %ld seconds\n", readtime);
			#ifdef CONFIG_DEBUG
			LOG(&log, LOG_DEBUG, "Create socket fd using function socket() failure:%s\n", strerror(errno));
			#endif
			continue;
		}
		else if( rv == -2 ) {
			LOG(&log, LOG_INFO, "Store data into local database failure, program will ignore this data\n");
			#ifdef CONFIG_DEBUG
			LOG(&log, LOG_DEBUG, "Insert data into table %s using funciton dbInsertData()failure: %s\n", db.tablename, strerror(errno));
			#endif
			continue;
		}
		else if( rv == 1 ) {
			LOG(&log, LOG_INFO, "Store data into local database success\n");
			continue;
		}
		else if( rv == -3 ) {
			LOG(&log, LOG_INFO, "Send local data to server failure, program will try after %ld seconds\n", readtime);
			#ifdef CONFIG_DEBUG
			LOG(&log, LOG_DEBUG, "Count table %s total rows using function dbCountData() failue: %s\n", db.tablename, strerror(errno));
			#endif
			continue;
		}
		else if( rv == -4 ) {
			LOG(&log, LOG_INFO, "Send local data to server failure, program will try after %ld seconds\n", readtime);
			#ifdef CONFIG_DEBUG
			LOG(&log, LOG_DEBUG, "Select first row data in table %s using function dbSelectData() failue: %s\n", db.tablename, strerror(errno));
			#endif
		}
		else if( rv == -5 ) {
			LOG(&log, LOG_INFO, "Send local data to server failure, program will try after %ld seconds\n", readtime);
			#ifdef CONFIG_DEBUG
			LOG(&log, LOG_DEBUG, "Send first row data in table %s using function write() failue: %s\n", db.tablename, strerror(errno));
			#endif
		}
		else if( rv == -6 ) {
			LOG(&log, LOG_INFO, "Send local data to server failure, program will try after %ld seconds\n", readtime);
			#ifdef CONFIG_DEBUG
			LOG(&log, LOG_DEBUG, "Delete first row data in table %s using function dbDeleteData() failue: %s\n", db.tablename, strerror(errno));
			#endif
		}
		else if( rv == -7 ) {
			LOG(&log, LOG_INFO, "Send this time data to server failure, program will try after %ld seconds\n", readtime);
			#ifdef CONFIG_DEBUG
			LOG(&log, LOG_DEBUG, "Send this time data using function write() failue: %s\n", strerror(errno));
			#endif
		}
		else {
			LOG(&log, LOG_INFO, "Send data to server success\n");
		}
		
		}//if( (elapsed >= readtime) || (first == 1) )
		
	}//while(1)
	
	return 0;
}
