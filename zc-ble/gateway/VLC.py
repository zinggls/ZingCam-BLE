import zmq
import hu205
import em2890
import subprocess
import threading
import time

context = zmq.Context()
socket = context.socket(zmq.REQ)
socket.connect("tcp://127.0.0.1:1234")
run = 0
status = 0

def req(cmd):
    socket.send(cmd.encode())
    return socket.recv().decode()

def vlc_player():
    global run
    while True:
        if (status == 1):
            format = hu205.HU205_FMT[fmt]
            resolution = hu205.HU205_IDX[idx]
            #vlc_command = "vlc --fullscreen dshow:// :dshow-vdev=ZingCam :dshow-adev=none  :live-caching=0 :dshow-size=\"{}\" :dshow-aspect-ratio=16\:9 :dshow-fps=28 :dshow-chroma=\"{}\"".format(res, fmt)
            vlc_command = [
                'vlc',
                'dshow://',
                ':dshow-vdev=ZingCam',
                ':dshow-adev=none',
                ':live-caching=0',
                ':dshow-size={}'.format(resolution),
                ':dshow-fps=28',
                ':dshow-chroma={}'.format(format)
            ]
            run = subprocess.Popen(vlc_command)
            run.wait()
        elif (status == 2):
            format = em2890.EM2890_FMT[fmt]
            resolution = em2890.EM2890_IDX[idx]
            #vlc_command = "vlc dshow:// :dshow-vdev=ZingCam :dshow-adev=none  :live-caching=0 :dshow-size=\"{}\" :dshow-aspect-ratio=4\:3 :dshow-fps=30 :dshow-chroma=\"{}\"".format(res, fmt)
            vlc_command = [
                'vlc',
                'dshow://',
                ':dshow-vdev=ZingCam',
                ':dshow-adev=none',
                ':live-caching=0',
                ':dshow-size={}'.format(resolution),
                ':dshow-fps=30',
                ':dshow-chroma={}'.format(format)
            ]
            run = subprocess.Popen(vlc_command)
            run.wait()
        time.sleep(1)


threading.Thread(target = vlc_player, daemon = True).start()

loop = 0
while True:
    if (loop == 0):
        oldcnt = req("host_cnt")
    else:
        newcnt = req("host_cnt")

        if (oldcnt != newcnt):
            oldcnt = newcnt
            fmt = req("host_fmt")
            idx = req("host_idx")
            vnd = int(req("host_vnd"), 16)
            prd = int(req("host_prd"), 16)

            if (vnd == hu205.HU205_VND and prd == hu205.HU205_PRD):
                status = 1
            elif (vnd == em2890.EM2890_VND and prd == em2890.EM2890_PRD):
                status = 2 
        else:
            status = 0
            if (run != 0):
                run.kill()
                run = 0
    loop = loop + 1
    time.sleep(1)