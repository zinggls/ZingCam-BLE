import zmq
import tkinter
import threading
import time
import json
import setting

request_context = zmq.Context()
request_socket = request_context.socket(zmq.REQ)
request_socket.connect("tcp://{}:{}".format(setting.IP, setting.REQREP_PORT))

subscribe_context = zmq.Context()
subscribe_socket = subscribe_context.socket(zmq.SUB)
subscribe_socket.connect("tcp://{}:{}".format(setting.IP, setting.PUBSUB_PORT))
subscribe_socket.subscribe("")

def req(cmd):
    request_socket.send(cmd.encode())
    return request_socket.recv().decode()

NUM_HOST_STATUS = int(req("num_host_status"))
NUM_DEVICE_STATUS = int(req("num_device_status"))

host_status = json.loads(subscribe_socket.recv())
device_status = json.loads(subscribe_socket.recv())

request_socket.send("host_status_name".encode())
host_status_name = json.loads(request_socket.recv())

request_socket.send("device_status_name".encode())
device_status_name = json.loads(request_socket.recv())

summary_names = ["Running", "Interference", "USB", "Format", "Resolution", "Channel", "Bulk/Isoc", "ACK", "PPC/DEV", "TXID", "RXID", "MFIR"]

def ZingGatewayGUI(host_status, device_status, host_status_name, device_status_name, NUM_HOST_STATUS, NUM_DEVICE_STATUS):
    root = tkinter.Tk()
    root.title("ZingGatewayGUI")

    host_sof_value_list = [tkinter.StringVar() for i in range(3)]
    device_sof_value_list = [tkinter.StringVar() for i in range(3)]
    host_acc_value_list = [tkinter.StringVar() for i in range(3)]
    device_acc_value_list = [tkinter.StringVar() for i in range(3)]
    host_gyr_value_list = [tkinter.StringVar() for i in range(3)]
    device_gyr_value_list = [tkinter.StringVar() for i in range(3)]
    host_mag_value_list = [tkinter.StringVar() for i in range(3)]
    device_mag_value_list = [tkinter.StringVar() for i in range(3)]
    host_status_value_list = [tkinter.StringVar() for i in range(NUM_HOST_STATUS)]
    device_status_value_list = [tkinter.StringVar() for i in range(NUM_DEVICE_STATUS)]
    host_summary_list = [tkinter.StringVar() for i in range(len(summary_names))]
    device_summary_list = [tkinter.StringVar() for i in range(len(summary_names))]

    status = [host_sof_value_list, device_sof_value_list,
              host_acc_value_list, device_acc_value_list,
              host_gyr_value_list, device_gyr_value_list,
              host_mag_value_list, device_mag_value_list,
              host_status_value_list, device_status_value_list,
              host_summary_list, device_summary_list]
    threading.Thread(target = set_value, args = (status, host_status, device_status, host_status_name, device_status_name, NUM_HOST_STATUS, NUM_DEVICE_STATUS), daemon = True).start()

    imu = [host_sof_value_list, device_sof_value_list,
              host_acc_value_list, device_acc_value_list,
              host_gyr_value_list, device_gyr_value_list,
              host_mag_value_list, device_mag_value_list]

    values = [host_status_value_list, device_status_value_list]
    summaries = [host_summary_list, device_summary_list]

    #create_imu_window(host_imu_frame, 0, imu)
    #create_imu_window(device_imu_frame, 1, imu)

    status_frame = tkinter.Frame(root)
    status_frame.grid(row = 1, column = 0, sticky = "NWES")

    host_summary_frame = tkinter.Frame(status_frame)
    host_status_frame = tkinter.Frame(status_frame)
    host_imu_frame = tkinter.Frame(status_frame)
    device_summary_frame = tkinter.Frame(status_frame)
    device_status_frame = tkinter.Frame(status_frame)
    device_imu_frame = tkinter.Frame(status_frame)

    create_summary_window(host_summary_frame, 0, summaries)
    create_status_window(host_status_frame, 0, values, host_status_name, device_status_name, NUM_HOST_STATUS, NUM_DEVICE_STATUS)
    create_imu_window(host_imu_frame, 0, imu)
    create_summary_window(device_summary_frame, 1, summaries)
    create_status_window(device_status_frame, 1, values, host_status_name, device_status_name, NUM_HOST_STATUS, NUM_DEVICE_STATUS)
    create_imu_window(device_imu_frame, 1, imu)

    host_summary_frame.grid(row = 0, column = 0)
    host_status_frame.grid(row = 1, column = 0)
    host_imu_frame.grid(row = 2, column = 0)
    device_summary_frame.grid(row = 0, column = 1)
    device_status_frame.grid(row = 1, column = 1)
    device_imu_frame.grid(row = 2, column = 1)

    root.mainloop()

