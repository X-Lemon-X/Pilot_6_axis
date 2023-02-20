
#include <WiFiUdp.h>
#include <string>
#include <stdlib.h>

#ifndef SENDDATA_H
#define SENDDATA_H
#endif


#ifdef SENDDATA_H

namespace SendDataProtocol
{
    //#define DEBUG

    class Send_WIFI_UDP
    {
    private:
        WiFiUDP *udp;
        std::string ip;
        int port;

    public:
        Send_WIFI_UDP(std::string ip, int port);
        ~Send_WIFI_UDP(){}
        void SendData(std::string buffer);

        std::string GetIp()
        {
            return this->ip;
        }
    };
    



} 

#endif