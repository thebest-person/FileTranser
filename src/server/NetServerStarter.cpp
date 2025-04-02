#include"..\..\inc\server\NetServerStarter.h"
#include"..\..\inc\server\FileControler.h"


NetServerStarter::NetServerStarter(const short server_ip_family,const short server_port,const char* server_ip)
:local_ip_family(server_ip_family),local_port(server_port)
{
    /*初始化成员*/
    strncpy(local_ip,server_ip,15);
    local_ip[15] = 0;

    /*初始化服务器地址*/
    if (inet_pton(local_ip_family, local_ip, &server_addr.sin_addr) <= 0) {
        std::cerr << "IP地址转换失败: " << local_ip << std::endl;
        closesocket(local_server_start_init_socket);
        return;
    }
    server_addr.sin_family = local_ip_family;
    server_addr.sin_port = htons(local_port);

    /*初始化Winsock*/
    WSADATA wsa_temp_databuf;
    if(WSAStartup(MAKEWORD(2,2),&wsa_temp_databuf) != 0)
    {
        std::cout << "WinSock初始化失败\n" << std::endl;
        exit(-1);
    }


    /*初始化服务器最初建交的套接字*/
    local_server_start_init_socket = socket(AF_INET,SOCK_STREAM,0);
    if (local_server_start_init_socket == INVALID_SOCKET) 
        std::cerr << "套接字创建失败，错误码: " << WSAGetLastError() << std::endl;

}

NetServerStarter::~NetServerStarter()
{
    for(auto ep_thread = client_task_thread.begin(); ep_thread != client_task_thread.end(); ep_thread++)
        ep_thread->join();
}



short NetServerStarter::StartRunning()
{

    
    /*0.先给服务器绑定通信地址*/
    if(bind(local_server_start_init_socket,reinterpret_cast<sockaddr*>(&server_addr),sizeof(server_addr)) < 0)
    {
        std::cout << "服务器绑定通信地址失败\n";
        return -1;
    }



    /*1.首先开启服务器监听,并设置最大监听数量*/
    std::cout << "请输入最大监听数量： \n";
    std::cin >> max_listen_amount;
    left_listen_amount = max_listen_amount;

    int lis_res = listen(local_server_start_init_socket,max_listen_amount);
    if(lis_res < 0)
    {
        std::cout << "服务器监听失败" << std::endl;
        return -1;
    }
    /*2.绑定的通信地址监听接收到后，开始Tcp循环接收返回建立的通讯套接字*/
    //
    while(left_listen_amount --)
    {
        /*1.使用局部变量暂时记忆一下创建的和客户端进行通信的套接字和客户端的地址*/
        sockaddr_in temp_accpeted_client_addr;
        SOCKET temp_client_socket_accepter = accept(local_server_start_init_socket,reinterpret_cast<sockaddr*>(&temp_accpeted_client_addr),nullptr);

        /*2.将客户端地址和对应的客户端通信套接字存入容器当中*/
        client_addr.push_back(temp_accpeted_client_addr);
        client_infoTrans_socket.push_back(temp_client_socket_accepter);

        /*3.开启对应线程处理客户端任务*/
        client_task_thread.push_back(std::thread(ClientTask,this,temp_client_socket_accepter)); //注意成员函数要添加this指针
    }

    return 0;
}




void NetServerStarter::ClientTask(SOCKET communicate_socket)
{
    FileControl m_file_controler;

    std::string test_screen;
    test_screen.append("服务器中当前可下载的文件有： \n");
    test_screen += m_file_controler.Traverse(R"(.\server_resources)");
    send(communicate_socket,test_screen.c_str(),test_screen.length(),0);



    while(1)   //注意，客户端中如何对面关闭了连接就会陷入死循环中
    {
        char temp_filenum_buf[4];
        if(recv(communicate_socket,temp_filenum_buf,4,0) <= 0)  //接收到要下载的文件的编号 //如果客户端关闭了连接则退出循环
            break;

        /*1.在当前目录中找到对应文件*/
        auto file_iterator = m_file_controler.file_group.find(std::atoi(temp_filenum_buf));
        std::string file_name = file_iterator->second.first;
        std::cout << "找到的文件是： " << file_name << std::endl;

        /*2.打开对应文件并发送内容*/
        char send_buf[1024]={0};
        std::ifstream open_file(file_name,std::ios::binary);   //文件对象，设定权限为读取
        if (open_file) {
            send(communicate_socket,file_iterator->second.second.c_str(),file_name.length(),0);  //把文件名先发送过去
            std::cout << "文件发送中..." << std::endl;
            while (open_file.read(send_buf,1024).good()) {
                int bytes_read = static_cast<int>(open_file.gcount());
                send(communicate_socket, send_buf, bytes_read, 0); // 分块发送
                memset(send_buf,0,1024); //清空缓冲区
            }
            
            //处理最后一次不完整的读取
            if(open_file.gcount() > 0)
                send(communicate_socket,send_buf,open_file.gcount(),0);      // 分块发送


            Sleep(1000); //延迟一秒后发送结束标志，否则可能出现结束和内容一起发送过去导致客户端无法结束接收后陷入阻塞
            /*3.发送完对应内容后关闭文件*/
            open_file.close();
            std::cout << "文件发送结束\n";
            send(communicate_socket,"file is over",strlen("file is over"),0);
        }
        else
            std::cout << "文件打开失败\n";

    }

    //任务执行结束后记得关闭套接字
    closesocket(communicate_socket);
}