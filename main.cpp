#include <QCoreApplication>
#include "udp_sender.h"
#include "offline_udp_reader.h"
#include "mod-ose/ose_time_sys.h"
#include "mod-config/config_manager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    auto config = ConfigManager::instance();

    UdpSender* udpSender = new UdpSender();
    udpSender->Init();

    OfflineUdpReader our;
    our.init(config->getConfig().path);

    offline_udp_info_t t = our.ReadFileInfo();
    printf("ReadFileInfo: %f %f %f\n", t.start_time, t.end_time, t.packet_total_count);

    our.start();

    bool have_data = true;
    int count = 0;
    while(have_data)
    {
        offline_udp_packet_t t;
        if(our.getNextPacket(t))
        {
            if(count == 0)
            {
                OSE_TIME_SYS::TimeSync(t.RecvTime - 1);
            }

            while(1)
            {
                if(OSE_TIME_SYS::GetUTCTime() > t.RecvTime)
                {
                    printf("%d\n",t.count);
                    udpSender->Send(t.data.data(), t.length);
                    break;
                }
            }
        }
        else
            have_data = false;
    }
    return a.exec();
}
