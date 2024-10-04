import tkinter
import tkinter.ttk
import array
import em2890
import subprocess
import zingcam
import aias
import I2C
import ICD

class Debug:
    def __init__(self, window, icd):
        self.debug = tkinter.Toplevel(window)
        self.icd = icd

        self.aias_icd_status_list = [
            "화기조준경 영상 종류",
            "화기조준경 영상 출력",
            "화기조준경 운용모드 상태",
            "화기조준경 배터리 잔량",
            "화기조준경 IR 모듈 상태",
            "화기조준경 EO 모듈 상태",
            "무선 채널 설정 모드",
            "60GHz 무선 채널 정보",
            "BLE 상태",
        ]

        self.aias_trx_status_list = [
            ("운용모드 상태", 1, 1),
            ("IMU Output Type", 1, 1),
            ("IMU 보정", 1, 1),
            ("배터리 잔량", 1, 0),
            ("모뎀 상태", 1, 1),
            ("IMU 상태", 1, 1),
            ("IMU Data 1", 1, 1),
            ("IMU Data 2", 1, 1),
            ("IMU Data 3", 1, 1),
            ("IMU Data 4", 1, 1),
            ("IMU Data 5", 1, 1),
            ("IMU Checksum", 1, 1),
        ]

        self.zing_status_list = [
            ("USB", 1, 1),
            ("Vendor ID", 1, 0),
            ("Product ID", 1, 0),
            ("Channel", 1, 0),
            ("PPID", 1, 1),
            ("Device ID", 1, 1),
            ("Format", 1, 1),
            ("Index", 1, 1),
            ("FPS", 0, 1),
            ("TRT", 1, 1),
            ("ACK", 1, 1),
            ("PPC", 1, 1),
            ("RUN", 1, 1),
            ("ITF", 0, 1),
            ("TXID", 1, 1),
            ("RXID", 1, 1),
            ("DestID_ERR_CNT", 0, 1),
            ("PHY_RX_FRAME_CNT", 0, 1),
            ("MFIR", 0, 1),
            ("Count", 1, 1),
        ]

        self.status_list = []

        self.ble_frame = tkinter.Frame(self.debug)
        self.aias_trx_frame = tkinter.Frame(self.debug)
        self.aias_icd_frame = tkinter.Frame(self.debug)
        self.ble_frame.pack()
        self.aias_trx_frame.pack()
        self.aias_icd_frame.pack(expand = tkinter.YES, fill = tkinter.BOTH)

        self.create_ble_frame()
        self.create_aias_trx_frame()
        self.create_aias_icd_frame()

        self.set_icd()

    def create_ble_frame(self):
        zing_status_frame = tkinter.LabelFrame(self.ble_frame, text = "ZING")
        self.hble_frame = tkinter.LabelFrame(self.ble_frame, text = "ZCH")
        dble_frame = tkinter.LabelFrame(self.ble_frame, text = "ZCD")

        for status in self.zing_status_list:
            name, hble, dble = status
            tkinter.Label(zing_status_frame, text = name, width = 20).pack()
            if (hble == 1):
                zch = "ZCH {}".format(name)
                idx = self.icd.icd_name_list.index(zch)
                hble_label = tkinter.Label(self.hble_frame, text = "", width = 20)
                self.status_list.append((idx, hble_label))
                print("ZCH", idx, hble_label)
                hble_label.pack()
            else:
                tkinter.Label(self.hble_frame).pack()

            if (dble == 1):
                zcd = "ZCD {}".format(name)
                idx = self.icd.icd_name_list.index(zcd)
                dble_label = tkinter.Label(dble_frame, text = "", width = 20)
                self.status_list.append((idx, dble_label))
                print("ZCD", idx, zcd)
                dble_label.pack()
            else:
                tkinter.Label(dble_frame).pack()

        zing_status_frame.pack(side = tkinter.LEFT)
        self.hble_frame.pack(side = tkinter.LEFT)
        dble_frame.pack()
                
    def create_aias_trx_frame(self):
        aias_trx_status_frame = tkinter.LabelFrame(self.aias_trx_frame, text = "AIAS 송수신기")
        aias_tx_frame = tkinter.LabelFrame(self.aias_trx_frame, text = "무선영상 송신기")
        aias_rx_frame = tkinter.LabelFrame(self.aias_trx_frame, text = "무선영상 수신기")

        for status in self.aias_trx_status_list:
            name, aias_tx, aias_rx = status
            tkinter.Label(aias_trx_status_frame, text = name, width = 20).pack()
            if (aias_tx == 1):
                aias_tx_label = "무선영상 송신기 {}".format(name)
                idx = self.icd.icd_name_list.index(aias_tx_label)
                aias_label = tkinter.Label(aias_tx_frame, text = "", width = 20)
                self.status_list.append((idx, aias_label))
                aias_label.pack()
            else:
                tkinter.Label(aias_tx_frame).pack()

            if (aias_rx == 1):
                aias_rx_label = "무선영상 수신기 {}".format(name)
                idx = self.icd.icd_name_list.index(aias_rx_label)
                aias_label = tkinter.Label(aias_rx_frame, text = "", width = 20)
                self.status_list.append((idx, aias_label))
                aias_label.pack()
            else:
                tkinter.Label(aias_rx_frame).pack()

        aias_trx_status_frame.pack(side = tkinter.LEFT)
        aias_tx_frame.pack(side = tkinter.LEFT)
        aias_rx_frame.pack()

    def create_aias_icd_frame(self):
        aias_icd_status_frame = tkinter.LabelFrame(self.aias_icd_frame, text = "AIAS")
        icd_frame = tkinter.LabelFrame(self.aias_icd_frame, text = "화기조준경 외")
        
        for name in self.aias_icd_status_list:
            tkinter.Label(aias_icd_status_frame, text = name, width = 20).pack()
            idx = self.icd.icd_name_list.index(name)
            aias_label = tkinter.Label(icd_frame, text = "", width = 20)
            self.status_list.append((idx, aias_label))
            aias_label.pack()

        aias_icd_status_frame.pack(side = tkinter.LEFT)
        icd_frame.pack(expand = tkinter.YES, fill = tkinter.BOTH)

    def set_icd(self):
        if (self.icd.icd_list[self.icd.icd_name_list.index("화기조준경 영상 종류")] == 0x04):
            self.hble_frame.config(text = "ZCH")
            ZED = 0
        else:
            self.hble_frame.config(text = "ZED")
            ZED = 1

        for status in self.status_list:
            idx, label = status
            value = self.icd.icd_list[idx]
            if (self.icd.icd_name_list[idx] == 'ZCH Vendor ID' or self.icd.icd_name_list[idx] == 'ZCH Product ID' ):
                if (ZED == 1):
                    label.config(text = "")
                else:
                    label.config(text = "0x{:X}".format(value))    
            elif (self.icd.icd_type_list[idx] == 'd'):
                label.config(text = "{:d}".format(value))
            elif (self.icd.icd_type_list[idx] == 'x'):
                label.config(text = "0x{:X}".format(value))
            elif (self.icd.icd_type_list[idx] == 'c'):
                label.config(text = "{:c}".format(value))
            elif (self.icd.icd_type_list[idx] == "MFIR"):
                upper = (value & 0xFFFF0000) >> 16
                lower = (value & 0x0000FFFF)
                label.config(text = "{:d} / {:d}".format(upper, lower))

            elif (self.icd.icd_type_list[idx] == "화기조준경 영상 종류"):
                try:
                    label.config(text = "{} (0x{})".format(aias.scope_camera[value], value))
                except:
                    label.config(text = "ERR (0x{})".format(value))
            elif (self.icd.icd_type_list[idx] == "화기조준경 영상 출력"):
                try:
                    label.config(text = "{} (0x{})".format(aias.scope_output[value], value))
                except:
                    label.config(text = "ERR (0x{})".format(value))
            elif (self.icd.icd_type_list[idx] == "무선 채널 설정 모드"):
                try:
                    label.config(text = "{} (0x{})".format(aias.wireless_video_channel_mode[value], value))
                except:
                    label.config(text = "ERR (0x{})".format(value))
            elif (self.icd.icd_type_list[idx] == "60GHz 무선 채널 정보"):
                try:
                    label.config(text = "{} (0x{})".format(aias.wireless_video_channel_information[value], value))
                except:
                    label.config(text = "ERR (0x{})".format(value))
            elif (self.icd.icd_type_list[idx] == "화기조준경 운용모드 상태"):
                try:
                    label.config(text = "{} (0x{})".format(aias.scope_operation_mode[value], value))
                except:
                    label.config(text = "ERR (0x{})".format(value))
            elif (self.icd.icd_type_list[idx] == "무선영상 송신기 운용모드 상태"):
                try:
                    label.config(text = "{} (0x{})".format(aias.wireless_video_transmitter_operation_mode_status[value], value))
                except:
                    label.config(text = "ERR (0x{})".format(value))
            elif (self.icd.icd_type_list[idx] == "무선영상 수신기 운용모드 상태"):
                try:
                    label.config(text = "{} (0x{})".format(aias.wireless_video_receiver_operation_mode_status[value], value))
                except:
                    label.config(text = "ERR (0x{})".format(value))
            elif (self.icd.icd_type_list[idx] == "무선영상 송신기 IMU Output Type"):
                try:
                    label.config(text = "{} (0x{})".format(aias.wireless_video_transmitter_imu_output_type[value], value))
                except:
                    label.config(text = "ERR (0x{})".format(value))
            elif (self.icd.icd_type_list[idx] == "무선영상 송신기 IMU 보정"):
                try:
                    label.config(text = "{} (0x{})".format(aias.wireless_video_transmitter_imu_calibrate[value], value))
                except:
                    label.config(text = "ERR (0x{})".format(value))
            elif (self.icd.icd_type_list[idx] == "무선영상 수신기 IMU Output Type"):
                try:
                    label.config(text = "{} (0x{})".format(aias.wireless_video_receiver_imu_output_type[value], value))
                except:
                    label.config(text = "ERR (0x{})".format(value))
            elif (self.icd.icd_type_list[idx] == "무선영상 수신기 IMU 보정"):
                try:
                    label.config(text = "{} (0x{})".format(aias.wireless_video_receiver_imu_calibrate[value], value))
                except:
                    label.config(text = "ERR (0x{})".format(value))
            elif (self.icd.icd_type_list[idx] == "화기조준경 IR 모듈 상태"):
                try:
                    label.config(text = "{} (0x{})".format(aias.scope_ir_module_status[value], value))
                except:
                    label.config(text = "ERR (0x{})".format(value))
            elif (self.icd.icd_type_list[idx] == "화기조준경 EO 모듈 상태"):
                try:
                    label.config(text = "{} (0x{})".format(aias.scope_eo_module_status[value], value))
                except:
                    label.config(text = "ERR (0x{})".format(value))
            elif (self.icd.icd_type_list[idx] == "무선영상 송신기 모뎀 상태"):
                try:
                    label.config(text = "{} (0x{})".format(aias.wireless_video_transmitter_modem_status[value], value))
                except:
                    label.config(text = "ERR (0x{})".format(value))
            elif (self.icd.icd_type_list[idx] == "무선영상 수신기 모뎀 상태"):
                try:
                    label.config(text = "{} (0x{})".format(aias.wireless_video_receiver_modem_status[value], value))
                except:
                    label.config(text = "ERR (0x{})".format(value))
            elif (self.icd.icd_type_list[idx] == "무선영상 송신기 IMU 상태"):
                try:
                    label.config(text = "{} (0x{})".format(aias.wireless_video_transmitter_imu_status[value], value))
                except:
                    label.config(text = "ERR (0x{})".format(value))
            elif (self.icd.icd_type_list[idx] == "무선영상 수신기 IMU 상태"):
                try:
                    label.config(text = "{} (0x{})".format(aias.wireless_video_receiver_imu_status[value], value))
                except:
                    label.config(text = "ERR (0x{})".format(value))
            elif (self.icd.icd_type_list[idx] == "BLE 상태"):
                try:
                    label.config(text = "{} (0x{})".format(aias.aias_ble_status[value], value))
                except:
                    label.config(text = "ERR (0x{})".format(value))
            elif (self.icd.icd_type_list[idx] == 'BND'):
                try:
                    label.config(text = zingcam.BND[value])
                except:
                    label.config(text = "ERR")
            elif (self.icd.icd_type_list[idx] == 'FMT'):
                try:
                    if (self.icd.icd_name_list[idx] == 'ZCH Format'):
                        if (ZED != 1):
                            label.config(text = em2890.FMT[value])
                        else:
                            label.config(text = "")
                    else:
                        label.config(text = zingcam.FMT[value])
                except:
                    label.config(text = "ERR")
            elif (self.icd.icd_type_list[idx] == 'IDX'):
                try:
                    if (self.icd.icd_name_list[idx] == 'ZCH Index'):
                        if (ZED != 1):
                            label.config(text = em2890.IDX[value])
                        else:
                            label.config(text = "")
                    else:
                        label.config(text = zingcam.IDX[value])
                except:
                    label.config(text = "ERR")
            elif (self.icd.icd_type_list[idx] == 'FPS'):
                try:
                    label.config(text = "{:d}".format(zingcam.FPSCAL(value)))
                except:
                    label.config(text = "ERR")
            elif (self.icd.icd_type_list[idx] == 'TRT'):
                try:
                    label.config(text = zingcam.TRT[value])
                except:
                    label.config(text = "ERR")
            elif (self.icd.icd_type_list[idx] == 'ACK'):
                try:
                    label.config(text = zingcam.ACK[value])
                except:
                    label.config(text = "ERR")
            elif (self.icd.icd_type_list[idx] == 'PPC'):
                try:
                    label.config(text = zingcam.PPC[value])
                except:
                    label.config(text = "ERR")
            elif (self.icd.icd_type_list[idx] == 'RUN'):
                try:
                    label.config(text = zingcam.RUN[value])
                except:
                    label.config(text = "ERR")
            elif (self.icd.icd_type_list[idx] == 'ITF'):
                try:
                    label.config(text = zingcam.ITF[value])
                except:
                    label.config(text = "ERR")

        self.debug.after(100, self.set_icd)

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
        self.selected_scope_camera = tkinter.StringVar(value="0")  # Default value

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

        scope_camera_dropdown = tkinter.ttk.Combobox(lframe, state="readonly", textvariable=self.selected_scope_camera)
        formatted_values = [f"{value} (0x{key:X})" for key, value in self.scope_camera_options.items()]
        scope_camera_dropdown.config(values=formatted_values)
        scope_camera_dropdown.current(0)  # Set default selection
        scope_camera_dropdown.pack()

    def i2c_read(self, device_addr):
        if (self.connected == True):
            read_data = self.i2c.read_use_addr(device_addr)
            idx = 0
            try:
                for data in read_data:
                    self.icd.icd_i2c_list[idx] = int(data)
                    idx = idx + 1
                self.icd.set_icd()
            except:
                pass
            #print(self.icd.icd_i2c_list)
            self.window.after(100, self.i2c_read, device_addr)

    def create_gui_window(self):
        if (self.connected == True):
            self.debug = Debug(self.window, self.icd)

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
