/*********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  CliPackage.c
 *    Description:  This file is a packing send data function file.
 *                 
 *        Version:  1.0.0(2024年03月14日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月14日 20时47分10秒"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "CliPackage.h"

// 获取设备名称
char *getDeviceName(int number) {

	static char name_str[8];
	sprintf(name_str, "Node%d", number);
		
	return name_str;
}

// 获取当前时间
char *getLocalTime() {
    
    static char time_str[32];
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    
    memset(time_str, 0, sizeof(time_str));
    strftime(time_str, 32, "%Y-%m-%d %H:%M:%S", tm);
    
    return time_str;
}

// 打包要发送的数据
void packData(float *temper, char sendbuf[SENDBUFSIZE]) {

	sprintf(sendbuf, "%s,%s,%.2f", getDeviceName(DEVICENUMBER), getLocalTime(), *temper);
	
	return;
}
