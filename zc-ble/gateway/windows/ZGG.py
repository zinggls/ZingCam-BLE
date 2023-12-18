import zmq
import tkinter
import threading
import json
import setting
import time
from tkinter import font
import hu205
import em2890
import zing_dev

def req(cmd):
    request_socket.send(cmd.encode())
    return request_socket.recv().decode()

request_context = zmq.Context()
request_socket = request_context.socket(zmq.REQ)
request_socket.connect("tcp://{}:{}".format(setting.IP, setting.REQREP_PORT))

subscribe_context = zmq.Context()
subscribe_socket = subscribe_context.socket(zmq.SUB)
subscribe_socket.connect("tcp://{}:{}".format(setting.IP, setting.PUBSUB_PORT))
subscribe_socket.subscribe("")

NUM_HOST_STATUS = int(req("num_host_status"))
NUM_DEVICE_STATUS = int(req("num_device_status"))

LABEL_WIDTH = 20
ZING_VALUE_WIDTH = 39
IMU_VALUE_WIDTH = int(ZING_VALUE_WIDTH / 3)

host_status = json.loads(subscribe_socket.recv())
device_status = json.loads(subscribe_socket.recv())

request_socket.send("host_status_name".encode())
host_status_name = json.loads(request_socket.recv())

request_socket.send("device_status_name".encode())
device_status_name = json.loads(request_socket.recv())

summary_names = ["Run", "Interference", "USB", "Format", "Resolution", "Channel", "Bulk/Isoc", "ACK", "PPC/DEV", "TxID", "RxID", "MFIR"]

show_zing = False
show_imu = False

def ZingGatewayGUI(host_status, device_status, host_status_name, device_status_name, NUM_HOST_STATUS, NUM_DEVICE_STATUS):
    root = tkinter.Tk()
    root.title("ZingGatewayGUI")
    default_font = font.nametofont("TkDefaultFont")
    default_font.configure(family = "Consolas", size = 10)

    host_status_value_list = [tkinter.StringVar() for i in range(NUM_HOST_STATUS + 12)]
    device_status_value_list = [tkinter.StringVar() for i in range(NUM_DEVICE_STATUS + 12)]
    host_summary_value_list = [tkinter.StringVar() for i in range(len(summary_names))]
    device_summary_value_list = [tkinter.StringVar() for i in range(len(summary_names))]

    status = [host_status_value_list, device_status_value_list, host_summary_value_list, device_summary_value_list]
    summaries = [host_summary_value_list, device_summary_value_list]
    
    threading.Thread(target = set_value, args = (status, 
                                                host_status,
                                                device_status,
                                                host_status_name,
                                                device_status_name,
                                                NUM_HOST_STATUS,
                                                NUM_DEVICE_STATUS), daemon = True).start()

    values = [host_status_value_list, device_status_value_list]

    status_frame = tkinter.Frame(root)
    status_frame.grid(row = 0, column = 0)

    button_frame = tkinter.Frame(status_frame)
    button_frame.grid(row = 0, column = 0)

    summary_frame = tkinter.LabelFrame(status_frame, text = "Summary")
    summary_frame.grid(row = 1, column = 0, sticky = "nsew")
    zing_frame = tkinter.LabelFrame(status_frame, text = "ZING")
    zing_frame.grid(row = 2, column = 0, sticky = "nsew")
    imu_frame = tkinter.LabelFrame(status_frame, text = "IMU")
    imu_frame.grid(row = 3, column = 0,  sticky = "nsew")

    summary_name_frame = tkinter.Frame(summary_frame)
    summary_name_frame.grid(row = 0, column = 0)
    zing_name_frame = tkinter.Frame(zing_frame)
    zing_name_frame.grid(row = 1, column = 0)
    imu_name_frame = tkinter.Frame(imu_frame)
    imu_name_frame.grid(row = 2, column = 0)

    summary_host_frame = tkinter.Frame(summary_frame)
    summary_host_frame.grid(row = 0, column = 1)
    zing_host_frame = tkinter.Frame(zing_frame)
    zing_host_frame.grid(row = 1, column = 1)
    imu_host_frame = tkinter.Frame(imu_frame)
    imu_host_frame.grid(row = 2, column = 1)

    summary_device_frame = tkinter.Frame(summary_frame)
    summary_device_frame.grid(row = 0, column = 2)
    zing_device_frame = tkinter.Frame(zing_frame)
    zing_device_frame.grid(row = 1, column = 2)
    imu_device_frame = tkinter.Frame(imu_frame)
    imu_device_frame.grid(row = 2, column = 2)

    tkinter.Label(summary_name_frame).pack()
    tkinter.Label(summary_host_frame, text = "Host").pack()
    tkinter.Label(summary_device_frame, text = "Device").pack()

    create_summary_value_window(summary_name_frame, summaries, summary_host_frame, summary_device_frame)
    create_zing_value_window(zing_name_frame, values, zing_host_frame, zing_device_frame)
    create_imu_value_window(imu_name_frame, values, imu_host_frame, imu_device_frame)

    zing_frame.grid_forget()
    imu_frame.grid_forget()

    zing_debug_button = tkinter.Button(button_frame, text = "Show ZING", command = lambda: zing_debug(True, zing_frame, zing_debug_button))
    zing_debug_button.pack(side = "left", pady = (5, 0), padx = 5)
    imu_debug_button = tkinter.Button(button_frame, text = "Show IMU", command = lambda: imu_debug(True, imu_frame, imu_debug_button))
    imu_debug_button.pack(side = "left", pady = (5, 0), padx = 5)

    root.mainloop()

