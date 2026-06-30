/*
 * BTHID.c
 *
 *  Created on: 2026-06-23
 *      Author: ChiaYen
 */

#include <string.h>
#include "BTD.h"
#include "BTHID.h"
#include "SwitchProType.h"

extern BTD_HandleTypeDef btd;
extern uint32_t usb_millis(void *p);
extern void HAL_Delay(uint32_t Delay);

BTHID_HandleTypeDef bthid;

bool checkHciHandle(uint8_t *buf, uint16_t handle)
{
  return (buf[0] == (handle & 0xFF)) && (buf[1] == ((handle >> 8) | 0x20));
}

/*
 * SwitchPro
 */

SwitchPro_Info_TypeDef swpInfo;

void SwitchPro_OnInitBTHID(void *phost)
{
}

void __attribute__((weak)) usr_switchpro_handle(const SwitchPro_Info_TypeDef *swpInfo)
{
}

void SwitchPro_ParseBTHIDData(uint8_t len, uint8_t *buf)
{
  if (buf[0] == 0x30)
  {
    switchpro_report_parser(buf + 1, &swpInfo);

    usr_switchpro_handle(&swpInfo);
  }
}

/*
 *
 */

void bthid_sdp_command(void *phost, uint8_t* data, uint8_t nbytes) // See page 223 in the Bluetooth specs
{
  btd_l2cap_command(phost, bthid.service.hci_handle, data, nbytes, bthid.sdp_scid[0], bthid.sdp_scid[1]);
}

void bthid_setProtocol(void *phost)
{
  if (bthid.protocolMode != USB_HID_BOOT_PROTOCOL && bthid.protocolMode != HID_RPT_PROTOCOL)
  {
    bthid.protocolMode = USB_HID_BOOT_PROTOCOL; // Use Boot Protocol by default
  }

  uint8_t command = 0x70 | bthid.protocolMode; // Set Protocol, see Bluetooth HID specs page 33
  btd_l2cap_command(phost, bthid.service.hci_handle, &command, 1, bthid.control_scid[0], bthid.control_scid[1]);
}

void bthid_reset(void *phost)
{
  bthid.connected = false;
  bthid.activeConnection = false;
  bthid.SDPConnected = false;
  bthid.service.l2cap_event_flag = 0; // Reset flags
  bthid.l2cap_sdp_state = L2CAP_SDP_WAIT;
  bthid.l2cap_state = L2CAP_WAIT;

  // TODO add ResetBTHID
//  ResetBTHID();
}

/*
 * BTHID class
 */

void bthid_initialize(void *phost)
{
  memset(&bthid, 0, sizeof(bthid));

  bthid.protocolMode = USB_HID_BOOT_PROTOCOL;

//  for (uint8_t i = 0; i < NUM_PARSERS; i++)
//    pRptParser[i] = NULL;

  btd.pairWithHIDDevice = true;
  btd.btdPin = "0000";

  /* Set device cid for the control and intterrupt channelse - LSB */
  bthid.sdp_dcid[0] = 0x50; // 0x0050
  bthid.sdp_dcid[1] = 0x00;
  bthid.control_dcid[0] = 0x70; // 0x0070
  bthid.control_dcid[1] = 0x00;
  bthid.interrupt_dcid[0] = 0x71; // 0x0071
  bthid.interrupt_dcid[1] = 0x00;

  bthid_reset(phost);
}

