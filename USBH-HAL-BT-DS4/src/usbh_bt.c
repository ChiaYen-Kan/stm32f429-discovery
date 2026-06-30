/**
  ******************************************************************************
  * @file    usbh_mtp.c
  * @author  MCD Application Team
  * @brief   This file is the MTP Layer Handlers for USB Host MTP class.
  *
  *
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

/* Includes ------------------------------------------------------------------*/
#include "usbh_bt.h"
#include "BTD.h"
#include "BTHID.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_CLASS
  * @{
  */

/** @addtogroup USBH_BT_CLASS
  * @{
  */

/** @defgroup USBH_BT_CORE
  * @brief    This file includes BT Layer Handlers for USB Host BT class.
  * @{
  */

/** @defgroup USBH_BT_CORE_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup USBH_BT_CORE_Private_Defines
  * @{
  */
/**
  * @}
  */


/** @defgroup USBH_BT_CORE_Private_Macros
  * @{
  */
/**
  * @}
  */


/** @defgroup USBH_BT_CORE_Private_Variables
  * @{
  */
/**
  * @}
  */


/** @defgroup USBH_BT_CORE_Private_FunctionPrototypes
  * @{
  */

static USBH_StatusTypeDef USBH_BT_InterfaceInit(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_BT_InterfaceDeInit(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_BT_Process(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_BT_ClassRequest(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_BT_SOFProcess(USBH_HandleTypeDef *phost);

USBH_ClassTypeDef  BT_Class =
{
  "BT",
  USB_BT_CLASS,
  USBH_BT_InterfaceInit,
  USBH_BT_InterfaceDeInit,
  USBH_BT_ClassRequest,
  USBH_BT_Process,
  USBH_BT_SOFProcess
};
/**
  * @}
  */


/** @defgroup USBH_BT_CORE_Private_Functions
  * @{
  */

/**
  * @brief  USBH_BT_InterfaceInit
  *         The function init the BT class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_BT_InterfaceInit(USBH_HandleTypeDef *phost)
{
  uint8_t interface = USBH_FindInterface(phost, phost->pActiveClass->ClassCode, WI_SUBCLASS_RF, WI_PROTOCOL_BT);
  if ((interface == 0xFFU) || (interface >= USBH_MAX_NUM_INTERFACES)) /* Not Valid Interface */
  {
    return USBH_FAIL;
  }

  USBH_StatusTypeDef status = USBH_SelectInterface(phost, interface);
  if (status != USBH_OK)
  {
    return USBH_FAIL;
  }

  phost->pActiveClass->pData = (BT_HandleTypeDef *)USBH_malloc(sizeof(BT_HandleTypeDef));
  BT_HandleTypeDef *bt_handle = (BT_HandleTypeDef *) phost->pActiveClass->pData;

  if (bt_handle == NULL)
  {
    return USBH_FAIL;
  }

  bt_handle->state = BT_INIT_SETUP;

  bt_handle->hci_event_pipe = 0;
  bt_handle->hci_event_ep_size = 0;
  bt_handle->hci_event_ep_addr = 0;
  bt_handle->hci_poll_interval = 10;
  bt_handle->hci_poll_timer = 0;
  bt_handle->hci_step = HIC_STEP_READ;

  bt_handle->acl_input_pipe = 0;
  bt_handle->acl_input_ep_size = 0;
  bt_handle->acl_input_ep_addr = 0;

  bt_handle->acl_step = ACL_STEP_READ;

  bt_handle->acl_output_pipe = 0;
  bt_handle->acl_output_ep_size = 0;
  bt_handle->acl_output_ep_addr = 0;

  for (int i = 0; i < phost->device.CfgDesc.Itf_Desc[interface].bNumEndpoints; i++)
  {
    if (phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[i].bmAttributes == 0x03)
    {
      // interrupt
      bt_handle->hci_event_ep_addr = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[i].bEndpointAddress;
      bt_handle->hci_event_ep_size = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[i].wMaxPacketSize;
    }
    else if (phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[i].bmAttributes == 0x02)
    {
      // bulk

      if (phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[i].bEndpointAddress & 0x80)
      {
        // input
        bt_handle->acl_input_ep_addr = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[i].bEndpointAddress;
        bt_handle->acl_input_ep_size = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[i].wMaxPacketSize;
      }
      else
      {
        // output
        bt_handle->acl_output_ep_addr = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[i].bEndpointAddress;
        bt_handle->acl_output_ep_size = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[i].wMaxPacketSize;
      }
    }
  }

  bt_handle->hci_event_pipe = USBH_AllocPipe(phost, bt_handle->hci_event_ep_addr);
  bt_handle->acl_input_pipe = USBH_AllocPipe(phost, bt_handle->acl_input_ep_addr);
  bt_handle->acl_output_pipe = USBH_AllocPipe(phost, bt_handle->acl_output_ep_addr);

  USBH_OpenPipe(phost, bt_handle->hci_event_pipe, bt_handle->hci_event_ep_addr,
    phost->device.address, phost->device.speed, USB_EP_TYPE_INTR, bt_handle->hci_event_ep_size);

  USBH_OpenPipe(phost, bt_handle->acl_input_pipe, bt_handle->acl_input_ep_addr,
    phost->device.address, phost->device.speed, USB_EP_TYPE_BULK, bt_handle->acl_input_ep_size);

  USBH_OpenPipe(phost, bt_handle->acl_output_pipe, bt_handle->acl_output_ep_addr,
    phost->device.address, phost->device.speed, USB_EP_TYPE_BULK, bt_handle->acl_output_ep_size);

  USBH_LL_SetToggle(phost, bt_handle->hci_event_pipe, 0U);
  USBH_LL_SetToggle(phost, bt_handle->acl_input_pipe, 0U);
  USBH_LL_SetToggle(phost, bt_handle->acl_output_pipe, 0U);

  btd_initialize(phost);
  bthid_initialize(phost);

  return USBH_OK;
}

/**
  * @brief  USBH_BT_InterfaceDeInit
  *         The function DeInit the Pipes used for the BT class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_BT_InterfaceDeInit(USBH_HandleTypeDef *phost)
{
  BT_HandleTypeDef *bt_handle = (BT_HandleTypeDef *)phost->pActiveClass->pData;

  if ((bt_handle->hci_event_pipe) != 0U)
  {
    USBH_ClosePipe(phost, bt_handle->hci_event_pipe);
    USBH_FreePipe(phost, bt_handle->hci_event_pipe);
    bt_handle->hci_event_pipe = 0U;
  }

  if ((bt_handle->acl_input_pipe) != 0U)
  {
    USBH_ClosePipe(phost, bt_handle->acl_input_pipe);
    USBH_FreePipe(phost, bt_handle->acl_input_pipe);
    bt_handle->acl_input_pipe = 0U;
  }

  if ((bt_handle->acl_output_pipe) != 0U)
  {
    USBH_ClosePipe(phost, bt_handle->acl_output_pipe);
    USBH_FreePipe(phost, bt_handle->acl_output_pipe);
    bt_handle->acl_output_pipe = 0U;
  }

  if ((phost->pActiveClass->pData) != NULL)
  {
    USBH_free(phost->pActiveClass->pData);
    phost->pActiveClass->pData = 0U;
  }

  return USBH_OK;
}


/**
  * @brief  USBH_BT_ClassRequest
  *         The function is responsible for handling Standard requests
  *         for BT class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_BT_ClassRequest(USBH_HandleTypeDef *phost)
{
  BT_HandleTypeDef *bt_handle = (BT_HandleTypeDef *) phost->pActiveClass->pData;
  USBH_StatusTypeDef status = USBH_BUSY;

  /* Switch MSC REQ state machine */
  switch (bt_handle->state)
  {
    case BT_INIT_SETUP:
    {
      //if (USBH_SetInterface(pdev, pphost, 1, 0) == USBH_OK)
      {
        bt_handle->state = BT_INIT_DONE;
        break;
      }

      break;
    }
    case BT_INIT_DONE:
    {
      status = USBH_OK;
      break;
    }
    default:
    {
      break;
    }
  }

  return status;

  return USBH_OK;
}


/**
  * @brief  USBH_BT_Process
  *         The function is for managing state machine for BT data transfers
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_BT_Process(USBH_HandleTypeDef *phost)
{
  BT_HandleTypeDef *bt_handle = (BT_HandleTypeDef *) phost->pActiveClass->pData;

  // HCI Read
  switch (bt_handle->hci_step)
  {
    case HIC_STEP_READ:
    {
      memset(bt_handle->hci_event_buf, 0, 255);

      USBH_InterruptReceiveData(phost, bt_handle->hci_event_buf,
        bt_handle->hci_event_ep_size,
        bt_handle->hci_event_pipe);

      bt_handle->hci_step = HIC_STEP_WAIT;
      bt_handle->hci_poll_timer = phost->Timer;
      bt_handle->hci_total_read = 0;
      break;
    }

    case HIC_STEP_WAIT:
    {
      if (phost->Timer - bt_handle->hci_poll_timer >= bt_handle->hci_poll_interval)
      {
        bt_handle->hci_step = HIC_STEP_READ;
      }

      if (USBH_LL_GetURBState(phost, bt_handle->hci_event_pipe) == USBH_URB_DONE)
      {
        uint32_t XferSize = USBH_LL_GetLastXferSize(phost, bt_handle->hci_event_pipe);

        uint16_t total = bt_handle->hci_event_buf[1];
        if ((XferSize - 2) == total)
        {
          btd_hci_event_task(phost, bt_handle->hci_event_buf, XferSize);
          bt_handle->hci_step = HIC_STEP_READ;
        }
        else
        {
          bt_handle->hci_total_read += XferSize;
          bt_handle->hci_step = HIC_STEP_READ_MULTI;
        }
      }
      else
      {
        if (USBH_LL_GetURBState(phost, bt_handle->hci_event_pipe) == USBH_URB_STALL)
        {
          if (USBH_ClrFeature(phost, bt_handle->hci_event_ep_addr) == USBH_OK)
          {
            bt_handle->hci_step = HIC_STEP_READ;
          }
        }
      }
      break;
    }

    case HIC_STEP_READ_MULTI:
    {
      USBH_InterruptReceiveData(phost, bt_handle->hci_event_buf + bt_handle->hci_total_read,
        bt_handle->hci_event_ep_size,
        bt_handle->hci_event_pipe);

      bt_handle->hci_step = HIC_STEP_WAIT_MULTI;
      bt_handle->hci_poll_timer = phost->Timer;

      break;
    }

    case HIC_STEP_WAIT_MULTI:
    {
      if (phost->Timer - bt_handle->hci_poll_timer >= bt_handle->hci_poll_interval)
      {
        bt_handle->hci_step = HIC_STEP_READ;
      }

      if (USBH_LL_GetURBState(phost, bt_handle->hci_event_pipe) == USBH_URB_DONE)
      {
        uint32_t XferSize = USBH_LL_GetLastXferSize(phost, bt_handle->hci_event_pipe);

        uint16_t total = bt_handle->hci_event_buf[1];
        if ((bt_handle->hci_total_read + XferSize - 2) == total)
        {
          btd_hci_event_task(phost, bt_handle->hci_event_buf, bt_handle->hci_total_read + XferSize);
          bt_handle->hci_step = HIC_STEP_READ;
        }
        else
        {
          bt_handle->hci_total_read += XferSize;
          bt_handle->hci_step = HIC_STEP_READ_MULTI;
        }
      }
      else
      {
        if (USBH_LL_GetURBState(phost, bt_handle->hci_event_pipe) == USBH_URB_STALL)
        {
          if (USBH_ClrFeature(phost, bt_handle->hci_event_ep_addr) == USBH_OK)
          {
            bt_handle->hci_step = HIC_STEP_READ;
          }
        }
      }
      break;
    }
  }

  btd_hci_task(phost);

  uint32_t xfercount = 0;
  switch (bt_handle->acl_step)
  {
    case ACL_STEP_READ:
    {
      memset(bt_handle->acl_input_buf, 0, 255);

      USBH_BulkReceiveData(phost, bt_handle->acl_input_buf,
#if 0
        bt_handle->acl_input_ep_size,
#else
        240,
#endif
        bt_handle->acl_input_pipe);

      bt_handle->acl_step = ACL_STEP_WAIT;
      bt_handle->acl_total_read = 0;
      break;
    }
    case ACL_STEP_WAIT:
    {
      if (USBH_LL_GetURBState(phost, bt_handle->acl_input_pipe) == USBH_URB_DONE)
      {
        xfercount = USBH_LL_GetLastXferSize(phost, bt_handle->acl_input_pipe);

        uint32_t total = (uint32_t)bt_handle->acl_input_buf[2] | (uint32_t)bt_handle->acl_input_buf[3] << 8;
        if ((xfercount - 4) == total)
        {
          bt_handle->acl_step = ACL_STEP_READ;
        }
        else
        {
          bt_handle->acl_total_read += xfercount;
          bt_handle->acl_step = ACL_STEP_READ_MULTI;
          xfercount = 0;
        }
      }
      /*else if (usb_status == URB_STALL)
      {
        if ((USBH_ClrFeature(pdev, phost, bt_handle->acl_input_ep_addr,
            bt_handle->hc_num_acl_input)) == USBH_OK)
        {
          bt_handle->acl_step = ACL_STEP_READ;
        }
      }*/
      break;
    }
    case ACL_STEP_READ_MULTI:
    {
      USBH_BulkReceiveData(phost,
        bt_handle->acl_input_buf + bt_handle->acl_total_read,
        bt_handle->acl_input_ep_size, bt_handle->acl_input_pipe);
      bt_handle->acl_step = ACL_STEP_WAIT_MULTI;
      break;
    }
    case ACL_STEP_WAIT_MULTI:
    {
      if (USBH_LL_GetURBState(phost, bt_handle->acl_input_pipe) == USBH_URB_DONE)
      {
        xfercount = USBH_LL_GetLastXferSize(phost, bt_handle->acl_input_pipe);

        uint32_t total = (uint32_t)bt_handle->acl_input_buf[2] | (uint32_t)bt_handle->acl_input_buf[3] << 8;
        if ((bt_handle->acl_total_read + xfercount - 4) == total)
        {
          xfercount = bt_handle->acl_total_read + xfercount;
          bt_handle->acl_step = ACL_STEP_READ;
        }
        else
        {
          bt_handle->acl_total_read += xfercount;
          bt_handle->acl_step = ACL_STEP_READ_MULTI;
          xfercount = 0;
        }
      }
      /*else if (usb_status == URB_STALL)
      {
        if ((USBH_ClrFeature(pdev, phost, bt_handle->acl_input_ep_addr,
            bt_handle->hc_num_acl_input)) == USBH_OK)
        {
          bt_handle->acl_step = ACL_STEP_READ_MULTI;
        }
      }*/
      break;
    }
  }

  btd_acl_event_task(phost, bt_handle->acl_input_buf, xfercount);

  return USBH_OK;
}

