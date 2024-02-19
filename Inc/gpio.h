#ifndef GPIO_H
#define GPIO_H

#include "stm32f031x6.h"

/**
 * @brief Initializes GPIOB
 * @note Sets B3 and B4 as output pins
 */
void static inline GPIOB_Init() {
    // Enable the GPIOB peripheral in 'RCC_AHBENR'.
    RCC->AHBENR   |= RCC_AHBENR_GPIOBEN;
    // Initialize the GPIOB pins.
    // B3 is connected to an LED on the 'Nucleo' board.
    GPIOB->MODER |= (0x1 << GPIO_MODER_MODER3_Pos);
    GPIOB->MODER |= (0x1 << GPIO_MODER_MODER4_Pos);
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT_3;
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT_4;
}

/**
 * @brief Toggles the state of the LED on pin B3
 */
void static inline Toggle_Pins() {
    GPIOB->ODR ^= GPIO_ODR_3; // Toggle the LED
    GPIOB->ODR ^= GPIO_ODR_4; // Toggle the LED
}

#endif // GPIO_H