def set_value(status, host_status, device_status, host_status_name, device_status_name, NUM_HOST_STATUS, NUM_DEVICE_STATUS):
    global host_running_label
    global device_running_label
    global device_interference_label

    host_sof_value_list = status[0]
    device_sof_value_list = status[1]
    host_acc_value_list = status[2]
    device_acc_value_list = status[3]
    host_gyr_value_list = status[4]
    device_gyr_value_list = status[5]
    host_mag_value_list = status[6]
    device_mag_value_list = status[7]
    host_status_value_list = status[8]
    device_status_value_list = status[9]
    host_summary_list = status[10]
    device_summary_list = status[11]
    
    while True:
        
        host_status = json.loads(subscribe_socket.recv())
        device_status = json.loads(subscribe_socket.recv())

        request_socket.send("host_status_name".encode())
        host_status_name = json.loads(request_socket.recv())

        request_socket.send("device_status_name".encode())
        device_status_name = json.loads(request_socket.recv())

        try:
            for i in range(3):
                host_sof_value_list[i].set(host_status[host_status_name[i]])
                device_sof_value_list[i].set(device_status[device_status_name[i]])
            for i in range(3):
                host_acc_value_list[i].set(host_status[host_status_name[i + 3]])
                device_acc_value_list[i].set(device_status[device_status_name[i + 3]])
            for i in range(3):
                host_gyr_value_list[i].set(host_status[host_status_name[i + 6]])
                device_gyr_value_list[i].set(device_status[device_status_name[i + 6]])
            for i in range(3):
                host_mag_value_list[i].set(host_status[host_status_name[i + 9]])
                device_mag_value_list[i].set(device_status[device_status_name[i + 9]])
            for i in range(NUM_HOST_STATUS):
                host_status_value_list[i].set(host_status[host_status_name[i + 12]])
                if (i == host_status_name.index("RUN") - 12): # RUN
                    if (host_status[host_status_name[i + 12]] == 'Y'):
                        host_summary_list[0].set("●")
                        host_running_label.configure(fg = "green")
                    elif (host_status[host_status_name[i + 12]] == 'N'):
                        host_summary_list[0].set("○")
                        host_running_label.configure(fg = "red")
                    else:
                        host_summary_list[0].set("X")
                if (i == host_status_name.index("USB") - 12): # USB
                    if (host_status[host_status_name[i + 12]] == '2'):
                        host_summary_list[2].set("USB2.0")
                    elif (host_status[host_status_name[i + 12]] == '3'):
                        host_summary_list[2].set("USB3.0")
                    else:
                        host_summary_list[2].set("Unknown")
                if (i == host_status_name.index("FMT") - 12): # FMT
                    if (host_status[host_status_name[i + 12]] == '1'):
                        host_summary_list[3].set("MJPEG")
                    elif (host_status[host_status_name[i + 12]] == '2'):
                        host_summary_list[3].set("Uncompressed")
                    else:
                        host_summary_list[3].set("etc")
                if (i == host_status_name.index("IDX") - 12): # IDX
                    if (host_status[host_status_name[i + 12]] == '1'):
                        host_summary_list[4].set("1920x1080")
                    elif (host_status[host_status_name[i + 12]] == '2'):
                        host_summary_list[4].set("640x480")
                    else:
                        host_summary_list[4].set("etc")
                if (i == host_status_name.index("BND") - 12): # BND
                    if (host_status[host_status_name[i + 12]] == 'L'):
                        host_summary_list[5].set("Low Band")
                    elif (host_status[host_status_name[i + 12]] == 'H'):
                        host_summary_list[5].set("High Band")
                    else:
                        host_summary_list[5].set("etc")
                if (i == host_status_name.index("TRT") - 12): # TRT
                    if (host_status[host_status_name[i + 12]] == 'I'):
                        host_summary_list[6].set("Isochronous")
                    elif (host_status[host_status_name[i + 12]] == 'B'):
                        host_summary_list[6].set("Bulk")
                    else:
                        host_summary_list[6].set("etc")
                if (i == host_status_name.index("ACK") - 12): # ACK
                    if (host_status[host_status_name[i + 12]] == 'Y'):
                        host_summary_list[7].set("Ack")
                    elif (host_status[host_status_name[i + 12]] == 'N'):
                        host_summary_list[7].set("No Ack")
                    else:
                        host_summary_list[7].set("etc")
                if (i == host_status_name.index("PPC") - 12): # PPC
                    if (host_status[host_status_name[i + 12]] == 'P'):
                        host_summary_list[8].set("PPC")
                    elif (host_status[host_status_name[i + 12]] == 'D'):
                        host_summary_list[8].set("DEV")
                    else:
                        host_summary_list[8].set("etc")
                if (i == host_status_name.index("TXID") - 12):
                    txid = int(host_status[host_status_name[i + 12]], 16)
                    ppcid = (txid & 0xFF000000) >> 24
                    devid = (txid & 0x00FF0000) >> 16
                    ppid = txid & 0x0000FFFF
                    txid_str = "0x{:X} | 0x{:X} | 0x{:X}".format(ppcid, devid, ppid)
                    host_summary_list[9].set(txid_str)
                if (i == host_status_name.index("RXID") - 12):
                    rxid = int(host_status[host_status_name[i + 12]], 16)
                    ppcid = (rxid & 0xFF000000) >> 24
                    devid = (rxid & 0x00FF0000) >> 16
                    ppid = rxid & 0x0000FFFF
                    rxid_str = "0x{:X} | 0x{:X} | 0x{:X}".format(ppcid, devid, ppid)
                    host_summary_list[10].set(rxid_str)
            for i in range(NUM_DEVICE_STATUS):
                device_status_value_list[i].set(device_status[device_status_name[i + 12]])
                if (i == device_status_name.index("RUN") - 12): # RUN
                    if (device_status[device_status_name[i + 12]] == 'Y'):
                        device_summary_list[0].set("●")
                        device_running_label.configure(fg = "green")
                    elif (device_status[device_status_name[i + 12]] == 'N'):
                        device_summary_list[0].set("○")
                        device_running_label.configure(fg = "green")
                    else:
                        device_summary_list[0].set("X")
                if (i == device_status_name.index("ITF") - 12): # ITF
                    if (device_status[device_status_name[i + 12]] == 'Y'):
                        device_summary_list[1].set("●")
                        device_interference_label.configure(fg = "red")
                    elif (device_status[device_status_name[i + 12]] == 'N'):
                        device_summary_list[1].set("○")
                        device_interference_label.configure(fg = "red")
                    else:
                        device_summary_list[1].set("X")
                if (i == device_status_name.index("USB") - 12): # USB
                    if (device_status[device_status_name[i + 12]] == '2'):
                        device_summary_list[2].set("USB2.0")
                    elif (device_status[device_status_name[i + 12]] == '3'):
                        device_summary_list[2].set("USB3.0")
                    else:
                        device_summary_list[2].set("Unknown")
                if (i == device_status_name.index("FMT") - 12): # FMT
                    if (device_status[device_status_name[i + 12]] == '1'):
                        device_summary_list[3].set("MJPEG")
                    elif (device_status[device_status_name[i + 12]] == '2'):
                        device_summary_list[3].set("Uncompressed")
                    else:
                        device_summary_list[3].set("etc")
                if (i == device_status_name.index("IDX") - 12): # IDX
                    if (device_status[device_status_name[i + 12]] == '1'):
                        device_summary_list[4].set("1920x1080")
                    elif (device_status[device_status_name[i + 12]] == '3'):
                        device_summary_list[4].set("640x480")
                    else:
                        device_summary_list[4].set("etc")
                if (i == device_status_name.index("TRT") - 12): # TRT
                    if (device_status[device_status_name[i + 12]] == 'I'):
                        device_summary_list[6].set("Isochronous")
                    elif (device_status[device_status_name[i + 12]] == 'B'):
                        device_summary_list[6].set("Bulk")
                    else:
                        device_summary_list[6].set("etc")
                if (i == device_status_name.index("ACK") - 12): # ACK
                    if (device_status[device_status_name[i + 12]] == 'Y'):
                        device_summary_list[7].set("Ack")
                    elif (device_status[device_status_name[i + 12]] == 'N'):
                        device_summary_list[7].set("No Ack")
                    else:
                        device_summary_list[7].set("etc")
                if (i == device_status_name.index("PPC") - 12): # PPC
                    if (device_status[device_status_name[i + 12]] == 'P'):
                        device_summary_list[8].set("PPC")
                    elif (device_status[device_status_name[i + 12]] == 'D'):
                        device_summary_list[8].set("DEV")
                    else:
                        device_summary_list[8].set("etc")
                if (i == device_status_name.index("TXID") - 12):
                    txid = int(device_status[device_status_name[i + 12]], 16)
                    ppcid = (txid & 0xFF000000) >> 24
                    devid = (txid & 0x00FF0000) >> 16
                    ppid = txid & 0x0000FFFF
                    txid_str = "0x{:X} | 0x{:X} | 0x{:X}".format(ppcid, devid, ppid)
                    device_summary_list[9].set(txid_str)
                if (i == device_status_name.index("RXID") - 12):
                    rxid = int(device_status[device_status_name[i + 12]], 16)
                    ppcid = (rxid & 0xFF000000) >> 24
                    devid = (rxid & 0x00FF0000) >> 16
                    ppid = rxid & 0x0000FFFF
                    rxid_str = "0x{:X} | 0x{:X} | 0x{:X}".format(ppcid, devid, ppid)
                    device_summary_list[10].set(rxid_str)
                if (i == device_status_name.index("MFIR") - 12):
                    dst_id_err_cnt_diff = device_status[device_status_name[i + 12]].split("/")[0]
                    rx_frame_err_cnt_diff = device_status[device_status_name[i + 12]].split("/")[1]
                    mfir = float(dst_id_err_cnt_diff) / float(rx_frame_err_cnt_diff)
                    device_summary_list[11].set("{:.2f}%".format(mfir))
        except Exception as e:
            pass
            
        time.sleep(0.1)

