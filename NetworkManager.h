#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <QNEthernet.h>
#include <QNDNSClient.h>
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
bool isServer;

class NetworkManager{
public:
  void Setup(){
    // Print the MAC address
    uint8_t mac[6];
    Ethernet.macAddress(mac);
    Serial.printf("MAC = %02x:%02x:%02x:%02x:%02x:%02x\n",
                  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    isServer = macAddressMatches(serverMacAddress, mac);
    Serial.print("Is Server:");
    Serial.println(isServer);
    
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

    if (isServer){
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
    }
    
    if (!isServer){
      Serial.println("Please give me the server IP address:");
      
      while(!Serial.available()){
        delay(1000);
      }
      String ipString = Serial.readStringUntil('\n');
      Serial.println(ipString);
      remoteNodeIP.fromString(ipString);
      Serial.print("Thankyou. IP is:");
      Serial.println(remoteNodeIP);
    }
    
  }

  void Loop(){    
    int size = udp.parsePacket();
    if (0 < size && size <= sizeof(buf)) {
      udp.read(buf, size);
      Serial.println("UDP Contents:");
      Serial.println((char *) buf);
    }
    if (!isServer){
      Serial.println("Sending message");
      char messageBuffer[] = "hello server\n"; 
      udp.send(remoteNodeIP,  kAudioPort, messageBuffer, sizeof(messageBuffer));
    }
  }

private:
  bool macAddressMatches(uint8_t* firstMac, uint8_t* secondMac){
    return (memcmp(firstMac, secondMac, sizeof(firstMac)) == 0);
  }

  IPAddress remoteNodeIP;
  
};

#endif
