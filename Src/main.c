#include "main.h"
#include "stm32f031x6.h"
#include "cmsis_os.h"

void SystemClock_Config(void);
void TIM2_Enable(void);
void StartTimer(uint16_t ms);

int core_clk = 48000000;

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
  NVIC_SetPriority(TIM2_IRQn, 0x3); // Set TIM2 priority
  NVIC_EnableIQR(TIM2_IRQn); // Enable TIM2 interrupt
}

// void start_timer(TIM_TypeDef *TIMx, uint16_t ms)
void StartTimer(uint16_t ms) {
  TIM2->CR1 &= ~TIM_CR1_CEN; // Make sure TIM2 is disabled
  RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST; // Reset TIM2
  RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST; // Clear TIM2 reset

  TIM2->PSC = core_clk / 1000; // Prescaler
  TIM2->ARR = ms; // Auto-reload register

  TIM2->EGR |= TIM_EGR_UG; // Generate an update event to update the prescaler value
  TIM2->DIER |= TIM_DIER_UIE; // Enable update interrupt
  TIM2->CR1 |= TIM_CR1_CEN; // Enable TIM2
}