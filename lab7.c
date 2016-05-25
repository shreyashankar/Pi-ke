#include "gpio.h"
#include "gpioextra.h"
#include "gpioevent.h"
#include "circular.h"
#include "printf.h"
#include "console.h"
#include "interrupts.h"

volatile static unsigned counter20 = 0;
volatile static unsigned counter21 = 0;
volatile static cir_t *buffer20;
volatile static cir_t *buffer21;

void int_handler(unsigned);

// should not get called.
void impossible_vector(unsigned pc) {
  printf("impossible exception at pc=%x\n", pc);
}

/* we only enable GPIO interrupts, so just forward it blindly  */
void interrupt_vector(unsigned pc) {
  int_handler(pc);
}

void int_handler(unsigned pc) {
  if (gpio_check_and_clear_event(GPIO_PIN21)) {
    counter21++;
    cir_enqueue(buffer21, counter21);
  }
  if (gpio_check_and_clear_event(GPIO_PIN20)) {
    counter20++;
    cir_enqueue(buffer20, counter20);
  }
  //cir_enqueue(buffer, counter);

  
  //printf("%d ", counter);
 
}

void button_test() {
  gpio_set_function(GPIO_PIN21, GPIO_FUNC_INPUT);
  gpio_set_pullup(GPIO_PIN21);
  while (gpio_read(GPIO_PIN21) == 1);
  printf("+");
  while (gpio_read(GPIO_PIN21) == 0);

}


static void setup_interrupts() {
    gpio_set_function(GPIO_PIN21, GPIO_FUNC_INPUT);
    gpio_set_pullup(GPIO_PIN21);
    gpio_detect_falling_edge(GPIO_PIN21);
    gpio_detect_rising_edge(GPIO_PIN21);
    gpio_set_function(GPIO_PIN20, GPIO_FUNC_INPUT);
    gpio_set_pullup(GPIO_PIN20);
    gpio_detect_falling_edge(GPIO_PIN20);
    gpio_detect_rising_edge(GPIO_PIN20);
    interrupts_enable(INTERRUPTS_GPIO3);
    system_enable_interrupts();
}


void main(void) {
  buffer20 = cir_new();
  buffer21 = cir_new();
  printf_init();
  setup_interrupts();
  while (1) {
    if (!cir_empty(buffer20)) {
      printf("20: %d\n", cir_dequeue(buffer20));
    }
    if (!cir_empty(buffer21)) {
      printf("21: %d\n", cir_dequeue(buffer21));
    }
    
    //printf("%d\n", counter);
    //button_test()
  }
}
