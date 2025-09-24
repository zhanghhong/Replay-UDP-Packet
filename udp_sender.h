#ifndef UDPSENDER_H
#define UDPSENDER_H
#include <string>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef int socklen_t;
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#endif

class UdpSender
{
public:
    UdpSender();
    ~UdpSender();

    // 初始化UDP套接字
    bool Init();
    // 停止发送
    bool stop();
    // 发送循环
    void Send(const char* packet, int packet_size);
private:
    std::string m_destIP;
    int m_destPort;
    std::string m_bind_address{"0.0.0.0"};
    uint16_t m_port{0};
    int m_socket;
    struct sockaddr_in  m_DstAddr;
};

#endif // UDPSENDER_H
