#pragma once

#include "SetingsPage.h"
#include "main.hpp"
#include <Arduino.h>
#include <map>
#include <string>


class PageGenerator {
public:
  PageGenerator(/* args */) = delete;

  static String generate_page(RcSettings &settings) {
    String page = "";
    page += MAIN_page_style;
    page += MAIN_page_beg;
    // page += MAIN_table_wifi;
    page += "<tr><td>Joystick left MMin</td><td><input type=\"text\" "
            "name=\"int_Joystick_left_MMin\" value=\"" +
            String(settings.int_Joystick_left_MMin) + "\"></td></tr>";
    page += "<tr><td>Joystick left MMax</td><td><input type=\"text\" "
            "name=\"int_Joystick_left_MMax\" value=\"" +
            String(settings.int_Joystick_left_MMax) + "\"></td></tr>";
    page += "<tr><td>Joystick right MMin</td><td><input type=\"text\" "
            "name=\"int_Joystick_right_MMin\" value=\"" +
            String(settings.int_Joystick_right_MMin) + "\"></td></tr>";
    page += "<tr><td>Joystick right MMax</td><td><input type=\"text\" "
            "name=\"int_Joystick_right_MMax\" value=\"" +
            String(settings.int_Joystick_right_MMax) + "\"></td></tr>";
    page += "<tr><td>Joystick left filter</td><td><input type=\"text\" "
            "name=\"flo_Joystick_left_filer\" value=\"" +
            String(settings.flo_Joystick_left_filer, 3) + "\"></td></tr>";
    page += "<tr><td>Joystick right filter</td><td><input type=\"text\" "
            "name=\"flo_Joystick_right_filer\" value=\"" +
            String(settings.flo_Joystick_right_filer, 3) + "\"></td></tr>";
    page += "<tr><td>Host Port</td><td><input type=\"text\" "
            "name=\"int_host_port\" value=\"" +
            String(settings.int_host_port) + "\"></td></tr>";
    page += "<tr><td>Update frequency</td><td><input type=\"text\" "
            "name=\"int_upd_freq\" value=\"" +
            String(settings.int_upd_freq) + "\"></td></tr>";
    page += "<tr><td>WIFI SSID</td><td><input type=\"text\" name=\"str_ssid\" "
            "value=\"" +
            String(settings.str_ssid) + "\"></td></tr>";
    page += "<tr><td>WIFI PASSWORD</td><td><input type=\"text\" "
            "name=\"str_passwd\" value=\"" +
            String(settings.str_passwd) + "\"></td></tr>";
    page += "<tr><td>Target IP</td><td><input type=\"text\" "
            "name=\"str_target_ip\" value=\"" +
            String(settings.str_target_ip) + "\"></td></tr>";

    page += MAIN_table_separator;

    for(int i = 1; i <= WIFI_NETWORKS_MAX; ++i) {
      page += "<tr><td>Wifi " + String(i) + ": Seed</td><td><input type=\"text\" name=\"str_WIFI_" + String(i) + "_S\" value=\"";
      page += String(settings.str_wifi_ssids[i - 1]);
      page += "\"></td></tr>";
      page += "<tr><td>Wifi " +
              String(i) + ": Password</td><td><input type=\"password\" id=\"wifipass\" class=\"password-input\" name=\"str_WIFI_" +
              String(i) + "_P\" value=\"";
      page += String(settings.str_wifi_passwords[i - 1]);
      page += "\"></td></tr>";
    }

    page += MAIN_page_end;
    return page;
  }

  static String get_saved_settings_page(bool success) {
    String page = "";
    page += MAIN_page_style;

    if(success) {
      page += "<h2 style=\"color:green;\">Settings saved successfully!</h2>";
    } else {
      page += "<h2 style=\"color:red;\">Failed to save settings!</h2>";
    }
    page += SETTING_page_end;
    return page;
  }

  static bool get_settings_from_page(AsyncWebServerRequest *request, RcSettings &settings) {
    int count = 0, param_count = 0;

#ifdef DEBUG
    Serial.println("Param count: ");
    Serial.println(request->params());
#endif

    for(size_t i = 0; i < request->params(); i++) {
      String name  = request->getParam(i)->name();
      String value = request->getParam(i)->value();

#ifdef DEBUG
      Serial.print(name);
      Serial.print(" = ");
      Serial.println(value);
#endif

      if(name == "int_Joystick_left_MMin") {
        settings.int_Joystick_left_MMin = value.toInt();
        Serial.println("X");
      } else if(name == "int_Joystick_left_MMax") {
        settings.int_Joystick_left_MMax = value.toInt();
        Serial.println("X");
      } else if(name == "int_Joystick_right_MMin") {
        settings.int_Joystick_right_MMin = value.toInt();
        Serial.println("X");
      } else if(name == "int_Joystick_right_MMax") {
        settings.int_Joystick_right_MMax = value.toInt();
        Serial.println("X");
      } else if(name == "flo_Joystick_left_filer") {
        settings.flo_Joystick_left_filer = value.toFloat();
        Serial.println("X");
      } else if(name == "flo_Joystick_right_filer") {
        settings.flo_Joystick_right_filer = value.toFloat();
        Serial.println("X");
      } else if(name == "int_host_port") {
        settings.int_host_port = value.toInt();
        Serial.println("X");
      } else if(name == "int_upd_freq") {
        settings.int_upd_freq = value.toInt();
        Serial.println("X");
      } else if(name == "str_ssid") {
        value.toCharArray(settings.str_ssid, sizeof(settings.str_ssid));
        Serial.println("X");
      } else if(name == "str_passwd") {
        value.toCharArray(settings.str_passwd, sizeof(settings.str_passwd));
        Serial.println("X");
      } else if(name == "str_target_ip") {
        value.toCharArray(settings.str_target_ip, sizeof(settings.str_target_ip));
        Serial.println("X");
      } else if(name.startsWith("str_WIFI")) {
        // Expected format: str_WIFI_{i}_S or str_WIFI_{i}_P
        int idx_start = String("str_WIFI_").length();
        int idx_end   = name.indexOf('_', idx_start);

        if(idx_end < idx_start) {
          continue;
        }
        int wifi_idx = name.substring(idx_start, idx_end).toInt() - 1;
        if(wifi_idx < 0 || wifi_idx > WIFI_NETWORKS_MAX)
          continue;


        char type = name.charAt(idx_end + 1);
        if(type == 'S') {
          strcpy(settings.str_wifi_ssids[wifi_idx], value.c_str());
        } else if(type == 'P') {
          strcpy(settings.str_wifi_passwords[wifi_idx], value.c_str());
        }
      } else {

        Serial.println("Unknown param: ");
        Serial.println(name);
        Serial.println(" = ");
        Serial.println(value);
        continue;
      }
      param_count++;
    }
    return param_count == 31 ? true : false;
  }
};