def zing_debug(show, zing_frame, btn):
    global show_zing
    global show_imu
    if (show == False):
        zing_frame.grid_forget()
        btn.configure(text = "Show ZING", command = lambda: zing_debug(True, zing_frame, btn))
        show_zing = False
    else:
        zing_frame.grid(row = 2, column = 0, sticky = "nswe")
        btn.configure(text = "Hide ZING", command = lambda: zing_debug(False, zing_frame, btn))
        show_zing = True

def imu_debug(show, imu_frame, btn):
    global show_zing
    global show_imu
    if (show == False):
        imu_frame.grid_forget()
        btn.configure(text = "Show IMU", command = lambda: imu_debug(True, imu_frame, btn))
        show_imu = False
    else:
        imu_frame.grid(row = 3, column = 0, sticky = "nswe")
        btn.configure(text = "Hide IMU", command = lambda: imu_debug(False, imu_frame, btn))
        show_imu = True

def set_value(status, host_status, device_status, host_status_name, device_status_name, NUM_HOST_STATUS, NUM_DEVICE_STATUS):
    global host_run_label
    global device_run_label
    global host_itf_label
    global device_itf_label

    host_status_value_list = status[0]
    device_status_value_list = status[1]
    host_summary_value_list = status[2]
    device_summary_value_list = status[3]

    while True:
        host_status = json.loads(subscribe_socket.recv())
        device_status = json.loads(subscribe_socket.recv())

        request_socket.send("host_status_name".encode())
        host_status_name = json.loads(request_socket.recv())

        request_socket.send("device_status_name".encode())
        device_status_name = json.loads(request_socket.recv())

        vnd = int(host_status["VND"], 16)
        prd = int(host_status["PRD"], 16)

        try:
            for i in range(NUM_HOST_STATUS + 12):
                host_status_value_list[i].set(host_status[host_status_name[i]])
                if (i == host_status_name.index("RUN")): # RUN
                    if (host_status[host_status_name[i]] == 'Y'):
                        host_summary_value_list[0].set("●")
                        host_run_label.configure(fg = "green", font = ("맑은 고딕", 10))
                    elif (host_status[host_status_name[i]] == 'N'):
                        host_summary_value_list[0].set("●")
                        host_run_label.configure(fg = "red", font = ("맑은 고딕", 10))
                    else:
                        host_summary_value_list[0].set("X")
                        host_run_label.configure(fg = "red", font = ("맑은 고딕", 10))
                host_summary_value_list[1].set("-")# ITF
                if (i == host_status_name.index("USB")): # USB
                    if (host_status[host_status_name[i]] == '2'):
                        host_summary_value_list[2].set("USB2.0")
                    elif (host_status[host_status_name[i]] == '3'):
                        host_summary_value_list[2].set("USB3.0")
                    else:
                        host_summary_value_list[2].set("etc")
                if (i == host_status_name.index("FMT")): # FMT
                    if (vnd == em2890.EM2890_VND and prd == em2890.EM2890_PRD):
                        host_summary_value_list[3].set(em2890.EM2890_FMT[host_status["FMT"]])
                    elif (vnd == hu205.HU205_FMT and prd == hu205.HU205_PRD):
                        host_summary_value_list[3].set(hu205.HU205_FMT[host_status["FMT"]])
                if (i == host_status_name.index("IDX")): # IDX
                    if (vnd == em2890.EM2890_VND and prd == em2890.EM2890_PRD):
                        host_summary_value_list[4].set(em2890.EM2890_IDX[host_status["IDX"]])
                    elif (vnd == hu205.HU205_FMT and prd == hu205.HU205_PRD):
                        host_summary_value_list[4].set(hu205.HU205_IDX[host_status["IDX"]])
                if (i == host_status_name.index("BND")): # BND
                    if (host_status[host_status_name[i]] == 'L'):
                        host_summary_value_list[5].set("Low Band")
                    elif (host_status[host_status_name[i]] == 'H'):
                        host_summary_value_list[5].set("High Band")
                    else:
                        host_summary_value_list[5].set("etc")
                if (i == host_status_name.index("TRT")): # TRT
                    if (host_status[host_status_name[i]] == 'I'):
                        host_summary_value_list[6].set("Isochronous")
                    elif (host_status[host_status_name[i]] == 'B'):
                        host_summary_value_list[6].set("Bulk")
                    else:
                        host_summary_value_list[6].set("etc")
                if (i == host_status_name.index("ACK")): # ACK
                    if (host_status[host_status_name[i]] == 'Y'):
                        host_summary_value_list[7].set("Ack")
                    elif (host_status[host_status_name[i]] == 'N'):
                        host_summary_value_list[7].set("No Ack")
                    else:
                        host_summary_value_list[7].set("etc")
                if (i == host_status_name.index("PPC")): # PPC
                    if (host_status[host_status_name[i]] == 'P'):
                        host_summary_value_list[8].set("PPC")
                    elif (host_status[host_status_name[i]] == 'D'):
                        host_summary_value_list[8].set("DEV")
                    else:
                        host_summary_value_list[8].set("etc")
                if (i == host_status_name.index("TXID")):
                    txid = int(host_status[host_status_name[i]], 16)
                    ppcid = (txid & 0xFF000000) >> 24
                    devid = (txid & 0x00FF0000) >> 16
                    ppid = txid & 0x0000FFFF
                    txid_str = "Dest(0x{:02X}) | Src(0x{:02X}) | PPID(0x{:04X})".format(ppcid, devid, ppid)
                    host_summary_value_list[9].set(txid_str)
                if (i == host_status_name.index("RXID")):
                    rxid = int(host_status[host_status_name[i]], 16)
                    ppcid = (rxid & 0xFF000000) >> 24
                    devid = (rxid & 0x00FF0000) >> 16
                    ppid = rxid & 0x0000FFFF
                    rxid_str = "Dest(0x{:02X}) | Src(0x{:02X}) | PPID(0x{:04X})".format(ppcid, devid, ppid)
                    host_summary_value_list[10].set(rxid_str)
                host_summary_value_list[11].set("-") # MFIR
            for i in range(NUM_DEVICE_STATUS + 12):
                device_status_value_list[i].set(device_status[device_status_name[i]])
                if (i == device_status_name.index("RUN")): # RUN
                    if (device_status[device_status_name[i]] == 'Y'):
                        device_summary_value_list[0].set("●")
                        device_run_label.configure(fg = "green", font = ("맑은 고딕", 10))
                    elif (device_status[device_status_name[i]] == 'N'):
                        device_summary_value_list[0].set("●")
                        device_run_label.configure(fg = "red", font = ("맑은 고딕", 10))
                    else:
                        device_summary_value_list[0].set("X")
                        device_run_label.configure(fg = "red", font = ("맑은 고딕", 10))
                if (i == device_status_name.index("ITF")): # ITF
                    if (device_status[device_status_name[i]] == 'Y'):
                        device_summary_value_list[1].set("●")
                        device_itf_label.configure(fg = "red", font = ("맑은 고딕", 10))
                    elif (device_status[device_status_name[i]] == 'N'):
                        device_summary_value_list[1].set("●")
                        device_itf_label.configure(fg = "green", font = ("맑은 고딕", 10))
                    else:
                        device_summary_value_list[1].set("X")
                        device_itf_label.configure(fg = "red", font = ("맑은 고딕", 10))
                if (i == device_status_name.index("USB")): # USB
                    if (device_status[device_status_name[i]] == '2'):
                        device_summary_value_list[2].set("USB2.0")
                    elif (device_status[device_status_name[i]] == '3'):
                        device_summary_value_list[2].set("USB3.0")
                    else:
                        device_summary_value_list[2].set("etc")
                if (i == device_status_name.index("FMT")): # FMT
                    device_summary_value_list[3].set(zing_dev.ZING_DEV_FMT[device_status["FMT"]])
                if (i == device_status_name.index("IDX")): # IDX
                    device_summary_value_list[4].set(zing_dev.ZING_DEV_IDX[device_status["IDX"]])
                device_summary_value_list[5].set("-") # BND
                if (i == device_status_name.index("TRT")): # TRT
                    if (device_status[device_status_name[i]] == 'I'):
                        device_summary_value_list[6].set("Isochronous")
                    elif (device_status[device_status_name[i]] == 'B'):
                        device_summary_value_list[6].set("Bulk")
                    else:
                        device_summary_value_list[6].set("etc")
                if (i == device_status_name.index("ACK")): # ACK
                    if (device_status[device_status_name[i]] == 'Y'):
                        device_summary_value_list[7].set("Ack")
                    elif (device_status[device_status_name[i]] == 'N'):
                        device_summary_value_list[7].set("No Ack")
                    else:
                        device_summary_value_list[7].set("etc")
                if (i == device_status_name.index("PPC")): # PPC
                    if (device_status[device_status_name[i]] == 'P'):
                        device_summary_value_list[8].set("PPC")
                    elif (device_status[device_status_name[i]] == 'D'):
                        device_summary_value_list[8].set("DEV")
                    else:
                        device_summary_value_list[8].set("etc")
                if (i == device_status_name.index("TXID")):
                    txid = int(device_status[device_status_name[i]], 16)
                    ppcid = (txid & 0xFF000000) >> 24
                    devid = (txid & 0x00FF0000) >> 16
                    ppid = txid & 0x0000FFFF
                    txid_str = "Dest(0x{:02X}) | Src(0x{:02X}) | PPID(0x{:04X})".format(ppcid, devid, ppid)
                    device_summary_value_list[9].set(txid_str)
                if (i == device_status_name.index("RXID")):
                    rxid = int(device_status[device_status_name[i]], 16)
                    ppcid = (rxid & 0xFF000000) >> 24
                    devid = (rxid & 0x00FF0000) >> 16
                    ppid = rxid & 0x0000FFFF
                    rxid_str = "Dest(0x{:02X}) | Src(0x{:02X}) | PPID(0x{:04X})".format(ppcid, devid, ppid)
                    device_summary_value_list[10].set(rxid_str)
                if (i == device_status_name.index("MFIR")):
                    dst_id_err_cnt_diff = device_status[device_status_name[i]].split("/")[0]
                    rx_frame_err_cnt_diff = device_status[device_status_name[i]].split("/")[1]
                    mfir = float(dst_id_err_cnt_diff) / float(rx_frame_err_cnt_diff)
                    device_summary_value_list[11].set("{}/{} ({:.2f}%)".format(dst_id_err_cnt_diff, rx_frame_err_cnt_diff, mfir))
        except Exception as e:
            pass
            
        time.sleep(0.1)

