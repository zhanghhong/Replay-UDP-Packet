#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H
#include "../mod-interface/inner_struct.h"

class ConfigManager
{
public:
    ConfigManager();
    ~ConfigManager();

private:
    static ConfigManager* singleton; // 设置单例
    config m_para;

public:
    static ConfigManager* instance(); // 获取单例

    int loadConfig(); // 加载配置文件
    bool UpdateConfig(const config& params); // 更新配置文件

    config getConfig(); // 返回配置 - 网络配置
};

#endif // CONFIGMANAGER_H
