#ifndef OFFLINEUDPREADER_H
#define OFFLINEUDPREADER_H

#include "mod-interface/inner_struct.h"
#include <atomic>
#include <mutex>
#include <queue>
#include <thread>

class OfflineUdpReader
{
public:
    OfflineUdpReader();
    ~OfflineUdpReader();

    // 获取文件统计信息
    offline_udp_info_t ReadFileInfo();

    // 初始化
    void init(const std::string &newFile_path);

    // 启动线程
    void start();

    // 停止线程
    void stop();

    // 循环读取数据
    void loop();

    // 检查线程是否运行
    bool isRunning() const { return m_running.load(); }

    // 获取单个缓存数据
    bool getNextPacket(offline_udp_packet_t& packet);

private:
    FILE*           fp;
    std::string     m_file_path;

    std::queue<offline_udp_packet_t> m_buf;
    offline_udp_info_t m_udp_info = {};

    // 线程相关
    std::thread m_thread;                       // 线程
    std::atomic<bool> m_running = false;        // 运行状态标志
    std::mutex m_mutex;                         // 互斥锁
};

#endif // OFFLINEUDPREADER_H
