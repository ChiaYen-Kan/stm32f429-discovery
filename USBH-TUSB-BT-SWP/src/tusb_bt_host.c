/*
 * tusb_bt_host.c
 *
 *  Created on: 2026-06-16
 *      Author: ChiaYen
 */

#include "tusb_bt_host.h"
#include "BTD.h"
#include "BTHID.h"

#if (TUSB_OPT_HOST_ENABLED && CFG_TUH_BT)

/* BT Class Codes */
#define USB_BT_CLASS        0xE0U

// Used to determine if it is a Bluetooth dongle
#define WI_SUBCLASS_RF      0x01 // RF Controller
#define WI_PROTOCOL_BT      0x01 // Bluetooth Programming Interface

static bth_interface_t bth_itf[CFG_TUH_BT];

#ifndef DRIVER_NAME
#if CFG_TUSB_DEBUG >= CFG_TUH_LOG_LEVEL
  #define DRIVER_NAME(_name)    .name = _name,
#else
  #define DRIVER_NAME(_name)
#endif
#endif

usbh_class_driver_t const usbh_bt_driver =
{
  DRIVER_NAME("BT")
  .init       = bth_init,
  .open       = bth_open,
  .set_config = bth_set_config,
  .xfer_cb    = bth_xfer_cb,
  .close      = bth_close
};

/*
 * internal
 */

bth_interface_t *find_new_itf(void)
{
  for (uint8_t i = 0; i < CFG_TUH_BT; i++)
  {
    if (bth_itf[i].dev_addr == 0)
    {
      return &bth_itf[i];
    }
  }

  return NULL;
}

bth_interface_t *get_itf_by_dev_addr(uint8_t dev_addr)
{
  for (uint8_t i = 0; i < CFG_TUH_BT; i++)
  {
    if (bth_itf[i].dev_addr == dev_addr)
    {
      return &bth_itf[i];
    }
  }

  return NULL;
}

bth_interface_t *get_itf_by_idx(uint8_t idx)
{
  if (idx >= CFG_TUH_BT)
  {
    return NULL;
  }

  return &bth_itf[idx];
}

bool tuh_bth_mounted(uint8_t idx)
{
  bth_interface_t *p_bth_itf = get_itf_by_idx(idx);
  TU_VERIFY(p_bth_itf);
  return p_bth_itf->mounted;
}

//--------------------------------------------------------------------+
// USBH API
//--------------------------------------------------------------------+

usbh_class_driver_t const *usbh_app_driver_get_cb(uint8_t *driver_count)
{
  *driver_count = 1;
  return &usbh_bt_driver;
}

bool bth_init(void)
{
  tu_memclr(bth_itf, sizeof(bth_itf));

  return true;
}

