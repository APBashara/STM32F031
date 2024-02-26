#include "main.h"

void SystemClock_Config(void);
void TIM2_Enable(void);
void StartTimer(TIM_TypeDef *TIM, uint16_t ms);
void TIM2_IRQ_handler(void);
void StopTimer(TIM_TypeDef *TIMx);
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

void TIM2_Enable() {
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Enable TIM2 clock
  TIM2->CR1 &= ~TIM_CR1_CEN; // Make sure the timer is Disabled
  TIM2->CR1 &= ~TIM_CR1_DIR; // Count Up
  
  // Set Interrupt Priority and Enable the Interupt
  NVIC_SetPriority(TIM2_IRQn, 0x03);
  NVIC_EnableIRQ(TIM2_IRQn);
  // priority = NVIC_GetPriority(TIM2_IRQn); // Debug signal to see if the priority was set correctly Seems to work
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

  TIM->PSC &= ~(TIM_PSC_PSC); // Clear the prescaler
  TIM->PSC |= CORE_CLK / 1000; // Divide the clock by 1000 to get 1ms counter
  TIM->ARR = ms; // Reset every ms

  TIM->EGR |= TIM_EGR_UG; // Send update event to reset the timer
  // TIM->DIER |= TIM_DIER_UIE; // Enable Update Interrupt TODO: Causing the system to hang
  TIM->CR1 |= TIM_CR1_CEN; // Enable the timer
  for (int i = 0; i < 100000; i++) { __asm__("nop"); } // Loop to test Debug
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