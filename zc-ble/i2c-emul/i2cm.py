import smbus
import time
import os

# Define the Scope class
class Scope:
    def __init__(self, camera=0, output=0, mode=0, battery_status=100, ir_status=0, eo_status=0):
        self.camera = camera
        self.output = output
        self.mode = mode
        self.battery_status = battery_status
        self.ir_status = ir_status
        self.eo_status = eo_status

    # Function to display the scope status
    def display(self):
        print("Current Scope Status:")
        print(f"  1: Camera: {self.camera}")
        print(f"  2: Output: {self.output}")
        print(f"  3: Mode: {self.mode}")
        print(f"  4: Battery Status: {self.battery_status}")
        print(f"  5: IR Status: {self.ir_status}")
        print(f"  6: EO Status: {self.eo_status}")
        print("\n")

# I2C Configuration
I2C_BUS = 1  # I2C bus number (typically 1 on Raspberry Pi)
I2C_SLAVE_ADDR = 0x3C  # Slave address

# Initialize I2C bus
bus = smbus.SMBus(I2C_BUS)

# Function to clear the screen
def clear_screen():
    os.system("clear")

# Function to display title and commands
def display_title_and_commands():
    print("I2C Master Emulator\n")

# Function to read 6-byte block from the I2C slave
def read_from_i2c():
    try:
        data = bus.read_i2c_block_data(I2C_SLAVE_ADDR, 0, 6)
        scope = Scope(camera=data[0], output=data[1], mode=data[2],
                      battery_status=data[3], ir_status=data[4], eo_status=data[5])
        return scope
    except OSError as e:
        print(f"Error reading from I2C device: {e}")
        return None

# Function to write 6-byte block to the I2C slave
def write_to_i2c(scope):
    data = [scope.camera, scope.output, scope.mode,
            scope.battery_status, scope.ir_status, scope.eo_status]
    try:
        bus.write_i2c_block_data(I2C_SLAVE_ADDR, data[0], data[1:])
        time.sleep(0.01)  # Delay to allow the slave to process
    except OSError as e:
        print(f"Error writing to I2C device: {e}")

# Main function to run the emulator
def main():
    scope = read_from_i2c()  # Initialize by reading from I2C
    if scope is None:
        print("Initialization failed. Exiting.")
        return

    while True:
        clear_screen()
        display_title_and_commands()
        scope.display()  # Display current scope data

        command = input("Enter command (1-6 to update, r to refresh, q to quit): ")

        if command == '1':
            scope.camera = int(input("Enter new value for Camera (0-255): "))
        elif command == '2':
            scope.output = int(input("Enter new value for Output (0-255): "))
        elif command == '3':
            scope.mode = int(input("Enter new value for Mode (0-255): "))
        elif command == '4':
            scope.battery_status = int(input("Enter new value for Battery Status (0-255): "))
        elif command == '5':
            scope.ir_status = int(input("Enter new value for IR Status (0-255): "))
        elif command == '6':
            scope.eo_status = int(input("Enter new value for EO Status (0-255): "))
        elif command == 'r':  # Refresh command
            updated_scope = read_from_i2c()
            if updated_scope is not None:
                scope = updated_scope  # Update local scope with new I2C values
                print("Scope values refreshed from I2C device.")
            else:
                print("Failed to refresh scope values.")
        elif command == 'q':
            print("Exiting program...")
            break
        else:
            print("Invalid input! Please enter 1-6, r to refresh, or q to quit.")

        write_to_i2c(scope)  # Write updated scope values to I2C
        time.sleep(1)  # Pause for a bit before refreshing the screen

if __name__ == "__main__":
    main()

