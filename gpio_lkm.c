#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h> // For GPIO functions
#include <linux/interrupt.h> // For interrupt handling

// Define GPIO pin numbers for the LED and push button
static unsigned int ledGreen = 4;     // GPIO4 connected to the Green LED
static unsigned int pushButton = 11;  // GPIO11 connected to the push Button
static unsigned int irqNumber;        // To hold the IRQ number for the button
static bool ledOn = 0;                // Used to toggle the LED state

// Interrupt handler function
static irq_handler_t rpi_gpio_isr(unsigned int irq, void *dev_id, struct pt_regs *regs) {
    // Toggle the LED state on button press
    ledOn = !ledOn;
    gpio_set_value(ledGreen, ledOn); // Set the LED accordingly
    printk(KERN_ALERT "GPIO Interrupt! Button pressed, LED state toggled\n");
    return (irq_handler_t) IRQ_HANDLED; // Announce that IRQ has been handled
}

// LKM Initialization function
static int __init rpi_gpio_init(void) {
    int result = 0;
    
    printk(KERN_ALERT "Initializing the GPIO LKM\n");

    // Set initial LED state to ON
    ledOn = true;
    gpio_request(ledGreen, "sysfs"); // Request control of the LED GPIO
    gpio_direction_output(ledGreen, ledOn); // Set LED as output and turn it on
    
    // Request control of the button GPIO and configure it as input
    gpio_request(pushButton, "sysfs");
    gpio_direction_input(pushButton); // Set button as input
    gpio_set_debounce(pushButton, 1000); // Set debounce to 1000ms

    // Map the button GPIO to an IRQ number
    irqNumber = gpio_to_irq(pushButton);
    printk(KERN_ALERT "Button mapped to IRQ: %d\n", irqNumber);

    // Request an interrupt line for the button
    result = request_irq(irqNumber,               // IRQ number requested
                        (irq_handler_t) rpi_gpio_isr, // ISR handler function
                        IRQF_TRIGGER_RISING,         // Trigger on rising edge
                        "rpi_gpio_handler",          // Name for /proc/interrupts
                        NULL);                       // No dev_id for shared lines
    
    if (result) {
        printk(KERN_ALERT "Failed to request interrupt for GPIO %d, error %d\n", pushButton, result);
        return result;
    }

    return 0; // Success
}

// LKM Exit function
static void __exit rpi_gpio_exit(void) {
    // Turn off the LED
    gpio_set_value(ledGreen, 0);

    // Free the GPIOs
    gpio_free(ledGreen);
    gpio_free(pushButton);

    // Free the IRQ number
    free_irq(irqNumber, NULL);

    printk(KERN_ALERT "Goodbye from the GPIO LKM!\n");
}

// Register the initialization and exit functions
module_init(rpi_gpio_init);
module_exit(rpi_gpio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CE3103");
MODULE_DESCRIPTION("GPIO LKM Example");
MODULE_VERSION("V1");
