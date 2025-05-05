# 
# Copyright (c) 2025 Patryk Dudziński
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
# 
#  Authors: Patryk Dudziński
# 

from pynput import keyboard
import threading
import time
from rc_receive import RemoteControler6D
import socket
import argparse

class KeyboardController:
  def __init__(self,ip:str, port: int, frequency: float=50.0, joystick_value:float=0.5):
    self.stop_event = threading.Event()
    self.IP = ip
    self.joystick_value = joystick_value * 2048
    self.PORT = port
    self.frequency = frequency
    self.key_pressed_map = {
      "w": False,
      "s": False,
      "a": False,
      "d": False,
      "q": False,
      "e" : False,
      "o": False,
      "l": False,
      "k": False,
      ";": False,
      "i": False,
      "p": False,
      "1": False,
      "2": False,
      "3": False,
      "4": False,
      "5": False,
      "6": False,
      "7": False,
      "space": False,
      "9": False,
      "0": False
    }

  def map_keys_to_rc(self) -> "RemoteControler6D":
    data = RemoteControler6D(signal_frequency=self.frequency)
    ### Joystick 1
    if self.key_pressed_map["w"]:
      data.joystick_1_x = self.joystick_value 
    if self.key_pressed_map["s"]:
      data.joystick_1_x = -self.joystick_value
    
    if self.key_pressed_map["a"]:
      data.joystick_1_y = -self.joystick_value
    if self.key_pressed_map["d"]:
      data.joystick_1_y = self.joystick_value

    if self.key_pressed_map["q"]:
      data.joystick_1_z = -self.joystick_value
    if self.key_pressed_map["e"]:
      data.joystick_1_z = self.joystick_value

    data.joystick_1_btn = int(self.key_pressed_map["9"])

    ### Joystick 2
    if self.key_pressed_map["o"]:
      data.joystick_2_x = self.joystick_value
    if self.key_pressed_map["l"]:
      data.joystick_2_x = -self.joystick_value

    if self.key_pressed_map["k"]:
      data.joystick_2_y = -self.joystick_value
    if self.key_pressed_map[";"]:
      data.joystick_2_y = self.joystick_value

    if self.key_pressed_map["i"]:
      data.joystick_2_z = -self.joystick_value
    if self.key_pressed_map["p"]:
      data.joystick_2_z = self.joystick_value
    
    data.joystick_2_btn = int(self.key_pressed_map["0"])
    
    ### Buttons
    data.button_1 = int(self.key_pressed_map["1"])
    data.button_2 = int(self.key_pressed_map["2"])
    data.button_3 = int(self.key_pressed_map["3"])
    data.button_4 = int(self.key_pressed_map["4"])
    data.button_5 = int(self.key_pressed_map["5"])
    data.button_6 = int(self.key_pressed_map["6"])
    data.button_7 = int(self.key_pressed_map["7"])
    data.button_8 = int(self.key_pressed_map["space"])
    return data

  def start(self):
    self.is_running = True
    # keyboard.on_press(self.send_key)
    threading.Thread(target=self.send_data_thread).start()

  def stop(self):
    self.stop_event.set()
    # keyboard.unhook_all()

  def on_press(self,key):
    try:
      key_str = key.char  # Get the character representation of the key
    except AttributeError:
      key_str = key.name
    if key_str in self.key_pressed_map:
      self.key_pressed_map[key_str] = True

  def on_release(self,key):
    try:
      key_str = key.char
    except AttributeError:
      key_str = key.name
    if key_str in self.key_pressed_map:
      self.key_pressed_map[key_str] = False
  
  def send_data_thread(self):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    time_interval = 1 / self.frequency
    listener = keyboard.Listener(on_press=self.on_press, on_release=self.on_release)
    listener.start()
    
    while True:
      data = self.map_keys_to_rc()
      sock.sendto(data.create_udp_package(), (self.IP, self.PORT))
      print(data)
      time.sleep(time_interval)
      if self.stop_event.is_set():
        break
    
    listener.stop()
    sock.close()

def test():
  keyboard_controller = KeyboardController("192.168.2.105",25000,50,0.5)
  keyboard_controller.start()
  while True:
    try:
      time.sleep(1)
    except KeyboardInterrupt:
      keyboard_controller.stop()
      break
                                           

def main():
  desc = """\n[HELP]
  Keyboard controller for the Remote Controler 6D
  The keyboard controller will send the data to the receiver (Robot) at the given frequency
  
  [KEYS MAPPING]

  ------------------------------------------------
  - W : x axis of joystick 1
  - S : x axis of joystick 1
  - A : y axis of joystick 1
  - D : y axis of joystick 1
  - Q : z axis of joystick 1
  - E : z axis of joystick 1
  ------------------------------------------------
  - O : x axis of joystick 2
  - L : x axis of joystick 2
  - K : y axis of joystick 2
  - ; : y axis of joystick 2
  - i : z axis of joystick 2
  - P : z axis of joystick 2
  ------------------------------------------------
  - Key 1: button 1
  - Key 2: button 2
  - Key 3: button 3
  - Key 4: button 4
  - Key 5: button 5
  - Key 6: button 6
  - Key 7: button 7
  - Key space: button 8
  - Key 9: joystick 1 button
  - Key 0: joystick 2 button
  ------------------------------------------------
  """
  parser = argparse.ArgumentParser(description="Keyboard controller for the Remote Controler 6D")
  parser.add_argument('-i', type=str, help='Ip of the receiver (Robot)')
  parser.add_argument('-p', type=int, default=25000, help='port of the receiver (Robot) default is 25000')
  parser.add_argument('-f', type=float, default=40.0, help='data frequency in Hz default is 40Hz')
  parser.add_argument('-v', type=float, default=0.5,  help='value of the joystick default is 0.5 (to avoid going at full speed)  value between 0 and 1')
  args = parser.parse_args()
  ip = args.i
  port = args.p
  frequency = args.f
  joystick_value = args.v

  if args.i is None:
    parser.print_help()
    print(desc)
    # ip='192.168.1.210'
    return 1  
  if joystick_value is not None and joystick_value < 0 or joystick_value > 1:
    print("Joystick value must be between 0 and 1")
    return 1

  # Create a keyboard controller object
  keyboard_controller = KeyboardController(ip,port,frequency,joystick_value)
  keyboard_controller.start()
  while True:
    try:
      time.sleep(1)
    except KeyboardInterrupt:
      keyboard_controller.stop()
      break

if __name__ == "__main__":
  main()