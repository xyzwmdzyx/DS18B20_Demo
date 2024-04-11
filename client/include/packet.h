/********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  packet.h
 *    Description:  This file is a pack data function declare file.
 *
 *        Version:  1.0.0(2024年03月25日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月25日 22时52分16秒"
 *                 
 ********************************************************************************/

#ifndef  _PACKET_H_
#define  _PACKET_H_

#include <stdint.h>
#include <time.h>

#define DEVID_LEN          16
#define TIME_LEN           32

typedef struct pack_info_s
{
    char          devid[DEVID_LEN];  // device ID
    struct tm     sample_time;       // sample time
    float         temper;            // sample temperature
} pack_info_t;

// packet function pointer type
typedef int (*packFunc)(pack_info_t *pack_info, char *pack_buf, int size);


/*	description:	get device ID
 *	 input args:	
 *					$devid : device ID buffer
 *                  $size  : device ID buffer size
 *                  $sn    : device chip number   
 * return value:    <0: failure   0: success
 */
extern int getDevid(char *devid, int size, int sn);


/*	description:	get sample time
 *	 input args:	
 *					$ptm : struct whitch store time information  
 * return value:    <0: failure   0: success
 */
extern int getTime(struct tm *ptm);


/*	description:	packet segment data into text, include device ID, sample time, sample temper
 *	 input args:	
 *					$pack_info : struct whitch store segment data
 *                  $pack_buf  : buffer whitch will store packeted data
 *                  $size      : buffer size 
 * return value:    <0: failure   >0: success
 */
extern int packetSegmentData(pack_info_t *pack_info, char *pack_buf, int size);


/*	description:	packet segment data into json, include device ID, sample time, sample temper
 *	 input args:	
 *					$pack_info : struct whitch store segment data
 *                  $pack_buf  : buffer whitch will store packeted data
 *                  $size      : buffer size 
 * return value:    <0: failure   >0: success
 */
extern int packetJsonData(pack_info_t *pack_info, char *pack_buf, int size);


#endif