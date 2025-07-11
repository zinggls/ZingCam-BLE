import serial
import time

class Zing_BLE:
    NUM_HOST_STATUS = 15
    NUM_DEVICE_STATUS = 16

    host_status_name = ["sofx", "accx", "gyrx", "magx",
                        "sofy", "accy", "gyry", "magy",
                        "sofz", "accz", "gyrz", "magz",
                        "USB", "VND", "PRD", "BND",
                        "PID", "DID", "FMT", "IDX",
                        "TRT", "ACK", "PPC", "TXID",
                        "RXID", "RUN", "CNT"]
    host_status_dict = { name : 0 for name in host_status_name }
    
    device_status_name = ["sofx", "accx", "gyrx", "magx",
                            "sofy", "accy", "gyry", "magy",
                            "sofz", "accz", "gyrz", "magz",
                            "USB", "PID", "DID", "FMT",
                            "IDX", "TRT", "ACK", "PPC",
                            "RUN", "ITF", "TXID", "RXID",
                            "DestID_ERR_CNT", "PHY_RX_FRAME_CNT", "MFIR", "CNT"]
    device_status_dict = { name : 0 for name in device_status_name }

    def __init__(self):
        pass

    def connect(self, port, baudrate):
        self.serial = serial.Serial(port, baudrate)

    def get_host_status(self):
        self.serial.write("*".encode())
        try:
            status = self.serial.readline().decode().replace("\r\n", "").split(",")
        except:
            return False

        if (status[0] != 'H'):
            self.serial.readline()
            return False

        idx = 0
        for name in self.host_status_name:
            if (idx < self.NUM_HOST_STATUS + 12):
                try:
                    self.host_status_dict[name] = status[idx + 1]
                    idx = idx + 1
                except:
                    print("error host idx={} name={} status={}".format(idx, name, status))
                    return False
            else:
                break

    def get_device_status(self):
        self.serial.write("&".encode())
        try:
            status = self.serial.readline().decode().replace("\r\n", "").split(",")
        except:
            return False

        if (status[0] != 'D'):
            self.serial.readline()
            return False
        
        idx = 0
        for name in self.device_status_name:
            if (idx < self.NUM_DEVICE_STATUS + 12):
                try:
                    self.device_status_dict[name] = status[idx + 1]
                    idx = idx + 1
                except:
                    print("error device idx={} name={} status={}".format(idx, name, status))
                    return False
            else:
                break

    
    def get_host_vnd(self):
        return self.host_status_dict["VND"]
    
    def get_host_prd(self):
        return self.host_status_dict["PRD"]
    
    def get_host_bnd(self):
        return self.host_status_dict["BND"]
    
    def get_host_pid(self):
        return self.host_status_dict["PID"]
    
    def get_host_did(self):
        return self.host_status_dict["DID"]
    
    def get_host_fmt(self):
        return self.host_status_dict["FMT"]
    
    def get_host_idx(self):
        return self.host_status_dict["IDX"]
    
    def get_host_trt(self):
        return self.host_status_dict["TRT"]
    
    def get_host_ack(self):
        return self.host_status_dict["ACK"]
    
    def get_host_ppc(self):
        return self.host_status_dict["PPC"]
    
    def get_host_rxid(self):
        return self.host_status_dict["RXID"]
    
    def get_host_run(self):
        return self.host_status_dict["RUN"]
    
    def get_host_cnt(self):
        return self.host_status_dict["CNT"]
    
    def get_device_cnt(self):
        return self.device_status_dict["CNT"]
    
    