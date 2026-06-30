/*
 * BTD.c
 *
 *  Created on: 2026-06-22
 *      Author: ChiaYen
 */

#include <stddef.h>
#include <string.h>
#include "BTD.h"
#include "BTHID.h"

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

extern void usb_hci_write(void *p, uint8_t *hci_buffer, uint32_t hci_len);
extern void usb_acl_write(void *p, uint8_t *acl_buffer, uint32_t acl_len);
extern uint32_t usb_millis(void *p);

BTD_HandleTypeDef btd;

/*
 * HCI Commands
 */

void btd_hci_command(void *phost, uint8_t *data, uint16_t nbytes)
{
  hci_clear_flag(btd.hci_event_flag, HCI_FLAG_CMD_COMPLETE);

  usb_hci_write(phost, data, nbytes);
}

void btd_hci_reset(void *phost)
{
  btd.hci_event_flag = 0; // Clear all the flags

  uint8_t hcibuf[3];
  hcibuf[0] = 0x03; // HCI OCF = 3
  hcibuf[1] = 0x03 << 2; // HCI OGF = 3
  hcibuf[2] = 0x00;

  btd_hci_command(phost, hcibuf, 3);
}

void btd_hci_pin_code_request_reply(void *phost)
{
  uint8_t hcibuf[26];
  hcibuf[0] = 0x0D; // HCI OCF = 0D
  hcibuf[1] = 0x01 << 2; // HCI OGF = 1
  hcibuf[2] = 0x17; // parameter length 23
  hcibuf[3] = btd.disc_bdaddr[0]; // 6 octet bdaddr
  hcibuf[4] = btd.disc_bdaddr[1];
  hcibuf[5] = btd.disc_bdaddr[2];
  hcibuf[6] = btd.disc_bdaddr[3];
  hcibuf[7] = btd.disc_bdaddr[4];
  hcibuf[8] = btd.disc_bdaddr[5];

  if (btd.pairWithWii)
  {
    hcibuf[9] = 6; // Pin length is the length of the Bluetooth address
    if (btd.pairWiiUsingSync)
    {
//      Notify(PSTR("\r\nPairing with Wii controller via SYNC"), 0x80);

      for (uint8_t i = 0; i < 6; i++)
        hcibuf[10 + i] = btd.my_bdaddr[i]; // The pin is the Bluetooth dongles Bluetooth address backwards
    }
    else
    {
      for (uint8_t i = 0; i < 6; i++)
        hcibuf[10 + i] = btd.disc_bdaddr[i]; // The pin is the Wiimote's Bluetooth address backwards
    }

    for (uint8_t i = 16; i < 26; i++)
      hcibuf[i] = 0x00; // The rest should be 0
  }
  else
  {
    hcibuf[9] = strlen(btd.btdPin); // Length of pin
    uint8_t i;
    for (i = 0; i < strlen(btd.btdPin); i++) // The maximum size of the pin is 16
      hcibuf[i + 10] = btd.btdPin[i];
    for (; i < 16; i++)
      hcibuf[i + 10] = 0x00; // The rest should be 0
  }

  btd_hci_command(phost, hcibuf, 26);
}

void btd_hci_pin_code_negative_request_reply(void *phost)
{
  uint8_t hcibuf[9];
  hcibuf[0] = 0x0E; // HCI OCF = 0E
  hcibuf[1] = 0x01 << 2; // HCI OGF = 1
  hcibuf[2] = 0x06; // parameter length 6
  hcibuf[3] = btd.disc_bdaddr[0]; // 6 octet bdaddr
  hcibuf[4] = btd.disc_bdaddr[1];
  hcibuf[5] = btd.disc_bdaddr[2];
  hcibuf[6] = btd.disc_bdaddr[3];
  hcibuf[7] = btd.disc_bdaddr[4];
  hcibuf[8] = btd.disc_bdaddr[5];

  btd_hci_command(phost, hcibuf, 9);
}

void btd_hci_link_key_request_negative_reply(void *phost)
{
  uint8_t hcibuf[9];
  hcibuf[0] = 0x0C; // HCI OCF = 0C
  hcibuf[1] = 0x01 << 2; // HCI OGF = 1
  hcibuf[2] = 0x06; // parameter length 6
  hcibuf[3] = btd.disc_bdaddr[0]; // 6 octet bdaddr
  hcibuf[4] = btd.disc_bdaddr[1];
  hcibuf[5] = btd.disc_bdaddr[2];
  hcibuf[6] = btd.disc_bdaddr[3];
  hcibuf[7] = btd.disc_bdaddr[4];
  hcibuf[8] = btd.disc_bdaddr[5];

  btd_hci_command(phost, hcibuf, 9);
}

void btd_hci_disconnect(void *phost, uint16_t handle) // This is called by the different services
{
  hci_clear_flag(btd.hci_event_flag, HCI_FLAG_DISCONNECT_COMPLETE);

  uint8_t hcibuf[6];
  hcibuf[0] = 0x06; // HCI OCF = 6
  hcibuf[1] = 0x01 << 2;// HCI OGF = 1
  hcibuf[2] = 0x03;// parameter length = 3
  hcibuf[3] = (uint8_t)(handle & 0xFF);//connection handle - low byte
  hcibuf[4] = (uint8_t)((handle >> 8) & 0x0F);//connection handle - high byte
  hcibuf[5] = 0x13;// reason

  btd_hci_command(phost, hcibuf, 6);
}

void btd_hci_io_capability_request_reply(void *phost)
{
  uint8_t hcibuf[12];
  hcibuf[0] = 0x2B; // HCI OCF = 2B
  hcibuf[1] = 0x01 << 2;// HCI OGF = 1
  hcibuf[2] = 0x09;
  hcibuf[3] = btd.disc_bdaddr[0];// 6 octet bdaddr
  hcibuf[4] = btd.disc_bdaddr[1];
  hcibuf[5] = btd.disc_bdaddr[2];
  hcibuf[6] = btd.disc_bdaddr[3];
  hcibuf[7] = btd.disc_bdaddr[4];
  hcibuf[8] = btd.disc_bdaddr[5];
  hcibuf[9] = 0x03;// NoInputNoOutput
  hcibuf[10] = 0x00;// OOB authentication data not present
  hcibuf[11] = 0x00;// MITM Protection Not Required ??No Bonding. Numeric comparison with automatic accept allowed

  btd_hci_command(phost, hcibuf, 12);
}

void btd_hci_user_confirmation_request_reply(void *phost)
{
  uint8_t hcibuf[9];
  hcibuf[0] = 0x2C; // HCI OCF = 2C
  hcibuf[1] = 0x01 << 2; // HCI OGF = 1
  hcibuf[2] = 0x06; // parameter length 6
  hcibuf[3] = btd.disc_bdaddr[0]; // 6 octet bdaddr
  hcibuf[4] = btd.disc_bdaddr[1];
  hcibuf[5] = btd.disc_bdaddr[2];
  hcibuf[6] = btd.disc_bdaddr[3];
  hcibuf[7] = btd.disc_bdaddr[4];
  hcibuf[8] = btd.disc_bdaddr[5];

  btd_hci_command(phost, hcibuf, 9);
}

