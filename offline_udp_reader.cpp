#include "offline_udp_reader.h"
#include <iostream>
#include <thread>


OfflineUdpReader::OfflineUdpReader()
{
}

OfflineUdpReader::~OfflineUdpReader()
{
    if (fp) {
        fclose(fp);
        fp = nullptr;
    }
}

void OfflineUdpReader::init(const std::string &newFile_path)
{
    // 打开文件
    m_file_path = newFile_path;
    const char* orifilename = m_file_path.c_str();
    fp = fopen(orifilename, "rb");
    if (fp == nullptr)
    {
        std::cerr << "Cannot open file!" << std::endl;
        return;
    }
}

void OfflineUdpReader::start()
{
    // 避免重复启动线程
    if (m_running.load()) 
    {
        printf("[%s] The simulation Thread is already running.\n", __FUNCTION__);
        return;
    }

    // 启动线程
    m_running = true;
    m_thread = std::thread(&OfflineUdpReader::loop, this);
    printf("[%s] The thread has been started.\n", __FUNCTION__);
}

void OfflineUdpReader::stop()
{
    if (!m_running.load())
    {
        return;
    }

    // 等待线程结束
    if (m_thread.joinable())
    {
        m_thread.join();
    }
    printf("[%s] The simulation thread has been stopped.\n", __FUNCTION__);
}

offline_udp_info_t OfflineUdpReader::ReadFileInfo()
{
    // 检查文件指针
    if (fp == nullptr)
    {
        return m_udp_info;
    }

    // 检查文件结束
    if (feof(fp))
    {
        return m_udp_info;
    }

    // 读取帧头
    while(!feof(fp))
    {
        udp_packet_info info;
        if(fread(&info, sizeof(udp_packet_info), 1, fp) == 1)
        {
            double cur_time = info.RecvTime;

            if(m_udp_info.start_time == 0) // 如果是初始化值
                m_udp_info.start_time = cur_time;

            if(cur_time > m_udp_info.end_time) // 更新结束时间
                m_udp_info.end_time = cur_time;

            m_udp_info.packet_total_count++; // 统计报文个数

            fseek(fp, info.length, SEEK_CUR);
        }
    }

    fseek(fp, 0, SEEK_SET);

    return m_udp_info;
}

void OfflineUdpReader::loop()
{
    while (m_running.load() )
    {
        // m_buf大小超过1000，不处理
        if(m_buf.size() > 1000)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        // m_buf大小低于1000，触发预取，补充至1500
        while(!feof(fp) && m_buf.size() < 1500)
        {
            offline_udp_packet_t udp;
            udp_packet_info info;
            char buf[65535];

            // 信息部分
            if(fread(&info, sizeof(udp_packet_info), 1, fp) != 1)
            {
                m_running = false;
                break;
            }

            udp.count = info.count;
            udp.length = info.length;
            udp.RecvTime = info.RecvTime;
            // printf("%d\n",udp.count);

            // 读取udp负载部分
            if(fread(buf, sizeof(char), info.length, fp) != info.length)
            {
                m_running = false;
                break;
            }

            udp.data.assign(buf, info.length);
            m_buf.push(udp);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

bool OfflineUdpReader::getNextPacket(offline_udp_packet_t &packet)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_buf.empty()) {
        return false;
    }

    packet = m_buf.front();
    m_buf.pop();
    return true;
}




