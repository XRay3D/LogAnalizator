#include "analyzer.h"
#include "Nokia1616_LCD_lib.h"
#include "main.h"
#include "stm32l4xx.h"
#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_custom_hid_if.h"
#include "usbd_customhid.h"

#define USB_CMD_START_CAPTURE 0x55

#define CAPTURE_GPIO GPIOA
#define CAPTURE_MASK 0xFF

#define LINE_INTERRUPT_MASK CAPTURE_MASK

#define DMA_FOR_CAPTURE DMA2
#define DMA_CAPTURE_CH DMA2_Channel2
#define DMA_CAPTURE_IRQ DMA2_Channel2_IRQn

#define CAPTURE_TIMER TIM5
#define CAPTURE_TIMER_IRQ TIM5_IRQn

#define CHANNELS_COUNT 8

#define CAPTURE_COMPLETE (1 << 0)

extern "C" void ReceiveData(uint8_t* data) {
    Analyzer.ReceiveData(data);
}

void AnalyzerUSB::pool(void* lcd_) {
    Nokia1616_LCD& lcd = *(Nokia1616_LCD*)lcd_;
    //    AnalyzerInit();
    uint8_t y {};
    //    for (;;) {
    static int ctr {};
    if (Analyzer.status() == Status::CAPTURE) {
        lcd.clear();
        lcd.print(++ctr, { 0, y += 8 }, White);

        CaptureT* Capture = (CaptureT*)Analyzer.GetPacket();
        uint16_t SamplesToCapture = Capture->SAMPLE;
        lcd.print("Start", { 0, y += 8 }, White);
        lcd.print("SAMPLE", { 0, y += 8 }, White);
        lcd.print(Capture->SAMPLE, { 0, y += 8 }, White);
        lcd.print("TIM_ARR", { 0, y += 8 }, White);
        lcd.print(Capture->TIM_ARR, { 0, y += 8 }, White);
        lcd.print("TIM_PSC", { 0, y += 8 }, White);
        lcd.print(Capture->TIM_PSC, { 0, y += 8 }, White);

        status_ = Status::CAPTURE;

        DMA_CAPTURE_CH->CNDTR = SamplesToCapture;
        DMA_CAPTURE_CH->CCR |= DMA_CCR_EN;

        CAPTURE_TIMER->SR = 0;
        CAPTURE_TIMER->PSC = Capture->TIM_PSC;
        CAPTURE_TIMER->CNT = CAPTURE_TIMER->ARR = Capture->TIM_ARR;
        CAPTURE_TIMER->DIER = TIM_DIER_UDE;

        if (0 && Capture->TRIGGER_ENABLE == CAPTURE::TRIG_ENABLE) {
            LevelTrigValue = 0;
            LevelTrigChnls = 0;
            CLEAR_BIT(EXTI->RTSR1, LINE_INTERRUPT_MASK);
            CLEAR_BIT(EXTI->FTSR1, LINE_INTERRUPT_MASK);
            uint8_t OnlyLevelTrigger = 1;
            uint8_t TrigSetBytes = CAPTURE::TRIG_BYTES_COUNT;
            uint8_t ChnlNumOffset = 0;
            uint16_t InterruptMask = CAPTURE_MASK;

            for (uint8_t TrigByte = 0; TrigByte < TrigSetBytes; TrigByte++) {
                uint8_t TrigSettings = Capture->TRIGGER_MASK;

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

            if (OnlyLevelTrigger == 1) { //у всех каналов триггеры по уровню
                while ((CAPTURE_GPIO->IDR & LevelTrigChnls) != LevelTrigValue) { }
                CAPTURE_TIMER->CR1 |= TIM_CR1_CEN;
            } else {
                EdgeTrigChnls = EXTI->RTSR1;
                EdgeTrigChnls = (EdgeTrigChnls | EXTI->FTSR1) & InterruptMask;
                EXTI->IMR1 |= EdgeTrigChnls;
            }
        } else {
            CAPTURE_TIMER->CR1 |= TIM_CR1_CEN;
            lcd.clear();
        }

        lcd.print("Wait", { 0, y += 8 }, White);

        while (status_ == Status::CAPTURE)
            __WFI();

        lcd.print("Complete", { 0, y += 8 }, White);

        uint16_t ctr {};
        lcd.print(ctr, { 0, y += 8 }, Red);
        while (ctr < SamplesToCapture) {
            while (!Analyzer.IsReadyToSend()) { }
            Analyzer.TransmitData(CaptureBuff + ctr);
            ctr += USB_MESSAGE_LENGTH;
        }
        lcd.print(ctr, { 0, y += 8 }, Red);

        //        uint8_t PacketsToSend = SamplesToCapture / USB_MESSAGE_LENGTH;
        //        for (uint8_t Packet {}; Packet < PacketsToSend; ++Packet) {
        //            while (!Analyzer.IsReadyToSend()) { }
        //            lcd.print(Analyzer.TransmitData(CaptureBuff + USB_MESSAGE_LENGTH * Packet), { 0, y += 8 }, Red);
        //        }
        //        lcd.print("Packet", { 0, y += 8 }, White);

        Analyzer.setStatus(Status::IDLE);
        lcd.print("End", { 0, y += 8 }, White);
    }

    //}
}

void AnalyzerUSB::Init() {
    //    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    //    GPIOA->OSPEEDR |= (3 << (2 * 0)) | (3 << (2 * 1)) | (3 << (2 * 2)) | (3 << (2 * 3)) | (3 << (2 * 4)) | (3 << (2 * 5)) | (3 << (2 * 6)) | (3 << (2 * 7));
    //    GPIOA->PUPDR |= (2 << (2 * 0)) | (2 << (2 * 1)) | (2 << (2 * 2)) | (2 << (2 * 3)) | (2 << (2 * 4)) | (2 << (2 * 5)) | (2 << (2 * 6)) | (2 << (2 * 7));

    //    NVIC_EnableIRQ(EXTI0_IRQn);
    //    NVIC_SetPriority(EXTI0_IRQn, 0);
    //    NVIC_EnableIRQ(EXTI1_IRQn);
    //    NVIC_SetPriority(EXTI1_IRQn, 0);
    //    NVIC_EnableIRQ(EXTI2_IRQn);
    //    NVIC_SetPriority(EXTI2_IRQn, 0);
    //    NVIC_EnableIRQ(EXTI3_IRQn);
    //    NVIC_SetPriority(EXTI3_IRQn, 0);
    //    NVIC_EnableIRQ(EXTI4_IRQn);
    //    NVIC_SetPriority(EXTI4_IRQn, 0);
    //    NVIC_EnableIRQ(EXTI9_5_IRQn);
    //    NVIC_SetPriority(EXTI9_5_IRQn, 0);

    //        RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    DMA_CAPTURE_CH->CPAR = (uint32_t)(&(GPIOA->IDR));
    DMA_CAPTURE_CH->CMAR = (uint32_t)(CaptureBuff + 1);
    DMA_CAPTURE_CH->CCR = DMA_CCR_MINC | DMA_CCR_TEIE | DMA_CCR_TCIE;
    //    NVIC_EnableIRQ(DMA1_Channel6_IRQn);
    //    NVIC_SetPriority(DMA1_Channel6_IRQn, 0);

    //    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
}

extern USBD_HandleTypeDef hUsbDeviceFS;

uint8_t* AnalyzerUSB::GetPacket() { return usbData; }

Status AnalyzerUSB::status() const { return Status(status_); }

bool AnalyzerUSB::IsReadyToSend() {
    return ((USBD_CUSTOM_HID_HandleTypeDef*)hUsbDeviceFS.pClassData)->state == CUSTOM_HID_IDLE;
}

void AnalyzerUSB::setStatus(volatile Status status) { status_ = status; }

void AnalyzerUSB::ReceiveData(uint8_t* data) {
    memcpy(usbData, data, USB_MESSAGE_LENGTH);
    switch (usbData[1]) {
    case USB_CMD_START_CAPTURE:
        status_ = Status::CAPTURE;
        break;
    default:
        break;
    }
}

int8_t AnalyzerUSB::TransmitData(uint8_t* data) {
    int8_t ret = { -1 };
    if (IsReadyToSend() && hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED) {
        data[0] = 2;
        ret = USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, data, USB_MESSAGE_LENGTH + 1);
    }
    return ret;
}

