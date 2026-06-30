/*
 * BTD.h
 *
 *  Created on: 2026-06-22
 *      Author: ChiaYen
 */

#ifndef BTD_H_
#define BTD_H_

#include <stdint.h>
#include <stdbool.h>


//PID and VID of the Sony PS3 devices
#define PS3_VID                 0x054C  // Sony Corporation
#define PS3_PID                 0x0268  // PS3 Controller DualShock 3
#define PS3NAVIGATION_PID       0x042F  // Navigation controller
#define PS3MOVE_PID             0x03D5  // Motion controller

// These dongles do not present themselves correctly, so we have to check for them manually
#define IOGEAR_GBU521_VID       0x0A5C
#define IOGEAR_GBU521_PID       0x21E8
#define BELKIN_F8T065BF_VID     0x050D
#define BELKIN_F8T065BF_PID     0x065A

/* Bluetooth dongle data taken from descriptors */
#define BULK_MAXPKTSIZE         64 // Max size for ACL data

// Used in control endpoint header for HCI Commands
#define bmREQ_HCI_OUT USB_SETUP_HOST_TO_DEVICE|USB_SETUP_TYPE_CLASS|USB_SETUP_RECIPIENT_DEVICE

/* Bluetooth HCI states for hci_task() */
#define HCI_INIT_STATE                  0
#define HCI_RESET_STATE                 1
#define HCI_CLASS_STATE                 2
#define HCI_BDADDR_STATE                3
#define HCI_LOCAL_VERSION_STATE         4
#define HCI_WRITE_NAME_STATE            5
#define HCI_CHECK_DEVICE_SERVICE        6

#define HCI_INQUIRY_STATE               7 // These three states are only used if it should pair and connect to a device
#define HCI_CONNECT_DEVICE_STATE        8
#define HCI_CONNECTED_DEVICE_STATE      9

#define HCI_SCANNING_STATE              10
#define HCI_CONNECT_IN_STATE            11
#define HCI_REMOTE_NAME_STATE           12
#define HCI_CONNECTED_STATE             13
#define HCI_DISABLE_SCAN_STATE          14
#define HCI_DONE_STATE                  15
#define HCI_DISCONNECT_STATE            16
#define HCI_LOCAL_EXTENDED_FEATURES_STATE       17
#define HCI_WRITE_SIMPLE_PAIRING_STATE          18
#define HCI_SET_EVENT_MASK_STATE                19

/* HCI event flags*/
#define HCI_FLAG_CMD_COMPLETE           (1UL << 0)
#define HCI_FLAG_CONNECT_COMPLETE       (1UL << 1)
#define HCI_FLAG_DISCONNECT_COMPLETE    (1UL << 2)
#define HCI_FLAG_REMOTE_NAME_COMPLETE   (1UL << 3)
#define HCI_FLAG_INCOMING_REQUEST       (1UL << 4)
#define HCI_FLAG_READ_BDADDR            (1UL << 5)
#define HCI_FLAG_READ_VERSION           (1UL << 6)
#define HCI_FLAG_DEVICE_FOUND           (1UL << 7)
#define HCI_FLAG_CONNECT_EVENT          (1UL << 8)
#define HCI_FLAG_LOCAL_EXTENDED_FEATURES    (1UL << 9)

/* Macros for HCI event flag tests */
//#define hci_check_flag(flag) (hci_event_flag & (flag))
//#define hci_set_flag(flag) (hci_event_flag |= (flag))
//#define hci_clear_flag(flag) (hci_event_flag &= ~(flag))
#define hci_check_flag(_hci_event_flag, _flag) (_hci_event_flag & (_flag))
#define hci_set_flag(_hci_event_flag, _flag) (_hci_event_flag |= (_flag))
#define hci_clear_flag(_hci_event_flag, _flag) (_hci_event_flag &= ~(_flag))

