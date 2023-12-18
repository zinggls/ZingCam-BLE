import zmq
import json
import zing_ble
import threading
import time
import hu205
import em2890
import subprocess
import setting

CAM_UNKNOWN = 0
CAM_HU205 = 1
CAM_EM2890 = 2

request_context = zmq.Context(10)
req = request_context.socket(zmq.REP)
req.bind("tcp://{}:{}".format(setting.IP, setting.REQREP_PORT))

subscribe_context = zmq.Context(10)
sub = request_context.socket(zmq.PUB)
sub.bind("tcp://{}:{}".format(setting.IP, setting.PUBSUB_PORT))

zble = zing_ble.Zing_BLE()
zble.connect(setting.COMPORT, setting.BAUDRATE)
commands = dict()
camera = CAM_UNKNOWN
run = 0
fmt = 0
idx = 0

def Zing():
    global commands
    global camera
    global run
    global fmt
    global idx
    loop = 0
    while True:
        if (zble.get_host_status() == False):
            print("ERROR")
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

                    if (vnd == hu205.HU205_VND and prd == hu205.HU205_PRD):
                        camera = CAM_HU205
                        fmt = zble.get_host_fmt()
                        idx = zble.get_host_idx()
                    elif (vnd == em2890.EM2890_VND and prd == em2890.EM2890_PRD):
                        camera = CAM_EM2890
                        fmt = zble.get_host_fmt()
                        idx = zble.get_host_idx()
                    else:
                        camera = CAM_UNKNOWN
                else:
                    print(oldcnt, newcnt)
                    if (run != 0):
                        run.kill()
                        run = 0
            
            sub.send(json.dumps(zble.host_status_dict).encode())
            sub.send(json.dumps(zble.device_status_dict).encode())

            commands = {
                "host_status_name": json.dumps(zble.host_status_name).encode(),
                "host_status": json.dumps(zble.host_status_dict).encode(),
                "device_status_name": json.dumps(zble.device_status_name).encode(),
                "device_status": json.dumps(zble.device_status_dict).encode(),
                "num_host_status": "{}".format(zble.NUM_HOST_STATUS).encode(),
                "num_device_status": "{}".format(zble.NUM_DEVICE_STATUS).encode(),
                "host_usb": zble.host_status_dict["USB"].encode(),
                "host_vnd": zble.host_status_dict["VND"].encode(),
                "host_prd": zble.host_status_dict["PRD"].encode(),
                "host_bnd": zble.host_status_dict["BND"].encode(),
                "host_pid": zble.host_status_dict["PID"].encode(),
                "host_did": zble.host_status_dict["DID"].encode(),
                "host_fmt": zble.host_status_dict["FMT"].encode(),
                "host_idx": zble.host_status_dict["IDX"].encode(),
                "host_trt": zble.host_status_dict["TRT"].encode(),
                "host_ack": zble.host_status_dict["ACK"].encode(),
                "host_ppc": zble.host_status_dict["PPC"].encode(),
                "host_rxid": zble.host_status_dict["RXID"].encode(),
                "host_run": zble.host_status_dict["RUN"].encode(),
                "host_cnt": zble.host_status_dict["CNT"].encode(),
                "device_pid": zble.device_status_dict["PID"].encode(),
                "device_did": zble.device_status_dict["DID"].encode(),
                "device_fmt": zble.device_status_dict["FMT"].encode(),
                "device_idx": zble.device_status_dict["IDX"].encode(),
                "device_trt": zble.device_status_dict["TRT"].encode(),
                "device_ack": zble.device_status_dict["ACK"].encode(),
                "device_ppc": zble.device_status_dict["PPC"].encode(),
                "device_rxid": zble.device_status_dict["RXID"].encode(),
                "device_run": zble.device_status_dict["RUN"].encode(),
                "device_cnt": zble.device_status_dict["CNT"].encode(),
            }
        loop = loop + 1
        #time.sleep(0.1)

def vlc_player():
    global run
    while True:
        if (camera == CAM_HU205):
            format = hu205.HU205_FMT[fmt]
            resolution = hu205.HU205_IDX[idx]
            res_x = resolution.split("x")[0]
            res_y = resolution.split("x")[1]

            """
            #vlc_command = "vlc --fullscreen dshow:// :dshow-vdev=ZingCam :dshow-adev=none  :live-caching=0 :dshow-size=\"{}\" :dshow-aspect-ratio=16\:9 :dshow-fps=28 :dshow-chroma=\"{}\"".format(res, fmt)
            command = [
                'vlc',
                'dshow://',
                ':dshow-vdev=ZingCam',
                ':dshow-adev=none',
                ':live-caching=0',
                ':dshow-size={}'.format(resolution),
                ':dshow-fps=28',
                ':dshow-chroma={}'.format(format)
            ]
            """

            if (format == "MJPG"):
                if (int(res_x) == 1920 and int(res_y) == 1080):
                    framerate = 28
                else:
                    framerate = 30

                command = [
                    "D:/gstreamer/1.0/msvc_x86_64/bin/gst-launch-1.0",
                    "ksvideosrc",
                    "device-index=0",
                    "!",
                    "image/jpeg,width={},height={},framerate={}/1".format(res_x, res_y, framerate),
                    "!",
                    "jpegdec",
                    "!",
                    "autovideosink",
                    "sync=false"
                ]
            run = subprocess.Popen(command)
            run.wait()
        elif (camera == CAM_EM2890):
            format = em2890.EM2890_FMT[fmt]
            resolution = em2890.EM2890_IDX[idx]
            res_x = resolution.split("x")[0]
            res_y = resolution.split("x")[1]
            """
            #vlc_command = "vlc dshow:// :dshow-vdev=ZingCam :dshow-adev=none  :live-caching=0 :dshow-size=\"{}\" :dshow-aspect-ratio=4\:3 :dshow-fps=30 :dshow-chroma=\"{}\"".format(res, fmt)
            command = [
                'vlc',
                'dshow://',
                ':dshow-vdev=ZingCam',
                ':dshow-adev=none',
                ':live-caching=0',
                ':dshow-size={}'.format(resolution),
                ':dshow-fps=30',
                ':dshow-chroma={}'.format(format)
            ]
            """
            # gst-launch-1.0 ksvideosrc device-index=0 ! "image/jpeg,width=640,height=480,framerate=30/1" ! jpegdec ! autovideosink sync=false
            if (format == "MJPG"):
                command = [
                    "D:/gstreamer/1.0/msvc_x86_64/bin/gst-launch-1.0",
                    "ksvideosrc",
                    "device-index=0",
                    "!",
                    "image/jpeg,width={},height={},framerate=30/1".format(res_x, res_y),
                    "!",
                    "jpegdec",
                    "!",
                    "autovideosink",
                    "sync=false"
                ]
            print(command)
            run = subprocess.Popen(command)
            run.wait()
        time.sleep(1)

threading.Thread(target = Zing, daemon = True).start()
threading.Thread(target = vlc_player, daemon = True).start()

while True:
    recv = req.recv()
    cmd = recv.decode()

    try:
        msg = commands[cmd]
        req.send(msg)
    except Exception as e:
        print(e)
        
