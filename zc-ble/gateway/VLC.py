import subprocess
import hu205
import em2890
import time

def VLCPlayer(camera, host_status, device_status, player):
    while True:
        try:            
            FORMAT = host_status["FMT"]
            INDEX = host_status["IDX"]
        except Exception as e:
            continue

        if (camera.value == 1):
            fmt = hu205.HU205_FMT[FORMAT]
            res = hu205.HU205_IDX[INDEX]
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
            player.value = run.pid
            run.wait()
            camera.value = 0
        elif (camera.value == 2):
            fmt = em2890.EM2890_FMT[FORMAT]
            res = em2890.EM2890_IDX[INDEX]
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
            player.value = run.pid
            run.wait()
            camera.value = 0
        time.sleep(1)
