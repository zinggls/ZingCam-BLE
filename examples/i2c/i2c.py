import os
import fcntl
import time

fd = os.open("/dev/i2c-1", os.O_RDWR)
fcntl.ioctl(fd, 0x703, 0x3C)

os.write(fd, bytes([1, 0, 0,]))
time.sleep(0.1)
print(os.read(fd, 6))
