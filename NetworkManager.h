#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <QNEthernet.h>
#include <QNDNSClient.h>
#include "StorageManager.h"

using namespace qindesign::network;

constexpr uint32_t kDHCPTimeout = 10000;  // 10 seconds
constexpr uint16_t kAudioPort = 8000;
constexpr char kServiceName[] = "teensy-audio-streaming"; //tas

EthernetUDP udp;
uint8_t buf[Ethernet.mtu() - 20 - 8];  // Maximum UDP payload size
                                       // 20-byte IP, 8-byte UDP header

//IPAddress staticIP{192, 168, 1, 1};
//IPAddress subnetMask{255, 255, 255, 0};
//IPAddress gateway{192, 168, 1, 2};

uint8_t macAddressUSBHeader[6] = {0x04, 0xe9, 0xe5, 0x0c, 0xec, 0x21};
uint8_t macAddressNoUSBHeader[6] = {0x04, 0xe9, 0xe5, 0x11, 0x22, 0x7c};
uint8_t* serverMacAddress = macAddressUSBHeader;
uint8_t* clientMacAddress = macAddressNoUSBHeader;

class NetworkManager{
public:
  void Setup(){
    // Print the MAC address
    uint8_t mac[6];
    Ethernet.macAddress(mac);
    Serial.printf("MAC = %02x:%02x:%02x:%02x:%02x:%02x\n",
                  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    
    // Initialize Ethernet, in this case with DHCP
    Serial.println("Starting Ethernet with DHCP...");
    if (!Ethernet.begin()) {
      Serial.println("Failed to start Ethernet");
      return;
    }
    if (!Ethernet.waitForLocalIP(kDHCPTimeout)) {
      Serial.println("Failed to get IP address from DHCP");
      return;
    }
  
    IPAddress ip = Ethernet.localIP();
    Serial.printf("    Local IP     = %u.%u.%u.%u\n", ip[0], ip[1], ip[2], ip[3]);
    ip = Ethernet.subnetMask();
    Serial.printf("    Subnet mask  = %u.%u.%u.%u\n", ip[0], ip[1], ip[2], ip[3]);
    ip = Ethernet.broadcastIP();
    Serial.printf("    Broadcast IP = %u.%u.%u.%u\n", ip[0], ip[1], ip[2], ip[3]);
    ip = Ethernet.gatewayIP();
    Serial.printf("    Gateway      = %u.%u.%u.%u\n", ip[0], ip[1], ip[2], ip[3]);
    ip = Ethernet.dnsServerIP();
    Serial.printf("    DNS          = %u.%u.%u.%u\n", ip[0], ip[1], ip[2], ip[3]);


    // Listen on port and start an mDNS service
    udp.begin(kAudioPort);
    Serial.println("Starting mDNS...");
    if (!MDNS.begin(kServiceName)) {
      Serial.println("ERROR: Starting mDNS.");
    } else {
      if (!MDNS.addService("_tas", "_udp", kAudioPort)) {
        Serial.println("ERROR: Adding service.");
      } else {
        Serial.printf("Started mDNS service:\n"
                      "    Name: %s\n"
                      "    Type: _tas._udp\n"
                      "    Port: %u\n",
                      kServiceName, kAudioPort);
      }
    }
    Serial.println("Waiting for TAS messages...");

    bool initialised = false;

    // TODO: tidy this up please
    while (!initialised) {
      byte savedIP0;
      byte savedIP1;
      byte savedIP2;
      byte savedIP3;
      StorageManager::getRemoteIPAddress(&savedIP0, &savedIP1, &savedIP2, &savedIP3);
      Serial.printf("Please give me the remote IP address or press enter to attempt %u.%u.%u.%u:\n", savedIP0, savedIP1, savedIP2, savedIP3);
      uint8_t count = 10;
      Serial.print("Auto-reconnect in: ");
      while(!Serial.available() && count > 0){
        if (udp.parsePacket()){
          remoteNodeIP = udp.remoteIP();
          Serial.print("Nevermind, we have incoming data from: ");
          Serial.println(remoteNodeIP);
          StorageManager::saveRemoteIPAddress(remoteNodeIP[0], remoteNodeIP[1], remoteNodeIP[2], remoteNodeIP[3]);
          return;
        }
        Serial.printf("%u, ", count);
        delay(1000);
        count--;
      }
      String ipString = Serial.readStringUntil('\n');
      if (!ipString.length()){
        Serial.print("Setting remote to: ");
        remoteNodeIP = IPAddress(savedIP0, savedIP1, savedIP2, savedIP3);
        Serial.println(remoteNodeIP);
        initialised = true;
        // returning so that we dont "waste" a write to the EEPROM because it's limited.
        return;
      } else {
        Serial.println(ipString);
        initialised = remoteNodeIP.fromString(ipString);
      }
      
      if (!initialised){
        Serial.println("Invalid IP address.");
      } else {
        Serial.print("Thankyou. Remote IP is:");
        Serial.println(remoteNodeIP);
        StorageManager::saveRemoteIPAddress(remoteNodeIP[0], remoteNodeIP[1], remoteNodeIP[2], remoteNodeIP[3]);
      }
    }
  }

  bool sendAudioBuffers(byte audioBufferLeft[256], byte audioBufferRight[256]){
    byte audioPacket[512];
    memcpy(audioPacket, audioBufferLeft, 256);
    memcpy(&audioPacket[256], audioBufferRight, 256);
    return udp.send(remoteNodeIP,  kAudioPort, audioPacket, 512);
  }

  bool receiveAudioBuffers(byte outputAudioBufferLeft[256], byte outputAudioBufferRight[256]){
    uint16_t size = udp.parsePacket();
    if (0 < size && size <= sizeof(buf)) {
      udp.read(buf, size);
      memcpy(outputAudioBufferLeft, buf, 256);
      memcpy(outputAudioBufferRight, &buf[256], 256);
      return true;
    }
    return false;
  }


private:
  IPAddress remoteNodeIP;
  
};

#endif
