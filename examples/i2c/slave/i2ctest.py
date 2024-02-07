import smbus

i2c = smbus.SMBus(1)

print(i2c.read_i2c_block_data(0x24, 124, 1))
