#include"../../inc/client/NetConnector.h"

int main(int argc, char const *argv[])
{
    std::string server_ip;
    std::cout << "请输入目标服务器的ip地址：  \n";
    std::cin >> server_ip;

    NetConnector m_connector(server_ip.c_str(),8900,AF_INET);
    m_connector.StartConnect();

    
    return 0;
}
