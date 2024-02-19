#include "main.h"

void SystemClock_Config(void);
void TIM2_Enable(void);
void StartTimer(uint16_t ms);

void SystemClock_Config(void) {
  FLASH->ACR |= (0x1 << FLASH_ACR_LATENCY_Pos); // 1 wait state, 48MHz Core Clock and 25Mhz Access
  FLASH->ACR |= FLASH_ACR_PRFTBE; // Enable Prefetch Buffer

  RCC->CFGR2 |= (0b1 << RCC_CFGR2_PREDIV_Pos); // DIV = 2
  RCC->CFGR |= (0b1010 << RCC_CFGR_PLLMUL_Pos); // MUL = 12

  RCC->CR |= RCC_CR_PLLON; // Enable PLL
  while (!(RCC->CR & RCC_CR_PLLRDY)) {} // Wait for PLL to be ready

  RCC->CFGR |= (0b10 << RCC_CFGR_SW_Pos); // Select PLL as system clock
  while (!(RCC->CFGR & RCC_CFGR_SWS_PLL)) {} // Wait for PLL to be system clock
}

int main(void) {
  SystemClock_Config();
  GPIOB_Init();
  while (1) {
    Toggle_Pins();
    for (int i = 0; i < 100000; i++) { __asm__("nop"); }
  }
}