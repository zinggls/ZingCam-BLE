import smbus
import time

# Define the I2C bus and slave address
I2C_BUS = 1  # I2C bus number (1 for Raspberry Pi)
I2C_SLAVE_ADDR = 0x3C  # Slave address

# Initialize I2C bus
bus = smbus.SMBus(I2C_BUS)

# Function to write bytes to the I2C slave
def write_to_i2c():
    # Define the data to be sent
    data = [0x1, 0x2, 0x3, 0x4, 0x5, 0x6]  # First element will be used as the register address

    try:
        # Use the first element as the register address
        register_address = data[0]
        # Send the rest of the data
        data_to_send = data[1:]

        print(f"Attempting to write to I2C device at address {hex(I2C_SLAVE_ADDR)}...")
        
        # Write the data block to the I2C device
        bus.write_i2c_block_data(I2C_SLAVE_ADDR, register_address, data_to_send)

        print(f"Written to I2C (Register: {register_address}, Data: {data_to_send})")  # Display the data being written
    except OSError as e:
        print(f"Error writing to I2C device: {e}")

if __name__ == "__main__":
    write_to_i2c()

