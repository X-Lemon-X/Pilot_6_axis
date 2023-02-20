    
#include "SendData.h"
#include <WiFiUdp.h>  
#include <string>
#include <stdlib.h>  
#include <Arduino.h>
    
using namespace SendDataProtocol;

Send_WIFI_UDP::Send_WIFI_UDP(std::string ip, int port)
{
    this->udp = new WiFiUDP();
    this->ip=ip;
    this->port=port;
}

void Send_WIFI_UDP::SendData(std::string data)
{
  uint8_t array[data.size()];
  for (size_t i = 0; i < data.size(); i++) {array[i] = (uint8_t)data[i];}

  udp->beginPacket(this->ip.c_str(), this->port);
  size_t i = udp->write(array, data.size());
  
  #ifdef DEBUG
    Serial.print("UDP bytes to send:");
    Serial.println(i);
  #endif

  udp->endPacket();
    
}
