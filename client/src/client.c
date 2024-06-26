/*********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  client.c
 *    Description:  This file is a MQTT client file.
 *                 
 *        Version:  1.0.0(2024年03月29日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月29日 20时12分02秒"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include <libgen.h>
#include <string.h>
#include <errno.h>

#include "logger.h"
#include "process.h"
#include "database.h"
#include "ds18b20.h"
#include "packet.h"
#include "socket.h"

#define PROG_VERSION               	"v1.0.0"
#define DAEMON_PIDFILE             	"/tmp/.client_mqttd.pid"

// print help information
static void printUsage(char *progname) {
    printf("Usage: %s [OPTION]...\n", progname);
    printf(" %s is LingYun studio temperature socket client program running on RaspberryPi\n", progname);

    printf("\nMandatory arguments to long options are mandatory for short options too:\n");
    printf("-i(ipaddr)    : sepcify server IP address\n");
    printf("-p(--port)    : sepcify server port.\n");
    printf("-t(--readtime): sepcify report time interval, default 60 seconds\n");
    printf("-d(--debug)   : running in debug mode\n");
    printf("-h(--help)    : display this help information\n");
    printf("-v(--version) : display the program version\n");

    printf("\n%s version %s\n", progname, PROG_VERSION);
    return;
}

int checkSampleTime(time_t *last_time, int interval);

int main(int argc, char* argv[]) {

	extern proc_signal_t	g_signal;
	int						daemon = 1;
	int						rv = -1;
	
	char                    *progname = NULL;
	
    char                    *logfile = "./log/client.log";
    int                     loglevel = LOG_INFO;
    int                     logsize = 10;
    
    char					*dbfile = "./data/client.db";

    char                    *servip = NULL;
    int                     servport = 0;
    int                     readtime = 60;

    socket_ctx_t            sock = {0};
    time_t                  last_time = 0;
    int                     sample_flag = 0;

    char                    pack_buf[1024] = {0};
    int                     pack_bytes = 0;
    pack_info_t             pack_info = {0};
    packFunc             	pack_function = packetSegmentData;

    struct option           opts[] = {
                            {"ipaddr", required_argument, NULL, 'i'},
                            {"port", required_argument, NULL, 'p'},
                            {"readtime", required_argument, NULL, 't'},
                            {"debug", no_argument, NULL, 'd'},
                            {"version", no_argument, NULL, 'v'},
                            {"help", no_argument, NULL, 'h'},
                            {NULL, 0, NULL, 0}
                    };
	
	// parament parse
	progname = (char *)basename(argv[0]);
	while( (rv = getopt_long(argc, argv, "i:p:t:dvh", opts, NULL)) != -1 ) {
        switch(rv) {
        
 			case 'i': // set server ip address
                servip = optarg;
                break;

            case 'p': // set server port
                servport = atoi(optarg);
                break;

            case 't': // set interval time
                readtime = atoi(optarg);
                break;
            case 'd': // set running mode debug
                daemon = 0;
                logfile = "console";
                loglevel = LOG_DEBUG;
                break;

            case 'v':  // get version information
                printf("%s version %s\n", progname, PROG_VERSION);
                return 0;

            case 'h':  // get help information
                printUsage(progname);
                return 0;

            default:
                break;
        }

    }
    
    // check input args
    if( !servip || !servport ) {
        printUsage(argv[0]);
        return 0;
    }
    
    // init log system
    if( logInit(logfile, loglevel, logsize, LOG_LOCK_DISABLE) < 0 ) {
        fprintf(stderr, "Initial log system failure, program will exit\n");
        return -1;
    }
    
    // install signal and it's defalut fuction
    installDefaultSignal();
    
    // check program already running as daemon or not. If not, then set program running in daemon mode
    if( checkSetProgramRunning(daemon, DAEMON_PIDFILE) < 0 ) {
    	logTerm();
    	return -2;
    }
    
    // init database system
    if( databaseInit(dbfile) < 0 ) {
        logError("Initial database system faliure, program will exit\n");
        unlink(DAEMON_PIDFILE);
    	logTerm();
    	return -3;
    }
    
    // init socket system
    if( socketInit(&sock, servip, servport) ) {
    	logError("Initial socket system faliure, program will exit\n");
    	goto Cleanup;
    }
    
    // continue running when g_signal.stop != 1
    while( !g_signal.stop ) {
    
    	// set sample flag = 0
    	sample_flag = 0;
    	// if time interval = readtime, then read ds18b20 temper
    	if( checkSampleTime(&last_time, readtime) ) {
            logDebug("start sample DS18B20 termperature\n");

            // read ds18b20 temper
            if( (rv = ds18b20GetTemperature(&pack_info.temper)) < 0 ) {
                logError("sample DS18B20 temperature failure, errcode = %d\n", rv);
                continue;
            }
            logInfo("sample DS18B20 termperature success, temper = %.3f oC\n", pack_info.temper);

            // get device id and sample time
            getDevid(pack_info.devid, DEVID_LEN, 40);
            getTime(pack_info.sample_time, TIME_LEN);

            // pack data into Segment pakcet
            pack_bytes = pack_function(&pack_info, pack_buf, sizeof(pack_buf));
            logDebug("packet sample data success, pack_buf = %s\n", pack_buf);
            // set sample flag = 1
            sample_flag = 1;
        }
        
        // connect to broker
        if( sock.fd < 0 ) {
        	socketConnect(&sock);
        }
        
        // check if client really connect to broker
        if( socketCheckConnect(sock.fd) < 0 ) {
        	if( sock.fd > 0 ) {
        		logError("mosquitto mqtt got disconnected, terminate it and reconnect now\n");
        		socketTerm(&sock);
        	}
        }
        
        // if client disconnect, then push data into database
        if( sock.fd < 0 ) {
        	if( sample_flag ) {
        		databasePushPacket(pack_buf, pack_bytes);
        	}
        	continue;
        }
        
        // if client connect, then send data to server
        if( sample_flag ) {
        	logDebug("socket send sample packet bytes[%d]: %s\n", pack_bytes, pack_buf);
        	if( socketSend(&sock, pack_buf, pack_bytes) < 0 ) {
                logWarn("socket send sample packet failure, save it in database now\n");
                databasePushPacket(pack_buf, pack_bytes);
                socketTerm(&sock);
            }
        }
        
        // socket send packet in database
        if( !databasePopPacket(pack_buf, sizeof(pack_buf), &pack_bytes) ) {
            logDebug("socket send database packet bytes[%d]: %s\n", pack_bytes, pack_buf);
            if( socketSend(&sock, pack_buf, pack_bytes) < 0 ) {
                logError("socket send database packet failure\n");
                socketTerm(&sock);
            }
            else {
                logWarn("socket send database packet success, remove it from database now\n");
                databaseDelPacket();
            }
        }
        
        msleep(50);
    }
    
 Cleanup:
  	socketTerm(&sock);
    databaseTerm();
    unlink(DAEMON_PIDFILE);
    logTerm();

    return 0;
}

int checkSampleTime(time_t *last_time, int interval) {

    int                  flag = 0;
    time_t               t = time(&t);
      
    if( t >= *last_time + interval ) {
        flag = 1;
        *last_time = t;
    }

    return flag;
}
