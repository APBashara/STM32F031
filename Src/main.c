#include "main.h"

void SystemClock_Config(void);
void TIM2_Enable(void);
void StartTimer(TIM_TypeDef *TIM, uint16_t ms);
void TIM2_IRQ_handler(void);
void StopTimer(TIM_TypeDef *TIMx);
void Error_Handler(void);

#define CORE_CLK 48000000

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

void TIM2_Enable() {
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Enable TIM2 clock
  
  // Set Interrupt Priority and Enable the Interupt
  __NVIC_SetPriority(TIM2_IRQn, 0x03);
  __NVIC_EnableIRQ(TIM2_IRQn);
}

/**
 * @brief Start the TIM peripheral
 * 
 * @param TIM Pointer to Timer Peripherial
 * @param ms Amount of time in ms before interrupt is triggered
 */
void StartTimer(TIM_TypeDef *TIM, uint16_t ms) {
  TIM->CR1 &= ~TIM_CR1_CEN; // Make sure the timer is Disabled
  // Start the timer
  RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST;
  RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;

  // TODO: Issue with the prescaler, need to figure out how to set it correctly
  TIM->PSC = CORE_CLK / 1000; // Divide out the core clock
  TIM->ARR = ms; // Set to the value of ms

  TIM->EGR |= TIM_EGR_UG; // Send update event to reset the timer
  TIM->DIER |= TIM_DIER_UIE; // Enable Update Interrupt
  TIM->CR1 |= TIM_CR1_CEN; // Enable the timer
}

/**
 * @brief Fuction to handle update event interrupts
 * 
 */
void TIM2_IRQ_handler(void) {
  // Handle a timer 'update' interrupt event
  if (TIM2->SR & TIM_SR_UIF) { // Check status register for update interrupt flag
    TIM2->SR &= ~(TIM_SR_UIF); // Reset the update interrupt flag
    Toggle_Pins(); // Toggle the LED output pins.
  } else {
    Error_Handler();
  }
}

/**
 * @brief Fucntion to stop the timer
 * 
 * @param TIMx Pointer to Peripheral
 */
void StopTimer(TIM_TypeDef *TIMx) {
  TIMx->CR1 &= ~(TIM_CR1_CEN); // Turn off the timer.
  TIMx->SR  &= ~(TIM_SR_UIF); // Clear the 'pending update interrupt' flag, just in case.
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