def create_summary_window(window, device, summary_list):
    global host_running_label
    global device_running_label
    global device_interference_label

    if (device == True):
        device_str = "Device"
    else:
        device_str = "Host"

    host_summary_list = summary_list[0]
    device_summary_list = summary_list[1]

    window_label = tkinter.LabelFrame(window, text = "{} Status Summary".format(device_str), width = 350, height = 300)
    window_label.grid(row = 0, column = 0, padx = 10, pady = 10)
    window_label.grid_propagate(0)

    if (device == True):
        tkinter.Label(window_label, text = "Value").grid(row = 0, column = 1, padx = 25)
        for i in range(0, len(summary_names)):
            if (i == summary_names.index("Channel")):
                continue
            tkinter.Label(window_label, text = summary_names[i]).grid(row = i + 1, column = 0, padx = 25)
            if (i == summary_names.index("Running")):
                device_running_label = tkinter.Label(window_label, textvariable = device_summary_list[i - 12])
                device_running_label.grid(row = i + 1, column = 1, padx = 25)
            elif (i == summary_names.index("Interference")):
                device_interference_label = tkinter.Label(window_label, textvariable = device_summary_list[i - 12])
                device_interference_label.grid(row = i + 1, column = 1, padx = 25)
            else:
                tkinter.Label(window_label, textvariable = device_summary_list[i - 12]).grid(row = i + 1, column = 1, padx = 25)
    else:
        tkinter.Label(window_label, text = "Value").grid(row = 0, column = 1, padx = 25)
        for i in range(0, len(summary_names)):
            if (i == summary_names.index("Interference") or i == summary_names.index("MFIR")):
                continue
            tkinter.Label(window_label, text = summary_names[i]).grid(row = i + 1, column = 0, padx = 25)
            if (i == summary_names.index("Running")):
                host_running_label = tkinter.Label(window_label, textvariable = host_summary_list[i - 12])
                host_running_label.grid(row = i + 1, column = 1, padx = 25)
            else:
                tkinter.Label(window_label, textvariable = host_summary_list[i - 12]).grid(row = i + 1, column = 1, padx = 25)


