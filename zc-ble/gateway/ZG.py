import zing_ble
import time
import em2890
import hu205
import os
import signal

def ZingGateway(comport, host_status, device_status, host_status_name, device_status_name, camera, player):
    zble = zing_ble.Zing_BLE()
    zble.connect(comport, 115200)
    loop = 0

    for i in range(12 + zble.NUM_HOST_STATUS):
        host_status_name[i] = zble.host_status_name[i]

    for i in range(12 + zble.NUM_DEVICE_STATUS):
        device_status_name[i] = zble.device_status_name[i]

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

                    for name in zble.host_status_name:
                        host_status[name] = zble.host_status_dict[name]

                    for name in zble.device_status_dict:
                        device_status[name] = zble.device_status_dict[name]

                    if (vnd == hu205.HU205_VND and prd == hu205.HU205_PRD):
                        camera.value = 1
                    elif (vnd == em2890.EM2890_VND and prd == em2890.EM2890_PRD):
                        camera.value = 2 
                    else:
                        camera.value = 0
                else:
                    if (player.value != 0):
                        os.kill(player.value, signal.SIGINT)
                        player.value = 0

            loop = loop + 1

        time.sleep(1)