import tkinter
import threading
import time

def ZingGatewayGUI(host_status, device_status, host_status_name, device_status_name, NUM_HOST_STATUS, NUM_DEVICE_STATUS):
    root = tkinter.Tk()

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

    status = [host_sof_value_list, device_sof_value_list,
              host_acc_value_list, device_acc_value_list,
              host_gyr_value_list, device_gyr_value_list,
              host_mag_value_list, device_mag_value_list,
              host_status_value_list, device_status_value_list]
    threading.Thread(target = set_value, args = (status, host_status, device_status, host_status_name, device_status_name, NUM_HOST_STATUS, NUM_DEVICE_STATUS), daemon = True).start()

    imu = [host_sof_value_list, device_sof_value_list,
              host_acc_value_list, device_acc_value_list,
              host_gyr_value_list, device_gyr_value_list,
              host_mag_value_list, device_mag_value_list]

    values = [host_status_value_list, device_status_value_list]

    #create_imu_window(host_imu_frame, 0, imu)
    #create_imu_window(device_imu_frame, 1, imu)

    status_frame = tkinter.Frame(root)
    status_frame.grid(row = 1, column = 0, sticky = "NWES")

    host_status_frame = tkinter.Frame(status_frame)
    host_imu_frame = tkinter.Frame(status_frame)
    device_status_frame = tkinter.Frame(status_frame)
    device_imu_frame = tkinter.Frame(status_frame)

    create_status_window(host_status_frame, 0, values, host_status_name, device_status_name, NUM_HOST_STATUS, NUM_DEVICE_STATUS)
    create_imu_window(host_imu_frame, 0, imu)
    create_status_window(device_status_frame, 1, values, host_status_name, device_status_name, NUM_HOST_STATUS, NUM_DEVICE_STATUS)
    create_imu_window(device_imu_frame, 1, imu)

    host_status_frame.grid(row = 0, column = 0)
    host_imu_frame.grid(row = 1, column = 0)
    device_status_frame.grid(row = 0, column = 1)
    device_imu_frame.grid(row = 1, column = 1)

    root.mainloop()

def set_value(status, host_status, device_status, host_status_name, device_status_name, NUM_HOST_STATUS, NUM_DEVICE_STATUS):
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

    while True:
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
            for i in range(NUM_DEVICE_STATUS):
                device_status_value_list[i].set(device_status[device_status_name[i + 12]])
        except Exception as e:
            pass
            
        time.sleep(1)


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
    
def create_status_window(window, device, status_list, host_status_name, device_status_name, NUM_HOST_STATUS, NUM_DEVICE_STATUS):
    if (device == True):
        device_str = "Device"
    else:
        device_str = "Host"

    host_status_value_list = status_list[0]
    device_status_value_list = status_list[1]

    window_label = tkinter.LabelFrame(window, text = "{} Status".format(device_str), width = 350, height = 350)
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
    