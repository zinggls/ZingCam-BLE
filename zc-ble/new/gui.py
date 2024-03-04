import tkinter
import tkinter.ttk
import win32com.client
import array

class I2C:
    READ = 0
    WRITE = 1
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
        result = self.dev.I2C_GetDeviceList()
        address = result[1][0]

        return address
    
    def read_use_addr(self, device_addr, addr, len):
        dataIN = []
        data_wr = self.READ
        data_addr = addr
#       data = 0
        data_len = len
        dataIN.append(data_wr)
        dataIN.append(data_addr)
        dataIN.append(0)
        dataIN.append(data_len)
        data = array.array('B', dataIN)

        self.dev.I2C_SendData(device_addr, bytearray(data))
        result = self.dev.I2C_ReadData(device_addr, len)
        read_data = result[1]

        return read_data

class GUI:
    def __init__(self, i2c):
        self.i2c = i2c
        self.root = tkinter.Tk()
        
        connect_frame = tkinter.Frame(self.root)
        self.create_connect_frame(connect_frame)
        connect_frame.pack()

    def create_connect_frame(self, connect_frame):
        self.ports = tkinter.StringVar()
        ports_list = self.i2c.get_ports()

        self.connect_frame_dropdown = tkinter.ttk.Combobox(connect_frame, state = "readonly", textvariable = self.ports)
        self.connect_frame_dropdown.config(values = ports_list, width = 25)
        self.connect_frame_dropdown.current(0)
        self.connect_frame_dropdown.pack(side = "left")

        self.connect_frame_button = tkinter.Button(connect_frame, text = "Connect", command = self.connect_frame_connect)
        self.connect_frame_button.pack(side = "left")

    def connect_frame_connect(self):
        port = self.ports.get()
        self.i2c.open_port(port)
        try:
            i2c_address = self.i2c.get_address()
        except IndexError:
            print("No devices found")
            return
        
        self.connect_frame_button.config(text = "Disconnect", command = self.connect_frame_disconnect)
        self.connect = True
        self.i2c_read(i2c_address)
    
    def connect_frame_disconnect(self):
        self.i2c.close_port()
        self.connect = False
        self.connect_frame_button.config(text = "Connect", command = self.connect_frame_connect)

    def i2c_read(self, device_addr):
        #print(device_addr)
        if (self.connect == True):
            read_data = self.i2c.read_use_addr(device_addr, 0x28, 0x1A)
            message = ""
            for data in read_data:
                message = message + "{}, ".format(hex(data))
            print(message)
        self.root.after(100, self.i2c_read, device_addr)

    def run(self):
        self.root.mainloop()

i2c = I2C()
gui = GUI(i2c = i2c)
gui.run()