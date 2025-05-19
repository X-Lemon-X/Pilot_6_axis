#pragma once


#include <Arduino.h>
#include "main.hpp"


class UdpPackages {
  UdpPackages() = default;

  InputsData unpack_data(std::vector<char> data) {
    InputsData input_data;

    return input_data;
  }


  std::vector<char> pack_data(InputsData &input_data) {
    return std::vector<char>();
  }

  static void
}