/* HCI Events managed */
#define EV_INQUIRY_COMPLETE                             0x01
#define EV_INQUIRY_RESULT                               0x02
#define EV_CONNECT_COMPLETE                             0x03
#define EV_INCOMING_CONNECT                             0x04
#define EV_DISCONNECT_COMPLETE                          0x05
#define EV_AUTHENTICATION_COMPLETE                      0x06
#define EV_REMOTE_NAME_COMPLETE                         0x07
#define EV_ENCRYPTION_CHANGE                            0x08
#define EV_CHANGE_CONNECTION_LINK                       0x09
#define EV_READ_REMOTE_VERSION_INFORMATION_COMPLETE     0x0C
#define EV_QOS_SETUP_COMPLETE                           0x0D
#define EV_COMMAND_COMPLETE                             0x0E
#define EV_COMMAND_STATUS                               0x0F
#define EV_ROLE_CHANGED                                 0x12
#define EV_NUM_COMPLETE_PKT                             0x13
#define EV_PIN_CODE_REQUEST                             0x16
#define EV_LINK_KEY_REQUEST                             0x17
#define EV_LINK_KEY_NOTIFICATION                        0x18
#define EV_DATA_BUFFER_OVERFLOW                         0x1A
#define EV_MAX_SLOTS_CHANGE                             0x1B
#define EV_LOOPBACK_COMMAND                             0x19
#define EV_PAGE_SCAN_REP_MODE                           0x20
#define EV_READ_REMOTE_EXTENDED_FEATURES_COMPLETE       0x23
#define EV_EXTENDED_INQUIRY_RESULT                      0x2F
#define EV_IO_CAPABILITY_REQUEST                        0x31
#define EV_IO_CAPABILITY_RESPONSE                       0x32
#define EV_USER_CONFIRMATION_REQUEST                    0x33
#define EV_SIMPLE_PAIRING_COMPLETE                      0x36

/* Bluetooth states for the different Bluetooth drivers */
#define L2CAP_WAIT                      0
#define L2CAP_DONE                      1

/* Used for HID Control channel */
#define L2CAP_CONTROL_CONNECT_REQUEST   2
#define L2CAP_CONTROL_CONFIG_REQUEST    3
#define L2CAP_CONTROL_SUCCESS           4
#define L2CAP_CONTROL_DISCONNECT        5

/* Used for HID Interrupt channel */
#define L2CAP_INTERRUPT_SETUP           6
#define L2CAP_INTERRUPT_CONNECT_REQUEST 7
#define L2CAP_INTERRUPT_CONFIG_REQUEST  8
#define L2CAP_INTERRUPT_DISCONNECT      9

/* Used for SDP channel */
#define L2CAP_SDP_WAIT                  10
#define L2CAP_SDP_SUCCESS               11

/* Used for RFCOMM channel */
#define L2CAP_RFCOMM_WAIT               12
#define L2CAP_RFCOMM_SUCCESS            13

#define L2CAP_DISCONNECT_RESPONSE       14 // Used for both SDP and RFCOMM channel

/* Bluetooth states used by some drivers */
#define TURN_ON_LED                     17
#define PS3_ENABLE_SIXAXIS              18
#define WII_CHECK_MOTION_PLUS_STATE     19
#define WII_CHECK_EXTENSION_STATE       20
#define WII_INIT_MOTION_PLUS_STATE      21

/* L2CAP event flags for HID Control channel */
#define L2CAP_FLAG_CONNECTION_CONTROL_REQUEST           (1UL << 0)
#define L2CAP_FLAG_CONFIG_CONTROL_SUCCESS               (1UL << 1)
#define L2CAP_FLAG_CONTROL_CONNECTED                    (1UL << 2)
#define L2CAP_FLAG_DISCONNECT_CONTROL_RESPONSE          (1UL << 3)

/* L2CAP event flags for HID Interrupt channel */
#define L2CAP_FLAG_CONNECTION_INTERRUPT_REQUEST         (1UL << 4)
#define L2CAP_FLAG_CONFIG_INTERRUPT_SUCCESS             (1UL << 5)
#define L2CAP_FLAG_INTERRUPT_CONNECTED                  (1UL << 6)
#define L2CAP_FLAG_DISCONNECT_INTERRUPT_RESPONSE        (1UL << 7)

