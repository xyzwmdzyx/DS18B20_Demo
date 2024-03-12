/********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  ServLogger.h
 *    Description:  This file is a log system declare file.
 *
 *        Version:  1.0.0(2024年03月01日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月01日 23时02分17秒"
 *                 
 ********************************************************************************/

#ifndef SERVLOGGER_H
#define SERVLOGGER_H

// 日志文件最大为10MB
#define LOGMAXSIZE	1024 * 1024 * 10

// 日志级别枚举类型
typedef enum {
	LOG_DEBUG, // 调试信息
  	LOG_INFO,  // 一般信息
  	LOG_WARN,  // 警告信息
  	LOG_ERROR, // 错误信息
  	LOG_FATAL  // 致命错误
} log_level_t;

// 日志输出方式枚举类型
typedef enum {
	STD_OUT, // 标准输出
	LOG_FILE // 日志文件
} log_type_t;

// 日志结构体
typedef struct {
log_level_t		loglevel; // 日志级别
log_type_t		logtype; // 日志输出类型
char			*logpath; // 日志文件路径
} log_t;

// 日志系统的初始化函数
int logInit(log_t *log);

// 日志系统的删除函数
int logDestroy(log_t *log);

// 日志系统的输出函数
int logPrint(log_t *log, log_level_t level, const char *filename, const char *funcname, int line, char *format, ...);

// 日志系统的回滚函数
int checkFileSize(log_t *log);

// 宏用于简化日志打印的调用，自动传入当前的文件名，函数名，行号，要打印的信息等
#define LOG(log, level, format, ...) logPrint(log, level, __FILE__, __func__, __LINE__, format, ##__VA_ARGS__)

#endif
