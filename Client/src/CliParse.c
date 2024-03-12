/********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  CliParse.c 
 *    Description:  This file is a parameter parse function file.
 *                 
 *        Version:  1.0.0(2024年03月03日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月03日 21时08分28秒"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <libgen.h>
#include "CliParse.h"

// 打印命令行参数帮助信息的函数
void print_usage(char *program_name) {

	printf("%s usage: \n", program_name);
	printf("-i(--ipaddr): specify server IP address\n");
	printf("-p(--port): specify server Port\n");
	printf("-t(--time): specify sampling interval time in minutes\n ");
	printf("-h(--help): get help informations\n");

	return;
}

// 命令行参数解析的函数
int parameter_parse(int argc, char* argv[], char **servip, int *servport, time_t *readtime) {
	
	// 命令行参数解析相关变量
	int					ch = 0; 							// 保存 getopt_long() 返回值
	struct option		opts[] = { 					        // 指定命令行参数
			{"ipaddr", required_argument, NULL, 'i'},
			{"port", required_argument, NULL, 'p'},
			{"time", required_argument, NULL, 't'},
			{"help", no_argument, NULL, 'h'},
			{NULL, 0, NULL, 0}
		};
	
	// 解析命令行参数，给servip, servport, readtime赋值
	while( (ch = getopt_long(argc, argv, "i:p:t:h", opts, NULL)) != -1 ) {
		switch(ch) {
			case 'i':
				*servip = optarg;
				break;
			case 'p':
				*servport = atoi(optarg);
				break;
			case 't':
				*readtime = atoi(optarg) * 60;
				break;
			case 'h':
				print_usage(basename(argv[0]));
				return 0;
		}
	}
	
	if( NULL == servip ) {
		return -1;
	}
	if( 0 == *servport ) {
		return -2;
	}
	if( 0 == *readtime ) {
		return -3;
	}

	return 1;
}
