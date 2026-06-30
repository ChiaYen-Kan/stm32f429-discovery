/*
 * main.c
 *
 *  Created on: 2026-06-08
 *      Author: ChiaYen
 */

#include <stdio.h>
#include <stdbool.h>
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "usbh_core.h"
#include "usbh_bt.h"

#include "DualShock4Type.h"
#include "SwitchProType.h"

#if 1
__attribute__ ((section(".ccmram"))) USBH_HandleTypeDef hUSBHost;
#else
USBH_HandleTypeDef hUSBHost;
#endif

void SystemClock_Config(void);
void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

int main(int argc, char **argv)
{
  HAL_Init();

  SystemClock_Config();

  BSP_LED_Init(LED3);
  BSP_LED_Init(LED4);

  BSP_LCD_Init();
  BSP_LCD_LayerDefaultInit(1, LCD_FRAME_BUFFER);

  /* Set LCD Foreground Layer  */
  BSP_LCD_SelectLayer(1);

  BSP_LCD_Clear(LCD_COLOR_BLACK);

  /* Clear the LCD */
  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
  BSP_LCD_Clear(LCD_COLOR_BLACK);

  /* Set the LCD Text Color */
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

  /* Display LCD messages */
  BSP_LCD_SetFont(&LCD_DEFAULT_FONT);
  BSP_LCD_DisplayStringAt(0, 10, (uint8_t *)"PS4 BT DEMO", CENTER_MODE);
  BSP_LCD_SetFont(&Font16);
  BSP_LCD_DisplayStringAt(0, 35, (uint8_t *)"Press", LEFT_MODE);
  BSP_LCD_DisplayStringAt(0, 51, (uint8_t *)"PS + SHARE button", LEFT_MODE);
  BSP_LCD_DisplayStringAt(0, 67, (uint8_t *)"for enter broadcast", LEFT_MODE);

  /* Init Host Library */
  USBH_Init(&hUSBHost, USBH_UserProcess, 0);

  /* Add Supported Class */
  USBH_RegisterClass(&hUSBHost, USBH_BT_CLASS);

  /* Start Host Process */
  USBH_Start(&hUSBHost);

  /* Run Application (Blocking mode) */
  while (1)
  {
    /* USB Host Background task */
    USBH_Process(&hUSBHost);
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 192000000
  *            HCLK(Hz)                       = 192000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 384
  *            PLL_P                          = 2
  *            PLL_Q                          = 8
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 384;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while (1);
  }

  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    /* Initialization Error */
    while (1);
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
      | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization Error */
    while (1);
  }
}

void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
  switch (id)
  {
    case HOST_USER_SELECT_CONFIGURATION:
      break;

    case HOST_USER_DISCONNECTION:
      break;

    case HOST_USER_CLASS_ACTIVE:
      break;

    case HOST_USER_CONNECTION:
      break;

    default:
      break;
  }
}


void usr_dualshock4_handle(const DualShock4_Info_TypeDef *ds4Info)
{
  static bool first = true;

  if (first)
  {
    BSP_LCD_Clear(LCD_COLOR_BLACK);

    BSP_LCD_SetFont(&LCD_DEFAULT_FONT);
    BSP_LCD_DisplayStringAt(0, 10, (uint8_t *)"DualShock4", CENTER_MODE);

    BSP_LCD_SetFont(&Font16);
    first = false;
  }

  char text[128] = {0};

  sprintf(text, "UP:%1d RIGHT:%1d DOWN:%1d", ds4Info->UP, ds4Info->RIGHT, ds4Info->DOWN);
  BSP_LCD_DisplayStringAt(0, 35, (uint8_t *)text, LEFT_MODE);

  sprintf(text, "LEFT:%1d", ds4Info->LEFT);
  BSP_LCD_DisplayStringAt(0, 51, (uint8_t *)text, LEFT_MODE);

  sprintf(text, "SQU:%1d CRO:%1d CIR:%1d", ds4Info->SQUARE, ds4Info->CROSS, ds4Info->CIRCLE);
  BSP_LCD_DisplayStringAt(0, 67, (uint8_t *)text, LEFT_MODE);

  sprintf(text, "TRI:%1d", ds4Info->TRIANGLE);
  BSP_LCD_DisplayStringAt(0, 83, (uint8_t *)text, LEFT_MODE);

  sprintf(text, "L1:%1d R1:%1d L2:%1d R2:%1d", ds4Info->L1, ds4Info->R1, ds4Info->L2, ds4Info->R2);
  BSP_LCD_DisplayStringAt(0, 99, (uint8_t *)text, LEFT_MODE);

  sprintf(text, "SHARE:%1d OPT:%1d", ds4Info->SHARE, ds4Info->OPT);
  BSP_LCD_DisplayStringAt(0, 115, (uint8_t *)text, LEFT_MODE);

  sprintf(text, "L3:%1d R3:%1d", ds4Info->L3, ds4Info->R3);
  BSP_LCD_DisplayStringAt(0, 131, (uint8_t *)text, LEFT_MODE);

  sprintf(text, "TPAD:%1d PS:%1d", ds4Info->TPAD, ds4Info->PS);
  BSP_LCD_DisplayStringAt(0, 147, (uint8_t *)text, LEFT_MODE);

  sprintf(text, "LeftX:%3d LeftY:%3d", ds4Info->leftX, ds4Info->leftY);
  BSP_LCD_DisplayStringAt(0, 163, (uint8_t *)text, LEFT_MODE);

  sprintf(text, "RightX:%3d RightY:%3d", ds4Info->rightX, ds4Info->rightY);
  BSP_LCD_DisplayStringAt(0, 179, (uint8_t *)text, LEFT_MODE);

  sprintf(text, "TriggerL2:%3d", ds4Info->triggerL2);
  BSP_LCD_DisplayStringAt(0, 195, (uint8_t *)text, LEFT_MODE);

  sprintf(text, "TriggerR2:%3d", ds4Info->triggerR2);
  BSP_LCD_DisplayStringAt(0, 211, (uint8_t *)text, LEFT_MODE);
}
