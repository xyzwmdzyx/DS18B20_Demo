/********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  CliPackage.h
 *    Description:  This file is packing send data function declare file.
 *
 *        Version:  1.0.0(2024年03月14日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月14日 20时52分54秒"
 *                 
 ********************************************************************************/

#ifndef CLIPACK_H
#define CLIPACK_H

#define DEVICENUMBER 1
#define SENDBUFSIZE  128

// 获取设备名称
char *getDeviceName(int number);

// 获取当前时间
char *getLocalTime();

// 打包要发送的数据
void packData(float *temper, char sendbuf[SENDBUFSIZE]);

#endif
