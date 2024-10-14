#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Define the Scope struct
typedef struct {
    uint8_t camera;
    uint8_t output;
    uint8_t mode;
    uint8_t battery_status;
    uint8_t ir_status;
    uint8_t eo_status;
} Scope;

// Function to clear the input buffer
void clearBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Function to clear the screen
void clearScreen() {
    system("clear");  // Unix/Linux command to clear screen
}

// Function to display the title and available commands
void displayTitleAndCommands() {
    printf("I2C Master Emulator\n");
    printf("Commands: 1: Camera, 2: Output, 3: Mode, 4: Battery, 5: IR, 6: EO, q to quit.\n");
}

// Function to display the current status of the Scope struct with command numbers
void displayScope(Scope *s) {
    printf("Current Scope Status:\n");
    printf("  1: Camera: %u\n", s->camera);
    printf("  2: Output: %u\n", s->output);
    printf("  3: Mode: %u\n", s->mode);
    printf("  4: Battery Status: %u\n", s->battery_status);
    printf("  5: IR Status: %u\n", s->ir_status);
    printf("  6: EO Status: %u\n", s->eo_status);
    printf("\n");
}

int main() {
    Scope scope = {0, 0, 0, 100, 0, 0};  // Initialize Scope with default values
    char input;
    uint8_t temp_value;  // Temporary value for updating fields

    // Infinite loop until 'q' is pressed
    while (1) {
        clearScreen();  // Clear the screen before showing the updated values

        // Display title and available commands
        displayTitleAndCommands();

        // Show current Scope data
        displayScope(&scope);

        printf("Enter command (1-6 to update, q to quit): ");
        input = getchar();

        // Clear the input buffer
        clearBuffer();

        if (input == '1') {
            printf("Enter new value for Camera: ");
            scanf("%hhu", &temp_value);  // Read uint8_t input
            scope.camera = temp_value;
        } else if (input == '2') {
            printf("Enter new value for Output: ");
            scanf("%hhu", &temp_value);
            scope.output = temp_value;
        } else if (input == '3') {
            printf("Enter new value for Mode: ");
            scanf("%hhu", &temp_value);
            scope.mode = temp_value;
        } else if (input == '4') {
            printf("Enter new value for Battery Status: ");
            scanf("%hhu", &temp_value);
            scope.battery_status = temp_value;
        } else if (input == '5') {
            printf("Enter new value for IR Status: ");
            scanf("%hhu", &temp_value);
            scope.ir_status = temp_value;
        } else if (input == '6') {
            printf("Enter new value for EO Status: ");
            scanf("%hhu", &temp_value);
            scope.eo_status = temp_value;
        } else if (input == 'q') {
            printf("Exiting program...\n");
            break;
        } else {
            printf("Invalid input! Please enter 1-6 or q to quit.\n");
        }

        // Clear the buffer after taking input with scanf
        clearBuffer();
    }

    return 0;
}
