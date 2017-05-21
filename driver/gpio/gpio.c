/*  
 *  led1.c - switch -> led kernel module
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>         //required for GPIO functions
#include <linux/interrupt.h>    // for IRQ code
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("John Doe");
MODULE_DESCRIPTION("LED Switch driver");
MODULE_VERSION("0.1");
 

static unsigned int gpioSW0 = 224; // first switch from the right. 
static unsigned int gpioSW1 = 225;
static unsigned int gpioSW2 = 226;
static unsigned int gpioSW3 = 227;
static unsigned int gpioSW4 = 228;
static unsigned int gpioSW5 = 229;
static unsigned int gpioSW6 = 230;
static unsigned int gpioSW7 = 231;

 
// function prototype for custom irq handler
//static irq_handler_t gpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);
 
static int __init gpio_init(void) {
        int result = 0;
        
 
        /*Switches*/
	gpio_request(gpioSW0, "sysfs");          // set up gpio switch
        gpio_direction_input(gpioSW0);            // set switch as input
        gpio_set_debounce(gpioSW0, 200);         // debounce with delay of 200ms
        gpio_export(gpioSW0, false);             // make it appear in /sys/class/gpio

        gpio_request(gpioSW1, "sysfs");          // set up gpio switch
        gpio_direction_input(gpioSW1);            // set switch as input
        gpio_set_debounce(gpioSW1, 200);         // debounce with delay of 200ms
        gpio_export(gpioSW1, false);             // make it appear in /sys/class/gpio
	

	gpio_request(gpioSW2, "sysfs");          // set up gpio switch
        gpio_direction_input(gpioSW2);            // set switch as input
        gpio_set_debounce(gpioSW2, 200);         // debounce with delay of 200ms
        gpio_export(gpioSW2, false);             // make it appear in /sys/class/gpio
	
	gpio_request(gpioSW3, "sysfs");          // set up gpio switch
        gpio_direction_input(gpioSW3);            // set switch as input
        gpio_set_debounce(gpioSW3, 200);         // debounce with delay of 200ms
        gpio_export(gpioSW3, false);             // make it appear in /sys/class/gpio

	gpio_request(gpioSW4, "sysfs");          // set up gpio switch
        gpio_direction_input(gpioSW4);            // set switch as input
        gpio_set_debounce(gpioSW4, 200);         // debounce with delay of 200ms
        gpio_export(gpioSW4, false);             // make it appear in /sys/class/gpio

	gpio_request(gpioSW5, "sysfs");          // set up gpio switch
        gpio_direction_input(gpioSW5);            // set switch as input
        gpio_set_debounce(gpioSW5, 200);         // debounce with delay of 200ms
        gpio_export(gpioSW5, false);             // make it appear in /sys/class/gpio

	gpio_request(gpioSW6, "sysfs");          // set up gpio switch
        gpio_direction_input(gpioSW6);            // set switch as input
        gpio_set_debounce(gpioSW6, 200);         // debounce with delay of 200ms
        gpio_export(gpioSW6, false);             // make it appear in /sys/class/gpio

	gpio_request(gpioSW7, "sysfs");          // set up gpio switch
        gpio_direction_input(gpioSW7);            // set switch as input
        gpio_set_debounce(gpioSW7, 200);         // debounce with delay of 200ms
        gpio_export(gpioSW7, false);             // make it appear in /sys/class/gpio

	
 
        //printk(KERN_INFO "GPIO_LED: switch currently in state %d\n", gpio_get_value(gpioSW));
 
        /*// GPIO numbers and IRQ numbers are not the same and this function will map them
        irqNumber = gpio_to_irq(gpioSW);
        printk(KERN_INFO "GPIO_LED: switch %d is mapped to IRQ %d\n", gpioSW, irqNumber);
 
        // request an interrupt line
        result = request_irq(irqNumber,                             // interrupt number requested
                        (irq_handler_t) gpio_irq_handler,           // pointer to the handler function
                        IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, // trigger both on rising and falling signal edge
                        "ledswitch_gpio_handler",                   // used in /proc/interrupts to identify the owner
                        NULL);                                      // *dev_id for shared interrupt lines
        printk(KERN_INFO "GPIO_LED: the interrupt request result is: %d\n", result);*/
        return result;
}
 
static void __exit gpio_exit(void) {
      
	gpio_unexport(gpioSW0);
        gpio_unexport(gpioSW1);
	gpio_unexport(gpioSW2);
	gpio_unexport(gpioSW3);
	gpio_unexport(gpioSW4);
	gpio_unexport(gpioSW5);
	gpio_unexport(gpioSW6);
	gpio_unexport(gpioSW7);
	gpio_free(gpioSW0);        
	gpio_free(gpioSW1);
	gpio_free(gpioSW2);
	gpio_free(gpioSW3);
	gpio_free(gpioSW4);
	gpio_free(gpioSW5);
	gpio_free(gpioSW6);
	gpio_free(gpioSW7);
	
        printk(KERN_INFO "GPIO: Module unloaded\n");
}
 
/*static irq_handler_t gpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs) {
        ledOn = gpio_get_value(gpioSW); // invert led state
        gpio_set_value(gpioLED, ledOn);
        return (irq_handler_t) IRQ_HANDLED; // announce that the IRQ has been handled correctly
}*/
 
module_init(gpio_init);
module_exit(gpio_exit);
