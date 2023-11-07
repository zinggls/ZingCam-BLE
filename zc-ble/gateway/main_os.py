import zing_ble
import hu205
import em2890
import time
import subprocess
import threading
import tkinter
import tkinter.ttk

zble = zing_ble.Zing_BLE()
running = False
root = tkinter.Tk()

def connect():
    global running
    comport = uart_comm_ports.get()
    zble.connect(comport, 115200)
    running = True

uart_comm_frame = tkinter.Frame(root)
uart_comm_frame.grid(row = 0, column = 0)
items = ["COM{}".format(i + 1) for i in range(1, 25)]
uart_comm_ports = tkinter.ttk.Combobox(uart_comm_frame, values = items)
uart_comm_ports.set("COM1")
uart_comm_ports.grid(row = 0, column = 0, padx = 10)
uart_comm_button = tkinter.Button(uart_comm_frame, text = "Connect", command = connect)
uart_comm_button.grid(row = 0, column = 1, padx = 10)

status = 0
run = 0

host_sof_value_list = [tkinter.StringVar() for i in range(3)]
device_sof_value_list = [tkinter.StringVar() for i in range(3)]
host_acc_value_list = [tkinter.StringVar() for i in range(3)]
device_acc_value_list = [tkinter.StringVar() for i in range(3)]
host_gyr_value_list = [tkinter.StringVar() for i in range(3)]
device_gyr_value_list = [tkinter.StringVar() for i in range(3)]
host_mag_value_list = [tkinter.StringVar() for i in range(3)]
device_mag_value_list = [tkinter.StringVar() for i in range(3)]
host_status_value_list = [tkinter.StringVar() for i in range(zble.NUM_HOST_STATUS)]
device_status_value_list = [tkinter.StringVar() for i in range(zble.NUM_DEVICE_STATUS)]

def zing_thread():
    global status
    global run
    loop = 0
    while True:
        if (running == False):
            time.sleep(0.1)
            continue

        if (zble.get_host_status() == False):
            print("Error")
        elif (zble.get_device_status() == False):
            print("Error")
        else:
            if (loop == 0):
                oldcnt = zble.get_host_cnt()
            else:
                newcnt = zble.get_host_cnt()

                if (oldcnt != newcnt):
                    oldcnt = newcnt
                    vnd = int(zble.get_host_vnd(), 16)
                    prd = int(zble.get_host_prd(), 16)
                    
                    for i in range(3):
                        host_sof_value_list[i].set(zble.host_status_dict[zble.host_status_name[i]])
                        device_sof_value_list[i].set(zble.device_status_dict[zble.device_status_name[i]])
                    for i in range(3):
                        host_acc_value_list[i].set(zble.host_status_dict[zble.host_status_name[i + 3]])
                        device_acc_value_list[i].set(zble.device_status_dict[zble.device_status_name[i + 3]])
                    for i in range(3):
                        host_gyr_value_list[i].set(zble.host_status_dict[zble.host_status_name[i + 6]])
                        device_gyr_value_list[i].set(zble.device_status_dict[zble.device_status_name[i + 6]])
                    for i in range(3):
                        host_mag_value_list[i].set(zble.host_status_dict[zble.host_status_name[i + 9]])
                        device_mag_value_list[i].set(zble.device_status_dict[zble.device_status_name[i + 9]])
                    for i in range(zble.NUM_HOST_STATUS):
                        host_status_value_list[i].set(zble.host_status_dict[zble.host_status_name[i + 12]])
                    for i in range(zble.NUM_DEVICE_STATUS):
                        device_status_value_list[i].set(zble.device_status_dict[zble.device_status_name[i + 12]])

                    if (vnd == hu205.HU205_VND and prd == hu205.HU205_PRD):
                        status = 1
                    elif (vnd == em2890.EM2890_VND and prd == em2890.EM2890_PRD):
                        status = 2 
                    else:
                        status = 0
                else:
                    if (run != 0):
                        run.kill()
                        run = 0
                    print("Host is not running")

            loop = loop + 1
        time.sleep(0.1)