def create_imu_window(window, device, imu_list):
    if (device == True):
        device_str = "Device"
    else:
        device_str = "Host"

    host_sof_value_list = imu_list[0]
    device_sof_value_list = imu_list[1]
    host_acc_value_list = imu_list[2]
    device_acc_value_list = imu_list[3]
    host_gyr_value_list = imu_list[4]
    device_gyr_value_list = imu_list[5]
    host_mag_value_list = imu_list[6]
    device_mag_value_list = imu_list[7]

    imu_label = tkinter.LabelFrame(window, text = "{} IMU".format(device_str), width = 350, height = 130)
    imu_label.grid(row = 1, column = 0, padx = 10, pady = 10)
    imu_label.grid_propagate(0)

    imux_label = tkinter.Label(imu_label, text = "X")
    imux_label.grid(row = 0, column = 1, padx = 25)
    imuy_label = tkinter.Label(imu_label, text = "Y")
    imuy_label.grid(row = 0, column = 2, padx = 25)
    imuz_label = tkinter.Label(imu_label, text = "Z")
    imuz_label.grid(row = 0, column = 3, padx = 25)

    sof_label = tkinter.Label(imu_label, text = "SOF")
    sof_label.grid(row = 1, column = 0, padx = 25)

    if (device == True):
        for i in range(1, 4):
            tkinter.Label(imu_label, textvariable = device_sof_value_list[i - 1]).grid(row = 1, column = i, padx = 25)
    else:
        for i in range(1, 4):
            tkinter.Label(imu_label, textvariable = host_sof_value_list[i - 1]).grid(row = 1, column = i, padx = 25)
        
    acc_label = tkinter.Label(imu_label, text = "ACC")
    acc_label.grid(row = 2, column = 0, padx = 25)

    if (device == True):
        for i in range(1, 4):
            tkinter.Label(imu_label, textvariable = device_acc_value_list[i - 1]).grid(row = 2, column = i, padx = 25)
    else:
        for i in range(1, 4):
            tkinter.Label(imu_label, textvariable = host_acc_value_list[i - 1]).grid(row = 2, column = i, padx = 25)
        
    gyr_label = tkinter.Label(imu_label, text = "GYR")
    gyr_label.grid(row = 3, column = 0, padx = 25)

    if (device == True):
        for i in range(1, 4):
            tkinter.Label(imu_label, textvariable = device_gyr_value_list[i - 1]).grid(row = 3, column = i, padx = 25)
    else:
        for i in range(1, 4):
            tkinter.Label(imu_label, textvariable = host_gyr_value_list[i - 1]).grid(row = 3, column = i, padx = 25)
        
    mag_label = tkinter.Label(imu_label, text = "MAG")
    mag_label.grid(row = 4, column = 0, padx = 25)

    if (device == True):
        for i in range(1, 4):
            tkinter.Label(imu_label, textvariable = device_mag_value_list[i - 1]).grid(row = 4, column = i, padx = 25)
    else:
        for i in range(1, 4):
            tkinter.Label(imu_label, textvariable = host_mag_value_list[i - 1]).grid(row = 4, column = i, padx = 25)
    
