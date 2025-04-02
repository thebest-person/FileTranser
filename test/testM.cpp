#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }

    // 错误修复：声明字符数组而非单个字符
    char hostname[256] = {0}; // 缓冲区大小需足够存储主机名
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        std::cerr << "Error getting hostname: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // 错误修复：直接传递 hostname 数组（非指针）
    struct hostent* host = gethostbyname(hostname);
    if (host == nullptr) {
        std::cerr << "gethostbyname error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // 遍历所有 IP 地址
    for (int i = 0; host->h_addr_list[i] != nullptr; ++i) {
        char* ip = inet_ntoa(*(struct in_addr*)host->h_addr_list[i]);
        std::cout << "IPv4 Address " << i+1 << ": " << ip << std::endl;
    }

    WSACleanup();
    return 0;
}