/**
  * @brief  USBH_BT_SOFProcess
  *         The function is for managing SOF callback
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_BT_SOFProcess(USBH_HandleTypeDef *phost)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(phost);

  return USBH_OK;
}


/**
  * @brief  USBH_BT_Init
  *         The function Initialize the BT function
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_BT_Init(USBH_HandleTypeDef *phost)
{
  USBH_StatusTypeDef Status = USBH_BUSY;

#if (USBH_USE_OS == 1U)
  osEvent event;

  event = osMessageGet(phost->class_ready_event, osWaitForever);

  if (event.status == osEventMessage)
  {
    if (event.value.v == USBH_CLASS_EVENT)
    {
#else
  while ((Status == USBH_BUSY) || (Status == USBH_FAIL))
  {
    /* Host background process */
    USBH_Process(phost);

    if (phost->gState == HOST_CLASS)
    {
#endif
      Status = USBH_OK;
    }
  }
  return Status;
}

/**
  * @brief  USBH_BT_IOProcess
  *         BT BT process
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_BT_IOProcess(USBH_HandleTypeDef *phost)
{
  if (phost->device.is_connected == 1U)
  {
    if (phost->gState == HOST_CLASS)
    {
      USBH_BT_Process(phost);
    }
  }

  return USBH_OK;
}

/**
  * @}
  */
