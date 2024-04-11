/*********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  socket.c
 *    Description:  This file is a socket function file.
 *                 
 *        Version:  1.0.0(2024年03月19日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月19日 21时46分50秒"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/un.h>
#include <poll.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/resource.h>

#include "socket.h"
#include "logger.h"


/*	description:	initial socket context
 *	 input args:	
 *					$sock: socket context pointer
 *					$host: connect server hostname for client mode, unused for server mode  
 *					$port: connect server port for client mode, listen port for server mode
 * return value:    <0: failure  0: success
 */
int	socketInit(socket_ctx_t *sock, char *host, int port) {

	// check input args
	if( !sock || !host || port <= 0 ) {
		return -1;
	}
	
	// init socket context
	memset(sock, 0, sizeof(*sock));
	sock -> port = port;
	sock -> fd = -1;
	
	// server no need this
	if( host ) {
		strncpy(sock -> host, host, HOSTNAME_LEN);
	}

	return 0;
}


/*	description:	close socket
 *	 input args:	$sock: socket context pointer
 * return value:    <0: failure  0: success
 */
int socketTerm(socket_ctx_t *sock) {

	// check input args
	if( !sock ) {
		return -1;
	}

	// close socket
	if( sock -> fd > 0 ) {
		close(sock -> fd);
		sock -> fd = -1;
	}
	
	return 0;
}


/*  description: 	server socket start listen a port
 *   input args:
 *               	$sock:  socket context pointer
 * return value: 	<0: failure   0: success
 */
int socketListen(socket_ctx_t *sock) {
    int                 	rv = 0;
    int						sockfd = -1;
    int                 	backlog = 15;
    int						on = 1;
    struct sockaddr_in		addr;

    if( !sock ) {
    	return -1;
    }
        
    // make sure this is not an already exist socket
	socketClose(sock);
	
 	// set max open socket count
    set_socket_rlimit();
    
    // create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if( sockfd < 0 ) {
    	return -2;
    }
    sock -> fd = sockfd;
    
    // fix port already used bug
    rv = socketSetReuseaddr(sock -> fd);
    if( rv < 0 ) {
    	return -2;
    }
    
    // set IP address and port
    memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
    addr.sin_port = htons(sock -> port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
    // bind on port
    rv = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if( rv < 0 ) {
		return -3;	
	}
	
	// start listen port
	rv = listen(sockfd, backlog);
	if( rv < 0 ) {
		return -4;
	}
	
	return rv;
}


/*	description:	client socket connect to server socket in block mode
 *	 input args:	
 *					$sock: socket context pointer
 * return value:    <0: failure  0: success
 */
int socketConnect(socket_ctx_t *sock) {

	int                 	rv = 0;
    int                 	sockfd = -1;
    char                	service[24];
    struct addrinfo     	hints, *rp;
    struct addrinfo    		*res = NULL;
    struct in_addr      	inaddr;
    
	if( !sock ) {
		return -1;
	}
	
	// make sure this is not an already exist socket
	socketClose(sock);

	memset(&hints, 0, sizeof(hints));
	// IPv4
	hints.ai_family = AF_INET;
	// TCP
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// if $host is a valid IP address, then don't use name resolution
	if( inet_aton(sock -> host, &inaddr) ) {
		hints.ai_flags |= AI_NUMERICHOST;
	}
	
	// do name resolution
	snprintf(service, sizeof(service), "%d", sock -> port);
	if( rv == getaddrinfo(sock -> host, service, &hints, &res) ) {
		logError("function getaddrinfo() parser [%s:%s] failure: %s\n", sock -> host, service, gai_strerror(rv));
		return -2;
	}
	
	// try each address until successfully connect or bind
	for(rp = res, rp != NULL; rp = rp -> ai_next) {
		// create socket
		sockfd = socket(rp -> ai_family, rp -> ai_socktype, rp -> ai_protocol);
		if( sockfd < 0 ) {
			logError("function socket() create socket fd faliure: %s\n", strerror(errno));
			rv = -3;
			continue;
		}
		// connect to server
		rv = connect(sockfd, rp -> ai_addr, sizeof(struct sockaddr_in));
		if( 0 == rv ) {
			sock -> fd = sockfd;
			logInfo("connect to server[%s:%d] on fd[%d] success!\n", sock -> host, sock -> port, sockfd);
			break;
		}
		else {
			// socket connect failure, try another IP address
			close(sockfd);
			continue;
		}
	}
	
	freeaddrinfo(res);
	
	return rv;
}


/*  description: 	send data from socket
 *   input args:
 *               	$sock :  socket context pointer
 *               	$data :  data will be sent
 *               	$bytes:  data bytes
 * return value: 	<0: failure   0: success
 */
