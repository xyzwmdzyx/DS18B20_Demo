/********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  ServParse.c 
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
#include "ServParse.h"

// 打印命令行参数帮助信息的函数
void print_usage(char *program_name) {

	printf("%s usage: \n", program_name);
	printf("-p(--port): specify server Port\n");
	printf("-h(--help): get help informations\n");

	return;
}

// 命令行参数解析的函数
int parameter_parse(int argc, char* argv[], int *servport) {
	
	// 命令行参数解析相关变量
	int					ch = 0; 							// 保存 getopt_long() 返回值
	struct option		opts[] = { 					        // 指定命令行参数
			{"port", required_argument, NULL, 'p'},
			{"help", no_argument, NULL, 'h'},
			{NULL, 0, NULL, 0}
		};
	
	// 解析命令行参数，给servport赋值
	while( (ch = getopt_long(argc, argv, "p:h", opts, NULL)) != -1 ) {
		switch(ch) {
			case 'p':
				*servport = atoi(optarg);
				break;
			case 'h':
				print_usage(basename(argv[0]));
				return 0;
		}
	}
	
	if( 0 == *servport ) {
		return -1;
	}

	return 1;
}
