#ifndef __ANALYZER_H
#define __ANALYZER_H

#include "stm32l4xx.h"

#define CAPTURE_GPIO GPIOA
#define CAPTURE_MASK 0xFF

#define LINE_INTERRUPT_MASK CAPTURE_MASK

//РџР”Рџ (DMA)
#define DMA_FOR_CAPTURE DMA1
//#define DMA_FOR_CAPTURE_ENR AHBENR
//#define DMA_FOR_CAPTURE_CLK_EN RCC_AHBENR_DMAEN
#define DMA_CAPTURE_CH DMA1_Channel6
#define DMA_CAPTURE_IRQ DMA1_Channel6_IRQn

//С‚Р°Р№РјРµСЂ РґР»СЏ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёР№ DMA
#define CAPTURE_TIMER TIM1
//#define CAPTURE_TIMER_ENR APB1ENR
//#define CAPTURE_TIMER_CLK_EN RCC_APB1ENR_TIM3EN
#define CAPTURE_TIMER_IRQ TIM1_IRQn

#define CAPTURE_BUFFER_SIZE 4096 /*0x1000*/
#define CHANNELS_COUNT 8

#define CAPTURE_COMPLETE (1 << 0)

extern uint8_t Capture[CAPTURE_BUFFER_SIZE];
extern volatile uint8_t Status_;
extern uint16_t LevelTrigValue;
extern uint16_t LevelTrigChnls;
extern uint16_t EdgeTrigChnls;

void LogAnaliz();
void AnalyzerInit();

#ifdef __cplusplus
extern "C" {
#endif
void DMA_FOR_CAPTURE_ISR(void);
#ifdef __cplusplus
}
#endif

#endif //__ANALYZER_H