def create_summary_value_window(name_frame, values, host_frame, device_frame):
    global host_run_label
    global device_run_label
    global host_itf_label
    global device_itf_label

    host_summary_values = values[0]
    device_summary_values = values[1]

    for name in summary_names:
        idx = summary_names.index(name)
        tkinter.Label(name_frame, text = name, width = LABEL_WIDTH).pack()
        if (name == "Run"):
            host_run_label = tkinter.Label(host_frame, textvariable = host_summary_values[idx], width = ZING_VALUE_WIDTH)
            host_run_label.pack()
            device_run_label = tkinter.Label(device_frame, textvariable = device_summary_values[idx], width = ZING_VALUE_WIDTH)
            device_run_label.pack()
        elif (name == "Interference"):
            host_itf_label = tkinter.Label(host_frame, textvariable = host_summary_values[idx], width = ZING_VALUE_WIDTH)
            host_itf_label.pack()
            device_itf_label = tkinter.Label(device_frame, textvariable = device_summary_values[idx], width = ZING_VALUE_WIDTH)
            device_itf_label.pack()
        else:
            tkinter.Label(host_frame, textvariable = host_summary_values[idx], width = ZING_VALUE_WIDTH).pack()
            tkinter.Label(device_frame, textvariable = device_summary_values[idx], width = ZING_VALUE_WIDTH).pack()


