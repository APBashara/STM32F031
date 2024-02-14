#include "main.h"
#include "stm32f031x6.h"
#include "cmsis_os.h"


int main(void) {
  // Enable the GPIOB peripheral in 'RCC_AHBENR'.
  RCC->AHBENR   |= RCC_AHBENR_GPIOBEN;
  // Initialize the GPIOB pins.
  // B3 is connected to an LED on the 'Nucleo' board.
  GPIOB->MODER |= (0x1 << GPIO_MODER_MODER3_Pos);
  GPIOB->MODER |= (0x1 << GPIO_MODER_MODER4_Pos);
  GPIOB->OTYPER &= ~GPIO_OTYPER_OT_3;
  GPIOB->OTYPER &= ~GPIO_OTYPER_OT_4;
  // Keep track of whether the button is pressed.
  while (1) {
    GPIOB->ODR ^= GPIO_ODR_3; // Toggle the LED
    GPIOB->ODR ^= GPIO_ODR_4; // Toggle the LED
    for (int i = 0; i < 100000; i++) { __asm__("nop"); }
  }
}