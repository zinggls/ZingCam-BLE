import win32com.client
import array

class I2C:
    def __init__(self):
        self.dev = win32com.client.Dispatch("PSoCProgrammerCOM.PSoCProgrammerCOM_Object")

    def get_ports(self):
        result = self.dev.GetPorts()
        ports = result[1]
        return ports
    
    def open_port(self, port):
        result = self.dev.OpenPort(port)
        return result
    
    def close_port(self):
        result = self.dev.ClosePort()
        return result
    
    def get_address(self):
        try:
            result = self.dev.I2C_GetDeviceList()
            address = result[1][0]
        except:
            self.close_port()
            exit()
        return address
    
    def read_use_addr(self, device_addr):
        result = self.dev.I2C_ReadData(device_addr, 44 + 26 + 42)
        read_data = result[1]
        return read_data
    
    def send_use_addr(self, device_addr, dataIN):
        data = array.array('B', dataIN)
        self.dev.I2C_SendData(device_addr, memoryview(data))