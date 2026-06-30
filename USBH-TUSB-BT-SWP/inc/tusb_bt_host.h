/*
 * tusb_bt_host.h
 *
 *  Created on: 2026-06-16
 *      Author: ChiaYen
 */

#ifndef TUSB_BT_HOST_H_
#define TUSB_BT_HOST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "tusb.h"
#include "host/usbh_pvt.h"

enum HCI_EVENT_STEP
{
  HIC_STEP_READ,
  HIC_STEP_WAIT,
  HIC_STEP_INCOME,
  HIC_STEP_READ_MULTI,
  HIC_STEP_WAIT_MULTI,
  HIC_STEP_INCOME_MULTI,
};

enum ACL_INPUT_STEP
{
  ACL_STEP_READ,
  ACL_STEP_WAIT,
  ACL_STEP_INCOME,
  ACL_STEP_READ_MULTI,
  ACL_STEP_WAIT_MULTI,
};

typedef struct
{
  uint8_t dev_addr;
  uint8_t bInterfaceNumber;

  bool mounted; // Enumeration is complete

  uint8_t hci_event_ep_addr;
  uint8_t hci_event_ep_size;
  uint8_t hci_event_ep_buf[255];
  uint32_t hci_total_read;
  uint8_t hci_step;
  uint32_t hci_poll_timer;
  uint32_t hci_poll_interval;

  uint8_t acl_input_ep_addr;
  uint8_t acl_input_ep_size;
  uint8_t acl_input_ep_buf[255];
  uint32_t acl_total_read;
  uint8_t acl_step;
  uint32_t acl_poll_timer;
  uint32_t acl_poll_interval;

  uint8_t acl_output_ep_addr;
  uint8_t acl_output_ep_size;

} bth_interface_t;

bool bth_init(void);
uint16_t bth_open(uint8_t rhport, uint8_t dev_addr, tusb_desc_interface_t const *desc_itf, uint16_t max_len);
bool bth_set_config(uint8_t dev_addr, uint8_t itf_num);
bool bth_xfer_cb(uint8_t dev_addr, uint8_t ep_addr, xfer_result_t result, uint32_t xferred_bytes);
void bth_close(uint8_t dev_addr);

bool bth_task();

#ifdef __cplusplus
}
#endif


#endif /* TUSB_BT_HOST_H_ */