void btd_hci_write_class_of_device(void *phost) // See http://bluetooth-pentest.narod.ru/software/bluetooth_class_of_device-service_generator.html
{
  uint8_t hcibuf[6];
  hcibuf[0] = 0x24; // HCI OCF = 24
  hcibuf[1] = 0x03 << 2;// HCI OGF = 3
  hcibuf[2] = 0x03;// parameter length = 3
  hcibuf[3] = 0x04;// Robot
  hcibuf[4] = 0x08;// Toy
  hcibuf[5] = 0x00;

  btd_hci_command(phost, hcibuf, 6);
}

void btd_hci_read_bdaddr(void *phost)
{
  hci_clear_flag(btd.hci_event_flag, HCI_FLAG_READ_BDADDR);

  uint8_t hcibuf[3];
  hcibuf[0] = 0x09; // HCI OCF = 9
  hcibuf[1] = 0x04 << 2; // HCI OGF = 4
  hcibuf[2] = 0x00;

  btd_hci_command(phost, hcibuf, 3);
}

void btd_hci_read_local_version_information(void *phost)
{
  hci_clear_flag(btd.hci_event_flag, HCI_FLAG_READ_VERSION);

  uint8_t hcibuf[3];
  hcibuf[0] = 0x01; // HCI OCF = 1
  hcibuf[1] = 0x04 << 2; // HCI OGF = 4
  hcibuf[2] = 0x00;

  btd_hci_command(phost, hcibuf, 3);
}

void btd_hci_write_local_name(void *phost, const char * name)
{
  uint8_t hcibuf[248 + 3];
  hcibuf[0] = 0x13; // HCI OCF = 13
  hcibuf[1] = 0x03 << 2; // HCI OGF = 3
  hcibuf[2] = strlen(name) + 1; // parameter length = the length of the string + end byte
  uint8_t i;
  for(i = 0; i < strlen(name); i++)
  {
    hcibuf[i + 3] = name[i];
  }
  hcibuf[i + 3] = 0x00; // End of string

  btd_hci_command(phost, hcibuf, 4 + strlen(name));
}

void btd_hci_read_local_extended_features(void *phost, uint8_t page_number)
{
  hci_clear_flag(btd.hci_event_flag, HCI_FLAG_LOCAL_EXTENDED_FEATURES);

  uint8_t hcibuf[4];
  hcibuf[0] = 0x04; // HCI OCF = 4
  hcibuf[1] = 0x04 << 2; // HCI OGF = 4
  hcibuf[2] = 0x01; // parameter length = 1
  hcibuf[3] = page_number;

  btd_hci_command(phost, hcibuf, 4);
}

void btd_hci_write_simple_pairing_mode(void *phost, bool enable)
{
  uint8_t hcibuf[4];
  hcibuf[0] = 0x56; // HCI OCF = 56
  hcibuf[1] = 0x03 << 2; // HCI OGF = 3
  hcibuf[2] = 1; // parameter length = 1
  hcibuf[3] = enable ? 1 : 0;

  btd_hci_command(phost, hcibuf, 4);
}

void btd_hci_set_event_mask(void *phost)
{
  uint8_t hcibuf[11];
  hcibuf[0] = 0x01; // HCI OCF = 01
  hcibuf[1] = 0x03 << 2;// HCI OGF = 3
  hcibuf[2] = 0x08;
  // The first 6 bytes are the default of 1FFF FFFF FFFF
  // However we need to set bits 48-55 for simple pairing to work
  hcibuf[3] = 0xFF;
  hcibuf[4] = 0xFF;
  hcibuf[5] = 0xFF;
  hcibuf[6] = 0xFF;
  hcibuf[7] = 0xFF;
  hcibuf[8] = 0x1F;
  hcibuf[9] = 0xFF;// Enable bits 48-55 used for simple pairing
  hcibuf[10] = 0x00;

  btd_hci_command(phost, hcibuf, 11);
}

void btd_hci_inquiry(void *phost)
{
  hci_clear_flag(btd.hci_event_flag, HCI_FLAG_DEVICE_FOUND);
  uint8_t hcibuf[8];
  hcibuf[0] = 0x01;
  hcibuf[1] = 0x01 << 2; // HCI OGF = 1
  hcibuf[2] = 0x05; // Parameter Total Length = 5
  hcibuf[3] = 0x33; // LAP: Genera/Unlimited Inquiry Access Code (GIAC = 0x9E8B33) - see https://www.bluetooth.org/Technical/AssignedNumbers/baseband.htm
  hcibuf[4] = 0x8B;
  hcibuf[5] = 0x9E;
  hcibuf[6] = 0x30; // Inquiry time = 61.44 sec (maximum)
  hcibuf[7] = 0x0A; // 10 number of responses

  btd_hci_command(phost, hcibuf, 8);
}

void btd_hci_inquiry_cancel(void *phost)
{
  uint8_t hcibuf[3];
  hcibuf[0] = 0x02;
  hcibuf[1] = 0x01 << 2; // HCI OGF = 1
  hcibuf[2] = 0x00; // Parameter Total Length = 0

  btd_hci_command(phost, hcibuf, 3);
}

void btd_hci_remote_name(void *phost)
{
  hci_clear_flag(btd.hci_event_flag, HCI_FLAG_REMOTE_NAME_COMPLETE);

  uint8_t hcibuf[13];
  hcibuf[0] = 0x19; // HCI OCF = 19
  hcibuf[1] = 0x01 << 2; // HCI OGF = 1
  hcibuf[2] = 0x0A; // parameter length = 10
  hcibuf[3] = btd.disc_bdaddr[0]; // 6 octet bdaddr
  hcibuf[4] = btd.disc_bdaddr[1];
  hcibuf[5] = btd.disc_bdaddr[2];
  hcibuf[6] = btd.disc_bdaddr[3];
  hcibuf[7] = btd.disc_bdaddr[4];
  hcibuf[8] = btd.disc_bdaddr[5];
  hcibuf[9] = 0x01; // Page Scan Repetition Mode
  hcibuf[10] = 0x00; // Reserved
  hcibuf[11] = 0x00; // Clock offset - low byte
  hcibuf[12] = 0x00; // Clock offset - high byte

  btd_hci_command(phost, hcibuf, 13);
}

void btd_hci_connect_with_addr(void *phost, uint8_t *bdaddr)
{
  hci_clear_flag(btd.hci_event_flag, HCI_FLAG_CONNECT_COMPLETE | HCI_FLAG_CONNECT_EVENT);

  uint8_t hcibuf[16];
  hcibuf[0] = 0x05; // HCI OCF = 5
  hcibuf[1] = 0x01 << 2; // HCI OGF = 1
  hcibuf[2] = 0x0D; // parameter Total Length = 13
  hcibuf[3] = bdaddr[0]; // 6 octet bdaddr (LSB)
  hcibuf[4] = bdaddr[1];
  hcibuf[5] = bdaddr[2];
  hcibuf[6] = bdaddr[3];
  hcibuf[7] = bdaddr[4];
  hcibuf[8] = bdaddr[5];
  hcibuf[9] = 0x18; // DM1 or DH1 may be used
  hcibuf[10] = 0xCC; // DM3, DH3, DM5, DH5 may be used
  hcibuf[11] = 0x01; // Page repetition mode R1
  hcibuf[12] = 0x00; // Reserved
  hcibuf[13] = 0x00; // Clock offset
  hcibuf[14] = 0x00; // Invalid clock offset
  hcibuf[15] = 0x00; // Do not allow role switch

  btd_hci_command(phost, hcibuf, 16);
}

