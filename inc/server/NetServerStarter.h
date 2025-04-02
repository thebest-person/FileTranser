#ifndef __NET_SERVER_STARTER__H__
#define __NET_SERVER_STARTER__H__

#include<vector>
#include<ws2tcpip.h>
#include<winsock2.h>
#include<string.h>
#include<iostream>
#include<thread>
#include<fstream>
#include<chrono>


class NetServerStarter
{
public:
    short local_ip_family;                       //存储ip地址族/地址类型
    short local_port;                            //存储服务器端口
    char local_ip[16];                           //存储服务器ip

    unsigned short max_listen_amount;            //保存最大监听数量
    unsigned short left_listen_amount;           //保存剩余监听数量

    sockaddr_in  server_addr;                    //保存服务器网络地址
    std::vector<sockaddr_in> client_addr;        //保存连接到的客户端的网络地址
    std::vector<SOCKET> client_infoTrans_socket; //保存和连接到的客户端通信的套接字

    SOCKET local_server_start_init_socket;       //保存服务器开启监听和接收用的套接字

private:
    std::vector<std::thread> client_task_thread;  //保存开辟处理客户端任务的线程



public:
    NetServerStarter(const short server_ip_family,const short server_port,const char* server_ip);
    ~NetServerStarter();

    short StartRunning();

private:
    void ClientTask(SOCKET communicate_socket);
};








#endif 