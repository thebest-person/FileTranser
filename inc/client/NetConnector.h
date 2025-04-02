#ifndef __NET_CONNECTOR__H__
#define __NET_CONNECTOR__H__

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>
#include <iostream>


class NetConnector
{
public:
    SOCKET netConnector_socket;


    short ip_family;               //存储服务器的Ip的地址族类型
    char server_ip[16];            //存储服务器的ip
    short server_port;             //存储服务器的端口
    sockaddr_in server_addr;       //存储服务器的地址

public:
    NetConnector(const char* ip,const short port,const short ip_family);
    short StartConnect();
};





#endif 