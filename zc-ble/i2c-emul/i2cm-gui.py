import smbus
import time
import threading
import tkinter as tk
from tkinter import messagebox  # Import messagebox for pop-up messages

# Define the Scope class
class Scope:
    def __init__(self, camera=0, output=0, mode=0, battery_status=100, ir_status=0, eo_status=0):
        self.camera = camera
        self.output = output
        self.mode = mode
        self.battery_status = battery_status
        self.ir_status = ir_status
        self.eo_status = eo_status

    def display(self):
        return (self.camera, self.output, self.mode, 
                self.battery_status, self.ir_status, self.eo_status)

# I2C Configuration
I2C_BUS = 1  # I2C bus number (typically 1 on Raspberry Pi)
I2C_SLAVE_ADDR = 0x3C  # Slave address

# Initialize I2C bus
bus = smbus.SMBus(I2C_BUS)

# Function to read from the I2C slave
def read_from_i2c():
    try:
        data = bus.read_i2c_block_data(I2C_SLAVE_ADDR, 0, 6)
        return Scope(camera=data[0], output=data[1], mode=data[2],
                     battery_status=data[3], ir_status=data[4], eo_status=data[5])
    except OSError as e:
        return None  # Return None if there's an error

# Function to write to the I2C slave
def write_to_i2c(scope):
    data = [scope.camera, scope.output, scope.mode,
            scope.battery_status, scope.ir_status, scope.eo_status]
    try:
        bus.write_i2c_block_data(I2C_SLAVE_ADDR, data[0], data[1:])
        time.sleep(0.01)  # Delay to allow the slave to process
    except OSError as e:
        pass  # Ignore errors during writing for now

# GUI Application Class
class I2CMasterEmulatorApp:
    def __init__(self, master):
        self.master = master
        self.master.title("I2C Master Emulator")
        self.scope = read_from_i2c()  # Initialize by reading from I2C

        # Create widgets before logging to avoid AttributeError
        self.create_widgets()

        if self.scope is None:
            self.log_message("Initialization failed. Exiting.")
            self.master.quit()
            return

        self.update_display()

        # Start the continuous refresh in a separate thread
        self.stop_refresh = False
        self.refresh_thread = threading.Thread(target=self.continuous_refresh)
        self.refresh_thread.start()

        # Bind the close button (window close event) to the quit method
        self.master.protocol("WM_DELETE_WINDOW", self.quit)

    def create_widgets(self):
        self.labels = [
            "Camera", "Output", "Mode", 
            "Battery Status", "IR Status", "EO Status"
        ]
        self.read_only_boxes = []
        self.write_only_boxes = []

        for i, label in enumerate(self.labels):
            tk.Label(self.master, text=label).grid(row=i, column=0, padx=10, pady=5)
            
            # Read-only display box
            read_only_box = tk.Entry(self.master, state='readonly', width=15)
            read_only_box.grid(row=i, column=1, padx=10, pady=5)
            self.read_only_boxes.append(read_only_box)

            # Write-only input box
            write_only_box = tk.Entry(self.master, width=10)
            write_only_box.grid(row=i, column=2, padx=10, pady=5)
            self.write_only_boxes.append(write_only_box)

        self.update_button = tk.Button(self.master, text="Update", command=self.update)
        self.update_button.grid(row=len(self.labels), column=1, padx=10, pady=10)

        # Create log box
        self.log_box = tk.Text(self.master, width=50, height=10, state='disabled')
        self.log_box.grid(row=len(self.labels)+1, column=0, columnspan=3, padx=10, pady=10)

    def log_message(self, message):
        self.log_box.config(state='normal')  # Enable editing
        self.log_box.insert(tk.END, message + "\n")  # Add message to log
        self.log_box.config(state='disabled')  # Disable editing
        self.log_box.see(tk.END)  # Auto-scroll to the bottom

    def update_display(self):
        for read_only_box, value in zip(self.read_only_boxes, self.scope.display()):
            hex_value = hex(value)[2:].upper()  # Convert to hex and remove '0x' prefix
            display_value = f"{value} (0x{hex_value})"  # Format as "decimal (0xhex)"
            read_only_box.config(state='normal')  # Enable box to update
            read_only_box.delete(0, tk.END)
            read_only_box.insert(0, display_value)
            read_only_box.config(state='readonly')  # Set back to readonly

    def continuous_refresh(self):
        while not self.stop_refresh:
            updated_scope = read_from_i2c()
            if updated_scope is not None:
                self.scope = updated_scope
                self.master.after(0, self.update_display)  # Update GUI safely from the thread
            time.sleep(0.1)  # Refresh every 100ms

    def update(self):
        try:
            new_values = []
            self.log_message("Attempting to update values:")
            for i, entry in enumerate(self.write_only_boxes):
                old_value = int(self.read_only_boxes[i].get().split()[0])  # Get the old value from the formatted string
                if entry.get():  # If the entry is not empty
                    new_value = int(entry.get())
                    self.log_message(f" - Changing {self.labels[i]} from {old_value} to {new_value}")
                    new_values.append(new_value)
                else:
                    # Use the current value from the read-only box if the entry is empty
                    new_values.append(old_value)
                    self.log_message(f" - Keeping {self.labels[i]} as {old_value}")

            # Check if all values are within the allowed range
            if all(0 <= value <= 255 for value in new_values):
                self.scope.camera, self.scope.output, self.scope.mode, \
                self.scope.battery_status, self.scope.ir_status, self.scope.eo_status = new_values
                
                write_to_i2c(self.scope)
                
                # Clear the write-only boxes without a message box
                for entry in self.write_only_boxes:
                    entry.delete(0, tk.END)
                
                self.log_message("Scope values updated successfully.")
            else:
                self.show_error_message("Input Error: Please enter valid integers between 0 and 255.")
        except ValueError:
            self.show_error_message("Input Error: Please enter valid integers between 0 and 255.")

    def show_error_message(self, message):
        messagebox.showerror("Error", message)  # Show the error message
        self.quit()  # Close the application after clicking OK

    def quit(self):
        self.stop_refresh = True  # Signal the thread to stop
        self.master.quit()

if __name__ == "__main__":
    root = tk.Tk()
    app = I2CMasterEmulatorApp(root)
    root.mainloop()

