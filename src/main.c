#include "main.h"

/**
 * Main program.
 */
int main(void) {
  // Enable the GPIOB peripheral in 'RCC_AHBENR'.
  RCC->AHBENR   |= RCC_AHBENR_GPIOBEN;
  // Initialize the GPIOB pins.
  // B3 is connected to an LED on the 'Nucleo' board.
  //    It should be set to push-pull low-speed output.
  GPIOB->MODER  &= ~(0x3 << (LED_PIN*2));
  GPIOB->MODER  |=  (0x1 << (LED_PIN*2));
  GPIOB->OTYPER &= ~(1 << LED_PIN);
  // Keep track of whether the button is pressed.
  while (1) {
    GPIOB->ODR ^= (1 << LED_PIN); // Toggle the LED
    for (int i = 0; i < 100000; i++) { __asm__("nop"); }
  }
}
