#include <Arduino.h>
#include <map>
#include <string>

#include <EEPROM.h> // https://randomnerdtutorials.com/esp32-flash-memory/
#include <Preferences.h>
#include "main.hpp"

class EPPROM_Settings {
private:
public:
  EPPROM_Settings() = default;

  void init(size_t size) {
    EEPROM.begin(size);
  };

  template <typename T> int load_settings(T &data, int address) {
    EEPROM.get(address, data);
    return sizeof(data);
  }

  template <typename T> int save_settings(T &data, int address) {
    EEPROM.put(address, data);
    EEPROM.commit();
    return sizeof(data);
  }
};