def vlc_thread():
    global status
    global run
    while True:
        if (running == False):
            time.sleep(0.1)
            continue

        if (status == 1):
            fmt = hu205.HU205_FMT[zble.get_host_fmt()]
            res = hu205.HU205_IDX[zble.get_host_idx()]
            #vlc_command = "vlc --fullscreen dshow:// :dshow-vdev=ZingCam :dshow-adev=none  :live-caching=0 :dshow-size=\"{}\" :dshow-aspect-ratio=16\:9 :dshow-fps=28 :dshow-chroma=\"{}\"".format(res, fmt)
            vlc_command = [
                'vlc',
                'dshow://',
                ':dshow-vdev=ZingCam',
                ':dshow-adev=none',
                ':live-caching=0',
                ':dshow-size={}'.format(res),
                ':dshow-fps=28',
                ':dshow-chroma={}'.format(fmt)
            ]
            run = subprocess.Popen(vlc_command)
            run.wait()
        elif (status == 2):
            fmt = em2890.EM2890_FMT[zble.get_host_fmt()]
            res = em2890.EM2890_IDX[zble.get_host_idx()]
            #vlc_command = "vlc dshow:// :dshow-vdev=ZingCam :dshow-adev=none  :live-caching=0 :dshow-size=\"{}\" :dshow-aspect-ratio=4\:3 :dshow-fps=30 :dshow-chroma=\"{}\"".format(res, fmt)
            vlc_command = [
                'vlc',
                'dshow://',
                ':dshow-vdev=ZingCam',
                ':dshow-adev=none',
                ':live-caching=0',
                ':dshow-size={}'.format(res),
                ':dshow-fps=30',
                ':dshow-chroma={}'.format(fmt)
            ]
            run = subprocess.Popen(vlc_command)
            run.wait()
        time.sleep(1)

def create_status_window(window, device):
    if (device == True):
        device_str = "Device"
    else:
        device_str = "Host"

    window_label = tkinter.LabelFrame(window, text = "{} Status".format(device_str), width = 350, height = 350)
    window_label.grid(row = 0, column = 0, padx = 10, pady = 10)
    window_label.grid_propagate(0)

    if (device == True):
        tkinter.Label(window_label, text = "Value").grid(row = 0, column = 1, padx = 25)
        for i in range(12, 12 + zble.NUM_DEVICE_STATUS):
            tkinter.Label(window_label, text = zble.device_status_name[i]).grid(row = i + 1 - 12, column = 0, padx = 25)
            tkinter.Label(window_label, textvariable = device_status_value_list[i - 12]).grid(row = i + 1 - 12, column = 1, padx = 25)
    else:
        tkinter.Label(window_label, text = "Value").grid(row = 0, column = 1, padx = 25)
        for i in range(12, 12 + zble.NUM_HOST_STATUS):
            tkinter.Label(window_label, text = zble.host_status_name[i]).grid(row = i + 1 - 12, column = 0, padx = 25)
            tkinter.Label(window_label, textvariable = host_status_value_list[i - 12]).grid(row = i + 1 - 12, column = 1, padx = 25)
    
def create_imu_window(window, device):
    if (device == True):
        device_str = "Device"
    else:
        device_str = "Host"

    imu_label = tkinter.LabelFrame(window, text = "{} IMU".format(device_str), width = 350, height = 150)
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
    
threading.Thread(target = zing_thread, daemon = True).start()
threading.Thread(target = vlc_thread, daemon = True).start()

#tab = tkinter.ttk.Notebook(root)
#tab.grid(row = 1, column = 0)

status_frame = tkinter.Frame(root)
status_frame.grid(row = 1, column = 0, sticky = "NWES")

host_status_frame = tkinter.Frame(status_frame)
host_imu_frame = tkinter.Frame(status_frame)
device_status_frame = tkinter.Frame(status_frame)
device_imu_frame = tkinter.Frame(status_frame)

create_status_window(host_status_frame, 0)
create_imu_window(host_imu_frame, 0)
create_status_window(device_status_frame, 1)
create_imu_window(device_imu_frame, 1)

host_status_frame.grid(row = 0, column = 0)
host_imu_frame.grid(row = 1, column = 0)
device_status_frame.grid(row = 0, column = 1)
device_imu_frame.grid(row = 1, column = 1)

#tab.add(host_frame, text = "Host")
#tab.add(device_frame, text = "Device")

root.mainloop()