void btd_hci_connect(void *phost)
{
  btd_hci_connect_with_addr(phost, btd.disc_bdaddr); // Use last discovered device
}

void btd_hci_authentication_request(void *phost)
{
  uint8_t hcibuf[5];
  hcibuf[0] = 0x11; // HCI OCF = 11
  hcibuf[1] = 0x01 << 2; // HCI OGF = 1
  hcibuf[2] = 0x02; // parameter length = 2
  hcibuf[3] = (uint8_t) (btd.hci_handle & 0xFF); //connection handle - low byte
  hcibuf[4] = (uint8_t) ((btd.hci_handle >> 8) & 0x0F); //connection handle - high byte

  btd_hci_command(phost, hcibuf, 5);
}

void btd_hci_write_scan_enable(void *phost)
{
  hci_clear_flag(btd.hci_event_flag, HCI_FLAG_INCOMING_REQUEST);

  uint8_t hcibuf[4];
  hcibuf[0] = 0x1A; // HCI OCF = 1A
  hcibuf[1] = 0x03 << 2; // HCI OGF = 3
  hcibuf[2] = 0x01; // parameter length = 1
  if (btd.btdName != NULL)
    hcibuf[3] = 0x03; // Inquiry Scan enabled. Page Scan enabled.
  else
    hcibuf[3] = 0x02; // Inquiry Scan disabled. Page Scan enabled.

  btd_hci_command(phost, hcibuf, 4);
}

void btd_hci_accept_connection(void *phost)
{
  hci_clear_flag(btd.hci_event_flag, HCI_FLAG_CONNECT_COMPLETE);

  uint8_t hcibuf[10];
  hcibuf[0] = 0x09; // HCI OCF = 9
  hcibuf[1] = 0x01 << 2; // HCI OGF = 1
  hcibuf[2] = 0x07; // parameter length 7
  hcibuf[3] = btd.disc_bdaddr[0]; // 6 octet bdaddr
  hcibuf[4] = btd.disc_bdaddr[1];
  hcibuf[5] = btd.disc_bdaddr[2];
  hcibuf[6] = btd.disc_bdaddr[3];
  hcibuf[7] = btd.disc_bdaddr[4];
  hcibuf[8] = btd.disc_bdaddr[5];
  hcibuf[9] = 0x00; // Switch role to master

  btd_hci_command(phost, hcibuf, 10);
}

/*******************************************************************
 *                                                                 *
 *                        HCI ACL Data Packet                      *
 *                                                                 *
 *   buf[0]          buf[1]          buf[2]          buf[3]
 *   0       4       8    11 12      16              24            31 MSB
 *  .-+-+-+-+-+-+-+-|-+-+-+-|-+-|-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-.
 *  |      HCI Handle       |PB |BC |       Data Total Length       |   HCI ACL Data Packet
 *  .-+-+-+-+-+-+-+-|-+-+-+-|-+-|-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-.
 *
 *   buf[4]          buf[5]          buf[6]          buf[7]
 *   0               8               16                            31 MSB
 *  .-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-.
 *  |            Length             |          Channel ID           |   Basic L2CAP header
 *  .-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-.
 *
 *   buf[8]          buf[9]          buf[10]         buf[11]
 *   0               8               16                            31 MSB
 *  .-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-.
 *  |     Code      |  Identifier   |            Length             |   Control frame (C-frame)
 *  .-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-.   (signaling packet format)
 */
/************************************************************/
/*                    L2CAP Commands                        */
/************************************************************/

void btd_l2cap_command(void *phost, uint16_t handle, uint8_t *data, uint8_t nbytes, uint8_t channelLow /*= 0x01*/, uint8_t channelHigh /*= 0x00*/)
{
  uint8_t buf[8 + 256];
  buf[0] = (uint8_t) (handle & 0xff); // HCI handle with PB,BC flag
  buf[1] = (uint8_t) (((handle >> 8) & 0x0f) | 0x20);
  buf[2] = (uint8_t) ((4 + nbytes) & 0xff); // HCI ACL total data length
  buf[3] = (uint8_t) ((4 + nbytes) >> 8);
  buf[4] = (uint8_t) (nbytes & 0xff); // L2CAP header: Length
  buf[5] = (uint8_t) (nbytes >> 8);
  buf[6] = channelLow;
  buf[7] = channelHigh;

  for (uint16_t i = 0; i < nbytes; i++) // L2CAP C-frame
    buf[8 + i] = data[i];

  usb_acl_write(phost, buf, 8 + nbytes);
}

void btd_l2cap_config_request(void *phost, uint16_t handle, uint8_t rxid,  uint8_t *dcid)
{
  uint8_t l2capoutbuf[12];
  l2capoutbuf[0] = L2CAP_CMD_CONFIG_REQUEST; // Code
  l2capoutbuf[1] = rxid; // Identifier
  l2capoutbuf[2] = 0x08; // Length
  l2capoutbuf[3] = 0x00;
  l2capoutbuf[4] = dcid[0]; // Destination CID
  l2capoutbuf[5] = dcid[1];
  l2capoutbuf[6] = 0x00; // Flags
  l2capoutbuf[7] = 0x00;
  l2capoutbuf[8] = 0x01; // Config Opt: type = MTU (Maximum Transmission Unit) - Hint
  l2capoutbuf[9] = 0x02; // Config Opt: length
  l2capoutbuf[10] = 0xFF; // MTU
  l2capoutbuf[11] = 0xFF;

  btd_l2cap_command(phost, handle, l2capoutbuf, 12, 0x01, 0x00);
}

void btd_l2cap_disconnection_response(void *phost, uint16_t handle, uint8_t rxid, uint8_t *dcid, uint8_t *scid)
{
  uint8_t l2capoutbuf[8];
  l2capoutbuf[0] = L2CAP_CMD_DISCONNECT_RESPONSE; // Code
  l2capoutbuf[1] = rxid; // Identifier
  l2capoutbuf[2] = 0x04; // Length
  l2capoutbuf[3] = 0x00;
  l2capoutbuf[4] = dcid[0];
  l2capoutbuf[5] = dcid[1];
  l2capoutbuf[6] = scid[0];
  l2capoutbuf[7] = scid[1];

  btd_l2cap_command(phost, handle, l2capoutbuf, 8, 0x01, 0x00);
}