uint16_t bth_open(uint8_t rhport, uint8_t dev_addr, tusb_desc_interface_t const *desc_itf, uint16_t max_len)
{
  TU_VERIFY(dev_addr <= CFG_TUH_DEVICE_MAX);

  bool is_bt_itf = desc_itf->bInterfaceClass == USB_BT_CLASS &&
      desc_itf->bInterfaceSubClass == WI_SUBCLASS_RF &&
      desc_itf->bInterfaceProtocol == WI_PROTOCOL_BT;
  if (is_bt_itf == false)
  {
    return false;
  }

  bth_interface_t *p_bth_itf = find_new_itf();
  if (p_bth_itf == NULL)
  {
    return false;
  }

  p_bth_itf->dev_addr = dev_addr;
  p_bth_itf->bInterfaceNumber = desc_itf->bInterfaceNumber;

  p_bth_itf->hci_total_read = 0;
  p_bth_itf->hci_step = HIC_STEP_READ;
  p_bth_itf->hci_poll_timer = 0;
  p_bth_itf->hci_poll_interval = 100;

  p_bth_itf->acl_total_read = 0;
  p_bth_itf->acl_step = ACL_STEP_READ;
  p_bth_itf->acl_poll_interval = 100;

  uint8_t const *p_desc = (uint8_t const *)desc_itf;
  int pos = 0;
  int endpoint = 0;
  while (endpoint < desc_itf->bNumEndpoints && pos < max_len)
  {
    if (tu_desc_type(p_desc) != TUSB_DESC_ENDPOINT)
    {
      pos += tu_desc_len(p_desc);
      p_desc = tu_desc_next(p_desc);
      continue;
    }

    tusb_desc_endpoint_t const *desc_ep = (tusb_desc_endpoint_t const *)p_desc;
    TU_ASSERT(TUSB_DESC_ENDPOINT == desc_ep->bDescriptorType);
    TU_ASSERT(tuh_edpt_open(dev_addr, desc_ep));

    if (desc_ep->bmAttributes.xfer == TUSB_XFER_INTERRUPT)
    {
      p_bth_itf->hci_event_ep_addr = desc_ep->bEndpointAddress;
      p_bth_itf->hci_event_ep_size = tu_edpt_packet_size(desc_ep);
    }
    else if (desc_ep->bmAttributes.xfer == TUSB_XFER_BULK)
    {
      if (tu_edpt_dir(desc_ep->bEndpointAddress) == TUSB_DIR_IN)
      {
        p_bth_itf->acl_input_ep_addr = desc_ep->bEndpointAddress;
        p_bth_itf->acl_input_ep_size = tu_edpt_packet_size(desc_ep);
      }
      else
      {
        p_bth_itf->acl_output_ep_addr = desc_ep->bEndpointAddress;
        p_bth_itf->acl_output_ep_size = tu_edpt_packet_size(desc_ep);
      }
    }

    endpoint++;
    pos += tu_desc_len(p_desc);
    p_desc = tu_desc_next(p_desc);
  }

  return pos;
}

bool bth_set_config(uint8_t dev_addr, uint8_t itf_num)
{
  bth_interface_t *bth_itf = get_itf_by_dev_addr(dev_addr);
  TU_VERIFY(bth_itf != NULL);

  bth_itf->mounted = true;

  usbh_driver_set_config_complete(dev_addr, itf_num);

  btd_initialize(bth_itf);
  bthid_initialize(bth_itf);
  return true;
}

bool bth_xfer_cb(uint8_t dev_addr, uint8_t ep_addr, xfer_result_t result, uint32_t xferred_bytes)
{
  uint8_t const dir = tu_edpt_dir(ep_addr);

  bth_interface_t *bth_itf = get_itf_by_dev_addr(dev_addr);
  TU_VERIFY(bth_itf != NULL);

  if (result != XFER_RESULT_SUCCESS)
  {
    return false;
  }

  if (dir == TUSB_DIR_IN)
  {
    if (ep_addr == bth_itf->hci_event_ep_addr)
    {
      bth_itf->hci_total_read += xferred_bytes;

      if (bth_itf->hci_step == HIC_STEP_WAIT)
      {
        bth_itf->hci_step = HIC_STEP_INCOME;
      }
      else if (bth_itf->hci_step == HIC_STEP_WAIT_MULTI)
      {
        bth_itf->hci_step = HIC_STEP_INCOME_MULTI;
      }
    }
    else if (ep_addr == bth_itf->acl_input_ep_addr)
    {
      if (bth_itf->acl_step == ACL_STEP_WAIT)
      {
        bth_itf->acl_total_read += xferred_bytes;
        bth_itf->acl_step = ACL_STEP_INCOME;
      }
    }
  }
//  else if (dir == TUSB_DIR_OUT)
//  {
//    if (ep_addr == bth_itf->acl_output_ep_addr)
//    {
//      acl_complete = true;
//    }
//  }

  return true;
}

void bth_close(uint8_t dev_addr)
{
  tu_memclr(bth_itf, sizeof(bth_itf));
}

/*
 *
 */