void bthid_sdp_task(void *phost)
{
  switch (bthid.l2cap_sdp_state)
  {
    case L2CAP_SDP_WAIT:
    {
      if (l2cap_check_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_CONNECTION_SDP_REQUEST))
      {
        l2cap_clear_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_CONNECTION_SDP_REQUEST); // Clear flag

        btd_l2cap_connection_response(phost, bthid.service.hci_handle, bthid.service.identifier, bthid.sdp_dcid, bthid.sdp_scid, PENDING);
        HAL_Delay(1);
        btd_l2cap_connection_response(phost, bthid.service.hci_handle, bthid.service.identifier, bthid.sdp_dcid, bthid.sdp_scid, SUCCESSFUL);
        bthid.service.identifier++;
        HAL_Delay(1);
        btd_l2cap_config_request(phost, bthid.service.hci_handle, bthid.service.identifier, bthid.sdp_scid);
        bthid.l2cap_sdp_state = L2CAP_SDP_SUCCESS;
      }
      else if (l2cap_check_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_DISCONNECT_SDP_REQUEST))
      {
        l2cap_clear_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_DISCONNECT_SDP_REQUEST); // Clear flag
        bthid.SDPConnected = false;

        btd_l2cap_disconnection_response(phost, bthid.service.hci_handle, bthid.service.identifier, bthid.sdp_dcid, bthid.sdp_scid);
      }
      break;
    }

    case L2CAP_SDP_SUCCESS:
    {
      if (l2cap_check_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_CONFIG_SDP_SUCCESS))
      {
        l2cap_clear_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_CONFIG_SDP_SUCCESS); // Clear flag

        bthid.SDPConnected = true;
        bthid.l2cap_sdp_state = L2CAP_SDP_WAIT;
      }
      break;
    }

    case L2CAP_DISCONNECT_RESPONSE: // This is for both disconnection response from the RFCOMM and SDP channel if they were connected
    {
      if (l2cap_check_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_DISCONNECT_RESPONSE))
      {
        btd_hci_disconnect(phost, bthid.service.hci_handle);
        bthid.service.hci_handle = -1; // Reset handle
        bthid_reset(phost);
      }
      break;
    }
  }
}

void bthid_l2cap_task(void *phost)
{
  switch (bthid.l2cap_state)
  {
    /* These states are used if the HID device is the host */
    case L2CAP_CONTROL_SUCCESS:
    {
      if (l2cap_check_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_CONFIG_CONTROL_SUCCESS))
      {
        bthid_setProtocol(phost); // Set protocol before establishing HID interrupt channel
        bthid.l2cap_state = L2CAP_INTERRUPT_SETUP;
      }
      break;
    }

    case L2CAP_INTERRUPT_SETUP:
    {
      if (l2cap_check_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_CONNECTION_INTERRUPT_REQUEST))
      {
        btd_l2cap_connection_response(phost, bthid.service.hci_handle, bthid.service.identifier, bthid.interrupt_dcid, bthid.interrupt_scid, PENDING);
        HAL_Delay(1);
        btd_l2cap_connection_response(phost, bthid.service.hci_handle, bthid.service.identifier, bthid.interrupt_dcid, bthid.interrupt_scid, SUCCESSFUL);
        bthid.service.identifier++;
        HAL_Delay(1);
        btd_l2cap_config_request(phost, bthid.service.hci_handle, bthid.service.identifier, bthid.interrupt_scid);

        bthid.l2cap_state = L2CAP_INTERRUPT_CONFIG_REQUEST;
      }
      break;
    }

    /* These states are used if the Arduino is the host */
    case L2CAP_CONTROL_CONNECT_REQUEST:
    {
      if (l2cap_check_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_CONTROL_CONNECTED))
      {

        bthid.service.identifier++;
        btd_l2cap_config_request(phost, bthid.service.hci_handle, bthid.service.identifier, bthid.control_scid);
        bthid.l2cap_state = L2CAP_CONTROL_CONFIG_REQUEST;
      }
      break;
    }

    case L2CAP_CONTROL_CONFIG_REQUEST:
    {
      if (l2cap_check_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_CONFIG_CONTROL_SUCCESS))
      {
        bthid_setProtocol(phost); // Set protocol before establishing HID interrupt channel
        HAL_Delay(1); // Short delay between commands - just to be sure

        bthid.service.identifier++;
        btd_l2cap_connection_request(phost, bthid.service.hci_handle, bthid.service.identifier, bthid.interrupt_dcid, HID_INTR_PSM);
        bthid.l2cap_state = L2CAP_INTERRUPT_CONNECT_REQUEST;
      }
      break;
    }

    case L2CAP_INTERRUPT_CONNECT_REQUEST:
    {
      if (l2cap_check_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_INTERRUPT_CONNECTED))
      {
        bthid.service.identifier++;
        btd_l2cap_config_request(phost, bthid.service.hci_handle, bthid.service.identifier, bthid.interrupt_scid);
        bthid.l2cap_state = L2CAP_INTERRUPT_CONFIG_REQUEST;
      }
      break;
    }

    case L2CAP_INTERRUPT_CONFIG_REQUEST:
    {
      if (l2cap_check_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_CONFIG_INTERRUPT_SUCCESS)) // Now the HID channels is established
      {
        btd.connectToHIDDevice = false;
        btd.pairWithHIDDevice = false;
        bthid.connected = true;

        // TODO : onInit
        //onInit();
        SwitchPro_OnInitBTHID(phost);

        bthid.l2cap_state = L2CAP_DONE;
      }
      break;
    }

    case L2CAP_DONE:
    {
      break;
    }

    case L2CAP_INTERRUPT_DISCONNECT:
    {
      if (l2cap_check_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_DISCONNECT_INTERRUPT_RESPONSE))
      {
        bthid.service.identifier++;
        btd_l2cap_disconnection_request(phost, bthid.service.hci_handle, bthid.service.identifier, bthid.control_scid, bthid.control_dcid);
        bthid.l2cap_state = L2CAP_CONTROL_DISCONNECT;
      }
      break;
    }

    case L2CAP_CONTROL_DISCONNECT:
    {
      if (l2cap_check_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_DISCONNECT_CONTROL_RESPONSE))
      {
        btd_hci_disconnect(phost, bthid.service.hci_handle);
        bthid.service.hci_handle = -1; // Reset handle
        bthid.service.l2cap_event_flag = 0; // Reset flags
        bthid.l2cap_state = L2CAP_WAIT;
      }
      break;
    }
  }
}

