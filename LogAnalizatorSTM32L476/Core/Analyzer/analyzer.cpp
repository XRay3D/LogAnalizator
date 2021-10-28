///*
//********************************************************************************
//* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2018
//*
//* Программное обеспечение предоставляется на условиях «как есть» (as is).
//* При распространении указание автора обязательно.
//********************************************************************************
//*/

#include "analyzer.h"
#include "Analyzer_USB.h"
#include "main.h"

uint8_t Capture[CAPTURE_BUFFER_SIZE] {};
volatile uint8_t Status_;
uint16_t LevelTrigValue;
uint16_t LevelTrigChnls;
uint16_t EdgeTrigChnls;

void LogAnaliz() {
    //    AnalyzerInit();

    //    for (;;) {
    if (Analyzer.status() == USB_Status::START_CAPTURE) {
        CaptureT* NewData = (CaptureT*)Analyzer.GetPacket();
        uint16_t SamplesToCapture = NewData->CAPTURE_SAMPLE_OFFSET;


        Status_ &= ~CAPTURE_COMPLETE;
        DMA_CAPTURE_CH->CNDTR = SamplesToCapture;
        DMA_CAPTURE_CH->CCR |= DMA_CCR_EN;

        CAPTURE_TIMER->SR = 0;
        CAPTURE_TIMER->PSC = NewData->CAPTURE_TIM_PSC_OFFSET;

        CAPTURE_TIMER->ARR = NewData->CAPTURE_TIM_ARR_OFFSET;
        CAPTURE_TIMER->CNT = CAPTURE_TIMER->ARR;
        CAPTURE_TIMER->DIER = TIM_DIER_UDE;
        if (NewData->CAPTURE_TRIGGER_ENABLE_OFFSET == CAPTURE::TRIG_ENABLE) {
            LevelTrigValue = 0;
            LevelTrigChnls = 0;
            EXTI->RTSR1 &= ~LINE_INTERRUPT_MASK;
            EXTI->FTSR1 &= ~LINE_INTERRUPT_MASK;
            uint8_t OnlyLevelTrigger = 1;
            uint8_t TrigSetBytes = CAPTURE::TRIG_BYTES_COUNT;
            uint8_t ChnlNumOffset = 0;
            uint16_t InterruptMask = CAPTURE_MASK;

            for (uint8_t TrigByte = 0; TrigByte < TrigSetBytes; TrigByte++) {
                uint8_t TrigSettings = NewData->CAPTURE_TRIGGER_MASK;

                for (uint8_t ChnlPart = 0; ChnlPart < 2; ChnlPart++) {
                    uint8_t ChnlNum = 2 * TrigByte + ChnlPart + ChnlNumOffset;
                    uint8_t ChnlTrigger = (TrigSettings >> (4 * ChnlPart)) & 0x0F;

                    if (ChnlTrigger != TRIGGER::NONE) {
                        switch (ChnlTrigger) {
                        case TRIGGER::LOW_LEVEL:
                            LevelTrigChnls |= 1 << ChnlNum;
                            break;

                        case TRIGGER::HIGH_LEVEL:
                            LevelTrigValue |= 1 << ChnlNum;
                            LevelTrigChnls |= 1 << ChnlNum;
                            break;

                        case TRIGGER::RISING_EDGE:
                            EXTI->RTSR1 |= 1 << ChnlNum;
                            OnlyLevelTrigger = 0;
                            break;

                        case TRIGGER::FALLING_EDGE:
                            EXTI->FTSR1 |= 1 << ChnlNum;
                            OnlyLevelTrigger = 0;
                            break;

                        case TRIGGER::ANY_EDGE:
                            EXTI->RTSR1 |= 1 << ChnlNum;
                            EXTI->FTSR1 |= 1 << ChnlNum;
                            OnlyLevelTrigger = 0;
                            break;

                        default:
                            break;
                        }
                    }
                }
            }

            if (OnlyLevelTrigger == 1) //у всех каналов триггеры по уровню
            {
                while ((CAPTURE_GPIO->IDR & LevelTrigChnls) != LevelTrigValue)
                    ;
                CAPTURE_TIMER->CR1 |= TIM_CR1_CEN;
            }

            else {
                EdgeTrigChnls = EXTI->RTSR1;
                EdgeTrigChnls = (EdgeTrigChnls | EXTI->FTSR1) & InterruptMask;
                EXTI->IMR1 |= EdgeTrigChnls;
            }
        }

        else
            CAPTURE_TIMER->CR1 |= TIM_CR1_CEN;

        while ((Status_ & CAPTURE_COMPLETE) != CAPTURE_COMPLETE) {
            __WFI();
        }

        uint8_t PacketsToSend = SamplesToCapture / USB_MESSAGE_LENGTH;

        for (uint8_t Packet = 0; Packet < PacketsToSend; Packet++) {
            while ((*Analyzer.IsReadyToSend()) != 1) { }

            Analyzer.SendData(&Capture[USB_MESSAGE_LENGTH * Packet]);
        }

        Analyzer.clearStatus(USB_Status::START_CAPTURE);
    }
    //}
}

