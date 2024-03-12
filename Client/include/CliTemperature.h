/********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  CliTemperature.h
 *    Description:  This file is get DS18B20 temperature function declare file.
 *
 *        Version:  1.0.0(2024年03月03日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月03日 21时35分01秒"
 *                 
 ********************************************************************************/

#ifndef CLIDSTEMP_H
#define CLIDSTEMP_H

#define W1_PATH "/home/wmd/code/mysys/bus/w1/devices/"

// 获取设备名称
char *getDeviceName(int number);

// 获取当前时间
char  *getLocalTime();

// 读取温度值
int getTemperature(float *temp);

#endif