void btd_l2cap_information_response(void *phost, uint16_t handle, uint8_t rxid, uint8_t infoTypeLow, uint8_t infoTypeHigh)
{
  uint8_t l2capoutbuf[12];
  l2capoutbuf[0] = L2CAP_CMD_INFORMATION_RESPONSE; // Code
  l2capoutbuf[1] = rxid; // Identifier
  l2capoutbuf[2] = 0x08; // Length
  l2capoutbuf[3] = 0x00;
  l2capoutbuf[4] = infoTypeLow;
  l2capoutbuf[5] = infoTypeHigh;
  l2capoutbuf[6] = 0x00; // Result = success
  l2capoutbuf[7] = 0x00; // Result = success
  l2capoutbuf[8] = 0x00;
  l2capoutbuf[9] = 0x00;
  l2capoutbuf[10] = 0x00;
  l2capoutbuf[11] = 0x00;

  btd_l2cap_command(phost, handle, l2capoutbuf, 12, 0x01, 0x00);
}

void btd_l2cap_connection_request(void *phost, uint16_t handle, uint8_t rxid, uint8_t *scid, uint16_t psm)
{
  uint8_t l2capoutbuf[8];
  l2capoutbuf[0] = L2CAP_CMD_CONNECTION_REQUEST; // Code
  l2capoutbuf[1] = rxid; // Identifier
  l2capoutbuf[2] = 0x04; // Length
  l2capoutbuf[3] = 0x00;
  l2capoutbuf[4] = (uint8_t) (psm & 0xff); // PSM
  l2capoutbuf[5] = (uint8_t) (psm >> 8);
  l2capoutbuf[6] = scid[0]; // Source CID
  l2capoutbuf[7] = scid[1];

  btd_l2cap_command(phost, handle, l2capoutbuf, 8, 0x01, 0x00);
}

void btd_l2cap_connection_response(void *phost, uint16_t handle, uint8_t rxid, uint8_t *dcid, uint8_t *scid, uint8_t result)
{
  uint8_t l2capoutbuf[12];
  l2capoutbuf[0] = L2CAP_CMD_CONNECTION_RESPONSE; // Code
  l2capoutbuf[1] = rxid; // Identifier
  l2capoutbuf[2] = 0x08; // Length
  l2capoutbuf[3] = 0x00;
  l2capoutbuf[4] = dcid[0]; // Destination CID
  l2capoutbuf[5] = dcid[1];
  l2capoutbuf[6] = scid[0]; // Source CID
  l2capoutbuf[7] = scid[1];
  l2capoutbuf[8] = result; // Result: Pending or Success
  l2capoutbuf[9] = 0x00;
  l2capoutbuf[10] = 0x00; // No further information
  l2capoutbuf[11] = 0x00;

  btd_l2cap_command(phost, handle, l2capoutbuf, 12, 0x01, 0x00);
}

void btd_l2cap_disconnection_request(void *phost, uint16_t handle, uint8_t rxid, uint8_t *dcid, uint8_t *scid)
{
  uint8_t l2capoutbuf[8];
  l2capoutbuf[0] = L2CAP_CMD_DISCONNECT_REQUEST; // Code
  l2capoutbuf[1] = rxid; // Identifier
  l2capoutbuf[2] = 0x04; // Length
  l2capoutbuf[3] = 0x00;
  l2capoutbuf[4] = dcid[0];
  l2capoutbuf[5] = dcid[1];
  l2capoutbuf[6] = scid[0];
  l2capoutbuf[7] = scid[1];

  btd_l2cap_command(phost, handle, l2capoutbuf, 8, 0x01, 0x00);
}

void btd_l2cap_config_response(void *phost, uint16_t handle, uint8_t rxid, uint8_t *scid)
{
  uint8_t l2capoutbuf[14];
  l2capoutbuf[0] = L2CAP_CMD_CONFIG_RESPONSE; // Code
  l2capoutbuf[1] = rxid; // Identifier
  l2capoutbuf[2] = 0x0A; // Length
  l2capoutbuf[3] = 0x00;
  l2capoutbuf[4] = scid[0]; // Source CID
  l2capoutbuf[5] = scid[1];
  l2capoutbuf[6] = 0x00; // Flag
  l2capoutbuf[7] = 0x00;
  l2capoutbuf[8] = 0x00; // Result
  l2capoutbuf[9] = 0x00;
  l2capoutbuf[10] = 0x01; // Config
  l2capoutbuf[11] = 0x02;
  l2capoutbuf[12] = 0xA0;
  l2capoutbuf[13] = 0x02;

  btd_l2cap_command(phost, handle, l2capoutbuf, 14, 0x01, 0x00);
}

/*
 * BTD class
 */

void btd_initialize(void *phost)
{
  memset(&btd, 0, sizeof(btd));

  /*
   * Consturct
   */
  btd.connectToWii = false;
  btd.pairWithWii = false;
  btd.connectToHIDDevice = false;
#if 1
  btd.pairWithHIDDevice = false;
#else
  btd.pairWithHIDDevice = true;
#endif
  btd.useSimplePairing = false;

  btd.qNextPollTime = 0; // Reset NextPollTime

  btd.simple_pairing_supported = false;

//  btd.bPollEnable(false);

  //
  for(uint32_t i = 0; i < BTD_NUM_SERVICES; i++)
  {
//    if (btService[i])
//      btService[i]->Reset(); // Reset all Bluetooth services
  }

  btd.connectToWii = false;
  btd.incomingWii = false;
  btd.connectToHIDDevice = false;
  btd.incomingHIDDevice = false;
  btd.incomingPSController = false;

  btd.qNextPollTime = 0; // Reset next poll time
//  btd.pollInterval = 0;
//  btd.bPollEnable = false; // Don't start polling before dongle is connected
  btd.simple_pairing_supported = false;

  btd.hci_num_reset_loops = 100; // only loop 100 times before trying to send the hci reset command
  btd.hci_counter = 0;
  btd.hci_state = HCI_INIT_STATE;
  btd.waitingForConnection = false;

  btd.btdPin = "0000";
  btd.btdName = "STM32F429";
}

