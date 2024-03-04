import win32com.client
import array

i2c = win32com.client.Dispatch("PSoCProgrammerCOM.PSoCProgrammerCOM_Object")
result = i2c.GetPorts()
ports = result[1]

idx = 0
for port in ports:
    print("{}. {}".format(idx, port))
    idx = idx + 1

try:
    select = int(input("Select index="))
    print("You select {}".format(ports[select]))
    port = ports[select]
except ValueError:
    print("Input only number")
    exit()
except IndexError:
    max = len(ports) - 1
    print("range = 0 ~ {}".format(max))
    exit()

i2c.OpenPort(port)
result = i2c.I2C_GetDeviceList()
devices = result[1]

print(devices)
print("{}".format(hex(devices[0])))

dataIN = []
data_wr = 0 # read
data_addr = 0x28
data = 0 
data_len = 0x1A # 26

dataIN.append(data_wr)
dataIN.append(data_addr)
dataIN.append(data)
dataIN.append(data_len)
i2c_data = array.array('B', dataIN)
result = i2c.I2C_SendData(devices[0], bytearray(i2c_data))
print(result)
result = i2c.I2C_ReadData(devices[0], data_len)
print(result[1])
recv_data = result[1]

data_str = ""
for data in recv_data:
    data_str = data_str + "{}, ".format(hex(data), 'X')

print(data_str)