def create_zing_value_window(name_frame, values, host_frame, device_frame):
    names = ["USB", "VND", "PRD", "BND", 
             "PID", "DID", "FMT", "IDX", 
             "TRT", "ACK", "PPC", "RUN", 
             "ITF", "TXID", "RXID", "DestID_ERR_CNT", 
             "PHY_RX_FRAME_CNT", "CNT"]
    
    host_status_values = values[0]
    device_status_values = values[1]

    for name in names:
        tkinter.Label(name_frame, text = name, width = LABEL_WIDTH).pack()
        try:
            host_idx = host_status_name.index(name)
            tkinter.Label(host_frame, textvariable = host_status_values[host_idx], width = ZING_VALUE_WIDTH).pack()
        except:
            tkinter.Label(host_frame, text = "-").pack()
        try:
            device_idx = device_status_name.index(name)
            tkinter.Label(device_frame, textvariable = device_status_values[device_idx], width = ZING_VALUE_WIDTH).pack()
        except:
            tkinter.Label(device_frame, text = "-").pack()

def create_imu_value_window(name_frame, values, host_frame, device_frame):
    names = ["SOF", "ACC", "GYR", "MAG"]
    
    host_status_values = values[0]
    device_status_values = values[1]

    imu_label = tkinter.Label(name_frame, text = "IMU")
    imu_label.pack()
    host_xyz_frame = tkinter.Frame(host_frame)
    host_xyz_frame.pack()
    device_xyz_frame = tkinter.Frame(device_frame)
    device_xyz_frame.pack()

    x = 0
    y = 0
    for label in ["X", "Y", "Z"]:
        tkinter.Label(host_xyz_frame, text = label, width = IMU_VALUE_WIDTH).grid(row = y, column = x)
        tkinter.Label(device_xyz_frame, text = label, width = IMU_VALUE_WIDTH).grid(row = y, column = x)
        x = x + 1
    y = y + 1
    for name in names:
        tkinter.Label(name_frame, text = name, width = LABEL_WIDTH).pack()
        x = 0
        for label in ["x", "y", "z"]:
            imu_name = "{}{}".format(name.lower(), label)
            host_idx = host_status_name.index(imu_name)
            device_idx = device_status_name.index(imu_name)
            tkinter.Label(host_xyz_frame, textvariable = host_status_values[host_idx]).grid(row = y, column = x)
            tkinter.Label(device_xyz_frame, textvariable = device_status_values[device_idx]).grid(row = y, column = x)
            x = x + 1
        y = y + 1


ZingGatewayGUI(host_status, device_status, host_status_name, device_status_name, NUM_HOST_STATUS, NUM_DEVICE_STATUS)
