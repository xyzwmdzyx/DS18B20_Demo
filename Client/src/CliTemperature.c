/*********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  CliTemperature.c
 *    Description:  This file is get DS18B20 temperature function file.
 *                 
 *        Version:  1.0.0(2024年03月05日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月05日 19时07分12秒"
 *                 
 ********************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "CliTemperature.h"

#define BUFSIZE 128
#define CHIP_SN_SIZE 64
#define DS18B20_PATH_SIZE 128

// 读取温度值
int getTemperature(float *temp) {

	int				fd = -1;
	int				found = 0;
	char			w1_path[] = W1_PATH;
	char			chip_sn[CHIP_SN_SIZE];
	char			ds18b20_path[DS18B20_PATH_SIZE];
	char			buf[BUFSIZE];
	char			*ptr = NULL;	
	DIR				*dirp = NULL;
	struct dirent 	*direntp = NULL;		
	
	// 打开文件夹：/sys/bus/w1/devices/
	dirp = opendir(w1_path);
	if( NULL == dirp ) {
		return -1;
	}
	
	// 找到芯片序列号，并将其保存至 chip_sn[] 内
	while( NULL != (direntp = readdir(dirp)) ) {
		if( strstr(direntp -> d_name, "28-") ) {
			memset(chip_sn, 0, sizeof(chip_sn));
			strncpy(chip_sn, direntp -> d_name, sizeof(chip_sn));
			found = 1;
		}
	}
	if( 0 == found ) {
		return -2;
	}
	
	// 拼接得到完整路径，并将其保存至 ds18b20_path[] 内
	memset(ds18b20_path, 0, sizeof(ds18b20_path));
	snprintf(ds18b20_path, sizeof(ds18b20_path), "%s/%s/w1_slave", w1_path, chip_sn);
	
	// 关闭文件夹
	closedir(dirp);
	
	// 打开文件：/sys/bus/w1/devices/28-xxxxxxxxxxxx/w1-slave
	fd = open(ds18b20_path, O_RDONLY);
	if(fd < 0) {
		return -3;
	}
	
	// 将文件内容读取至 buf[] 内
	memset(buf, 0, sizeof(buf));
	if( (read(fd ,buf, sizeof(buf))) < 0 ) {
		return -4;
	}
	
	// 找到 t=xxxxxx 字符串
	ptr = strstr(buf, "t=");
	if( NULL == ptr ) {
		return -5;
	}
	
	// 找到 温度 字符串并换算
	ptr += 2;
	*temp = atof(ptr) / 1000;
	
	// 关闭文件
	close(fd);
	
	return 1;
}
