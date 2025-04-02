#include"../../inc/client/NetConnector.h"
#include<memory>
#include<fstream>


NetConnector::NetConnector(const char* ip,const short port,const short ip_family)
{
    /*初始化内部成员*/
    this->ip_family = ip_family;
    this->server_port = htons(port);
    strncpy(server_ip,ip,15);
    server_ip[15] = 0;

    inet_pton(ip_family,server_ip,&server_addr.sin_addr);
    server_addr.sin_family = ip_family;
    server_addr.sin_port = server_port;


    /*初始化Winsock*/
    WSADATA wsa_temp_databuf;
    if(WSAStartup(MAKEWORD(2,2),&wsa_temp_databuf) != 0)
    {
        std::cout << "WinSock初始化失败\n" << std::endl;
        exit(-1);
    }

    /*获取ipv4类型的tcp套接字*/
    netConnector_socket = socket(AF_INET,SOCK_STREAM,0); //获取ipv4类型的tcp套接字并设置自动获取协议
}



short NetConnector::StartConnect()
{
    int conn_res = connect(netConnector_socket,reinterpret_cast<sockaddr*>(&server_addr),sizeof(server_addr));
    if(conn_res < 0)
    {
        std::cout << "连接失败\n" << std::endl;
        return -1;
    }
    char buf[1024];
    recv(netConnector_socket,buf,1024,0);
    std::cout << buf << std::endl;

//增加循环询问要不要进行下一次文件下载和设置程序结束标志
    while(1)
    {
        char temp_filenum_buf[4];
        std::cout << "请输入要下载的文件的编号：  ";
        std::cin >> temp_filenum_buf;
        send(netConnector_socket,temp_filenum_buf,4,0);

        /*在指定目录下创建对应的文件*/
        std::fstream file_recv;
        char file_name[100] = {0};
        std::string store_path = R"(.\recepter_file\)";
        strcpy(file_name,store_path.c_str());
        recv(netConnector_socket,file_name+store_path.length(),80,0);

        std::cout << "接受到的文件是：" <<file_name<< std::endl;
        file_recv.open(file_name,std::ios::out | std::ios::app |std::ios::binary);

        /*打开文件接收发送的内容并写入对应的文件中*/
        while(1)
        {
            memset(buf,0,1024); //情况缓冲区
            if(recv(netConnector_socket,buf,1024,0) < 1) //如果接完毕退出循环
                break;
            if(strcmp(buf,"file is over") == 0)  //如果接受完毕退出循环
                break;
            file_recv.write(buf,sizeof(buf));
        }

        /*写入完毕后关闭打开的文件*/
        std::cout << "文件接收完毕\n";
        file_recv.close();

        /*是否继续下载文件的询问*/
        char continue_symbol = 'n';
        std::cout << "想要继续下载文件还是结束程序？ y/n\n";
        std::cin >> continue_symbol;
        if(continue_symbol == 'N'||continue_symbol == 'n')
            break;

    }

    return 0;
}

