#include <string>
#include <map>
#include <Arduino.h>

#include <EEPROM.h> // https://randomnerdtutorials.com/esp32-flash-memory/
#include <Preferences.h>
#define STRING_SIZE 35

namespace Setings
{

  enum setings_ERRORS
  {
    ERRORS_OK = 0,
    ERRORS_NO_KEY = 1,
    ERROR_INVALID_CONVERSION = 2
  };

  union setings_seting_data
  {
    int _int;
    char _string[STRING_SIZE];
    float _float;
    bool _bool;
  };

  enum setings_dataType
  {
    _int = 1,
    _float = 2,
    _bool = 3,
    _string = 4
  };

  struct setings_seting
  {
    union setings_seting_data data;
    enum setings_dataType type;
  };

  class Setings
  {
  private:
  public:
    void InitEPPROM();
    void LoadSetingsFromFlash();
    void SaveSetingsToFlash();
    std::map<string, setings_seting> _setings;

    setings_seting GetSeting(std::string name);
    void AddSeting(std::string name, int seting);
    void AddSeting(std::string name, float seting);
    void AddSeting(std::string name, bool seting);
    void AddSeting(std::string name, std::string seting);

    void AddSeting(std::string name, setings_seting seting);

    int UpdateSeting(std::string name, std::string input_data);

    int GetSeting(std::string name, setings_seting *seting);

    Setings();
    ~Setings();
  };

  static std::string GetStringFromSeting(setings_seting seting)
  {
    switch (seting.type)
    {
    case _int:
      return std::to_string(seting.data._int);

    case _float:
      return std::to_string(seting.data._float);
    case _string:
      return std::string(seting.data._string);
      break;
    case _bool:
      return std::to_string(seting.data._bool);
      break;
    default:
      return "";
    }
  }

  int Setings::GetSeting(std::string name, setings_seting *seting)
  {
    if (_setings.count(name))
      return ERRORS_NO_KEY;

    *seting = _setings[name];

    return ERRORS_OK;
  }

  setings_seting Setings::GetSeting(std::string name)
  {
    return _setings[name];
  }

  void Setings::AddSeting(std::string name, int seting)
  {
    setings_seting ss;
    ss.type = _int;
    ss.data._int = seting;
    _setings.insert(std::pair<string, setings_seting>(name, ss));

    auto iterator = _setings.begin();
  }

  void Setings::AddSeting(std::string name, bool seting)
  {
    setings_seting ss;
    ss.type = _bool;
    ss.data._bool = seting;
    _setings.insert(std::pair<string, setings_seting>(name, ss));
  }

  void Setings::AddSeting(std::string name, float seting)
  {
    setings_seting ss;
    ss.type = _float;
    ss.data._float = seting;
    _setings.insert(std::pair<string, setings_seting>(name, ss));
  }

  void Setings::AddSeting(std::string name, std::string seting)
  {
    setings_seting ss;
    ss.type = _string;
    seting.resize(STRING_SIZE);
    strcpy(ss.data._string, seting.c_str());
    _setings.insert(std::pair<string, setings_seting>(name, ss));
  }

  int Setings::UpdateSeting(std::string name, std::string input_data)
  {
    if (!_setings.count(name))
      return ERRORS_NO_KEY;

    setings_seting ss = _setings[name];
    try
    {
      switch (ss.type)
      {
      case _int:
        ss.data._int = std::stoi(input_data);
        break;
      case _float:
        ss.data._float = std::stof(input_data);
        break;
      case _string:
        input_data.resize(STRING_SIZE);
        strcpy(ss.data._string, input_data.c_str());
        break;
      case _bool:
        ss.data._bool = std::stoi(input_data) > 0 ? 1 : 0;
        break;
      default:
        break;
      }
    }
    catch (const std::exception &e)
    {
      return ERROR_INVALID_CONVERSION;
    }

    _setings[name] = ss;
    return ERRORS_OK;
  }

  static String PrintSeting(setings_seting ss)
  {
    String str;
    str = "s> ";
    switch (ss.type)
    {
    case _int:
      str += "int:";
      str += String(ss.data._int);
      break;
    case _float:
      str += "float:";
      str += String(ss.data._float);
      break;
    case _string:
      str += ("str:");
      str += String(ss.data._string);
      break;
    case _bool:
      str += "bool:";
      str += String(ss.data._bool);
      break;
    default:
      str += "ERROR";
      break;
    }
    return str;
  }

  void Setings::LoadSetingsFromFlash()
  {
    int addres = 0;
    setings_seting ss, sp;
    for (auto element = _setings.begin(); element != _setings.end(); element++)
    {
      EEPROM.get(addres, ss);
      // if(ss.type != element->second.type)
      //     ss = element->second;
      _setings[element->first] = ss;
      addres += sizeof(ss);
    }
  }

  void Setings::SaveSetingsToFlash()
  {
    setings_seting ss;
    int addres = 0;
    for (auto element = _setings.begin(); element != _setings.end(); element++)
    {
      ss = element->second;
      EEPROM.put(addres, ss);
      EEPROM.commit();
      addres += sizeof(ss);
    }
  }

  void Setings::InitEPPROM()
  {
    EEPROM.begin(_setings.size() * sizeof(setings_seting));
  }

  Setings::Setings(/* args */) {}
  Setings::~Setings() {}
}
