#ifndef TIMER_H
#define TIMER_H

#include "stm32f031x6.h"

#define CORE_CLK 48000000 // 48MHz Core Clock


/**
 * @brief Enables the TIM2 peripheral
 * @note Uses Interupts
 * 
 */
void static inline TIM2_Enable() {
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Enable TIM2 clock
  TIM2->CR1 &= ~TIM_CR1_CEN; // Make sure the timer is Disabled
  TIM2->CR1 &= ~TIM_CR1_DIR; // Count Up
  
  // Set Interrupt Priority and Enable the Interupt
  NVIC_SetPriority(TIM2_IRQn, 0x03);
  NVIC_EnableIRQ(TIM2_IRQn);
}

/**
 * @brief Start the TIM peripheral
 * 
 * @param TIM Pointer to Timer Peripherial
 * @param ms Amount of time in ms before interrupt is triggered
 */
void static inline StartTimer(TIM_TypeDef *TIM, uint16_t ms) {
  TIM->CR1 &= ~TIM_CR1_CEN; // Make sure the timer is Disabled
  // Start the timer
  RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST;
  RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;

  TIM->PSC &= ~(TIM_PSC_PSC); // Clear the prescaler
  TIM->PSC |= CORE_CLK / 1000; // Divide the clock by 1000 to get 1ms counter
  TIM->ARR = ms; // Reset every ms

  TIM->EGR |= TIM_EGR_UG; // Send update event to reset the timer
  TIM->DIER |= TIM_DIER_UIE; // Enable Update Interrupt TODO: Causing the system to hang
  TIM->CR1 |= TIM_CR1_CEN; // Enable the timer
}

/**
 * @brief Fucntion to stop the timer
 * 
 * @param TIMx Pointer to Peripheral
 */
void static inline Stop_Timer(TIM_TypeDef *TIMx) {
  TIMx->CR1 &= ~(TIM_CR1_CEN); // Turn off the timer.
  TIMx->SR  &= ~(TIM_SR_UIF); // Clear the 'pending update interrupt' flag, just in case.
}

#endif // TIMER_H