int socketSend(socket_ctx_t *sock, char *data, int bytes) {

	int		rv = 0;
	int		i = 0;
	int		left_bytes = bytes;

	// check input args
	if( !sock || !data || bytes <= 0 ) {
		return -1;
	}

	// make sure write() has already put all data into system buffer
	while( left_bytes > 0 ) {
		rv = write(sock -> fd, &data[i], left_bytes);
		if( rv < 0 ) {
			logInfo("socket[%d] write() data failure: %s, close socket now\n", sock -> fd, strerror(errno));
			socketTerm(sock);
			return -2;
		}

		else if( rv == left_bytes ) {
			logInfo("socket[%d] write() %d bytes data over\n", sock -> fd, bytes);
			return 0;
		} 

		else {
			// continue put left data into system buffer
			i += rv;
			left_bytes -= rv;
			continue;
		}
	}

	return 0;
}


/*  description: 	receive data from socket
 *   input args:
 *               	$sock :  socket context pointer
 *               	$buf  :  receive data buffer
 *               	$size :  receive data buffer size
 *                $timeout:  receive data time, <=0 will never timeout
 * return value: 	<0: failure   0: success
 */
int socketRecv(socket_ctx_t *sock, char *buf, int size, int timeout) {

    int 				rv = 0;
    int         		maxfd;
	fd_set      		rdset;
	struct timeval  	tv;

	// check input args
    if( !sock || !buf || size <= 0 ) {
		return -1;
	}

    memset(buf, 0, size);

    maxfd = sock -> fd;
    FD_ZERO(&rdset);
    FD_SET(sock -> fd, &rdset);

	// never timeout
    if( timeout <= 0 ) {
        rv = select(maxfd + 1, &rdset, NULL, NULL, NULL);
    }
	// has timeout
    else {
        tv.tv_sec = timeout;
        tv.tv_usec = 0;
        rv = select(maxfd + 1, &rdset, NULL, NULL, &tv);
    }

    if( rv < 0 ) {
        logError("select() on socket[%d] gets faliure: %s\n", sock -> fd, strerror(errno));
        return -2;
    }
    else if( 0 == rv ) {
        logError("select() on socket[%d] gets timeout\n", sock -> fd);
        return 0;
    }
    else {
        rv = read(sock -> fd, buf, size);
        if( rv <= 0 ) {
            logError("socket[%d] read() data failure or gets disconnected: %s, close socket now\n", sock -> fd, strerror(errno));
            socketTerm(sock);
            return -3;
        }
        else {
            logDebug("socket[%d] read() %d bytes data\n", sock -> fd, rv);
            return rv;
        }
    }
}


/*  description: 	check socket connected or not
 *   input args:
 *               	$sockfd :	socket fd
 * return value: 	<0: failure   0: success
 */
int socketCheckConnect(int sockfd) {

    struct tcp_info   	info;
    int               	len = sizeof(info);

    if( sockfd < 0 ) {
		return -1;
	}

    getsockopt(sockfd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);

    if( TCP_CLOSE == info.tcpi_state || TCP_CLOSING == info.tcpi_state || TCP_CLOSE_WAIT == info.tcpi_state ) {
        return -2;
    }

    return 0;
}


/*  description: 	set socket listen port as reusable, fix port already used bug 
 *   input args:
 *               	$sockfd :	socket fd
 * return value: 	<0: failure   0: success
 */
int socketSetReuseaddr(int sockfd) {

    int 		opt = 1;
    int 		len = sizeof (int);

    if ( setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, (void *) &opt, len) ) {
        logError("Set socket[%d] option SO_REUSEADDR failure:%s\n", sockfd, strerror(errno));
        return -1;
    }
    logDebug("Set socket[%d] option SO_REUSEADDR success\n", sockfd);

    return 0;
}


/*  description: 	set socket as non-block mode, common socket default work as block mode
 *   input args:
 *               	$sockfd :	socket fd
 * return value: 	<0: failure   >0: success
 */
int socketSetNonblock(int sockfd) {

    int 		opts;

    /*
     * fcntl may set:
     *
     * EACCES, EAGAIN: Operation is prohibited by locks held by other
     *          processes. Or, operation is prohibited because the file has
     *          been memory-mapped by another process.
     * EBADF:   fd is not an open file descriptor, or the command was F_SETLK
     *          or F_SETLKW and the file descriptor open mode doesn't match
     *          with the type of lock requested.
     * EDEADLK: It was detected that the specified F_SETLKW command would
     *          cause a deadlock.
     * EFAULT:  lock is outside your accessible address space.
     * EINTR:   For F_SETLKW, the command was interrupted by a signal. For
     *          F_GETLK and F_SETLK, the command was interrupted by a signal
     *          before the lock was checked or acquired. Most likely when
     *          locking a remote file (e.g. locking over NFS), but can
     *          sometimes happen locally.
     * EINVAL:  For F_DUPFD, arg is negative or is greater than the maximum
     *          allowable value. For F_SETSIG, arg is not an allowable signal
     *          number.
     * EMFILE:  For F_DUPFD, the process already has the maximum number of
     *          file descriptors open.
     * ENOLCK:  Too many segment locks open, lock table is full, or a remote
     *          locking protocol failed (e.g. locking over NFS).
     * EPERM:   Attempted to clear the O_APPEND flag on a file that has the
     *          append-only attribute set.
     */

    opts = fcntl(sockfd, F_GETFL);
    if ( opts < 0 ) {
        logWarn("fcntl() get socket options failure: %s\n", strerror(errno));
        return -1;
    }

    opts |= O_NONBLOCK;

    if ( fcntl(sockfd, F_SETFL, opts) < 0 ) {
        logWarn("fcntl() set socket options failure: %s\n", strerror(errno));
        return -2;
    }

    logDebug("Set socket[%d] none blocking success\n", sockfd);
    return opts;
}