void bthid_acldata(void *phost, uint8_t *l2capinbuf)
{
  if (!bthid.connected)
  {
    if (l2capinbuf[8] == L2CAP_CMD_CONNECTION_REQUEST)
    {
      if ((l2capinbuf[12] | (l2capinbuf[13] << 8)) == SDP_PSM
          && !btd.sdpConnectionClaimed)
      {
        btd.sdpConnectionClaimed = true;
        bthid.service.hci_handle = btd.hci_handle; // Store the HCI Handle for the connection
        bthid.l2cap_sdp_state = L2CAP_SDP_WAIT; // Reset state
      }
    }
  }

  if (!btd.l2capConnectionClaimed && btd.incomingHIDDevice && !bthid.connected && !bthid.activeConnection)
  {
    if (l2capinbuf[8] == L2CAP_CMD_CONNECTION_REQUEST)
    {
      if ((l2capinbuf[12] | (l2capinbuf[13] << 8)) == HID_CTRL_PSM)
      {
        btd.incomingHIDDevice = false;
        btd.l2capConnectionClaimed = true; // Claim that the incoming connection belongs to this service
        bthid.activeConnection = true;
        bthid.service.hci_handle = btd.hci_handle; // Store the HCI Handle for the connection
        bthid.l2cap_state = L2CAP_WAIT;
      }
    }
  }

  if (checkHciHandle(l2capinbuf, bthid.service.hci_handle)) // acl_handle_ok
  {
    if ((l2capinbuf[6] | (l2capinbuf[7] << 8)) == 0x0001U)  // l2cap_control - Channel ID for ACL-U
    {
      if (l2capinbuf[8] == L2CAP_CMD_COMMAND_REJECT) {
      }
      else if (l2capinbuf[8] == L2CAP_CMD_CONNECTION_RESPONSE)
      {
        if (((l2capinbuf[16] | (l2capinbuf[17] << 8)) == 0x0000) && ((l2capinbuf[18] | (l2capinbuf[19] << 8)) == SUCCESSFUL)) // Success
        {
          if (l2capinbuf[14] == bthid.sdp_dcid[0] && l2capinbuf[15] == bthid.sdp_dcid[1])
          {
            bthid.service.identifier = l2capinbuf[9];
            bthid.sdp_scid[0] = l2capinbuf[12];
            bthid.sdp_scid[1] = l2capinbuf[13];

            bthid.service.identifier++;
            btd_l2cap_config_request(phost, bthid.service.hci_handle, bthid.service.identifier, bthid.sdp_scid);
          }
          else if (l2capinbuf[14] == bthid.control_dcid[0] && l2capinbuf[15] == bthid.control_dcid[1])
          {
            bthid.service.identifier = l2capinbuf[9];
            bthid.control_scid[0] = l2capinbuf[12];
            bthid.control_scid[1] = l2capinbuf[13];
            l2cap_set_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_CONTROL_CONNECTED);
          }
          else if (l2capinbuf[14] == bthid.interrupt_dcid[0] && l2capinbuf[15] == bthid.interrupt_dcid[1])
          {
            bthid.service.identifier = l2capinbuf[9];
            bthid.interrupt_scid[0] = l2capinbuf[12];
            bthid.interrupt_scid[1] = l2capinbuf[13];
            l2cap_set_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_INTERRUPT_CONNECTED);
          }
        }
      }
      else if (l2capinbuf[8] == L2CAP_CMD_CONNECTION_REQUEST)
      {
        if ((l2capinbuf[12] | (l2capinbuf[13] << 8)) == SDP_PSM)
        {
          bthid.service.identifier = l2capinbuf[9];
          bthid.sdp_scid[0] = l2capinbuf[14];
          bthid.sdp_scid[1] = l2capinbuf[15];
          l2cap_set_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_CONNECTION_SDP_REQUEST);
        }
        else if ((l2capinbuf[12] | (l2capinbuf[13] << 8)) == HID_CTRL_PSM)
        {
          bthid.service.identifier = l2capinbuf[9];
          bthid.control_scid[0] = l2capinbuf[14];
          bthid.control_scid[1] = l2capinbuf[15];
          l2cap_set_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_CONNECTION_CONTROL_REQUEST);
        }
        else if ((l2capinbuf[12] | (l2capinbuf[13] << 8)) == HID_INTR_PSM)
        {
          bthid.service.identifier = l2capinbuf[9];
          bthid.interrupt_scid[0] = l2capinbuf[14];
          bthid.interrupt_scid[1] = l2capinbuf[15];
          l2cap_set_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_CONNECTION_INTERRUPT_REQUEST);
        }
      }
      else if (l2capinbuf[8] == L2CAP_CMD_CONFIG_RESPONSE)
      {
        if ((l2capinbuf[16] | (l2capinbuf[17] << 8)) == 0x0000) // Success
        {
          if (l2capinbuf[12] == bthid.sdp_dcid[0] && l2capinbuf[13] == bthid.sdp_dcid[1])
          {
            bthid.service.identifier = l2capinbuf[9];
            l2cap_set_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_CONFIG_SDP_SUCCESS);
          }
          else if (l2capinbuf[12] == bthid.control_dcid[0] && l2capinbuf[13] == bthid.control_dcid[1])
          {
            bthid.service.identifier = l2capinbuf[9];
            l2cap_set_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_CONFIG_CONTROL_SUCCESS);
          }
          else if (l2capinbuf[12] == bthid.interrupt_dcid[0] && l2capinbuf[13] == bthid.interrupt_dcid[1])
          {
            bthid.service.identifier = l2capinbuf[9];
            l2cap_set_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_CONFIG_INTERRUPT_SUCCESS);
          }
        }
      }
      else if (l2capinbuf[8] == L2CAP_CMD_CONFIG_REQUEST)
      {
        if (l2capinbuf[12] == bthid.sdp_dcid[0] && l2capinbuf[13] == bthid.sdp_dcid[1])
        {
          btd_l2cap_config_response(phost, bthid.service.hci_handle, l2capinbuf[9], bthid.sdp_scid);
        }
        else if (l2capinbuf[12] == bthid.control_dcid[0] && l2capinbuf[13] == bthid.control_dcid[1])
        {
          btd_l2cap_config_response(phost, bthid.service.hci_handle, l2capinbuf[9], bthid.control_scid);
        }
        else if (l2capinbuf[12] == bthid.interrupt_dcid[0] && l2capinbuf[13] == bthid.interrupt_dcid[1])
        {
          btd_l2cap_config_response(phost, bthid.service.hci_handle, l2capinbuf[9], bthid.interrupt_scid);
        }
      }
      else if (l2capinbuf[8] == L2CAP_CMD_DISCONNECT_REQUEST)
      {
        if (l2capinbuf[12] == bthid.sdp_dcid[0] && l2capinbuf[13] == bthid.sdp_dcid[1])
        {
          bthid.service.identifier = l2capinbuf[9];
          l2cap_set_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_DISCONNECT_SDP_REQUEST);
        }
        else if (l2capinbuf[12] == bthid.control_dcid[0] && l2capinbuf[13] == bthid.control_dcid[1])
        {
          bthid.service.identifier = l2capinbuf[9];
          btd_l2cap_disconnection_response(phost, bthid.service.hci_handle, bthid.service.identifier, bthid.control_dcid, bthid.control_scid);
          bthid_reset(phost);
        }
        else if (l2capinbuf[12] == bthid.interrupt_dcid[0] && l2capinbuf[13] == bthid.interrupt_dcid[1])
        {
          bthid.service.identifier = l2capinbuf[9];
          btd_l2cap_disconnection_response(phost, bthid.service.hci_handle, bthid.service.identifier, bthid.interrupt_dcid, bthid.interrupt_scid);
          bthid_reset(phost);
        }
      }
      else if (l2capinbuf[8] == L2CAP_CMD_DISCONNECT_RESPONSE)
      {
        if (l2capinbuf[12] == bthid.sdp_scid[0] && l2capinbuf[13] == bthid.sdp_scid[1])
        {
          bthid.service.identifier = l2capinbuf[9];
          l2cap_set_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_DISCONNECT_RESPONSE);
        }
        else if (l2capinbuf[12] == bthid.control_scid[0] && l2capinbuf[13] == bthid.control_scid[1])
        {
          bthid.service.identifier = l2capinbuf[9];
          l2cap_set_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_DISCONNECT_CONTROL_RESPONSE);
        }
        else if (l2capinbuf[12] == bthid.interrupt_scid[0] && l2capinbuf[13] == bthid.interrupt_scid[1])
        {
          bthid.service.identifier = l2capinbuf[9];
          l2cap_set_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_DISCONNECT_INTERRUPT_RESPONSE);
        }
      }
      else if (l2capinbuf[8] == L2CAP_CMD_INFORMATION_REQUEST)
      {
        bthid.service.identifier = l2capinbuf[9];
        btd_l2cap_information_response(phost, bthid.service.hci_handle, bthid.service.identifier, l2capinbuf[12], l2capinbuf[13]);
      }
      else
      {
        bthid.service.identifier = l2capinbuf[9];
      }
    }
    else if (l2capinbuf[6] == bthid.sdp_dcid[0] && l2capinbuf[7] == bthid.sdp_dcid[1]) // SDP
    {
      if (l2capinbuf[8] == SDP_SERVICE_SEARCH_REQUEST)
      {
        // Send response
        uint8_t l2capoutbuf[10];
        l2capoutbuf[0] = SDP_SERVICE_SEARCH_RESPONSE;
        l2capoutbuf[1] = l2capinbuf[9];//transactionIDHigh;
        l2capoutbuf[2] = l2capinbuf[10];//transactionIDLow;

        l2capoutbuf[3] = 0x00; // MSB Parameter Length
        l2capoutbuf[4] = 0x05; // LSB Parameter Length = 5

        l2capoutbuf[5] = 0x00; // MSB TotalServiceRecordCount
        l2capoutbuf[6] = 0x00; // LSB TotalServiceRecordCount = 0

        l2capoutbuf[7] = 0x00; // MSB CurrentServiceRecordCount
        l2capoutbuf[8] = 0x00; // LSB CurrentServiceRecordCount = 0

        l2capoutbuf[9] = 0x00; // No continuation state

        bthid_sdp_command(phost, l2capoutbuf, 10);
      }
      else if (l2capinbuf[8] == SDP_SERVICE_ATTRIBUTE_REQUEST)
      {
        // Send response
        uint8_t l2capoutbuf[10];
        l2capoutbuf[0] = SDP_SERVICE_ATTRIBUTE_RESPONSE;
        l2capoutbuf[1] = l2capinbuf[9];//transactionIDHigh;
        l2capoutbuf[2] = l2capinbuf[10];//transactionIDLow;

        l2capoutbuf[3] = 0x00; // MSB Parameter Length
        l2capoutbuf[4] = 0x05; // LSB Parameter Length = 5

        l2capoutbuf[5] = 0x00; // MSB AttributeListByteCount
        l2capoutbuf[6] = 0x02; // LSB AttributeListByteCount = 2

        // TODO: What to send?
        l2capoutbuf[7] = 0x35; // Data element sequence - length in next byte
        l2capoutbuf[8] = 0x00; // Length = 0

        l2capoutbuf[9] = 0x00; // No continuation state

        bthid_sdp_command(phost, l2capoutbuf, 10);
      }
      else if (l2capinbuf[8] == SDP_SERVICE_SEARCH_ATTRIBUTE_REQUEST)
      {
        // TODO: serviceNotSupported
        //serviceNotSupported(l2capinbuf[9], l2capinbuf[10]); // The service is not supported
      }
    }
    else if (l2capinbuf[6] == bthid.interrupt_dcid[0] && l2capinbuf[7] == bthid.interrupt_dcid[1]) // l2cap_interrupt
    {
      if (l2capinbuf[8] == 0xA1) // HID BT DATA (0xA0) | Report Type (Input 0x01)
      {
          bthid.lastBtDataInputIntMillis = (uint32_t)usb_millis(phost); // Store the timestamp of the report

          uint16_t length = ((uint16_t)l2capinbuf[5] << 8 | l2capinbuf[4]);

          // TODO : ParseBTHIDData
          //ParseBTHIDData((uint8_t)(length - 1), &l2capinbuf[9]); // First byte will be the report ID
          SwitchPro_ParseBTHIDData((uint8_t)(length - 1), &l2capinbuf[9]);

          switch (l2capinbuf[9]) // Report ID
          {
            case 0x01: // Keyboard or Joystick events
            {
              // TODO : Keyboard
//              if (pRptParser[KEYBOARD_PARSER_ID])
//                pRptParser[KEYBOARD_PARSER_ID]->Parse(reinterpret_cast<USBHID *>(this), 0, (uint8_t)(length - 2), &l2capinbuf[10]); // Use reinterpret_cast again to extract the instance
              break;
            }

            case 0x02: // Mouse events
            {
              // TODO : Mouse
//              if (pRptParser[MOUSE_PARSER_ID])
//                pRptParser[MOUSE_PARSER_ID]->Parse(reinterpret_cast<USBHID *>(this), 0, (uint8_t)(length - 2), &l2capinbuf[10]); // Use reinterpret_cast again to extract the instance
              break;
            }
          }
      }
      else
      {
      }
    }
    else if (l2capinbuf[6] == bthid.control_dcid[0] && l2capinbuf[7] == bthid.control_dcid[1]) // l2cap_control
    {
      if (l2capinbuf[8] == 0xA3) // HID BT DATA (0xA0) | Report Type (Feature 0x03)
      {
        uint16_t length = ((uint16_t)l2capinbuf[5] << 8 | l2capinbuf[4]);

        // TODO : ParseBTHIDControlData
        //ParseBTHIDControlData((uint8_t)(length - 1), &l2capinbuf[9]); // First byte will be the report ID
      }
      else
      {
      }
    }

    bthid_sdp_task(phost);
    bthid_l2cap_task(phost);
  }
}

