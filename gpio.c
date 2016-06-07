#include "gpio.h"

#define FSEL0 0x20200000
#define SET0 0x2020001C 
#define CLR0 0x20200028
#define LEV0 0x20200034
#define BYTEDIFF 4


//#define FSEL1  0x20200004
//#define FSEL2  0x20200008


/*
 * initializes gpio - does not need to anything
 */
void gpio_init(void) {
}

/*
 * Set GPIO pin number `pin` to the GPIO function `function`. Does not affect
 * other pins.
 *
 * @param pin the pin number to initialize
 * @param function the GPIO function to set for the pin
 */
void gpio_set_function(unsigned int pin, unsigned int function) {
    
    int fsel_num = pin / 10;
    volatile int *setting_location = (volatile int *) (FSEL0 + BYTEDIFF * fsel_num);
    int shift_amount = (pin % 10) * 3;

    int pin_settings = *setting_location;
    pin_settings &= ~(7 << shift_amount); //clear the pin
    pin_settings |= (function << shift_amount); //set the pin
    *setting_location = pin_settings;
}

/*
 * Get the function for GPIO pin number `pin`. Should not affect
 * any registers.
 *
 * @param pin the pin number to initialize
 */
unsigned int gpio_get_function(unsigned int pin) {
    int fsel_num = pin / 10;
    volatile int *setting_location = (volatile int *) (FSEL0 + BYTEDIFF * fsel_num);
    int shift_amount = (pin % 10) * 3;

    int pin_settings = *setting_location;
    int shifted_setting = pin_settings & (7 << shift_amount); //the three bits for this pin
    return (shifted_setting >> shift_amount);
}

/*
 * Sets the pin to input
 *
 * @param pin the pin to set
 */
void gpio_set_input(unsigned pin) {
    gpio_set_function(pin, GPIO_FUNC_INPUT);
}

/*
 * Sets the pin to output
 *
 * @param pin the pin to set
 */

void gpio_set_output(unsigned pin) {
    gpio_set_function(pin, GPIO_FUNC_OUTPUT);
}

/*
 * Sets GPIO pin number `pin` to the value `value`. The GPIO pin should be set
 * the output mode for this call to function correctly.
 *
 * @param pin the pin number to set or clear
 * @param value 1 if the pin should be set, 0 otherwise
 */
void gpio_write(unsigned int pin, unsigned int value) {
    volatile int * setting_location;
    if (value == 1) {
        //turn bit on
        setting_location = (volatile int *) (SET0 + BYTEDIFF * (pin / 32));
    } else {
        //clear bit
        setting_location = (volatile int *) (CLR0 + BYTEDIFF * (pin / 32));
    }
    int pin_setting = *setting_location;
    pin_setting = 1 << (pin % 32);
    *setting_location = pin_setting;
}

/*
 * Read GPIO pin number `pin`. 
 *
 * @param pin the pin number to be read
 */
unsigned int gpio_read(unsigned int pin) {
    int * setting_location = (int *) (LEV0 + BYTEDIFF * (pin / 32));
    int pin_setting = *setting_location;
    
    int shifted_value = pin_setting & (1 << (pin % 32));
    return shifted_value >> (pin % 32);
}
