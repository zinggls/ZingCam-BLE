import smbus

bus = smbus.SMBus(1)

while True:
    try:
        gpio, num = input().split(" ")
    except:
        print("Input 2 arguments")
        continue
    
    if (gpio == 'x' and num == 'x'):
        exit()

    try:
        gpio = int(gpio)
        num = int(num)
    except:
        print("Input only number")
        continue
    
    bus.write_i2c_block_data(0x24, gpio, [num])
    print(bus.read_i2c_block_data(0x24, gpio, num))