bool bth_task()
{
  const uint8_t idx = 0;

  if (!tuh_bth_mounted(idx))
  {
    return false;
  }

  bth_interface_t *bth_itf = get_itf_by_idx(idx);

  // HCI Read
  switch (bth_itf->hci_step)
  {
    case HIC_STEP_READ:
    {
      memset(bth_itf->hci_event_ep_buf, 0, 255);
      bth_itf->hci_total_read = 0;

      TU_VERIFY(usbh_edpt_claim(bth_itf->dev_addr, bth_itf->hci_event_ep_addr));
      if (!usbh_edpt_xfer(bth_itf->dev_addr, bth_itf->hci_event_ep_addr, bth_itf->hci_event_ep_buf, bth_itf->hci_event_ep_size))
      {
        TU_VERIFY(usbh_edpt_release(bth_itf->dev_addr, bth_itf->hci_event_ep_addr));
        break;
      }

      bth_itf->hci_step = HIC_STEP_WAIT;
      bth_itf->hci_poll_timer = tusb_time_millis_api();

      break;
    }
    case HIC_STEP_WAIT:
    {
      // TODO: How to handle read timeout??
      /*if ((tusb_time_millis_api() - bth_itf->hci_poll_timer) >= bth_itf->hci_poll_interval)
      {
        TU_VERIFY(usbh_edpt_release(bth_itf->dev_addr, bth_itf->hci_event_ep_addr));
        bth_itf->hci_step = HIC_STEP_READ;
      }*/

      break;
    }
    case HIC_STEP_INCOME:
    {
      uint8_t total = bth_itf->hci_event_ep_buf[1];
      if (bth_itf->hci_total_read - 2 == total)
      {
        btd_hci_event_task(bth_itf, bth_itf->hci_event_ep_buf, bth_itf->hci_total_read);
        bth_itf->hci_step = HIC_STEP_READ;
      }
      else
      {
        TU_VERIFY(usbh_edpt_claim(bth_itf->dev_addr, bth_itf->hci_event_ep_addr));
        if (!usbh_edpt_xfer(bth_itf->dev_addr, bth_itf->hci_event_ep_addr, bth_itf->hci_event_ep_buf + bth_itf->hci_total_read, bth_itf->hci_event_ep_size))
        {
          TU_VERIFY(usbh_edpt_release(bth_itf->dev_addr, bth_itf->hci_event_ep_addr));
          return false;
        }

        bth_itf->hci_step = HIC_STEP_WAIT_MULTI;
        bth_itf->hci_poll_timer = tusb_time_millis_api();
      }

      break;
    }
    case HIC_STEP_WAIT_MULTI:
    {
      // TODO: how to handle read timeout??
      /*if ((tusb_time_millis_api() - bth_itf->hci_poll_timer) >= bth_itf->hci_poll_interval)
      {
        TU_VERIFY(usbh_edpt_release(bth_itf->dev_addr, bth_itf->hci_event_ep_addr));
        bth_itf->hci_step = HIC_STEP_READ;
      }*/

      break;
    }
    case HIC_STEP_INCOME_MULTI:
    {
      uint8_t total = bth_itf->hci_event_ep_buf[1];
      if (bth_itf->hci_total_read - 2 == total)
      {
        btd_hci_event_task(bth_itf, bth_itf->hci_event_ep_buf, bth_itf->hci_total_read);
        bth_itf->hci_step = HIC_STEP_READ;
      }
      else
      {
        usbh_edpt_claim(bth_itf->dev_addr, bth_itf->hci_event_ep_addr);
        if (!usbh_edpt_xfer(bth_itf->dev_addr, bth_itf->hci_event_ep_addr, bth_itf->hci_event_ep_buf + bth_itf->hci_total_read, bth_itf->hci_event_ep_size))
        {
          TU_VERIFY(usbh_edpt_release(bth_itf->dev_addr, bth_itf->hci_event_ep_addr));
          return false;
        }

        bth_itf->hci_step = HIC_STEP_WAIT_MULTI;
        bth_itf->hci_poll_timer = tusb_time_millis_api();
      }

      break;
    }
  }

  btd_hci_task(bth_itf);

  uint32_t xfercount = 0;

  // ACL read
  switch (bth_itf->acl_step)
  {
    case ACL_STEP_READ:
    {
      memset(bth_itf->acl_input_ep_buf, 0, 255);
      bth_itf->acl_total_read = 0;

      tuh_xfer_t xfer = {
        .daddr       = bth_itf->dev_addr,
        .ep_addr     = bth_itf->acl_input_ep_addr,
        .buflen      = 255,
        .buffer      = bth_itf->acl_input_ep_buf,
      };
      tuh_edpt_xfer(&xfer);

      bth_itf->acl_step = ACL_STEP_WAIT;
      bth_itf->acl_poll_timer = tusb_time_millis_api();
      break;
    }
    case ACL_STEP_WAIT:
    {
      // TODO: how to handle read timeout??
      /*if ((tusb_time_millis_api() - bth_itf->acl_poll_timer) >= bth_itf->acl_poll_interval)
      {
        TU_VERIFY(usbh_edpt_release(bth_itf->dev_addr, bth_itf->acl_input_ep_addr));
        bth_itf->acl_step = ACL_STEP_READ;
      }*/
      break;
    }
    case ACL_STEP_INCOME:
    {
      uint32_t total = (uint32_t)bth_itf->acl_input_ep_buf[2] | (uint32_t)bth_itf->acl_input_ep_buf[3] << 8;

      if (total == bth_itf->acl_total_read - 4)
      {
        xfercount = bth_itf->acl_total_read;
        bth_itf->acl_step = ACL_STEP_READ;
      }
      else
      {
        // TODO :  handle
      }

      break;
    }
  }

  btd_acl_event_task(bth_itf, bth_itf->acl_input_ep_buf, xfercount);

  return true;
}

