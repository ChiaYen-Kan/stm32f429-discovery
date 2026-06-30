/**
  ******************************************************************************
  * @file    Templates/Src/stm32f4xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.
  *         
  @verbatim
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
    [..]
    This file is generated automatically by STM32CubeMX and eventually modified 
    by the user

  @endverbatim
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "usbh_core.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @defgroup HAL_MSP
  * @brief HAL MSP module.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define HOST_POWERSW_PORT                 GPIOC
#define HOST_POWERSW_VBUS                 GPIO_PIN_4
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief  Initializes the Global MSP.
  * @param  None
  * @retval None
  */
void HAL_MspInit(void)
{
  /* NOTE : This function is generated automatically by STM32CubeMX and eventually  
            modified by the user
   */
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
}

/**
  * @brief  DeInitializes the Global MSP.
  * @param  None  
  * @retval None
  */
void HAL_MspDeInit(void)
{
  /* NOTE : This function is generated automatically by STM32CubeMX and eventually  
            modified by the user
   */
}

/**
  * @brief HCD MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hhcd: HCD handle pointer
  * @retval None
  */
void HAL_HCD_MspInit(HCD_HandleTypeDef* hhcd)
{
  /* On STM32F429I-DISCO, USB OTG HS Core will operate in Full speed mode */
  GPIO_InitTypeDef GPIO_InitStruct;

  /* EMBEDDED Physical interface */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  GPIO_InitStruct.Pin = (GPIO_PIN_14 | GPIO_PIN_15);
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_OTG_HS_FS;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Configure VBUS Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Enable USB HS Clocks */
  __HAL_RCC_USB_OTG_HS_CLK_ENABLE();

  /* Configure Power Switch Vbus Pin */
  GPIO_InitStruct.Pin = HOST_POWERSW_VBUS;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(HOST_POWERSW_PORT, &GPIO_InitStruct);

  /* By Default, DISABLE is needed on output of the Power Switch */
  HAL_GPIO_WritePin(HOST_POWERSW_PORT, HOST_POWERSW_VBUS, GPIO_PIN_SET);

  USBH_Delay(200); /* Delay is need for stabilising the Vbus Low */

  /* Set USBHS Interrupt to the lowest priority */
  HAL_NVIC_SetPriority(OTG_HS_IRQn, 5, 0);

  /* Enable USBFS Interrupt */
  HAL_NVIC_EnableIRQ(OTG_HS_IRQn);
}

/**
  * @brief HCD MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param hhcd: HCD handle pointer
  * @retval None
  */
void HAL_HCD_MspDeInit(HCD_HandleTypeDef* hhcd)
{
  __HAL_RCC_USB_OTG_HS_CLK_DISABLE();
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
