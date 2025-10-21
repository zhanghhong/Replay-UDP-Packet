# Replay-UDP-Packet API

本文档简要说明公开类与主要方法（不涉及 mod-ose 解释）。

## OfflineUdpReader
离线UDP文件读取与缓存。

```cpp
class OfflineUdpReader {
public:
    OfflineUdpReader();
    ~OfflineUdpReader();

    offline_udp_info_t ReadFileInfo();                   // 获取文件统计信息
    void init(const std::string& newFile_path);          // 初始化并设置文件路径
    void start();                                        // 启动读取线程
    void stop();                                         // 停止读取线程
    void loop();                                         // 读取主循环（内部线程使用）

    bool isRunning() const;                              // 线程是否运行
    bool getNextPacket(offline_udp_packet_t& packet);    // 从队列获取一个包
};
```

- 数据类型定义见 `mod-interface/inner_struct.h`
- 内部包含线程、安全队列与文件指针

## UdpSender
UDP发送封装（跨平台）。

```cpp
class UdpSender {
public:
    UdpSender();
    ~UdpSender();

    bool Init();                                         // 初始化（创建socket、设置目标等）
    bool stop();                                         // 关闭socket
    void Send(const char* packet, int packet_size);      // 发送单个UDP数据包
};
```

- Windows 依赖 `ws2_32`
- Linux 依赖标准BSD Sockets

## ConfigManager
配置管理（网络与路径等）。

```cpp
class ConfigManager {
public:
    static ConfigManager* instance();                    // 单例

    int loadConfig();                                    // 加载配置文件
    bool UpdateConfig(const config& params);             // 更新配置
    config getConfig();                                  // 获取当前配置
};
```

- 配置结构见 `mod-interface/inner_struct.h`

## 主程序流程（main.cpp）
- 读取配置 -> 初始化 `UdpSender`
- 初始化并启动 `OfflineUdpReader`
- 循环取包，根据 `RecvTime` 同步/等待后调用 `UdpSender::Send`

## 典型用法
```cpp
auto config = ConfigManager::instance();
UdpSender sender; sender.Init();
OfflineUdpReader reader; reader.init(config->getConfig().path); reader.start();

offline_udp_packet_t pkt;
while (reader.getNextPacket(pkt)) {
    // 根据 pkt.RecvTime 做时间同步/等待
    sender.Send(pkt.data.data(), pkt.length);
}
```
