#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <linux/gpio.h>

int main(int argc, char *argv[])
{ 
    int fd0 = open("/dev/gpiochip0", O_RDWR); // Open the file descriptor
    struct gpiochip_info cinfo;
    ioctl(fd0, GPIO_GET_CHIPINFO_IOCTL, &cinfo); // Get the chip information
    fprintf(stdout, "GPIO chip 0: %s, \"%s\", %u lines\n", 
        cinfo.name, cinfo.label, cinfo.lines);

    struct gpiohandle_request req_pb; // Push button
    struct gpiohandle_data event;     // For reading button state
    struct gpiohandle_request req_GY; // Green, Yellow, and Red LEDs
    struct gpiohandle_data data_GY;   // For LED data

    // Configure push button as input
    req_pb.lines = 1;
    req_pb.lineoffsets[0] = 11; // GPIO pin 11 - Push Button
    req_pb.flags = GPIOHANDLE_REQUEST_INPUT;
    ioctl(fd0, GPIO_GET_LINEHANDLE_IOCTL, &req_pb); // Get the line handler for push button

    // Configure Green, Yellow, and Red LEDs as output
    req_GY.lines = 3;
    req_GY.lineoffsets[0] = 4;   // GPIO pin 4 - Green LED
    req_GY.lineoffsets[1] = 17;  // GPIO pin 17 - Yellow LED
    req_GY.lineoffsets[2] = 27;  // GPIO pin 27 - Red LED
    req_GY.flags = GPIOHANDLE_REQUEST_OUTPUT;
    ioctl(fd0, GPIO_GET_LINEHANDLE_IOCTL, &req_GY); // Get the line handler for LEDs

    // Set initial states: Green LED ON, Yellow LED OFF, Red LED OFF
    data_GY.values[0] = 1; // Green LED ON
    data_GY.values[1] = 0; // Yellow LED OFF
    data_GY.values[2] = 0; // Red LED OFF
    ioctl(req_GY.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_GY); // Output initial states

    for(int i=0; i<5; i++){
      data_GY.values[0] = !data_GY.values[0]; // Toggle Green LED
      data_GY.values[1] = !data_GY.values[1]; // Toggle Yellow LED
ioctl(req_GY.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_GY); // Output LED states
      usleep(1000000);}

    while (1) {
        // Check the PUSH button state, get values
        ioctl(req_pb.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &event); 

        // Toggle Red LED based on button state
        if (event.values[0] == 1) { // Button is pressed
          for(int i=0; i<5; i++){
            data_GY.values[2] = 1;  // Turn ON Red LED
  ioctl(req_GY.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_GY); // Output LED states
            data_GY.values[2] = 0;  // Turn OFF Red LED
ioctl(req_GY.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_GY); // Output LED states
      usleep(1000000); 
        }]

        
    }

    // Release GPIO line handles and close the file descriptors
    close(req_pb.fd); // Release push button handle
    close(req_GY.fd); // Release LEDs handle
    close(fd0);       // Close the GPIO chip device

    return EXIT_SUCCESS;
}
