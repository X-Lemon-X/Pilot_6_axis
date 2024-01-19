import rclpy
from rclpy.node import Node
import copy
from sensor_msgs.msg import Joy
from reciver_python.recive import UDPReceiver, RemoteControler6D
class MinimalPublisher(Node):

    def __init__(self):
        super().__init__('minimal_publisher')
        self.publisher_ = self.create_publisher(Joy, 'joy', 10)
        timer_period = 0.1  # seconds
        self.timer = self.create_timer(timer_period, self.timer_callback)
        self.i = 0
        self.axes = [] *6
        self.buttons = []*10
        receiver = UDPReceiver(25000)
        receiver.on_receive(self.handle_receive)
        receiver.start()

    def handle_receive(self, data: RemoteControler6D):
        print("Handluje")
        self.axes[0]= float(data.joystick_1_x)
        self.axes[1] =    float(data.joystick_1_y )
        self.axes[2]=    float(data.joystick_1_z)
        self.axes[3]=   float(data.joystick_2_x )
        self.axes[4]=   float(data.joystick_2_y )
        self.axes[5]=   float(data.joystick_2_z )
        self.buttons[0]  =  data.joystick_1_btn 
        self.buttons[1] = data.joystick_2_btn 
        self.buttons[2] =    data.button_1 
        self.buttons[3] =    data.button_2 
        self.buttons[4] =    data.button_3 
        self.buttons[5] =    data.button_4 
        self.buttons[6] =    data.button_5
        self.buttons[7] =   data.button_6 
        self.buttons[8] =   data.button_7 
        self.buttons[9] =    data.button_8 
        print(f"Jesteś pusta? {data}")
        self.get_logger().info(data)
        
    def timer_callback(self):
        
        msg = Joy()
        msg.header.frame_id = "joy"
        msg.header.stamp = self.get_clock().now().to_msg()
        msg.axes = copy.copy(self.axes)
        msg.buttons = copy.copy(self.buttons)
       
        self.publisher_.publish(msg)


def main(args=None):
    rclpy.init(args=args)

    minimal_publisher = MinimalPublisher()

    rclpy.spin(minimal_publisher)

    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    minimal_publisher.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()