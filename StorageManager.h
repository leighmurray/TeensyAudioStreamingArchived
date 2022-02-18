#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <EEPROM.h>

class StorageManager{
public:
  static bool saveRemoteIPAddress(byte ip0, byte ip1, byte ip2,byte ip3){
    EEPROM.write(0, ip0);
    EEPROM.write(1, ip1);
    EEPROM.write(2, ip2);
    EEPROM.write(3, ip3);
    return true;
  }

  static bool getRemoteIPAddress(byte* ip0, byte* ip1, byte* ip2, byte* ip3){
    *ip0 = EEPROM.read(0);
    *ip1 = EEPROM.read(1);
    *ip2 = EEPROM.read(2);
    *ip3 = EEPROM.read(3);
    return true;
  }
};

#endif
