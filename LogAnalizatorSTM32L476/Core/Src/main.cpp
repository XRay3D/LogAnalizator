#include "main.h"
#include "Nokia1616_LCD_lib.h"
#include "analyzer.h"
#include "dma.h"
#include "gpio.h"
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "usb_device.h"
#include <stdio.h>

extern "C" void SystemClock_Config(void);
void PeriphCommonClock_Config(void);

extern "C" int _write(int file, char* ptr, int len) {
    //return usart_write(platform_get_console(), (u8 *)ptr, len);
    int i = 0;
    for (i = 0; i < len; i++)
        ITM_SendChar((*ptr++));
    return len;
}

int main(void) {
    HAL_Init();
    SystemClock_Config();
    PeriphCommonClock_Config();

    MX_GPIO_Init();
    MX_DMA_Init();
    MX_SPI2_Init();
    MX_USB_DEVICE_Init();
    MX_TIM5_Init();

    Nokia1616_LCD lcd;

    Analyzer.Init();

    while (1) {
        Analyzer.pool(&lcd);
    }
}

void SystemClock_Config(void) {
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
void PeriphCommonClock_Config(void) {
    LL_RCC_PLLSAI1_ConfigDomain_48M(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 24, LL_RCC_PLLSAI1Q_DIV_2);
    LL_RCC_PLLSAI1_EnableDomain_48M();
    LL_RCC_PLLSAI1_Enable();

    /* Wait till PLLSAI1 is ready */
    while (LL_RCC_PLLSAI1_IsReady() != 1) {
    }
}

extern "C" void Error_Handler(void) {
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
void assert_failed(uint8_t* file, uint32_t line) {

    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
