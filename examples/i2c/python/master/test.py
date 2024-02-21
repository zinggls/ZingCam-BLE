import time
import pigpio

I2C_ADDR = 0x24

def i2c(id, tick):
    global pi

    s, b, d = pi.bsc_i2c(I2C_ADDR)
    d = int.from_bytes(bytes(d))
    if b:
        if d == 0x01:
            pi.bsc_i2c(I2C_ADDR, data=[1, 0])
        elif d == 0x02:
            pi.bsc_i2c(I2C_ADDR, data=[2, 0])
        elif d == 0x03:
            pi.bsc_i2c(I2C_ADDR, data=[3, 0])

pi = pigpio.pi()

if not pi.connected:
    exit()

e = pi.event_callback(pigpio.EVENT_BSC, i2c)
pi.bsc_i2c(I2C_ADDR)

time.sleep(600)
e.cancel()

pi.bsc_i2c(0)
pi.stop()
