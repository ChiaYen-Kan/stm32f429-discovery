/**
  ******************************************************************************
  * @file    usbh_bt.h
  * @author  MCD Application Team
  * @brief   This file contains all the prototypes for the usbh_bt.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive  ----------------------------------------------*/
#ifndef __USBH_BT_H
#define __USBH_BT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"


/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_CLASS
  * @{
  */

/** @addtogroup USBH_BT_CLASS
  * @{
  */

/** @defgroup USBH_BT_CLASS
  * @brief This file is the Header file for usbh_bt.c
  * @{
  */


/**
  * @}
  */

/** @defgroup USBH_BT_CLASS_Exported_Types
  * @{
  */

enum
{
  BT_INIT_SETUP,
  BT_INIT_DONE,
};

enum HCI_EVENT_STEP
{
  HIC_STEP_READ,
  HIC_STEP_WAIT,
  HIC_STEP_READ_MULTI,
  HIC_STEP_WAIT_MULTI,
};

enum ACL_INPUT_STEP
{
  ACL_STEP_READ,
  ACL_STEP_WAIT,
  ACL_STEP_READ_MULTI,
  ACL_STEP_WAIT_MULTI,
};

typedef struct
{
  uint8_t   hci_event_pipe;
  uint16_t  hci_event_ep_size;
  uint8_t   hci_event_ep_addr;
  uint8_t   hci_event_buf[255];
  uint8_t   hci_step;
  uint32_t  hci_poll_interval;
  uint32_t  hci_poll_timer;
  uint32_t  hci_total_read;

  uint8_t   acl_input_pipe;
  uint16_t  acl_input_ep_size;
  uint8_t   acl_input_ep_addr;
  uint8_t   acl_input_buf[255];
  uint8_t   acl_step;
  uint32_t  acl_total_read;

  uint8_t   acl_output_pipe;
  uint16_t  acl_output_ep_size;
  uint8_t   acl_output_ep_addr;

  uint8_t   state;
} BT_HandleTypeDef;

/* States for BT State Machine */


/**
  * @}
  */

/** @defgroup USBH_BT_CLASS_Exported_Defines
  * @{
  */

/* BT Class Codes */
#define USB_BT_CLASS        0xE0U

// Used to determine if it is a Bluetooth dongle
#define WI_SUBCLASS_RF      0x01 // RF Controller
#define WI_PROTOCOL_BT      0x01 // Bluetooth Programming Interface

/**
  * @}
  */

/** @defgroup USBH_BT_CLASS_Exported_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup USBH_BT_CLASS_Exported_Variables
  * @{
  */
extern USBH_ClassTypeDef  BT_Class;
#define USBH_BT_CLASS    &BT_Class

/**
  * @}
  */

/** @defgroup USBH_BT_CLASS_Exported_FunctionsPrototype
  * @{
  */
USBH_StatusTypeDef USBH_BT_IOProcess(USBH_HandleTypeDef *phost);
USBH_StatusTypeDef USBH_BT_Init(USBH_HandleTypeDef *phost);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __USBH_BT_H */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

