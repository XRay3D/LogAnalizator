#include "main.h"
#include "Nokia1616_LCD_lib.h"
#include "analyzer.h"
#include "ctre.hpp"
#include "dma.h"
#include "gpio.h"
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "usb_device.h"
#include <stdio.h>

extern "C" void SystemClock_Config(void);
void PeriphCommonClock_Config(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    PeriphCommonClock_Config();

    MX_GPIO_Init();
    MX_DMA_Init();
    MX_SPI2_Init();
    MX_USB_DEVICE_Init();
    MX_TIM1_Init();

    Nokia1616_LCD lcd;

    uint64_t v {};
    uint8_t row {};
    lcd.clear();

    AnalyzerInit();

    while (1) {
        LogAnaliz();
        //        constexpr uint8_t Rows = 20;
        //        if (1) {
        //            lcd.print(v++, { 0, uint8_t(row++ % Rows * 8) }, White, 2);
        //            lcd.print(v++, { 0, uint8_t(row++ % Rows * 8) }, Red, 8);
        //            lcd.print(v++, { 0, uint8_t(row++ % Rows * 8) }, Green, 10);
        //            lcd.print(v++, { 0, uint8_t(row++ % Rows * 8) }, Blue, 16);
        //        } else {
        //            char str[Nokia1616_LCD::Width / 6 + 1];
        //            //srand(SysTick->VAL);
        //            for (auto&& chr : str)
        //                chr = rand() & 0x7F;
        //            str[std::size(str) - 1] = 0;
        //            if (auto [whole, cap1, cap2] = ctre::match<R"(.*(\d{2})\S*(\d{2})\S*)">(str); whole) { //22520
        //                lcd.clear();
        //                row = 8 * 3;
        //                lcd.print(whole.to_view(), { 0, row += 8 }, Green);
        //                lcd.print(cap1.to_view(), { 0, row += 8 }, Red);
        //                lcd.print(cap2.to_view(), { 0, row += 8 }, Red);
        //            } else {
        //                lcd.print("Error", { 0, 0 }, White);
        //                lcd.print({ str, std::size(str) }, { 0, 8 }, White);
        //                lcd.print(++v, { 0, 16 }, Blue);
        //            }
        //        }
    }
}

void SystemClock_Config(void)
{
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
    while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_4) {
    }
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
    LL_RCC_MSI_Enable();

    /* Wait till MSI is ready */
    while (LL_RCC_MSI_IsReady() != 1) {
    }
    LL_RCC_MSI_EnableRangeSelection();
    LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_6);
    LL_RCC_MSI_SetCalibTrimming(0);
    LL_PWR_EnableBkUpAccess();
    LL_RCC_LSE_SetDriveCapability(LL_RCC_LSEDRIVE_LOW);
    LL_RCC_LSE_Enable();

    /* Wait till LSE is ready */
    while (LL_RCC_LSE_IsReady() != 1) {
    }
    LL_RCC_MSI_EnablePLLMode();
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 40, LL_RCC_PLLR_DIV_2);
    LL_RCC_PLL_EnableDomain_SYS();
    LL_RCC_PLL_Enable();

    /* Wait till PLL is ready */
    while (LL_RCC_PLL_IsReady() != 1) {
    }
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    /* Wait till System clock is ready */
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {
    }
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
    LL_SetSystemCoreClock(80000000);

    /* Update the time base */
    if (HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK) {
        Error_Handler();
    }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
    LL_RCC_PLLSAI1_ConfigDomain_48M(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 24, LL_RCC_PLLSAI1Q_DIV_2);
    LL_RCC_PLLSAI1_EnableDomain_48M();
    LL_RCC_PLLSAI1_Enable();

    /* Wait till PLLSAI1 is ready */
    while (LL_RCC_PLLSAI1_IsReady() != 1) {
    }
}

extern "C" void Error_Handler(void)
{
    __disable_irq();
    while (1) {
    }
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{

    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
