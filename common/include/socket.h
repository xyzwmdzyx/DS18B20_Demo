/********************************************************************************
 *		Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  socket.h
 *    Description:  This file is a socket function declare file.
 *
 *        Version:  1.0.0(2024年03月19日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月19日 21时49分20秒"
 *                 
 ********************************************************************************/

#ifndef _SOCKET_H_
#define	_SOCKET_H_

#define HOSTNAME_LEN	64
#define TIMEOUT_NONE    0

typedef struct {
	
	char	host[HOSTNAME_LEN];	// for client: server hostname or IP address; for server: no use
	int		port;				// for client: server port; for server: listen port
	int		fd;					// file descriptor
	
}socket_ctx_t;

/*	description:	initial socket context
 *	 input args:	
 *					$sock: socket context pointer
 *					$host: connect server hostname for client mode, unused for server mode  
 *					$port: connect server port for client mode, listen port for server mode
 * return value:    <0: failure  0: success
 */
extern int socketInit(socket_ctx_t *sock, char *host, int port);


/*	description:	close socket
 *	 input args:	
 *					$sock: socket context pointer
 * return value:    <0: failure  0: success
 */
extern int socketTerm(socket_ctx_t *sock);


/*  description: 	server socket start listen a port
 *   input args:
 *               	$sock:  socket context pointer
 * return value: 	<0: failure   0: success
 */
extern int socketListen(socket_ctx_t *sock);


/*	description:	client socket connect to server socket in block mode
 *	 input args:	
 *					$sock: socket context pointer
 * return value:    <0: failure  0: success
 */
extern int socketConnect(socket_ctx_t *sock);


/*  description: 	send data from socket
 *   input args:
 *               	$sock :  socket context pointer
 *               	$data :  data will be sent
 *               	$bytes:  data bytes
 * return value: 	<0: failure   0: success
 */
extern int socketSend(socket_ctx_t *sock, char *data, int bytes);


/*  description: 	receive data from socket
 *   input args:
 *               	$sock :  socket context pointer
 *               	$buf  :  receive data buffer
 *               	$size :  receive data buffer size
 *                $timeout:  receive data time, <=0 will never timeout
 * return value: 	<0: failure   0: success
 */
extern int socketRecv(socket_ctx_t *sock, char *buf, int size, int timeout);

/*  description: 	check socket connected or not
 *   input args:
 *               	$sockfd :	socket fd
 * return value: 	<0: failure   0: success
 */
extern int socketCheckConnect(int sockfd);


/*  description: 	send data from socket
 *   input args:
 *               	$sock :  socket context pointer
 *               	$data :  data will be sent
 *               	$bytes:  data bytes
 * return value: 	<0: failure   0: success
 */
extern int socketSend(socket_ctx_t *sock, char *data, int bytes);


/*  description: 	receive data from socket
 *   input args:
 *               	$sock :  socket context pointer
 *               	$buf  :  receive data buffer
 *               	$size :  receive data buffer size
 *                $timeout:  receive data time, <=0 will never timeout
 * return value: 	<0: failure   0: success
 */
extern int socketRecv(socket_ctx_t *sock, char *buf, int size, int timeout);


/*  description: 	check socket connected or not
 *   input args:
 *               	$sockfd :	socket fd
 * return value: 	<0: failure   0: success
 */
extern int socketCheckConnect(int sockfd);


/*  description: 	set socket listen port as reusable, fix port already used bug 
 *   input args:
 *               	$sockfd :	socket fd
 * return value: 	<0: failure   0: success
 */
extern int socketSetReuseaddr(int sockfd);


/*  description: 	set socket as non-block mode, common socket default work as block mode
 *   input args:
 *               	$sockfd :	socket fd
 * return value: 	<0: failure   >0: success
 */
extern int socketSetNonblock(int sockfd);


/*  description: 	set socket receive and send buffer size in linux kernel space
 *   input args:
 *               	$sockfd :	socket fd
 * 					$rsize  :	recive buffer size
 * 					$ssize	:   send buffer size
 * return value: 	<0: failure   0: success
 */
extern int socketSetBuffer(int sockfd, int rsize, int ssize);


/*
 * Enable socket SO_KEEPALIVE, if the connection disconnected, any system call on socket
 * will return immediately and errno will be set to "WSAENOTCONN"
 *
 * keepalive is not program related, but socket related, * so if you have multiple sockets,
 * you can handle keepalive for each of them separately.
 *
 */
extern int socketSetKeepalive(int sockfd, int keepintvl, int keepcnt)


/*  description: 	Set open file description count to max */
extern void setSocketRlimit(void);


#endif
