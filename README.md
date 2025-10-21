# Replay-UDP-Packet

轻量级离线UDP数据回放工具：从文件（Storage-Net-Packet生成文件）读取抓包数据，按原始时间戳节奏回放到指定目标地址。

## 特性
- 简洁：极少依赖，易编译、易部署
- 跨平台：Windows / Linux（ubuntu 20.04）
- 时间还原：计算当前系统时间与数据包`RecvTime`的差值，然后按差值进行时间同步，然后按同步后的时间重放，尽量保持原始节奏
- 模块化：读取、回放、配置三部分解耦

## 目录结构
- `offline_udp_reader.*`：离线文件读取与缓冲
- `udp_sender.*`：UDP发送封装
- `mod-config/`：配置加载（不解释 mod-ose）
- `main.cpp`：示例入口
- `doc/`：文档（API等）

## 构建

Windows（MinGW/GCC 示例）：
```bash
./bin/build.bat
```

Linux：
```bash
./bin/build.sh
```

## 运行
1) 准备离线数据文件（结构由[inner_struct.h](mod-interface/inner_struct.h)定义）
2) 配置回放路径与发送目标（见 [config/comm_conf.xml](config/comm_conf.xml)）
3) 执行程序开始回放

## 基本流程
- 读取线程按顺序读取离线包，缓存至队列
- 主循环按时间戳等待，到点后通过`UdpSender`发送

更多接口与示例，见 [doc/API.md](doc/API.md)。
