/*********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  Logger.c
 *    Description:  This file is a log system file.
 *                 
 *        Version:  1.0.0(2024年03月01日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月01日 23时01分42秒"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include "Logger.h"

// 日至级别与其对应字符串的数组
char *log_level_str[] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

// 获取当前时间字符串的函数
char *getTimeStr() {
    
    static char time_str[20];
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    
    memset(time_str, 0, sizeof(time_str));
    strftime(time_str, 20, "%Y-%m-%d %H:%M", tm);
    
    return time_str;
}

// 获取当前的日志级别字符串的函数
char *getLevelStr(int level) {

    return log_level_str[level];
}

// 日志系统的回滚函数
int checkFileSize(log_t *log) {
	
	FILE	*fp = NULL;
	long	size = 0;
	
	fp = fopen(log -> logpath, "r");
	if( NULL == fp ) {
		return -1;
	}
	
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fclose(fp);
	
	if(size > LOGMAXSIZE) {
		fp = fopen(log -> logpath, "w");
		if( NULL == fp ) {
			return -2;
		}
		fclose(fp);
	}

	return 1;
}

// 日志系统的初始化函数。成功返回1，失败返回0
int logInit(log_t *log) {

	FILE	*fp = NULL;

	if( log -> logtype == STD_OUT ) {
		return 1;
	}
	fp = fopen(log -> logpath, "a+");
	if( NULL == fp ) {
		return -1;
	}
	fclose(fp);

	return 2;
}

// 日志系统的删除函数
int logDestroy(log_t *log) {
	
	int		rv = -1;

	if( log -> logtype == STD_OUT ) {
		return 1;
	}
	rv = remove(log -> logpath);
	if( 0 != rv ) {
		return -1;
	}
	
	return 2;
}

// 日志系统的输出函数
int logPrint(log_t *log, log_level_t level, const char *filename, const char *funcname, int line, char *format, ...) {
	
	int		rv = -1;
	FILE	*fp = NULL;
	
	// 如果当前级别小于设定级别，则不输出
	if( level < log -> loglevel ) {
		return 1;
	}
	
	va_list args;
	va_start(args, format);
	
	// 如果日志类型为STD_OUT，则输出到屏幕
	if( log -> logtype == STD_OUT ) {
        printf("[%s] [%s] [%s : %s : %d] ", getTimeStr(), getLevelStr(level), filename, funcname, line);
        vprintf(format, args);
        printf("\n");
	}
	
	// 如果日志类型为LOG_FILE，则输出到文件
	if( log -> logtype == LOG_FILE ) {
		// 先检查日志文件是否超过规定大小，如果超过则删除原日志文件并新建一个		
		rv = checkFileSize(log);
		if( rv < 0 ) {
			return -1;
		}
	
		fp = fopen(log -> logpath, "a+");
		if( NULL == fp ) {
			return -2;
		}
		
		fprintf(fp, "[%s] [%s] [%s : %s : %d] ", getTimeStr(), getLevelStr(level), filename, funcname, line);
        vfprintf(fp, format, args);
        fprintf(fp, "\n");
        
        fclose(fp);
	}
	
	va_end(args);

	return 2;
}
