import tkinter
import tkinter.ttk
import em2890
import subprocess
import aias
import I2C
import ICD
import Debug
import re

class IE:
    def __init__(self, i2c):
        self.i2c = i2c
        self.icd = ICD.ICD()
        self.window = tkinter.Tk()
        self.initialize_scope_camera_options()

        self.ports = tkinter.StringVar()
        self.connected = False
        self.tx_imu_type = 0
        self.rx_imu_type = 0
        
        self.create_connect_frame()
        self.create_main_frame()

        self.window.title("IE")
        self.window.mainloop()

    def initialize_scope_camera_options(self):
        self.scope_camera_options = aias.scope_camera
        self.selected_scope_camera = tkinter.StringVar(value="EO")  # Set "EO" as the default

    def create_connect_frame(self):
        ports_list = self.i2c.get_ports()
        self.connect_frame = tkinter.Frame(self.window)
        label_frame = tkinter.LabelFrame(self.window, text = "Connected to Ports")

        connect_frame_dropdown = tkinter.ttk.Combobox(label_frame, state = "readonly", textvariable = self.ports)
        connect_frame_dropdown.config(values = ports_list, width = 25)
        connect_frame_dropdown.current(0)
        self.connect_frame_button = tkinter.Button(label_frame, text = "Connect", command = self.connect, width = 10)

        connect_frame_dropdown.pack(side = tkinter.LEFT, padx = 5, pady = 5)
        self.connect_frame_button.pack(side = tkinter.LEFT, padx= 5, pady = 5)

        label_frame.pack(fill = tkinter.BOTH)

        self.connect_frame.pack(fill = tkinter.BOTH, expand = tkinter.YES)

    def connect(self):
        if (self.connected == False):
            self.connect_frame_button.config(state="disabled")
            self.connected = True 
            self.i2c.open_port(self.ports.get())
            try:
                i2c_address = self.i2c.get_address()
            except IndexError:
                print("No devices found")
                return
            self.i2c_read(i2c_address)

    def ch1_button(self):
        data = [0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0]
        self.i2c.send_use_addr(self.i2c.get_address(), data)

    def ch2_button(self):
        data = [0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0]
        self.i2c.send_use_addr(self.i2c.get_address(), data)

    def auto_button(self):
        data = [0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0]
        self.i2c.send_use_addr(self.i2c.get_address(), data)

    def tx_euler_button(self):
        self.tx_imu_type = 0
        data = [0, 0, 2, 1, 0, 0, 0, self.tx_imu_type, 0, self.rx_imu_type, 0]
        self.i2c.send_use_addr(self.i2c.get_address(), data)

    def tx_quaternion_button(self):
        self.tx_imu_type = 1
        data = [0, 0, 2, 1, 0, 0, 0, self.tx_imu_type, 0, self.rx_imu_type, 0]
        self.i2c.send_use_addr(self.i2c.get_address(), data)

    def rx_euler_button(self):
        self.rx_imu_type = 0
        data = [0, 0, 2, 1, 0, 0, 0, self.tx_imu_type, 0, self.rx_imu_type, 0]
        self.i2c.send_use_addr(self.i2c.get_address(), data)

    def rx_quaternion_button(self):
        self.rx_imu_type = 1
        data = [0, 0, 2, 1, 0, 0, 0, self.tx_imu_type, 0, self.rx_imu_type, 0]
        self.i2c.send_use_addr(self.i2c.get_address(), data)

    def tx_imu_gyro_cal_button(self):
        data = [0, 0, 2, 1, 0, 0, 0, self.tx_imu_type, 1, self.rx_imu_type, 0]
        self.i2c.send_use_addr(self.i2c.get_address(), data)

    def tx_imu_acc_cal_button(self):
        data = [0, 0, 2, 1, 0, 0, 0, self.tx_imu_type, 2, self.rx_imu_type, 0]
        self.i2c.send_use_addr(self.i2c.get_address(), data)

    def tx_imu_mag_cal_button(self):
        data = [0, 0, 2, 1, 0, 0, 0, self.tx_imu_type, 3, self.rx_imu_type, 0]
        self.i2c.send_use_addr(self.i2c.get_address(), data)

    def tx_imu_mag_cal_done_button(self):
        data = [0, 0, 2, 1, 0, 0, 0, self.tx_imu_type, 4, self.rx_imu_type, 0]
        self.i2c.send_use_addr(self.i2c.get_address(), data)

    def rx_imu_gyro_cal_button(self):
        data = [0, 0, 2, 1, 0, 0, 0, self.tx_imu_type, 0, self.rx_imu_type, 1]
        self.i2c.send_use_addr(self.i2c.get_address(), data)

    def rx_imu_acc_cal_button(self):
        data = [0, 0, 2, 1, 0, 0, 0, self.tx_imu_type, 0, self.rx_imu_type, 2]
        self.i2c.send_use_addr(self.i2c.get_address(), data)

    def rx_imu_mag_cal_button(self):
        data = [0, 0, 2, 1, 0, 0, 0, self.tx_imu_type, 0, self.rx_imu_type, 3]
        self.i2c.send_use_addr(self.i2c.get_address(), data)

    def rx_imu_mag_cal_done_button(self):
        data = [0, 0, 2, 1, 0, 0, 0, self.tx_imu_type, 0, self.rx_imu_type, 4]
        self.i2c.send_use_addr(self.i2c.get_address(), data)

    def eo_camera(self):
        data = [1, 0, 2, 1, 0, 0, 0, self.tx_imu_type, 0, self.rx_imu_type, 4]
        self.i2c.send_use_addr(self.i2c.get_address(), data)

    def ir_camera(self):
        data = [2, 0, 2, 1, 0, 0, 0, self.tx_imu_type, 0, self.rx_imu_type, 4]
        self.i2c.send_use_addr(self.i2c.get_address(), data)

    def create_main_frame(self):
        self.main_frame = tkinter.Frame(self.window)
        label_frame = tkinter.LabelFrame(self.main_frame, text = "Open GUI")

        gui_button = tkinter.Button(label_frame, text = "Open Debug Window", width = 20, command = self.create_gui_window)
        gui_button.pack(padx = 5, pady = 5)
        cam_button = tkinter.Button(label_frame, text = "Open Camera Output", width = 20, command = self.create_camera_output)
        cam_button.pack(padx = 5, pady = 5)
        ch1_button = tkinter.Button(label_frame, text = "Set CH1", width = 20, command = self.ch1_button)
        ch1_button.pack(padx = 5, pady = 5)
        ch2_button = tkinter.Button(label_frame, text = "Set CH2", width = 20, command = self.ch2_button)
        ch2_button.pack(padx = 5, pady = 5)
        auto_button = tkinter.Button(label_frame, text = "자동채널변경", width = 20, command = self.auto_button)
        auto_button.pack(padx = 5, pady = 5)
        tx_euler_button = tkinter.Button(label_frame, text = "Set TX IMU Euler", width = 20, command = self.tx_euler_button)
        tx_euler_button.pack(padx = 5, pady = 5)
        tx_quaternion_button = tkinter.Button(label_frame, text = "Set TX IMU Quaternion", width = 20, command = self.tx_quaternion_button)
        tx_quaternion_button.pack(padx = 5, pady = 5)
        rx_euler_button = tkinter.Button(label_frame, text = "Set RX IMU Euler", width = 20, command = self.rx_euler_button)
        rx_euler_button.pack(padx = 5, pady = 5)
        rx_quaternion_button = tkinter.Button(label_frame, text = "Set RX IMU Quaternion", width = 20, command = self.rx_quaternion_button)
        rx_quaternion_button.pack(padx = 5, pady = 5)
        tx_imu_gyro_cal_button = tkinter.Button(label_frame, text = "TX IMU GYRO Calibration", width = 20, command = self.tx_imu_gyro_cal_button)
        tx_imu_gyro_cal_button.pack(padx = 5, pady = 5)
        tx_imu_acc_cal_button = tkinter.Button(label_frame, text = "TX IMU ACC Calibration", width = 20, command = self.tx_imu_acc_cal_button)
        tx_imu_acc_cal_button.pack(padx = 5, pady = 5)
        tx_imu_mag_cal_button = tkinter.Button(label_frame, text = "TX IMU MAG Calibration", width = 20, command = self.tx_imu_mag_cal_button)
        tx_imu_mag_cal_button.pack(padx = 5, pady = 5)
        tx_imu_mag_cal_done_button = tkinter.Button(label_frame, text = "TX IMU MAG Calibration Done", width = 20, command = self.tx_imu_mag_cal_done_button)
        tx_imu_mag_cal_done_button.pack(padx = 5, pady = 5)
        rx_imu_gyro_cal_button = tkinter.Button(label_frame, text = "RX IMU GYRO Calibration", width = 20, command = self.rx_imu_gyro_cal_button)
        rx_imu_gyro_cal_button.pack(padx = 5, pady = 5)
        rx_imu_acc_cal_button = tkinter.Button(label_frame, text = "RX IMU ACC Calibration", width = 20, command = self.rx_imu_acc_cal_button)
        rx_imu_acc_cal_button.pack(padx = 5, pady = 5)
        rx_imu_mag_cal_button = tkinter.Button(label_frame, text = "RX IMU MAG Calibration", width = 20, command = self.rx_imu_mag_cal_button)
        rx_imu_mag_cal_button.pack(padx = 5, pady = 5)
        rx_imu_mag_cal_done_button = tkinter.Button(label_frame, text = "RX IMU Calibration Done", width = 20, command = self.rx_imu_mag_cal_done_button)
        rx_imu_mag_cal_done_button.pack(padx = 5, pady = 5)
        rx_imu_mag_cal_done_button = tkinter.Button(label_frame, text = "EO", width = 20, command = self.eo_camera)
        rx_imu_mag_cal_done_button.pack(padx = 5, pady = 5)
        rx_imu_mag_cal_done_button = tkinter.Button(label_frame, text = "IR", width = 20, command = self.ir_camera)
        rx_imu_mag_cal_done_button.pack(padx = 5, pady = 5)

        self.scope_camera_combo(label_frame)

        label_frame.pack(fill = tkinter.BOTH)

        self.main_frame.pack(fill = tkinter.BOTH, expand = tkinter.YES)

    def scope_camera_combo(self,lframe):
        # Combo box for selecting scope_camera
        scope_camera_label = tkinter.Label(lframe, text="Scope Camera")
        scope_camera_label.pack(side="left")

        self.scope_camera_dropdown = tkinter.ttk.Combobox(lframe, state="readonly", textvariable=self.selected_scope_camera)
        formatted_values = [f"{value} (0x{key:X})" for key, value in self.scope_camera_options.items()]
        self.scope_camera_dropdown.config(values=formatted_values)

        # Find the index of the initial value to set as the default
        selected_value = self.selected_scope_camera.get()
        initial_index = formatted_values.index(f"{selected_value} (0x{list(self.scope_camera_options.keys())[list(self.scope_camera_options.values()).index(selected_value)]:X})")

        self.scope_camera_dropdown.current(initial_index)  # Set the default selection
        self.scope_camera_dropdown.pack()

        # Bind event to detect combo box value change
        self.scope_camera_dropdown.bind("<<ComboboxSelected>>", self.on_scope_camera_selected)

    def get_scope_camera_value(self,scope_camera_combo_val):
        hex_value = re.search(r'0x[0-9a-fA-F]+', scope_camera_combo_val)
        return int(hex_value.group(0), 16)
    
    def on_scope_camera_selected(self, event):
        # This method is called whenever the user selects a new item in the combobox
        selected_value = self.scope_camera_dropdown.get()
        # Store the new value in the selected_scope_camera
        self.selected_scope_camera.set(selected_value)
        print(f"Updated selected_scope_camera: {self.selected_scope_camera.get()}")

        read_values = []
        for name in self.icd.icd_name_list[:11]:
            index = self.icd.icd_name_list.index(name)
            val = self.icd.icd_list[index]
            read_values.append(val)
            print(name, val)
        print(read_values)

        write_values = read_values
        write_values[0] = self.get_scope_camera_value(selected_value)
        print(write_values)

        self.i2c.send_use_addr(self.i2c.get_address(),write_values)

    def update_values(self):
        scope_camera_idx = self.icd.icd_name_list.index("화기조준경 영상 종류")
        cur = self.scope_camera_dropdown.current()
        val = self.icd.icd_list[scope_camera_idx]
        if val != cur:
            self.scope_camera_dropdown.current(val)

    def i2c_read(self, device_addr):
        if (self.connected == True):
            read_data = self.i2c.read_use_addr(device_addr)
            idx = 0
            try:
                for data in read_data:
                    self.icd.icd_i2c_list[idx] = int(data)
                    idx = idx + 1
                self.icd.set_icd()
                self.update_values()
            except:
                pass
            #print(self.icd.icd_i2c_list)
            self.window.after(100, self.i2c_read, device_addr)

    def create_gui_window(self):
        if (self.connected == True):
            self.debug = Debug.Debug(self.window, self.icd)

    def create_camera_output(self):
        vendor_id_idx = self.icd.icd_name_list.index("ZCH Vendor ID")
        product_id_idx = self.icd.icd_name_list.index("ZCH Product ID")
        #format_idx = self.icd.icd_name_list.index("ZCH Format")
        #index_idx = self.icd.icd_name_list.index("ZCH Index")
        fps_idx = self.icd.icd_name_list.index("ZCD FPS")

        vendor_id = self.icd.icd_list[vendor_id_idx]
        product_id = self.icd.icd_list[product_id_idx]
        #format = self.icd.icd_list[format_idx]
        #index = self.icd.icd_list[index_idx]
        fps = self.icd.icd_list[fps_idx]

        if (self.connected == True):
            if (vendor_id == em2890.VND and product_id == em2890.PRD):
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
            else:
                scope_camera_idx = self.icd.icd_name_list.index("화기조준경 영상 종류")
                scope_camera = self.icd.icd_list[scope_camera_idx]

                if (scope_camera == 0x01):
                    command = [
                        "D:/gstreamer/1.0/msvc_x86_64/bin/gst-launch-1.0",
                        "ksvideosrc",
                        "device-index=0",
                        "!"
                        "video/x-raw,width=640,height=480,framerate=30/1,format=UYVY",
                        "!"
                        "videoconvert"
                        "!"
                        "autovideosink",
                        "sync=false",
                        ]
                    self.run = subprocess.Popen(command)
                elif (scope_camera == 0x02):
                    command = [
                        "D:/gstreamer/1.0/msvc_x86_64/bin/gst-launch-1.0",
                        "ksvideosrc",
                        "device-index=0",
                        "!"
                        "video/x-raw,width=640,height=480,framerate=30/1",
                        "!"
                        "videoconvert"
                        "!"
                        "autovideosink",
                        "sync=false",
                        ]
                    self.run = subprocess.Popen(command)


i2c = I2C.I2C()
ie = IE(i2c)
