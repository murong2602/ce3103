#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>
#include <string.h>

#define GPIO_GREEN_LED 4
#define GPIO_YELLOW_LED 17
#define GPIO_RED_LED 27
#define GPIO_PUSH_BUTTON 11

// Function to blink the Green and Yellow LEDs
void blink_green_yellow_leds(int fd) {
    struct gpiohandle_request req_GY;
    struct gpiohandle_data data_GY;
    
    req_GY.lines = 2;  // We are controlling 2 lines (Green and Yellow LEDs)
    req_GY.lineoffsets[0] = GPIO_GREEN_LED;  // Green LED connected to GPIO 4
    req_GY.lineoffsets[1] = GPIO_YELLOW_LED; // Yellow LED connected to GPIO 17
    req_GY.flags = GPIOHANDLE_REQUEST_OUTPUT;  // We want to set them as outputs

    // Initial states: Green LED ON, Yellow LED OFF
    data_GY.values[0] = 1;  // Green ON
    data_GY.values[1] = 0;  // Yellow OFF

    // Get handle to control the lines
    if (ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req_GY) == -1) {
        perror("Failed to request line handle for Green/Yellow LEDs");
        exit(EXIT_FAILURE);
    }

    // Blink the Green and Yellow LEDs 5 times
    for (int i = 0; i < 5; ++i) {
        ioctl(req_GY.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_GY);  // Set LED states
        usleep(1000000);  // Sleep for 1 second
        
        // Toggle the LED states
        data_GY.values[0] = !data_GY.values[0];  // Toggle Green LED
        data_GY.values[1] = !data_GY.values[1];  // Toggle Yellow LED
    }

    close(req_GY.fd);  // Release the handle
}

// Function to blink the Red LED when the button is pressed
void blink_red_led_on_button_press(int fd) {
    struct gpiohandle_request req_button, req_red;
    struct gpiohandle_data data_button, data_red;

    // Set up GPIO 11 as input for Push Button
    req_button.lineoffsets[0] = GPIO_PUSH_BUTTON;
    req_button.flags = GPIOHANDLE_REQUEST_INPUT;
    req_button.lines = 1;

    // Set up GPIO 27 as output for Red LED
    req_red.lineoffsets[0] = GPIO_RED_LED;
    req_red.flags = GPIOHANDLE_REQUEST_OUTPUT;
    req_red.lines = 1;
    data_red.values[0] = 0;  // Initial state of Red LED is OFF

    // Request handle for both the button and the red LED
    if (ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req_button) == -1) {
        perror("Failed to request line handle for button");
        exit(EXIT_FAILURE);
    }
    if (ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req_red) == -1) {
        perror("Failed to request line handle for Red LED");
        exit(EXIT_FAILURE);
    }

    printf("Monitoring button on GPIO %d...\n", GPIO_PUSH_BUTTON);

    // Monitor the button state continuously
    while (1) {
        // Read the button state
        if (ioctl(req_button.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data_button) == -1) {
            perror("Failed to read button value");
            exit(EXIT_FAILURE);
        }

        // If the button is pressed (assuming active-low logic, 0 means pressed)
        if (data_button.values[0] == 0) {
            printf("Button pressed! Blinking Red LED...\n");
            
            // Blink Red LED 3 times
            for (int i = 0; i < 3; ++i) {
                data_red.values[0] = 1;  // Red LED ON
                ioctl(req_red.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_red);
                usleep(500000);  // 500 ms delay
                
                data_red.values[0] = 0;  // Red LED OFF
                ioctl(req_red.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_red);
                usleep(500000);  // 500 ms delay
            }
        } else {
            printf("Button released.\n");
        }

        usleep(500000);  // Sleep for 500 ms before polling again
    }

    close(req_button.fd);  // Release button handle
    close(req_red.fd);     // Release red LED handle
}

int main() {
    // Open the GPIO chip 0 device
    int fd0 = open("/dev/gpiochip0", O_RDWR);
    if (fd0 == -1) {
        perror("Failed to open /dev/gpiochip0");
        exit(EXIT_FAILURE);
    }

    // Blink the Green and Yellow LEDs
    blink_green_yellow_leds(fd0);

    // Monitor button and blink Red LED upon button press
    blink_red_led_on_button_press(fd0);

    close(fd0);  // Close the GPIO chip
    return 0;
}
