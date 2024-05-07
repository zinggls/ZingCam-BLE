import tkinter
import tkinter.ttk
import win32com.client
import array
import time
import subprocess
import threading

class I2C:
    READ = 0x38
    WRITE = 0x39
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
    
    def read_use_addr(self, device_addr, addr, len):
        result = self.dev.I2C_ReadData(device_addr, 40 + 30 + 38)
        read_data = result[1]

        return read_data

class Player:
    playing = 0
    
    def play(self):
        self.playing = 1

    def stop(self):
        self.playing = 0
        self.run.kill()
        
    def Thread(self):
        while True:
            if (self.playing == 1):                
                command = [
                    "D:/gstreamer/1.0/msvc_x86_64/bin/gst-launch-1.0",
                    "ksvideosrc",
                    "device-index=0",
                    "!"
                    "image/jpeg,width=640,height=480,framerate=30/1",
                    "!"
                    "jpegdec",
                    "!",
                    "videoconvert",
                    "!",
                    "autovideosink",
                    "sync=false",
                    ]
                self.run = subprocess.Popen(command)
                self.run.wait()
            time.sleep(1)

class GUI:
    icd = [
        "화기조준경 영상 종류", "화기조준경 영상 출력", "무선 채널 설정 모드", "60GHz 무선 채널 정보", "화기조준경 운용모드 상태",
        "무선영상 송신기 운용모드 상태", "무선영상 수신기 운용모드 상태", "무선영상 송신기 IMU Output Type", "무선영상 송신기 IMU 보정", "무선영상 수신기 IMU Output Type",
        "무선영상 수신기 IMU 보정", "화기조준경 배터리 잔량", "무선영상 송신기 배터리 잔량", "화기조준경 IR 모듈 상태", "화기조준경 EO 모듈 상태", 
        "송신기 모뎀 상태", "수신기 모뎀 상태", "송신기 IMU 상태", "수신기 IMU 상태", "BLE 상태", 
        "무선영상 송신기 IMU Data 1 H", "무선영상 송신기 IMU Data 1 L", "무선영상 송신기 IMU Data 2 H", "무선영상 송신기 IMU Data 2 L", "무선영상 송신기 IMU Data 3 H", 
        "무선영상 송신기 IMU Data 3 L", "무선영상 송신기 IMU Data 4 H", "무선영상 송신기 IMU Data 4 L", "무선영상 송신기 IMU Data 5 H", "무선영상 송신기 IMU Data 5 L", 
        "무선영상 수신기 IMU Data 1 H", "무선영상 수신기 IMU Data 1 L", "무선영상 수신기 IMU Data 2 H", "무선영상 수신기 IMU Data 2 L", "무선영상 수신기 IMU Data 3 H", 
        "무선영상 수신기 IMU Data 3 L", "무선영상 수신기 IMU Data 4 H", "무선영상 수신기 IMU Data 4 L", "무선영상 수신기 IMU Data 5 H", "무선영상 수신기 IMU Data 5 L", 
        "USB", "Vender ID H", "Vender ID L", "Product ID H", "Product ID L",
        "Channel", "PPID_H", "PPID_L", "DevID H", "DevID L",
        "Format", "Index", "TRT", "ACK", "PPC",
        "RUN", "TXID_HH", "TXID_HL", "TXID_LH", "TXID_LL",
        "RXID_HH", "RXID_HL", "RXID_LH", "RXID_LL", "CNT_H",
        "CNT_L",
        "USB", "PPID_H", "PPID_L", "DevID H", "DevID L",
        "Format", "Index",
        "FPS_HH", "FPS_HL", "FPS_LH", "FPS_LL",
        "TRT", "ACK", "PPC", "RUN", "ITF",
        "TXID_HH", "TXID_HL", "TXID_LH", "TXID_LL",
        "RXID_HH", "RXID_HL", "RXID_LH", "RXID_LL",
        "DST_ID_ERR_CNT_HH", "DST_ID_ERR_CNT_HL", "DST_ID_ERR_CNT_LH", "DST_ID_ERR_CNT_LL",
        "DST_ID_ERR_DIFF_CNT_H", "DST_ID_ERR_DIFF_CNT_L",
        "PHY_RX_FRAME_CNT_HH", "PHY_RX_FRAME_CNT_HL", "PHY_RX_FRAME_CNT_LH", "PHY_RX_FRAME_CNT_LL",
        "PHY_RX_FRAME_DIFF_CNT_H", "PHY_RX_FRAME_DIFF_CNT_L",
        "MFIR_HH", "MFIR_HL", "MFIR_LH", "MFIR_LL",
        "CNT_H", "CNT_L",
    ]
    def __init__(self, i2c):
        self.i2c = i2c
        self.root = tkinter.Tk()
        
        connect_frame = tkinter.Frame(self.root)
        self.create_connect_frame(connect_frame)
        connect_frame.pack()

        log_frame = tkinter.Frame(self.root)
        self.create_log_frame(log_frame)
        log_frame.pack()

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
        player.play()        
    
    def connect_frame_disconnect(self):
        self.i2c.close_port()
        self.connect = False
        player.stop()
        self.connect_frame_button.config(text = "Connect", command = self.connect_frame_connect)

    def create_log_frame(self, frame):
        idx = 0
        self.i2c_values = [tkinter.StringVar() for value in range(len(self.icd))]
        for log in self.icd:
            if ((idx % 8) == 0):
                label_line = tkinter.Frame(frame)
                label_line.pack()
                value_line = tkinter.Frame(frame)
                value_line.pack()
            tkinter.Label(label_line, text = log, width = 30).pack(side = "left")
            tkinter.Label(value_line, textvariable = self.i2c_values[idx], width = 30).pack(side = "left")
            idx = idx + 1

    def i2c_read(self, device_addr):
        #print(device_addr)
        if (self.connect == True):
            read_data = self.i2c.read_use_addr(device_addr, 0, len(self.icd))
            #message = ""
            idx = 0
            for data in read_data:
                self.i2c_values[idx].set(data)
                idx = idx + 1
                #message = message + "{}, ".format(hex(data))
            #print(message)
            self.root.after(100, self.i2c_read, device_addr)

    def run(self):
        self.root.mainloop()

i2c = I2C()
gui = GUI(i2c = i2c)
player = Player()
threading.Thread(target = player.Thread, daemon = True).start()
gui.run()
