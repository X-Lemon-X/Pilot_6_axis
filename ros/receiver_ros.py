import rclpy
from rclpy.duration import Duration
from rclpy.node import Node
from sensor_msgs.msg import Joy
from std_msgs.msg import Float64MultiArray
from lemonx_receive import UDPReceiver, RemoteControler6D

###
# dead mean switch


class lemonJoyPublisher(Node):
    def __init__(self):
        super().__init__("lemonx_joy_publisher")
        self.publisher_ = self.create_publisher(Joy, "joy", 10)
        self.gripper_pub = self.create_publisher(Float64MultiArray, "/velocity_controllers/commands", 10)
        timer_period = 0.1  # seconds
        self.timer = self.create_timer(timer_period, self.timer_callback)
        self.i = 0
        self.gripper_msg = Float64MultiArray()
        self.griper_open_close = 0.0
        self.gripper_rotation = 0.0
        self.axes = [0.0] * 6  # Fill self.axes with a list of 6 float zeros
        self.buttons = [0] * 10  # Fill self.buttons with 10 zero integers
        self.conncected = False
        self.last_connection = self.get_clock().now()
        self.max_zero_position = 0.01
        self.analog_divider = 2048
        receiver = UDPReceiver(25000)
        receiver.on_receive(self.handle_receive)
        receiver.start()
        self.get_logger().info("Receiver started waiting for pilot")

    def map_value(self, value):
        value = value / self.analog_divider
        if value < self.max_zero_position and value > -self.max_zero_position:
            return 0.0
        return value

    # This function is called when the UDPReceiver receives a message from the RC
    def handle_receive(self, data: RemoteControler6D):
        if not self.conncected:
            self.get_logger().info("Pilot connected")

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

        self.griper_open_close = float(data.button_5 - data.button_6)
        self.gripper_rotation = float(data.button_3 - data.button_4)
        self.get_logger().debug(str(data))

    def timer_callback(self):
        msg = Joy()
        msg.header.frame_id = "joy"
        msg.header.stamp = self.get_clock().now().to_msg()

        if (
            self.get_clock().now() - self.last_connection > Duration(nanoseconds=5e8)
            and self.conncected
        ):
            self.get_logger().warn("Pilot disconnected")
            self.conncected = False
            self.gripper_msg.data[0] = self.griper_open_close
            self.gripper_msg.data[1] = self.gripper_rotation

        if not self.conncected:
            self.axes = [0.0] * 6
            self.buttons = [0] * 10


        msg.axes = self.axes
        msg.buttons = self.buttons
        self.publisher_.publish(msg)
        self.gripper_pub.publish(self.gripper_msg)


def main(args=None):
    rclpy.init(args=args)
    minimal_publisher = lemonJoyPublisher()
    rclpy.spin(minimal_publisher)
    minimal_publisher.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
    main()