void usb_hci_write(void *p, uint8_t *hci_buffer, uint32_t hci_len)
{
  bth_interface_t *bth_itf = p;

  tusb_control_request_t const request =
  {
    .bmRequestType_bit = {
      .recipient = TUSB_REQ_RCPT_DEVICE,
      .type      = TUSB_REQ_TYPE_CLASS,
      .direction = TUSB_DIR_OUT
    },
    .bRequest = 0,
    .wValue   = 0,
    .wIndex   = 0,
    .wLength  = hci_len
  };

//  volatile bool isComplete = false;
  tuh_xfer_t xfer =
  {
    .daddr       = bth_itf->dev_addr, // from mount callback
    .ep_addr     = 0, // control endpoint has address 0
    .setup       = &request,
    .buffer      = hci_buffer,
    .complete_cb = NULL,
    .user_data   = 0,

//    .complete_cb = hci_command_complete_cb, // your callback to be called when the request completes
//    .user_data   = (uintptr_t)&isComplete,
  };

  tuh_control_xfer(&xfer);
}

void usb_acl_write_transfer_cb(tuh_xfer_t *transfer)
{
  bool *complete = (bool *)transfer->user_data;

  *complete = true;
}

void usb_acl_write(void *p, uint8_t *acl_buffer, uint32_t acl_len)
{
  bth_interface_t *bth_itf = p;

  bool complete = false;
  tuh_xfer_t transfer =
  {
    .daddr = bth_itf->dev_addr,
    .ep_addr = bth_itf->acl_output_ep_addr,
    .buffer = acl_buffer,
    .buflen = acl_len,
    .complete_cb = usb_acl_write_transfer_cb,
    .user_data = (uintptr_t)&complete
  };

  tuh_edpt_xfer(&transfer);

  while (complete == false)
  {
    tuh_task();
  }
}

uint32_t usb_millis(void *p)
{
  return tusb_time_millis_api();
}
#endif