void AnalyzerInit() {
    //    //HSI, PLL, 48 MHz
    //    FLASH->ACR = FLASH_ACR_PRFTBE | (uint32_t)FLASH_ACR_LATENCY;
    //    // HCLK = SYSCLK / 1
    //    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
    //    // PCLK2 = HCLK / 1
    //    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE_DIV1;
    //    // частота PLL: (HSI / 2) * 12 = (8 / 2) * 12 = 48 (МГц)
    //    //RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMUL));
    //    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSI_DIV2 | RCC_CFGR_PLLMUL12);
    //    RCC->CR |= RCC_CR_PLLON;
    //    while ((RCC->CR & RCC_CR_PLLRDY) == 0)
    //        ;
    //    //RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    //    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;
    //    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL)
    //        ;

    //    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    //    GPIOA->OSPEEDR |= (3 << (2 * 0)) | (3 << (2 * 1)) | (3 << (2 * 2)) | (3 << (2 * 3)) | (3 << (2 * 4)) | (3 << (2 * 5)) | (3 << (2 * 6)) | (3 << (2 * 7));

    //    GPIOA->PUPDR |= (2 << (2 * 0)) | (2 << (2 * 1)) | (2 << (2 * 2)) | (2 << (2 * 3)) | (2 << (2 * 4)) | (2 << (2 * 5)) | (2 << (2 * 6)) | (2 << (2 * 7));

    //    NVIC_EnableIRQ(EXTI0_1_IRQn);
    //    NVIC_SetPriority(EXTI0_1_IRQn, 0);
    //    NVIC_EnableIRQ(EXTI2_3_IRQn);
    //    NVIC_SetPriority(EXTI2_3_IRQn, 0);
    //    NVIC_EnableIRQ(EXTI4_15_IRQn);
    //    NVIC_SetPriority(EXTI4_15_IRQn, 0);

    //    //ПДП (DMA)
    //    RCC->DMA_FOR_CAPTURE_ENR |= DMA_FOR_CAPTURE_CLK_EN;
    DMA_CAPTURE_CH->CPAR = (uint32_t)(&(GPIOA->IDR));
    DMA_CAPTURE_CH->CMAR = (uint32_t)(Capture);
    DMA_CAPTURE_CH->CCR = DMA_CCR_MINC | DMA_CCR_TEIE | DMA_CCR_TCIE;
    //    NVIC_EnableIRQ(DMA_CNL_FOR_CAPTURE_IRQ);
    //    NVIC_SetPriority(DMA_CNL_FOR_CAPTURE_IRQ, 0);

    //    //таймер для захвата
    //    RCC->CAPTURE_TIMER_ENR |= CAPTURE_TIMER_CLK_EN;

    //    Analyzer.Init();
}

extern "C" void EXTI_LogAnalizer() {
    if (((EXTI->PR2 & EdgeTrigChnls) == EdgeTrigChnls) && ((CAPTURE_GPIO->IDR & LevelTrigChnls) == LevelTrigValue)) {
        LL_TIM_EnableCounter(TIM1);
        EXTI->IMR2 &= ~EdgeTrigChnls;
    }
    EXTI->PR2 |= EdgeTrigChnls;
}

void DMA_FOR_CAPTURE_ISR(void) {

    if ((DMA_FOR_CAPTURE->ISR & DMA_ISR_TEIF3) == DMA_ISR_TEIF3) {
        DMA_CAPTURE_CH->CCR |= DMA_CCR_EN;
    }

    else if ((DMA_FOR_CAPTURE->ISR & DMA_ISR_TCIF3) == DMA_ISR_TCIF3) {
        CAPTURE_TIMER->CR1 &= ~TIM_CR1_CEN;
        DMA_FOR_CAPTURE->IFCR |= DMA_IFCR_CGIF3;
        DMA_CAPTURE_CH->CCR &= ~DMA_CCR_EN;
        CAPTURE_TIMER->DIER &= ~TIM_DIER_UDE;
        Status_ |= CAPTURE_COMPLETE;
    }
}