/*  description: 	set socket receive and send buffer size in linux kernel space
 *   input args:
 *               	$sockfd :	socket fd
 * 					$rsize  :	recive buffer size
 * 					$ssize	:   send buffer size
 * return value: 	<0: failure   0: success
 */
int socketSetBuffer(int sockfd, int rsize, int ssize) {

    int        		opt;
    socklen_t  		optlen = sizeof(opt);

	// check input args
    if( sockfd < 0 ) {
		return -1;
	}

    // get system default receive buffer size, Linux X86: 85K
    if( getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char *)&opt, &optlen) ) {
        logWarn("getsockopt() get receive buffer failure: %s\n", strerror(errno));
        return -2;
    }

    // only when current receive buffer size larger than the default one will change it
    if( rsize > opt ) {
        opt = (int)rsize;
        if( setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char *)&opt, optlen) ) {
            logWarn("setsockopt() set receive buffer to %d failure: %s\n", opt, strerror(errno));
            return -2;
        }
    }

    // get system default send buffer size, Linux X86: 16K
    if( getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *)&opt, &optlen) ) {
        logWarn("getsockopt() get send buffer failure: %s\n", strerror(errno));
        return -3;
    }

    // only when current send buffer size larger than the default one will change it
    if( ssize > opt ) {
        opt = (int)ssize;
        if( setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *)&opt, optlen) ) {
            logWarn("setsockopt() set send buffer to %d failure: %s\n", opt, strerror(errno));
            return -3;
        }
    }

    logInfo("set socket[%d] RCVBUF size:%d  SNDBUF size:%d\n", sockfd, rsize, ssize);
    return 0;
}


/*
 * Enable socket SO_KEEPALIVE, if the connection disconnected, any system call on socket
 * will return immediately and errno will be set to "WSAENOTCONN"
 *
 * keepalive is not program related, but socket related, * so if you have multiple sockets,
 * you can handle keepalive for each of them separately.
 *
 */
int socketSetKeepalive(int sockfd, int keepintvl, int keepcnt) {

    int  opt;

	// check input args
    if( sockfd < 0 ) {
		return -1;
	}

    /* when SO_KEEPALIVE is enabled, if no data is exchanged for a certain period of time, 
     * TCP automatically sends a keepalive probe packet to the other party to ensure that 
     * the connection is still active. 
     */
    opt = 1;
    if( setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *)&opt, sizeof(opt)) ) {
        logError("setsockopt() enable SO_KEEPALIVE failure: %s\n", strerror(errno));
        return -2;
    }

    if( keepintvl || keepcnt ) {
       	/* TCP_KEEPIDLE is used to set the amount of time before a TCP keep-alive probe packet is sent 
       	 * for the first time after being idle. This time is the amount of time that must be waited 
       	 * between two packet transmissions, and if no data is exchanged during this time, TCP sends a 
       	 * keep-alive probe packet. 
       	 */
		// 3 seconds
        opt = 3; 
        if( setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, (char *)&opt, sizeof(opt)) ) {
            logError("setsockopt() set TCP_KEEPIDLE to %d seconds failure: %s\n", opt, strerror(errno));
            return -3;
        }

        if( (opt = keepintvl) > 0 ) {
			/* TCP_KEEPINTVL is used to set the time interval between sending a keep-alive probe packet after a 
			 * connection is detected as idle. If the first keep-alive probe packet (trigger time set by TCP_KEEPIDLE) 
			 * does not receive a response, the time interval set by TCP_KEEPINTVL determines how often subsequent 
			 * keep-alive probe packets are sent.
			 */
            if ( setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL, (char *)&opt, sizeof(opt)) ) {
                logError("setsockopt() set TCP_KEEPINTVL to %d failure: %s\n", opt, strerror(errno));
                return -4;
            }
        }

        if( (opt = keepcnt) > 0 ) {
        	/* TCP_KEEPCNT is used to set how many keep-alive probe packets TCP should send before closing 
        	 * an idle connection.
        	 */
            if ( setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT, (char *)&opt, sizeof(opt)) ) {
                logError("setsockopt() set TCP_KEEPCNT to %d failure: %s\n", opt, strerror(errno));
                return -5;
            }
        }
    }

    logDebug("Set socket[%d] KEEPINTVL:%d  KEEPCNT:%d\n", sockfd, keepintvl, keepcnt);
    return 0;
}


/*  description: 	Set open file description count to max */
void setSocketRlimit(void) {

    struct rlimit limit = {0};

    getrlimit(RLIMIT_NOFILE, &limit );
    limit.rlim_cur  = limit.rlim_max;
    setrlimit(RLIMIT_NOFILE, &limit );

    logInfo("set socket open fd max count to %d\n", limit.rlim_max);
	return;
}
