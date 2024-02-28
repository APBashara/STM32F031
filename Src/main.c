#include "main.h"

void SystemClock_Config(void);
void TIM2_IRQHandler(void);
void Error_Handler(void);

uint32_t priority;

#define CORE_CLK 48000000 // 48MHz Core Clock

void SystemClock_Config(void) {
  FLASH->ACR |= (0x1 << FLASH_ACR_LATENCY_Pos); // 1 wait state, 48MHz Core Clock and 25Mhz Access
  FLASH->ACR |= FLASH_ACR_PRFTBE; // Enable Prefetch Buffer

  RCC->CFGR &= ~(RCC_CFGR_PLLMUL | RCC_CFGR_PLLSRC); // Resest the PLL Multiplier and Source
  RCC->CFGR |= (RCC_CFGR_PLLSRC_HSI_DIV2 | RCC_CFGR_PLLMUL12); // Set PLL to Div by 2 and Mul by 12
  
  RCC->CR |= (RCC_CR_PLLON); // Turn the PLL on and wait for it to be ready.
  while (!(RCC->CR & RCC_CR_PLLRDY)) {};
  
  RCC->CFGR &= ~(RCC_CFGR_SW); // Select the PLL as the system clock source.
  RCC->CFGR |= (RCC_CFGR_SW_PLL); // Select the PLL as the system clock source.
  while (!(RCC->CFGR & RCC_CFGR_SWS_PLL)) {};
}

/**
 * @brief Fuction to handle update event interrupts
 * 
 */
void TIM2_IRQHandler(void) {
  // Handle a timer 'update' interrupt event
  if (TIM2->SR & TIM_SR_UIF) { // Check status register for update interrupt flag
    TIM2->SR &= ~(TIM_SR_UIF); // Reset the update interrupt flag
    Toggle_Pins(); // Toggle the LED output pins.
  } else {
    Error_Handler();
  }
}


int main(void) {
  SystemClock_Config();
  GPIOB_Init();
  TIM2_Enable();
  StartTimer(TIM2, 1000);
  while (1) {
    // Toggle_Pins();
    // for (int i = 0; i < 100000; i++) { __asm__("nop"); }
  }
}

void Error_Handler(void) {
  while (1) {}
}