def create_status_window(window, device, status_list, host_status_name, device_status_name, NUM_HOST_STATUS, NUM_DEVICE_STATUS):
    if (device == True):
        device_str = "Device"
    else:
        device_str = "Host"

    host_status_value_list = status_list[0]
    device_status_value_list = status_list[1]

    window_label = tkinter.LabelFrame(window, text = "{} Status".format(device_str), width = 350, height = 400)
    window_label.grid(row = 0, column = 0, padx = 10, pady = 10)
    window_label.grid_propagate(0)

    if (device == True):
        tkinter.Label(window_label, text = "Value").grid(row = 0, column = 1, padx = 25)
        for i in range(12, 12 + NUM_DEVICE_STATUS):
            tkinter.Label(window_label, text = device_status_name[i]).grid(row = i + 1 - 12, column = 0, padx = 25)
            tkinter.Label(window_label, textvariable = device_status_value_list[i - 12]).grid(row = i + 1 - 12, column = 1, padx = 25)
    else:
        tkinter.Label(window_label, text = "Value").grid(row = 0, column = 1, padx = 25)
        for i in range(12, 12 + NUM_HOST_STATUS):
            tkinter.Label(window_label, text = host_status_name[i]).grid(row = i + 1 - 12, column = 0, padx = 25)
            tkinter.Label(window_label, textvariable = host_status_value_list[i - 12]).grid(row = i + 1 - 12, column = 1, padx = 25)


ZingGatewayGUI(host_status, device_status, host_status_name, device_status_name, NUM_HOST_STATUS, NUM_DEVICE_STATUS)
