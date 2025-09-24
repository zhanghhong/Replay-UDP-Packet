#define _WIN32_WINNT 0x0600  
#define _USE_MATH_DEFINES
#include "udp_sender.h"
#include <iostream>
#include <thread>
#include <ctime>
#include <chrono>
#include <atomic>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include "mod-config/config_manager.h"

UdpSender::UdpSender()
{
    m_destIP = ConfigManager::instance()->getConfig().ip_dst;
    m_destPort = ConfigManager::instance()->getConfig().port_dst;

    m_bind_address = ConfigManager::instance()->getConfig().ip_dst;
    m_port = ConfigManager::instance()->getConfig().port_dst;

    m_socket = -1;

    m_DstAddr.sin_family = AF_INET;
    m_DstAddr.sin_addr.S_un.S_addr = inet_addr(m_destIP.c_str());
    m_DstAddr.sin_port = htons( m_destPort);
}

UdpSender::~UdpSender()
{    
    if (m_socket != -1) {
#ifdef _WIN32
        closesocket(m_socket);
        WSACleanup();
#else
        close(m_socket);
#endif
    }
}

bool UdpSender::Init()
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup 失败" << std::endl;
            return false;
    }
#endif

    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_socket == -1) {
        std::cerr << "创建套接字失败" << std::endl;
            return false;
    }

    // 设置发送缓冲区大小
    int sendBufSize = 1024 * 1024; // 1MB
    if (setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufSize, sizeof(sendBufSize)) < 0) {
        std::cerr << "设置发送缓冲区大小失败" << std::endl;
    }

// 设置非阻塞模式
#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(m_socket, FIONBIO, &mode);
#else
    int flags = fcntl(m_socket, F_GETFL, 0);
    fcntl(m_socket, F_SETFL, flags | O_NONBLOCK);
#endif

    // 绑定地址和端口
    struct sockaddr_in bind_addr;
    std::memset(&bind_addr, 0, sizeof(bind_addr));
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_port = htons(m_port);

    if (m_bind_address == "0.0.0.0") {
        bind_addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        bind_addr.sin_addr.s_addr = inet_addr(m_bind_address.c_str());
    }

    if (bind(m_socket, (struct sockaddr*)&bind_addr, sizeof(bind_addr)) < 0) {
        std::cerr << "UdpSender: Bind failed on port " << m_port << std::endl;
        closesocket(m_socket);
        return false;
    }

    return true;
}

void UdpSender::Send(const char* packet, int packet_size)
{
     sendto(m_socket, packet, packet_size, 0, (struct sockaddr*)&m_DstAddr, sizeof(m_DstAddr));
}