void btd_hci_event_task(void *phost, uint8_t *hcibuf, uint32_t len)
{
  // Switch on event type
  switch (hcibuf[0])
  {
    case EV_COMMAND_COMPLETE:
    {
      if (!hcibuf[5]) // Check if command succeeded
      {
        hci_set_flag(btd.hci_event_flag, HCI_FLAG_CMD_COMPLETE); // Set command complete flag

        if ((hcibuf[3] == 0x01) && (hcibuf[4] == 0x10)) // Parameters from read local version information
        {
          btd.hci_version = hcibuf[6]; // Used to check if it supports 2.0+EDR - see http://www.bluetooth.org/Technical/AssignedNumbers/hci.htm
          hci_set_flag(btd.hci_event_flag, HCI_FLAG_READ_VERSION);
        }
        else if ((hcibuf[3] == 0x04) && (hcibuf[4] == 0x10)) // Parameters from read local extended features
        {
          if (!hci_check_flag(btd.hci_event_flag,
            HCI_FLAG_LOCAL_EXTENDED_FEATURES))
          {
            if (hcibuf[6] == 0) // Page 0
            {
              if (hcibuf[8 + 6] & (1U << 3))
              {
                btd.simple_pairing_supported = true;

              }
              else
              {
                btd.simple_pairing_supported = false;
              }

            }
            else if (hcibuf[6] == 1) // Page 1
            {
            }
          }

          hci_set_flag(btd.hci_event_flag, HCI_FLAG_LOCAL_EXTENDED_FEATURES);
        }
        else if ((hcibuf[3] == 0x09) && (hcibuf[4] == 0x10)) // Parameters from read local bluetooth address
        {
          for (uint8_t i = 0; i < 6; i++)
            btd.my_bdaddr[i] = hcibuf[6 + i];
          hci_set_flag(btd.hci_event_flag, HCI_FLAG_READ_BDADDR);
        }
      }
      break;
    }

    case EV_COMMAND_STATUS:
    {
      if (hcibuf[2]) // Show status on serial if not OK
      {
//        Notify(PSTR("\r\nHCI Command Failed: "), 0x80);
//        D_PrintHex<uint8_t > (hcibuf[2], 0x80);
//        Notify(PSTR("\r\nNum HCI Command Packets: "), 0x80);
//        D_PrintHex<uint8_t > (hcibuf[3], 0x80);
//        Notify(PSTR("\r\nCommand Opcode: "), 0x80);
//        D_PrintHex<uint8_t > (hcibuf[4], 0x80);
//        Notify(PSTR(" "), 0x80);
//        D_PrintHex<uint8_t > (hcibuf[5], 0x80);
      }
      break;
    }

    case EV_INQUIRY_COMPLETE:
    {
      if (btd.inquiry_counter >= 5 && (btd.pairWithWii || btd.pairWithHIDDevice))
      {
        btd.inquiry_counter = 0;

//        if (pairWithWii)
//          Notify(PSTR("\r\nCouldn't find Wiimote"), 0x80);
//        else
//          Notify(PSTR("\r\nCouldn't find HID device"), 0x80);

        btd.connectToWii = false;
        btd.pairWithWii = false;
        btd.connectToHIDDevice = false;
        btd.pairWithHIDDevice = false;
        btd.hci_state = HCI_SCANNING_STATE;
      }
      btd.inquiry_counter++;
      break;
    }

    case EV_INQUIRY_RESULT:
    case EV_EXTENDED_INQUIRY_RESULT:
    {
      if (hcibuf[2]) // Check that there is more than zero responses
      {
//        Notify(PSTR("\r\nNumber of responses: "), 0x80);
//        Notify(hcibuf[2], 0x80); // This will always be 1 for an extended inquiry result

        for (uint8_t i = 0; i < hcibuf[2]; i++)
        {
          uint8_t classOfDevice_offset;
          if (hcibuf[0] == EV_INQUIRY_RESULT)
            classOfDevice_offset = 9 * hcibuf[2]; // 6-byte bd_addr, 1 byte page_scan_repetition_mode, 2 byte reserved
          else
            classOfDevice_offset = 8 * hcibuf[2]; // 6-byte bd_addr, 1 byte page_scan_repetition_mode, 1 byte reserved

          for (uint8_t j = 0; j < 3; j++)
            btd.classOfDevice[j] = hcibuf[3 + classOfDevice_offset + 3 * i + j];

//          Notify(PSTR("\r\nClass of device: "), 0x80);
//          D_PrintHex<uint8_t > (classOfDevice[2], 0x80);
//          Notify(PSTR(" "), 0x80);
//          D_PrintHex<uint8_t > (classOfDevice[1], 0x80);
//          Notify(PSTR(" "), 0x80);
//          D_PrintHex<uint8_t > (classOfDevice[0], 0x80);

          // See http://wiibrew.org/wiki/Wiimote#SDP_information
          if (btd.pairWithWii && (btd.classOfDevice[2] == 0x00)
              && ((btd.classOfDevice[1] & 0x0F) == 0x05)
              && (btd.classOfDevice[0] & 0x0C))
          {
            btd.checkRemoteName = true; // Check remote name to distinguish between the different controllers

            for (uint8_t j = 0; j < 6; j++)
              btd.disc_bdaddr[j] = hcibuf[3 + 6 * i + j];

            hci_set_flag(btd.hci_event_flag, HCI_FLAG_DEVICE_FOUND);
            break;
          }
          // Check if it is a mouse, keyboard or a gamepad - see: http://bluetooth-pentest.narod.ru/software/bluetooth_class_of_device-service_generator.html
          else if (btd.pairWithHIDDevice && ((btd.classOfDevice[1] & 0x0F) == 0x05)
              && (btd.classOfDevice[0] & 0xC8))
          {
//            btd.checkRemoteName = true; // Used to print name in the serial monitor if serial debugging is enabled
//
//            if (btd.hci_event_flag, classOfDevice[0] & 0x80)
//              Notify(PSTR("\r\nMouse found"), 0x80);
//            if (btd.hci_event_flag, classOfDevice[0] & 0x40)
//              Notify(PSTR("\r\nKeyboard found"), 0x80);
//            if (btd.hci_event_flag, classOfDevice[0] & 0x08)
//              Notify(PSTR("\r\nGamepad found"), 0x80);

            for (uint8_t j = 0; j < 6; j++)
              btd.disc_bdaddr[j] = hcibuf[3 + 6 * i + j];

            hci_set_flag(btd.hci_event_flag, HCI_FLAG_DEVICE_FOUND);
            break;
          }
        }
      }
      break;
    }

    case EV_CONNECT_COMPLETE:
    {
      hci_set_flag(btd.hci_event_flag, HCI_FLAG_CONNECT_EVENT);
      if (!hcibuf[2]) // Check if connected OK
      {
//        Notify(PSTR("\r\nConnection established"), 0x80);

        btd.hci_handle = hcibuf[3] | ((hcibuf[4] & 0x0F) << 8); // Store the handle for the ACL connection
        hci_set_flag(btd.hci_event_flag, HCI_FLAG_CONNECT_COMPLETE); // Set connection complete flag
      }
      else
      {
        btd.hci_state = HCI_CHECK_DEVICE_SERVICE;

//        Notify(PSTR("\r\nConnection Failed: "), 0x80);
//        D_PrintHex<uint8_t > (hcibuf[2], 0x80);

      }
      break;
    }

    case EV_DISCONNECT_COMPLETE:
    {
      if (!hcibuf[2]) // Check if disconnected OK
      {
        hci_set_flag(btd.hci_event_flag, HCI_FLAG_DISCONNECT_COMPLETE); // Set disconnect command complete flag
        hci_clear_flag(btd.hci_event_flag, HCI_FLAG_CONNECT_COMPLETE); // Clear connection complete flag
      }
      break;
    }

    case EV_REMOTE_NAME_COMPLETE:
    {
      if (!hcibuf[2]) // Check if reading is OK
      {
        for (uint8_t i = 0; i < min(sizeof(btd.remote_name), sizeof(hcibuf) - 9); i++)
        {
          btd.remote_name[i] = hcibuf[9 + i];
          if (btd.remote_name[i] == '\0') // End of string
            break;
        }
        // TODO: Always set '\0' in remote name!
        hci_set_flag(btd.hci_event_flag, HCI_FLAG_REMOTE_NAME_COMPLETE);
      }
      break;
    }

    case EV_INCOMING_CONNECT:
    {
      for(uint8_t i = 0; i < 6; i++)
        btd.disc_bdaddr[i] = hcibuf[i + 2];

      for(uint8_t i = 0; i < 3; i++)
        btd.classOfDevice[i] = hcibuf[i + 8];

      if (((btd.classOfDevice[1] & 0x0F) == 0x05)
          && (btd.classOfDevice[0] & 0xC8)) // Check if it is a mouse, keyboard or a gamepad
      {
//        if (btd.classOfDevice[0] & 0x80)
//          Notify(PSTR("\r\nMouse is connecting"), 0x80);
//        if (btd.classOfDevice[0] & 0x40)
//          Notify(PSTR("\r\nKeyboard is connecting"), 0x80);
//        if (btd.classOfDevice[0] & 0x08)
//          Notify(PSTR("\r\nGamepad is connecting"), 0x80);

        btd.incomingHIDDevice = true;
      }


//      Notify(PSTR("\r\nClass of device: "), 0x80);
//      D_PrintHex<uint8_t > (btd.classOfDevice[2], 0x80);
//      Notify(PSTR(" "), 0x80);
//      D_PrintHex<uint8_t > (btd.classOfDevice[1], 0x80);
//      Notify(PSTR(" "), 0x80);
//      D_PrintHex<uint8_t > (btd.classOfDevice[0], 0x80);

      hci_set_flag(btd.hci_event_flag, HCI_FLAG_INCOMING_REQUEST);
      break;
    }

    case EV_PIN_CODE_REQUEST:
    {
      if (btd.pairWithWii)
      {
//        Notify(PSTR("\r\nPairing with Wiimote"), 0x80);

        btd_hci_pin_code_request_reply(phost);
      }
      else if (btd.btdPin != NULL)
      {
//        Notify(PSTR("\r\nBluetooth pin is set too: "), 0x80);
//        NotifyStr(btdPin, 0x80);

        btd_hci_pin_code_request_reply(phost);
      }
      else
      {
//        Notify(PSTR("\r\nNo pin was set"), 0x80);
        btd_hci_pin_code_negative_request_reply(phost);
      }
      break;
    }

    case EV_LINK_KEY_REQUEST:
    {
//      Notify(PSTR("\r\nReceived Key Request"), 0x80);
      btd_hci_link_key_request_negative_reply(phost);
      break;
    }

    case EV_AUTHENTICATION_COMPLETE:
    {
      if (!hcibuf[2]) // Check if pairing was successful
      {
        if (btd.pairWithWii && !btd.connectToWii)
        {
//          Notify(PSTR("\r\nPairing successful with Wiimote"), 0x80);

          btd.connectToWii = true; // Used to indicate to the Wii service, that it should connect to this device
        }
        else if (btd.pairWithHIDDevice && !btd.connectToHIDDevice)
        {
//          Notify(PSTR("\r\nPairing successful with HID device"), 0x80);

          btd.connectToHIDDevice = true; // Used to indicate to the BTHID service, that it should connect to this device
        }
        else
        {
//          Notify(PSTR("\r\nPairing was successful"), 0x80);
        }
      }
      else
      {
//        Notify(PSTR("\r\nPairing Failed: "), 0x80);
//        D_PrintHex<uint8_t > (hcibuf[2], 0x80);

        btd_hci_disconnect(phost, btd.hci_handle);
        btd.hci_state = HCI_DISCONNECT_STATE;
      }
      break;
    }

    case EV_IO_CAPABILITY_REQUEST:
    {
//      Notify(PSTR("\r\nReceived IO Capability Request"), 0x80);

      btd_hci_io_capability_request_reply(phost);
      break;
    }

    case EV_IO_CAPABILITY_RESPONSE:
    {
//      Notify(PSTR("\r\nReceived IO Capability Response: "), 0x80);
//      Notify(PSTR("\r\nIO capability: "), 0x80);
//      D_PrintHex<uint8_t > (hcibuf[8], 0x80);
//      Notify(PSTR("\r\nOOB data present: "), 0x80);
//      D_PrintHex<uint8_t > (hcibuf[9], 0x80);
//      Notify(PSTR("\r\nAuthentication request: "), 0x80);
//      D_PrintHex<uint8_t > (hcibuf[10], 0x80);

      break;
    }

    case EV_USER_CONFIRMATION_REQUEST:
    {
//      Notify(PSTR("\r\nUser confirmation Request"), 0x80);

//    Notify(PSTR(": \r\nNumeric value: "), 0x80);
//    for(uint8_t i = 0; i < 4; i++) {
//      Notify(PSTR(" "), 0x80);
//      D_PrintHex<uint8_t > (hcibuf[8 + i], 0x80);
//    }

      // Simply confirm the connection, as the host has no "NoInputNoOutput" capabilities
      btd_hci_user_confirmation_request_reply(phost);
      break;
    }

    case EV_SIMPLE_PAIRING_COMPLETE:
    {
//      if (!hcibuf[2]) // Check if connected OK
//      {
//        Notify(PSTR("\r\nSimple Pairing succeeded"), 0x80);
//      }
//      else
//      {
//        Notify(PSTR("\r\nSimple Pairing failed: "), 0x80);
//        D_PrintHex < uint8_t > (hcibuf[2], 0x80);
//      }

      break;
    }

    /* We will just ignore the following events */

    case EV_MAX_SLOTS_CHANGE:
    case EV_NUM_COMPLETE_PKT:
    {
      break;
    }

    case EV_ROLE_CHANGED:
    case EV_PAGE_SCAN_REP_MODE:
    case EV_LOOPBACK_COMMAND:
    case EV_DATA_BUFFER_OVERFLOW:
    case EV_CHANGE_CONNECTION_LINK:
    case EV_QOS_SETUP_COMPLETE:
    case EV_LINK_KEY_NOTIFICATION:
    case EV_ENCRYPTION_CHANGE:
    case EV_READ_REMOTE_VERSION_INFORMATION_COMPLETE:
    {
//      if (hcibuf[0] != 0x00)
//      {
//        Notify(PSTR("\r\nIgnore HCI Event: "), 0x80);
//        D_PrintHex<uint8_t > (hcibuf[0], 0x80);
//      }
      break;
    }

    default:
    {
//      if (hcibuf[0] != 0x00)
//      {
//        Notify(PSTR("\r\nUnmanaged HCI Event: "), 0x80);
//        D_PrintHex < uint8_t > (hcibuf[0], 0x80);
//        Notify(PSTR(", data: "), 0x80);
//        for (uint16_t i = 0; i < hcibuf[1]; i++)
//        {
//          D_PrintHex < uint8_t > (hcibuf[2 + i], 0x80);
//          Notify(PSTR(" "), 0x80);
//        }
//      }
      break;
    }
  }
}

