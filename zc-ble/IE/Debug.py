import tkinter
import em2890

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