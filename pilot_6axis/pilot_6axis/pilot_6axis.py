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


import rclpy
from rclpy.duration import Duration
from rclpy.node import Node
from sensor_msgs.msg import Joy
from std_msgs.msg import Float64MultiArray
from .rc_receive import UDPReceiver, RemoteControler6D

###
# set parameter pilot_six_axis_joy_topic to change the topic where the joy message will be published

class lemonJoyPublisher(Node):
  def __init__(self):
    super().__init__("pilot_6axis")
    # parameters to set topic where the joy message will be published

    self.declare_parameter("pilot_six_axis_joy_topic", "/joy")
    joy_topic = self.get_parameter("pilot_six_axis_joy_topic").value
    self.get_logger().info(f"Joy topic: {joy_topic}")


    self.publisher_ = self.create_publisher(Joy, joy_topic , 10)
    timer_period = 0.1  # seconds
    self.timer = self.create_timer(timer_period, self.timer_callback)
    self.i = 0
    self.axes = [0.0] * 6  # Fill self.axes with a list of 6 float zeros
    self.buttons = [0] * 10  # Fill self.buttons with 10 zero integers
    self.conncected = False
    self.last_connection = self.get_clock().now()
    self.max_zero_position = 0.01
    self.analog_divider = 2048
    self.bind_port = 25000
    try:
      receiver = UDPReceiver(self.bind_port)
      receiver.on_receive(self.handle_receive)
      receiver.start()
    except OSError as e:
      self.get_logger().error(f"Binding to port: \"{self.bind_port}\" failed: {e}")
      raise e
    
    self.get_logger().info("Receiver started waiting for the RC...")

  def map_value(self, value):
    value = value / self.analog_divider
    if value < self.max_zero_position and value > -self.max_zero_position:
        return 0.0
    return float(value)

  # This function is called when the UDPReceiver receives a message from the RC
  def handle_receive(self, data: RemoteControler6D):
    if not self.conncected:
        self.get_logger().info("Pilot Connected")

    self.conncected = True
    self.last_connection = self.get_clock().now()
    self.axes[0] = self.map_value(float(data.joystick_1_x))
    self.axes[1] = self.map_value(float(data.joystick_1_y))
    self.axes[2] = self.map_value(float(data.joystick_1_z))
    self.axes[3] = self.map_value(float(data.joystick_2_x))
    self.axes[4] = self.map_value(float(data.joystick_2_y))
    self.axes[5] = self.map_value(float(data.joystick_2_z))
    self.buttons[0] = data.joystick_1_btn
    self.buttons[1] = data.joystick_2_btn
    self.buttons[2] = data.button_1
    self.buttons[3] = data.button_2
    self.buttons[4] = data.button_3
    self.buttons[5] = data.button_4
    self.buttons[6] = data.button_5
    self.buttons[7] = data.button_6
    self.buttons[8] = data.button_7
    self.buttons[9] = data.button_8
    self.get_logger().debug(str(data))

  def timer_callback(self):
    msg = Joy()
    msg.header.frame_id = "joy"
    msg.header.stamp = self.get_clock().now().to_msg()

    if (
        self.get_clock().now() - self.last_connection > Duration(nanoseconds=5e8)
        and self.conncected
      ):
        self.get_logger().warn("Pilot Disconnected")
        self.conncected = False

    if not self.conncected:
        self.axes = [0.0] * 6
        self.buttons = [0] * 10

    msg.axes = self.axes
    msg.buttons = self.buttons
    self.publisher_.publish(msg)


def main(args=None):
  rclpy.init(args=args)
  minimal_publisher = lemonJoyPublisher()
  try:
    rclpy.spin(minimal_publisher)
  except KeyboardInterrupt:
    pass
  finally:
    minimal_publisher.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
  main()