extern "C" void EXTI_LogAnalizer() {
    if (((EXTI->PR1 & Analyzer.EdgeTrigChnls) == Analyzer.EdgeTrigChnls)
        && ((CAPTURE_GPIO->IDR & Analyzer.LevelTrigChnls) == Analyzer.LevelTrigValue)) {
        LL_TIM_EnableCounter(TIM5);
        CLEAR_BIT(EXTI->IMR1, Analyzer.EdgeTrigChnls);
    }
    SET_BIT(EXTI->PR1, Analyzer.EdgeTrigChnls);
}

extern "C" void DMA_FOR_CAPTURE_ISR(void) {
    if (READ_BIT(DMA_FOR_CAPTURE->ISR, DMA_ISR_TEIF2) == DMA_ISR_TEIF2) {
        SET_BIT(DMA_CAPTURE_CH->CCR, DMA_CCR_EN);
        Analyzer.setStatus(Status::ERROR);
    } else if (READ_BIT(DMA_FOR_CAPTURE->ISR, DMA_ISR_TCIF2) == DMA_ISR_TCIF2) {
        SET_BIT(DMA_FOR_CAPTURE->IFCR, DMA_IFCR_CGIF2);
        CLEAR_BIT(DMA_CAPTURE_CH->CCR, DMA_CCR_EN);
        CLEAR_BIT(CAPTURE_TIMER->CR1, TIM_CR1_CEN);
        CLEAR_BIT(CAPTURE_TIMER->DIER, TIM_DIER_UDE);
        Analyzer.setStatus(Status::COMPLETE);
    }
}