/* L2CAP event flags for SDP channel */
#define L2CAP_FLAG_CONNECTION_SDP_REQUEST               (1UL << 8)
#define L2CAP_FLAG_CONFIG_SDP_SUCCESS                   (1UL << 9)
#define L2CAP_FLAG_DISCONNECT_SDP_REQUEST               (1UL << 10)

/* L2CAP event flags for RFCOMM channel */
#define L2CAP_FLAG_CONNECTION_RFCOMM_REQUEST            (1UL << 11)
#define L2CAP_FLAG_CONFIG_RFCOMM_SUCCESS                (1UL << 12)
#define L2CAP_FLAG_DISCONNECT_RFCOMM_REQUEST            (1UL << 13)

#define L2CAP_FLAG_DISCONNECT_RESPONSE                  (1UL << 14)

/* Macros for L2CAP event flag tests */
//#define l2cap_check_flag(flag) (l2cap_event_flag & (flag))
//#define l2cap_set_flag(flag) (l2cap_event_flag |= (flag))
//#define l2cap_clear_flag(flag) (l2cap_event_flag &= ~(flag))
#define l2cap_check_flag(_l2cap_event_flag, _flag) (_l2cap_event_flag & (_flag))
#define l2cap_set_flag(_l2cap_event_flag, _flag) (_l2cap_event_flag |= (_flag))
#define l2cap_clear_flag(_l2cap_event_flag, _flag) (_l2cap_event_flag &= ~(_flag))

/* L2CAP signaling commands */
#define L2CAP_CMD_COMMAND_REJECT        0x01
#define L2CAP_CMD_CONNECTION_REQUEST    0x02
#define L2CAP_CMD_CONNECTION_RESPONSE   0x03
#define L2CAP_CMD_CONFIG_REQUEST        0x04
#define L2CAP_CMD_CONFIG_RESPONSE       0x05
#define L2CAP_CMD_DISCONNECT_REQUEST    0x06
#define L2CAP_CMD_DISCONNECT_RESPONSE   0x07
#define L2CAP_CMD_INFORMATION_REQUEST   0x0A
#define L2CAP_CMD_INFORMATION_RESPONSE  0x0B

// Used For Connection Response - Remember to Include High Byte
#define PENDING     0x01
#define SUCCESSFUL  0x00

/* Bluetooth L2CAP PSM - see http://www.bluetooth.org/Technical/AssignedNumbers/logical_link.htm */
#define SDP_PSM         0x01 // Service Discovery Protocol PSM Value
#define RFCOMM_PSM      0x03 // RFCOMM PSM Value
#define HID_CTRL_PSM    0x11 // HID_Control PSM Value
#define HID_INTR_PSM    0x13 // HID_Interrupt PSM Value

/* Used for SDP */
#define SDP_SERVICE_SEARCH_REQUEST                  0x02
#define SDP_SERVICE_SEARCH_RESPONSE                 0x03
#define SDP_SERVICE_ATTRIBUTE_REQUEST               0x04
#define SDP_SERVICE_ATTRIBUTE_RESPONSE              0x05
#define SDP_SERVICE_SEARCH_ATTRIBUTE_REQUEST        0x06 // See the RFCOMM specs
#define SDP_SERVICE_SEARCH_ATTRIBUTE_RESPONSE       0x07 // See the RFCOMM specs
#define PNP_INFORMATION_UUID    0x1200
#define SERIALPORT_UUID         0x1101 // See http://www.bluetooth.org/Technical/AssignedNumbers/service_discovery.htm
#define L2CAP_UUID              0x0100

// Used to determine if it is a Bluetooth dongle
#define WI_SUBCLASS_RF      0x01 // RF Controller
#define WI_PROTOCOL_BT      0x01 // Bluetooth Programming Interface

#define BTD_MAX_ENDPOINTS   4
#define BTD_NUM_SERVICES    4 // Max number of Bluetooth services - if you need more than 4 simply increase this number

#define PAIR    1

