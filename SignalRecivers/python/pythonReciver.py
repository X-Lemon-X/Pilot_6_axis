import socket
import time


class RemoteControler:
    def __init__(self) -> None:
        self.ZeroElemnts()
    def __str__(self) -> str:
        return str(self.data)
    
    def ZeroElemnts(self) -> None:
        self.data = { 
            'X1':0,
            'Y1':0,
            'Z1':0,
            'X2':0,
            'Y2':0,
            'Z2':0,
            'B1':1,
            'B2':1,
            'H1':1,
            'H2':1,
            'H3':1,
            'H4':1,
            'H5':1,
            'H6':1,
            'H7':1,
            'H8':1
        }
    
    def DecodeData(self,  buffer:bytes) -> bool:
        try:
            self.ZeroElemnts()
            buffer = buffer.decode()
            if len(buffer) < 50:
                return False
            if buffer[0] != '#' or buffer[-1] != "@":
                return False
            for key in self.data.keys():
                index = buffer.find(key)
                if(index == -1):
                    return False
                substr = buffer[index + len(key):]
                index = substr.find(':')
                if(index == -1):
                    return False
                substr = substr[:index]
                self.data[key]=int(substr)
            return True
        except:
            return False
    
    def GetLX(self) -> int:
        return self.data['X1']
    def GetLY(self) -> int:
        return self.data['Y1']
    def GetLZ(self) -> int:
        return self.data['Z1']
    def GetLB(self) -> int:
        return self.data['B1']
    
    def GetRX(self) -> int:
        return self.data['X2']
    def GetRY(self) -> int:
        return self.data['Y2']
    def GetRZ(self) -> int:
        return self.data['Z2']
    def GetRB(self) -> int:
        return self.data['B2']
    
    def GetB1(self) -> int:
        return self.data['H1']
    def GetB2(self) -> int:
        return self.data['H2']
    def GetB3(self) -> int:
        return self.data['H3']
    def GetB4(self) -> int:
        return self.data['H4']
    def GetB5(self) -> int:
        return self.data['H5']
    def GetB6(self) -> int:
        return self.data['H6']
    def GetB7(self) -> int:
        return self.data['H7']
    def GetB8(self) -> int:
        return self.data['H8']
    

HOST = '0.0.0.0'  
PORT = 25000   
BUFFER_SIZE = 1024    
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# Bind the socket to the host and port
s.bind((HOST, PORT))
rd= RemoteControler()
beg = time.time()

try:
    while True:
        # Receive BUFFER_SIZE bytes data
        # data is a list with 2 elements
        # first is data
        #second is client address
        data = s.recvfrom(BUFFER_SIZE)
        if data:
            #decode Data using RemoteControler class
            rd.DecodeData(data[0])
            diff = time.time() - beg
            print("time: " + str(diff) + "[s] -> " + str(round(diff/60,1)) + "[m] " + str(rd))

except:
    s.close()

exit(0)