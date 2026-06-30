/*
 * BTHID.h
 *
 *  Created on: 2026-06-23
 *      Author: ChiaYen
 */

#ifndef BTHID_H_
#define BTHID_H_

#include <stdint.h>
#include "BluetoothService.h"

#define KEYBOARD_PARSER_ID      0
#define MOUSE_PARSER_ID         1
#define NUM_PARSERS             2

/* Protocol Selection */
#define USB_HID_BOOT_PROTOCOL                   0x00
#define HID_RPT_PROTOCOL                        0x01

typedef struct
{
  BluetoothService_t service;

  /** True if a device is connected */
  bool connected;

  /** L2CAP source CID for HID_Control */
  uint8_t control_scid[2];

  /** L2CAP source CID for HID_Interrupt */
  uint8_t interrupt_scid[2];

  uint8_t l2cap_sdp_state;
  uint8_t sdp_scid[2]; // L2CAP source CID for SDP

  bool activeConnection; // Used to indicate if it already has established a connection
  bool SDPConnected;

  /* Variables used for L2CAP communication */
  uint8_t control_dcid[2]; // L2CAP device CID for HID_Control - Always 0x0070
  uint8_t interrupt_dcid[2]; // L2CAP device CID for HID_Interrupt - Always 0x0071
  uint8_t sdp_dcid[2];
  uint8_t l2cap_state;

  uint32_t lastBtDataInputIntMillis; // Variable used to store the millis value of the last Bluetooth DATA input report received on the interrupt channel

  uint8_t protocolMode;

} BTHID_HandleTypeDef;

void bthid_initialize(void *phost);

void bthid_acldata(void *phost, uint8_t *l2capinbuf);
void bthid_run(void *phost);

#endif /* BTHID_H_ */
