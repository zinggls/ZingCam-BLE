import zmq
import json
import zing_ble
import threading
import time

context = zmq.Context(10)
req = context.socket(zmq.REP)
req.bind("tcp://127.0.0.1:1234")

zble = zing_ble.Zing_BLE()
zble.connect("COM6", 115200)
commands = dict()

def Zing():
    global commands
    while True:
        if (zble.get_host_status() == False):
            print("Error")
        elif (zble.get_device_status() == False):
            print("Error")
            
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
        time.sleep(0.1)

threading.Thread(target = Zing, daemon = True).start()

while True:
    recv = req.recv()
    cmd = recv.decode()

    try:
        msg = commands[cmd]
        req.send(msg)
    except Exception as e:
        print(e)
        
