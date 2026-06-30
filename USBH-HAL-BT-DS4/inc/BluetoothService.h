/*
 * BluetoothService.h
 *
 *  Created on: 2026-06-23
 *      Author: ChiaYen
 */

#ifndef BLUETOOTHSERVICE_H_
#define BLUETOOTHSERVICE_H_

#include <stdint.h>

typedef struct
{
  /** The HCI Handle for the connection. */
  uint16_t hci_handle;

  /** L2CAP flags of received Bluetooth events. */
  uint32_t l2cap_event_flag;

  /** Identifier for L2CAP commands. */
  uint8_t identifier;

} BluetoothService_t;

#endif /* BLUETOOTHSERVICE_H_ */