void usb_hci_write(void *p, uint8_t *hci_buffer, uint32_t hci_len)
{
  USBH_HandleTypeDef *phost = p;

  phost->Control.setup.b.bmRequestType = USB_REQ_TYPE_CLASS | USB_H2D | USB_REQ_RECIPIENT_DEVICE;
  phost->Control.setup.b.bRequest = 0x00;
  phost->Control.setup.b.wValue.w = 0;
  phost->Control.setup.b.wIndex.w = 0;
  phost->Control.setup.b.wLength.w = hci_len;

  USBH_StatusTypeDef status = USBH_CtlReq(phost, hci_buffer, hci_len);

  uint32_t startTimer = phost->Timer;
  do
  {
    status = USBH_CtlReq(phost, hci_buffer, hci_len);
    if (status == USBH_OK)
    {
      break;
    }
    else if (status == USBH_FAIL /*|| status == USBH_STALL*/
        || status == USBH_NOT_SUPPORTED)
    {
      break;
    }
    else
    {
      uint32_t diffTimer = phost->Timer - startTimer;
      if (diffTimer > (0x4000  / 2))
      {
        break;
      }
    }
  } while (1);
}

/**
  * @}
  */
void usb_acl_write(void *p, uint8_t *acl_buffer, uint32_t acl_len)
{
  USBH_HandleTypeDef *phost = p;

  BT_HandleTypeDef *bt_handle = (BT_HandleTypeDef *) phost->pActiveClass->pData;

  USBH_BulkSendData(phost, acl_buffer, acl_len, bt_handle->acl_output_pipe, 1);

  uint32_t startTimer = phost->Timer;
  do
  {
    uint32_t status = USBH_LL_GetURBState(phost, bt_handle->acl_output_pipe);
    if (status == URB_DONE)
    {
      break;
    }
    else if (status == URB_ERROR)
    {
      break;
    }
    else
    {
      uint32_t diffTimer = phost->Timer - startTimer;
      if (diffTimer > (0x4000 / 2))
      {
        break;
      }
    }
  } while (1);
}

/**
  * @}
  */
uint32_t usb_millis(void *p)
{
  USBH_HandleTypeDef *phost = p;
  return phost->Timer;
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

