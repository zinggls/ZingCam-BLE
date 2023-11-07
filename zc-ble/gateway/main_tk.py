import zing_ble
import hu205
import em2890
import time
import tkinter
import vlc
import threading

zble = zing_ble.Zing_BLE("COM6", 115200)
loop = 0
root = tkinter.Tk()
#root.state("zoomed")
status_frame = tkinter.Frame(root)
video_frame = tkinter.Frame(root)

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

def create_window(device):
    window = tkinter.Toplevel(root)

    imu_label = tkinter.LabelFrame(window, text = "IMU")
    imu_label.grid(row = 1, column = 0, padx = 10, pady = 10)

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
  
    if (device == True):
        window_label = tkinter.LabelFrame(window, text = "Device")

        tkinter.Label(window_label, text = "Value").grid(row = 0, column = 1, padx = 25)
        for i in range(12, 12 + zble.NUM_DEVICE_STATUS):
            tkinter.Label(window_label, text = zble.device_status_name[i]).grid(row = i + 1 - 12, column = 0, padx = 25)
            tkinter.Label(window_label, textvariable = device_status_value_list[i - 12]).grid(row = i + 1 - 12, column = 1, padx = 25)
    else:
        window_label = tkinter.LabelFrame(window, text = "Host")

        tkinter.Label(window_label, text = "Value").grid(row = 0, column = 1, padx = 25)
        for i in range(12, 12 + zble.NUM_HOST_STATUS):
            tkinter.Label(window_label, text = zble.host_status_name[i]).grid(row = i + 1 - 12, column = 0, padx = 25)
            tkinter.Label(window_label, textvariable = host_status_value_list[i - 12]).grid(row = i + 1 - 12, column = 1, padx = 25)

    window_label.grid(row = 0, column = 0, padx = 10, pady = 10, sticky = "WE")
    


menu = tkinter.Menu(root)
status_menu = tkinter.Menu(menu, tearoff = 0)
status_menu.add_command(label = "Host", command = lambda: create_window(False))
status_menu.add_command(label = "Device", command = lambda: create_window(True))
menu.add_cascade(label = "Status", menu = status_menu)

vlc_instance = vlc.Instance("-q")
vlc_media = vlc_instance.media_player_new()

def Thread(vlc_media, loop, video_frame):
    hu205_media = None
    em2890_media = None
    playing = False
    while True:
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
                        fmt = hu205.HU205_FMT[zble.get_host_fmt()]
                        res = hu205.HU205_IDX[zble.get_host_idx()]
                        if (playing == False):
                            x, y = res.split("x")
                            video_frame.config(width = x, height = y)
                            if (hu205_media == None):
                                hu205_media = vlc_instance.media_new("dshow://", 
                                                                    "dshow-vdev=ZingCam", 
                                                                    "dshow-adev=none",
                                                                    "live-caching=0",
                                                                    "dshow-size={}".format(res),
                                                                    "dshow-fps=28",
                                                                    "dshow-chroma={}".format(fmt))
                            vlc_media.set_hwnd(video_frame.winfo_id())
                            vlc_media.set_media(hu205_media)
                            vlc_media.play()
                            video_frame.pack()
                            playing = True
                    elif (vnd == em2890.EM2890_VND and prd == em2890.EM2890_PRD):
                        fmt = em2890.EM2890_FMT[zble.get_host_fmt()]
                        res = em2890.EM2890_IDX[zble.get_host_idx()]
                        if (playing == False):
                            x, y = res.split("x")
                            video_frame.config(width = x, height = y)
                            if (em2890_media == None):
                                em2890_media = vlc_instance.media_new("dshow://", 
                                                                    "dshow-vdev=ZingCam", 
                                                                    "dshow-adev=none",
                                                                    "live-caching=0",
                                                                    "dshow-size={}".format(res),
                                                                    "dshow-fps=30",
                                                                    "dshow-chroma={}".format(fmt))
                            vlc_media.set_hwnd(video_frame.winfo_id())
                            vlc_media.set_media(em2890_media)
                            vlc_media.play()
                            video_frame.pack()
                            root.geometry(res)
                            playing = True
                    else:
                        print("Unknown CAM")
                        video_frame.pack_forget()
                        playing = False
                else:
                    print("Not Connected may be.")
                    video_frame.pack_forget()
                    if (hu205_media != None):
                        hu205_media.release()
                        hu205_media = None
                    if (em2890_media != None):
                        em2890_media.release()
                        em2890_media = None
                    playing = False

            loop = loop + 1
        time.sleep(1)

status_frame.pack()
threading.Thread(target = Thread, daemon = True, args = [vlc_media, loop, video_frame]).start()
root.config(menu = menu)
root.mainloop()