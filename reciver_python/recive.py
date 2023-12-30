import socket
import threading
from typing import Callable
import time


BUFFER_SISE = 1024

class UDPReceiver:
  def __init__(self, port: int):
    self.port = port
    self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    self.socket.bind(('', self.port))
    self.receive_event = None
    self.prevoious_time = time.time()

  def start(self):
    threading.Thread(target=self._receive_loop, daemon=True).start()

  def on_receive(self, callback: Callable[['RemoteControler6D'], None]):
    self.receive_event = callback

  def _receive_loop(self):
    while True:
      try:
        data, _ = self.socket.recvfrom(BUFFER_SISE)
        frequency = 1/(time.time()-self.prevoious_time)
        self.prevoious_time = time.time()
        try:
          data_structure = RemoteControler6D(data.decode('ascii'),frequency)
          if self.receive_event:
            self.receive_event(data_structure)
        except ValueError:
          pass
      except Exception:
        pass

class RemoteControler6D:
  def __init__(self, bytes: str,frequency:float=0.0):
    self.__bytes = bytes
    self.frequency = frequency
    l = len(bytes)
    if len(bytes) != 62:
      raise ValueError('Invalid data length')
    
    splited_bytes = bytes.split(':')
    if len(splited_bytes) != 18 or splited_bytes[0] != '$RC' or splited_bytes[17] != '#\r':
      raise ValueError('data Error')
    
    self.joystick_1_x = int(splited_bytes[1])
    self.joystick_1_y = int(splited_bytes[2])
    self.joystick_1_z = int(splited_bytes[3])
    self.joystick_1_btn = int(splited_bytes[4])
    self.joystick_2_x = int(splited_bytes[5])
    self.joystick_2_y = int(splited_bytes[6])
    self.joystick_2_z = int(splited_bytes[7])
    self.joystick_2_btn = int(splited_bytes[8])
    self.button_1 = int(splited_bytes[9])
    self.button_2 = int(splited_bytes[10])
    self.button_3 = int(splited_bytes[11])
    self.button_4 = int(splited_bytes[12])
    self.button_5 = int(splited_bytes[13])
    self.button_6 = int(splited_bytes[14])
    self.button_7 = int(splited_bytes[15])
    self.button_8 = int(splited_bytes[16])
  def __str__(self) -> str:
    return f"frequency={self.frequency:.2f}Hz   Data=" + self.__bytes


da="$RC:  -17:  -13:  -11:1:  -11:   -9:   -4:1:1:1:1:1:1:1:1:1:#\r"

# data = RemoteControler6D(da)

# print(data.joystick_1_x)
# Usage example
listLastData = []
def handle_receive(data: RemoteControler6D):
  print(data)
  
receiver = UDPReceiver(25000)
receiver.on_receive(handle_receive)
receiver.start()

while True:
  time.sleep(1)

