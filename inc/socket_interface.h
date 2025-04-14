#ifndef __SOCKET_INTERFACE_H__
#define __SOCKET_INTERFACE_H__
#include "control.h"
#include "socket.h"

//定义服务器IP地址和端口号
#define IPPORT "8192"
#define IPADDR "192.168.10.36"
#define BUFFER_SIZE 6

struct control *add_socket_to_cntl_list(struct control *phead);

#endif /* __SOCKET_INTERFACE_H__ */