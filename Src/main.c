#include "main.h"

#define TIM_EnableIT_UPDATE(TIMx) SET_BIT(TIMx->DIER, TIM_DIER_UIE)
#define TIM_EnableCounter(TIMx) SET_BIT(TIMx->CR1, TIM_CR1_CEN)
#define LED_ON() GPIOA->BSRR = GPIO_BSRR_BS5;
#define LED_OFF() GPIOA->BSRR = GPIO_BSRR_BR5;

uint8_t counter = 0;


	void tim10_init(){
		RCC->APB2ENR |= RCC_APB2ENR_TIM10EN; // тактирование
		NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn); // глоб прер
		TIM10->PSC = 16000-1;// преддел
		TIM10->ARR = 10-1;
		TIM10->EGR |= TIM_EGR_UG; // Принудительно обновляем shadow-регистры
		TIM10->SR &= ~TIM_SR_UIF;

	}

	void led_init(){

		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; //RCC on for GPIO C
		GPIOA->MODER &= ~0x00000C00; /* clear pin mode */
		GPIOA->MODER |= GPIO_MODER_MODER5_0; /* set pin to output mode */

		}

	void exti_init(){

		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // тактирование на порт
		RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
		GPIOC->MODER &= ~GPIO_MODER_MODER13;
		GPIOC->PUPDR |= GPIO_PUPDR_PUPD13_0;
		SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC ;
		EXTI->IMR |= EXTI_IMR_MR13;
		EXTI->FTSR |= EXTI_FTSR_TR13;
		EXTI->RTSR |= EXTI_RTSR_TR13;
		NVIC_EnableIRQ(EXTI15_10_IRQn);
	}


	void TIM1_UP_TIM10_IRQHandler(void){
		if(READ_BIT(TIM10->SR, TIM_SR_UIF)){
			CLEAR_BIT(TIM10->CR1, TIM_CR1_CEN); // остановили таймер
			EXTI->PR |= EXTI_PR_PR13;
			NVIC_ClearPendingIRQ(EXTI15_10_IRQn); // Сброс ожидающего запроса прерывания для EXTI15_10_IRQn
			NVIC_EnableIRQ(EXTI15_10_IRQn); // // Включение прерывания для EXTI15_10_IRQn
			TIM10->SR &= ~TIM_SR_UIF; //// Сброс флага прерывания таймера TIM10
			}
	}

	// прерывания по восходящему фронту
	void EXTI15_10_IRQHandler(void){
			NVIC_DisableIRQ(EXTI15_10_IRQn); // Отключение прерывания для EXTI15_10_IRQn
			GPIOA->ODR ^= GPIO_ODR_OD5;
			TIM10->CNT = 0; // Очистили с
			TIM_EnableCounter(TIM10); // Включение счетчика таймера TIM10
		 }


	int main(void){

		tim10_init();
		led_init();
		exti_init();
		TIM_EnableIT_UPDATE(TIM10);


		while(1){

		}

	}
