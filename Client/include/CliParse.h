/********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  CliParse.h
 *    Description:  This file is a parameter parse function declare file.
 *
 *        Version:  1.0.0(2024年03月03日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月03日 21时09分18秒"
 *                 
 ********************************************************************************/

#ifndef CLIPARSE_H
#define CLIPARSE_H

// 打印命令行参数帮助信息的函数
void print_usage(char *program_name);

// 命令行参数解析的函数
int parameter_parse(int argc, char* argv[], char **servip, int *servport, time_t *readtime);

#endif