void bthid_run(void *phost)
{
  switch (bthid.l2cap_state)
  {
    case L2CAP_WAIT:
    {
      if (btd.connectToHIDDevice && !btd.l2capConnectionClaimed && !bthid.connected && !bthid.activeConnection)
      {
        btd.l2capConnectionClaimed = true;
        bthid.activeConnection = true;

        bthid.service.hci_handle = btd.hci_handle; // Store the HCI Handle for the connection
        bthid.service.l2cap_event_flag = 0; // Reset flags
        bthid.service.identifier = 0;
        btd_l2cap_connection_request(phost, bthid.service.hci_handle, bthid.service.identifier, bthid.control_dcid, HID_CTRL_PSM);
        bthid.l2cap_state = L2CAP_CONTROL_CONNECT_REQUEST;
      }
      else if (l2cap_check_flag(bthid.service.l2cap_event_flag, L2CAP_FLAG_CONNECTION_CONTROL_REQUEST))
      {
        btd_l2cap_connection_response(phost, bthid.service.hci_handle, bthid.service.identifier, bthid.control_dcid, bthid.control_scid, PENDING);
        HAL_Delay(1);
        btd_l2cap_connection_response(phost, bthid.service.hci_handle, bthid.service.identifier, bthid.control_dcid, bthid.control_scid, SUCCESSFUL);
        bthid.service.identifier++;
        HAL_Delay(1);
        btd_l2cap_config_request(phost, bthid.service.hci_handle, bthid.service.identifier, bthid.control_scid);
        bthid.l2cap_state = L2CAP_CONTROL_SUCCESS;
      }
      break;
    }
  }
}
