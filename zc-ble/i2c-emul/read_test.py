import smbus

# Define the I2C bus and slave address
I2C_BUS = 1  # I2C bus number (1 for Raspberry Pi)
I2C_SLAVE_ADDR = 0x3C  # Slave address

# Initialize I2C bus
bus = smbus.SMBus(I2C_BUS)

# Function to read 6 bytes from the I2C slave
def read_from_i2c():
    try:
        data = bus.read_i2c_block_data(I2C_SLAVE_ADDR, 0, 6)  # Read 6 bytes from the device
        print(f"Read data: {data}")  # Print the read data
    except OSError as e:
        print(f"Error reading from I2C device: {e}")

if __name__ == "__main__":
    read_from_i2c()