typedef struct
{

  /** Use this to see if it is waiting for a incoming connection. */
  bool waitingForConnection;
  /** This is used by the service to know when to store the device information. */
  bool l2capConnectionClaimed;
  /** This is used by the SPP library to claim the current SDP incoming request. */
  bool sdpConnectionClaimed;
  /** This is used by the SPP library to claim the current RFCOMM incoming request. */
  bool rfcommConnectionClaimed;

  /** The name you wish to make the dongle show up as. It is set automatically by the SPP library. */
  const char* btdName;
  /** The pin you wish to make the dongle use for authentication. It is set automatically by the SPP and BTHID library. */
  const char* btdPin;

  /** The bluetooth dongles Bluetooth address. */
  uint8_t my_bdaddr[6];
  /** HCI handle for the last connection. */
  uint16_t hci_handle;
  /** Last incoming devices Bluetooth address. */
  uint8_t disc_bdaddr[6];
  /** First 30 chars of last remote name. */
  char remote_name[30];
  /**
   * The supported HCI Version read from the Bluetooth dongle.
   * Used by the PS3BT library to check the HCI Version of the Bluetooth dongle,
   * it should be at least 3 to work properly with the library.
   */
  uint8_t hci_version;

  /** Used to only send the ACL data to the Wiimote. */
  bool connectToWii;
  /** True if a Wiimote is connecting. */
  bool incomingWii;
  /** True when it should pair with a Wiimote. */
  bool pairWithWii;
  /** True if it's the new Wiimote with the Motion Plus Inside or a Wii U Pro Controller. */
  bool motionPlusInside;
  /** True if it's a Wii U Pro Controller. */
  bool wiiUProController;

  /** Used to only send the ACL data to the HID device. */
  bool connectToHIDDevice;
  /** True if a HID device is connecting. */
  bool incomingHIDDevice;
  /** True when it should pair with a device like a mouse or keyboard. */
  bool pairWithHIDDevice;

  /** Used by the drivers to enable simple pairing */
  bool useSimplePairing;

  /** Next poll time based on poll interval taken from the USB descriptor. */
  uint32_t qNextPollTime;

  bool simple_pairing_supported;

  bool pairWiiUsingSync; // True if pairing was done using the Wii SYNC button.
  bool checkRemoteName; // Used to check remote device's name before connecting.
  bool incomingPSController; // True if a PS4/PS5 controller is connecting
  uint8_t classOfDevice[3]; // Class of device of last device

  uint8_t hci_state; // Current state of Bluetooth HCI connection
  uint16_t hci_counter; // Counter used for Bluetooth HCI reset loops
  uint16_t hci_num_reset_loops; // This value indicate how many times it should read before trying to reset
  uint16_t hci_event_flag; // HCI flags of received Bluetooth events
  int8_t inquiry_counter;

} BTD_HandleTypeDef;

void btd_initialize(void *phost);

void btd_hci_disconnect(void *phost, uint16_t handle);

void btd_hci_event_task(void *phost, uint8_t *hcibuf, uint32_t len);
void btd_hci_task(void *phost);

void btd_acl_event_task(void *phost, uint8_t *l2capinbuf, uint32_t len);

void btd_l2cap_command(void *phost, uint16_t handle, uint8_t *data, uint8_t nbytes, uint8_t channelLow /*= 0x01*/, uint8_t channelHigh /*= 0x00*/);
void btd_l2cap_config_request(void *phost, uint16_t handle, uint8_t rxid, uint8_t *dcid);
void btd_l2cap_disconnection_response(void *phost, uint16_t handle, uint8_t rxid, uint8_t *dcid, uint8_t *scid);
void btd_l2cap_information_response(void *phost, uint16_t handle, uint8_t rxid, uint8_t infoTypeLow, uint8_t infoTypeHigh);
void btd_l2cap_connection_request(void *phost, uint16_t handle, uint8_t rxid, uint8_t *scid, uint16_t psm);
void btd_l2cap_connection_response(void *phost, uint16_t handle, uint8_t rxid, uint8_t *dcid, uint8_t *scid, uint8_t result);
void btd_l2cap_disconnection_request(void *phost, uint16_t handle, uint8_t rxid, uint8_t *dcid, uint8_t *scid);
void btd_l2cap_config_response(void *phost, uint16_t handle, uint8_t rxid, uint8_t *scid);

#endif /* BTD_H_ */
