import tkinter as tk
from tkinter import ttk

# Define the scope_camera dictionary
scope_camera = {
    0: "Default",
    1: "EO",
    2: "IR",
}

# Create the main application window
root = tk.Tk()
root.title("Scope Camera Selector")

# Function to handle selection from the combo box
def on_select(event):
    selected_value = combo_box.get()
    print(f"Selected Camera: {selected_value}")

# Create a label
label = tk.Label(root, text="Select Camera Mode:")
label.pack(pady=10)

# Create a combo box
combo_box = ttk.Combobox(root, values=list(scope_camera.values()))
combo_box.bind("<<ComboboxSelected>>", on_select)  # Bind the selection event
combo_box.pack(pady=10)

# Set default value (optional)
combo_box.current(0)  # Sets the default to "Default"

# Start the GUI event loop
root.mainloop()