void btd_hci_task(void *phost)
{
  switch (btd.hci_state)
  {
    case HCI_INIT_STATE:
    {
      uint32_t cur = usb_millis(phost);
      if ((cur - btd.hci_counter) > btd.hci_num_reset_loops) // wait until we have looped x times to clear any old events
      {
        btd_hci_reset(phost);
        btd.hci_state = HCI_RESET_STATE;
        btd.hci_counter = usb_millis(phost);
      }
      break;
    }

    case HCI_RESET_STATE:
    {
      uint32_t cur = usb_millis(phost);
      if (hci_check_flag(btd.hci_event_flag, HCI_FLAG_CMD_COMPLETE))
      {
        btd.hci_counter = usb_millis(phost);

//        Notify(PSTR("\r\nHCI Reset complete"), 0x80);

        btd.hci_state = HCI_CLASS_STATE;
        btd_hci_write_class_of_device(phost);
      }
      else if ((cur - btd.hci_counter) > btd.hci_num_reset_loops)
      {
        btd.hci_num_reset_loops *= 10;
        if (btd.hci_num_reset_loops > 2000)
          btd.hci_num_reset_loops = 2000;

//        Notify(PSTR("\r\nNo response to HCI Reset"), 0x80);

        btd.hci_state = HCI_INIT_STATE;
        btd.hci_counter = 0;
      }
      break;
    }

    case HCI_CLASS_STATE:
    {
      if (hci_check_flag(btd.hci_event_flag, HCI_FLAG_CMD_COMPLETE))
      {
//        Notify(PSTR("\r\nWrite class of device"), 0x80);

        btd.hci_state = HCI_BDADDR_STATE;
        btd_hci_read_bdaddr(phost);
      }
      break;
    }

    case HCI_BDADDR_STATE:
    {
      if (hci_check_flag(btd.hci_event_flag, HCI_FLAG_READ_BDADDR))
      {
//        Notify(PSTR("\r\nLocal Bluetooth Address: "), 0x80);
//        for(int8_t i = 5; i > 0; i--) {
//                D_PrintHex<uint8_t > (my_bdaddr[i], 0x80);
//                Notify(PSTR(":"), 0x80);
//        }
//        D_PrintHex<uint8_t > (my_bdaddr[0], 0x80);

        btd_hci_read_local_version_information(phost);
        btd.hci_state = HCI_LOCAL_VERSION_STATE;
      }
      break;
    }

    case HCI_LOCAL_VERSION_STATE: // The local version is used by the PS3BT class
    {
      if (hci_check_flag(btd.hci_event_flag, HCI_FLAG_READ_VERSION))
      {
        if (btd.btdName != NULL)
        {
          btd_hci_write_local_name(phost, btd.btdName);
          btd.hci_state = HCI_WRITE_NAME_STATE;
        }
        else if (btd.useSimplePairing)
        {
          btd_hci_read_local_extended_features(phost, 0); // "Requests the normal LMP features as returned by Read_Local_Supported_Features"
          //hci_read_local_extended_features(1); // Read page 1
          btd.hci_state = HCI_LOCAL_EXTENDED_FEATURES_STATE;
        }
        else
          btd.hci_state = HCI_CHECK_DEVICE_SERVICE;
      }
      break;
    }

    case HCI_WRITE_NAME_STATE:
    {
      if (hci_check_flag(btd.hci_event_flag, HCI_FLAG_CMD_COMPLETE))
      {
//        Notify(PSTR("\r\nThe name was set to: "), 0x80);
//        NotifyStr(btdName, 0x80);
        if (btd.useSimplePairing)
        {
          btd_hci_read_local_extended_features(phost, 0); // "Requests the normal LMP features as returned by Read_Local_Supported_Features"
          //hci_read_local_extended_features(1); // Read page 1
          btd.hci_state = HCI_LOCAL_EXTENDED_FEATURES_STATE;
        }
        else
        {
          btd.hci_state = HCI_CHECK_DEVICE_SERVICE;
        }
      }
      break;
    }

    case HCI_LOCAL_EXTENDED_FEATURES_STATE:
    {
      if (hci_check_flag(btd.hci_event_flag, HCI_FLAG_LOCAL_EXTENDED_FEATURES))
      {
        if (btd.simple_pairing_supported)
        {
          btd_hci_write_simple_pairing_mode(phost, true);
          btd.hci_state = HCI_WRITE_SIMPLE_PAIRING_STATE;
        }
        else
        {
          btd.hci_state = HCI_CHECK_DEVICE_SERVICE;
        }
      }
      break;
    }

    case HCI_WRITE_SIMPLE_PAIRING_STATE:
    {
      if (hci_check_flag(btd.hci_event_flag, HCI_FLAG_CMD_COMPLETE))
      {
//        Notify(PSTR("\r\nSimple pairing was enabled"), 0x80);

        btd_hci_set_event_mask(phost);
        btd.hci_state = HCI_SET_EVENT_MASK_STATE;
      }
      break;
    }

    case HCI_SET_EVENT_MASK_STATE:
    {
      if (hci_check_flag(btd.hci_event_flag, HCI_FLAG_CMD_COMPLETE))
      {
//        Notify(PSTR("\r\nSet event mask completed"), 0x80);

        btd.hci_state = HCI_CHECK_DEVICE_SERVICE;
      }
      break;
    }

    case HCI_CHECK_DEVICE_SERVICE:
    {
      if (btd.pairWithHIDDevice || btd.pairWithWii) // Check if it should try to connect to a Wiimote
      {
//        if (btd.pairWithWii)
//        {
//          Notify(PSTR("\r\nStarting inquiry\r\nPress 1 & 2 on the Wiimote\r\nOr press the SYNC button if you are using a Wii U Pro Controller or a Wii Balance Board"), 0x80);
//        }
//        else
//        {
//          Notify(PSTR("\r\nPlease enable discovery of your device"), 0x80);
//        }

        btd_hci_inquiry(phost);
        btd.hci_state = HCI_INQUIRY_STATE;
      }
      else
      {
        btd.hci_state = HCI_SCANNING_STATE; // Don't try to connect to a Wiimote
      }

      break;
    }

    case HCI_INQUIRY_STATE:
    {
      if (hci_check_flag(btd.hci_event_flag, HCI_FLAG_DEVICE_FOUND))
      {
        btd_hci_inquiry_cancel(phost); // Stop inquiry

//        if (pairWithWii)
//          Notify(PSTR("\r\nWiimote found"), 0x80);
//        else
//          Notify(PSTR("\r\nHID device found"), 0x80);
//
//        Notify(PSTR("\r\nNow just create the instance like so:"), 0x80);
//        if (pairWithWii)
//          Notify(PSTR("\r\nWII Wii(&Btd);"), 0x80);
//        else
//          Notify(PSTR("\r\nBTHID bthid(&Btd);"), 0x80);
//
//        Notify(PSTR("\r\nAnd then press any button on the "), 0x80);
//        if (pairWithWii)
//          Notify(PSTR("Wiimote"), 0x80);
//        else
//          Notify(PSTR("device"), 0x80);

        if (btd.checkRemoteName)
        {
          btd_hci_remote_name(phost); // We need to know the name to distinguish between the Wiimote, the new Wiimote with Motion Plus inside, a Wii U Pro Controller and a Wii Balance Board
          btd.hci_state = HCI_REMOTE_NAME_STATE;
        }
        else
        {
          btd.hci_state = HCI_CONNECT_DEVICE_STATE;
        }
      }
      break;
    }

    case HCI_CONNECT_DEVICE_STATE:
    {
      if (hci_check_flag(btd.hci_event_flag, HCI_FLAG_CMD_COMPLETE))
      {
//        if (pairWithWii)
//          Notify(PSTR("\r\nConnecting to Wiimote"), 0x80);
//        else
//          Notify(PSTR("\r\nConnecting to HID device"), 0x80);

        btd.checkRemoteName = false;
        btd_hci_connect(phost);
        btd.hci_state = HCI_CONNECTED_DEVICE_STATE;
      }
      break;
    }

    case HCI_CONNECTED_DEVICE_STATE:
    {
      if (hci_check_flag(btd.hci_event_flag, HCI_FLAG_CONNECT_EVENT))
      {
        if (hci_check_flag(btd.hci_event_flag, HCI_FLAG_CONNECT_COMPLETE))
        {
//          if (pairWithWii)
//            Notify(PSTR("\r\nConnected to Wiimote"), 0x80);
//          else
//            Notify(PSTR("\r\nConnected to HID device"), 0x80);

          btd_hci_authentication_request(phost); // This will start the pairing with the device
          btd.hci_state = HCI_SCANNING_STATE;
        }
        else
        {
//          Notify(PSTR("\r\nTrying to connect one more time..."), 0x80);

          btd_hci_connect(phost); // Try to connect one more time
        }
      }
      break;
    }

    case HCI_SCANNING_STATE:
    {
      if (!btd.connectToWii && !btd.pairWithWii && !btd.connectToHIDDevice && !btd.pairWithHIDDevice)
      {
//        Notify(PSTR("\r\nWait For Incoming Connection Request"), 0x80);

        btd_hci_write_scan_enable(phost);
        btd.waitingForConnection = true;
        btd.hci_state = HCI_CONNECT_IN_STATE;
      }
      break;
    }

    case HCI_CONNECT_IN_STATE:
    {
      if (hci_check_flag(btd.hci_event_flag, HCI_FLAG_INCOMING_REQUEST))
      {
        btd.waitingForConnection = false;

//        Notify(PSTR("\r\nIncoming Connection Request"), 0x80);

        btd_hci_remote_name(phost);
        btd.hci_state = HCI_REMOTE_NAME_STATE;
      }
      else if (hci_check_flag(btd.hci_event_flag, HCI_FLAG_DISCONNECT_COMPLETE))
      {
        btd.hci_state = HCI_DISCONNECT_STATE;
      }
      break;
    }

    case HCI_REMOTE_NAME_STATE:
    {
      if (hci_check_flag(btd.hci_event_flag, HCI_FLAG_REMOTE_NAME_COMPLETE))
      {
//          Notify(PSTR("\r\nRemote Name: "), 0x80);
//          for(uint8_t i = 0; i < strlen(remote_name); i++)
//            Notifyc(remote_name[i], 0x80);

        if (strncmp((const char *)btd.remote_name, "Nintendo", 8) == 0)
        {
          btd.incomingWii = true;
          btd.motionPlusInside = false;
          btd.wiiUProController = false;
          btd.pairWiiUsingSync = false;

//          Notify(PSTR("\r\nWiimote is connecting"), 0x80);

          if (strncmp((const char *)btd.remote_name, "Nintendo RVL-CNT-01-TR", 22) == 0)
          {
//            Notify(PSTR(" with Motion Plus Inside"), 0x80);

            btd.motionPlusInside = true;
          }
          else if (strncmp((const char *)btd.remote_name, "Nintendo RVL-CNT-01-UC", 22) == 0)
          {
//            Notify(PSTR(" - Wii U Pro Controller"), 0x80);
            btd.wiiUProController = btd.motionPlusInside = btd.pairWiiUsingSync = true;
          }
          else if (strncmp((const char *)btd.remote_name, "Nintendo RVL-WBC-01", 19) == 0)
          {
//            Notify(PSTR(" - Wii Balance Board"), 0x80);
            btd.pairWiiUsingSync = true;
          }
        }

        if (btd.classOfDevice[2] == 0 && btd.classOfDevice[1] == 0x25 && btd.classOfDevice[0] == 0x08 && strncmp((const char *)btd.remote_name, "Wireless Controller", 19) == 0)
        {
//          Notify(PSTR("\r\nPS4/PS5 controller is connecting"), 0x80);

          btd.incomingPSController = true;
        }

        if ((btd.pairWithWii || btd.pairWithHIDDevice) && btd.checkRemoteName)
        {
          btd.hci_state = HCI_CONNECT_DEVICE_STATE;
        }
        else
        {
          btd_hci_accept_connection(phost);
          btd.hci_state = HCI_CONNECTED_STATE;
        }
      }
      break;
    }

    case HCI_CONNECTED_STATE:
    {
      if (hci_check_flag(btd.hci_event_flag, HCI_FLAG_CONNECT_COMPLETE))
      {
//        Notify(PSTR("\r\nConnected to Device: "), 0x80);
//        for(int8_t i = 5; i > 0; i--) {
//                D_PrintHex<uint8_t > (disc_bdaddr[i], 0x80);
//                Notify(PSTR(":"), 0x80);
//        }
//        D_PrintHex<uint8_t > (disc_bdaddr[0], 0x80);

        if (btd.incomingPSController)
        {
          btd.connectToHIDDevice = true; // We should always connect to the PS4/PS5 controller
        }

        // Clear these flags for a new connection
        btd.l2capConnectionClaimed = false;
        btd.sdpConnectionClaimed = false;
        btd.rfcommConnectionClaimed = false;

        btd.hci_event_flag = 0;
        btd.hci_state = HCI_DONE_STATE;
      }
      break;
    }

    case HCI_DONE_STATE:
    {
      uint32_t cur = usb_millis(phost);
      if ((cur - btd.hci_counter) > 1000)  // Wait until we have looped 1000 times to make sure that the L2CAP connection has been started
      {
        btd.hci_counter = usb_millis(phost);
        btd.hci_state = HCI_SCANNING_STATE;
      }
      break;
    }

    case HCI_DISCONNECT_STATE:
    {
      if (hci_check_flag(btd.hci_event_flag, HCI_FLAG_DISCONNECT_COMPLETE))
      {
//        Notify(PSTR("\r\nHCI Disconnected from Device"), 0x80);

        btd.hci_event_flag = 0; // Clear all flags

        // Reset all buffers
//        memset(btd.hcibuf, 0, BULK_MAXPKTSIZE);
//        memset(btd.l2capinbuf, 0, BULK_MAXPKTSIZE);

        btd.connectToWii = btd.incomingWii = btd.pairWithWii = false;
        btd.connectToHIDDevice = btd.incomingHIDDevice = btd.pairWithHIDDevice = btd.checkRemoteName = false;
        btd.incomingPSController = false;

        btd.hci_state = HCI_SCANNING_STATE;
      }
      break;
    }

    default:
    {
      break;
    }
  }
}

void btd_acl_event_task(void *phost, uint8_t *l2capinbuf, uint32_t len)
{
  if (len > 0)
  {
    bthid_acldata(phost, l2capinbuf);
  }

  bthid_run(phost);
}
