#include"..\..\inc\server\NetServerStarter.h"

int main(int argc, char const *argv[])
{
    std::string server_ip;
    std::cout << "请输入当前主机的本地Ip作为服务器网络Ip地址： ";
    std::cin >> server_ip;
    NetServerStarter   m_server(AF_INET,8900,server_ip.c_str());
    m_server.StartRunning();
    


    
    return 0;
}
