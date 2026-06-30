/*
 * hid_app.c
 *
 *  Created on: 2026-06-10
 *      Author: ChiaYen
 */

#include <stdio.h>
#include "tusb.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"

#include "HIDReportParser.h"
#include "DualShock.h"

__attribute__ ((section(".ccmram"))) DualShock3_Info_TypeDef ds3Info;
__attribute__ ((section(".ccmram"))) DualShock4_Info_TypeDef ds4Info;

static uint8_t ds3BootCode[4] =
{
  0x42, // Special PS3 Controller enable commands
  0x0c,
  0x00,
  0x00
};

#define GET_INPUT_REPOT 0
#if GET_INPUT_REPOT
uint8_t inputReport[12];
#endif

typedef enum
{
  DEVICE_TYPE_NONE,
  DEVICE_TYPE_DS3,
  DEVICE_TYPE_DS4,
} DEVICE_TYPE;

DEVICE_TYPE deviceType = DEVICE_TYPE_NONE;


void hid_app_task(void)
{
  char text[128] = {0};

  if (deviceType == DEVICE_TYPE_DS3)
  {
    BSP_LCD_SetFont(&Font16);

    sprintf(text, "UP:%1d RIGHT:%1d DOWN:%1d", ds3Info.UP, ds3Info.RIGHT, ds3Info.DOWN);
    BSP_LCD_DisplayStringAt(0, 35, (uint8_t *)text, LEFT_MODE);

    sprintf(text, "LEFT:%1d", ds3Info.LEFT);
    BSP_LCD_DisplayStringAt(0, 51, (uint8_t *)text, LEFT_MODE);

    sprintf(text, "SQU:%1d CRO:%1d CIR:%1d", ds3Info.SQUARE, ds3Info.CROSS, ds3Info.CIRCLE);
    BSP_LCD_DisplayStringAt(0, 67, (uint8_t *)text, LEFT_MODE);

    sprintf(text, "TRI:%1d", ds3Info.TRIANGLE);
    BSP_LCD_DisplayStringAt(0, 83, (uint8_t *)text, LEFT_MODE);

    sprintf(text, "L1:%1d R1:%1d L2:%1d R2:%1d", ds3Info.L1, ds3Info.R1, ds3Info.L2, ds3Info.R2);
    BSP_LCD_DisplayStringAt(0, 99, (uint8_t *)text, LEFT_MODE);

    sprintf(text, "SELECT:%1d START:%1d", ds3Info.SELECT, ds3Info.START);
    BSP_LCD_DisplayStringAt(0, 115, (uint8_t *)text, LEFT_MODE);

    sprintf(text, "L3:%1d R3:%1d", ds3Info.L3, ds3Info.R3);
    BSP_LCD_DisplayStringAt(0, 131, (uint8_t *)text, LEFT_MODE);

    sprintf(text, "LeftX:%3d LeftY:%3d", ds3Info.leftX, ds3Info.leftY);
    BSP_LCD_DisplayStringAt(0, 147, (uint8_t *)text, LEFT_MODE);

    sprintf(text, "RightX:%3d RightY:%3d", ds3Info.rightX, ds3Info.rightY);
    BSP_LCD_DisplayStringAt(0, 163, (uint8_t *)text, LEFT_MODE);
  }
  else if (deviceType == DEVICE_TYPE_DS4)
  {
    BSP_LCD_SetFont(&Font16);

    sprintf(text, "UP:%1d RIGHT:%1d DOWN:%1d", ds4Info.UP, ds4Info.RIGHT, ds4Info.DOWN);
    BSP_LCD_DisplayStringAt(0, 35, (uint8_t *)text, LEFT_MODE);

    sprintf(text, "LEFT:%1d", ds4Info.LEFT);
    BSP_LCD_DisplayStringAt(0, 51, (uint8_t *)text, LEFT_MODE);

    sprintf(text, "SQU:%1d CRO:%1d CIR:%1d", ds4Info.SQUARE, ds4Info.CROSS, ds4Info.CIRCLE);
    BSP_LCD_DisplayStringAt(0, 67, (uint8_t *)text, LEFT_MODE);

    sprintf(text, "TRI:%1d", ds4Info.TRIANGLE);
    BSP_LCD_DisplayStringAt(0, 83, (uint8_t *)text, LEFT_MODE);

    sprintf(text, "L1:%1d R1:%1d L2:%1d R2:%1d", ds4Info.L1, ds4Info.R1, ds4Info.L2, ds4Info.R2);
    BSP_LCD_DisplayStringAt(0, 99, (uint8_t *)text, LEFT_MODE);

    sprintf(text, "SHARE:%1d OPT:%1d", ds4Info.SHARE, ds4Info.OPT);
    BSP_LCD_DisplayStringAt(0, 115, (uint8_t *)text, LEFT_MODE);

    sprintf(text, "L3:%1d R3:%1d", ds4Info.L3, ds4Info.R3);
    BSP_LCD_DisplayStringAt(0, 131, (uint8_t *)text, LEFT_MODE);

    sprintf(text, "TPAD:%1d PS:%1d", ds4Info.TPAD, ds4Info.PS);
    BSP_LCD_DisplayStringAt(0, 147, (uint8_t *)text, LEFT_MODE);

    sprintf(text, "LeftX:%3d LeftY:%3d", ds4Info.leftX, ds4Info.leftY);
    BSP_LCD_DisplayStringAt(0, 163, (uint8_t *)text, LEFT_MODE);

    sprintf(text, "RightX:%3d RightY:%3d", ds4Info.rightX, ds4Info.rightY);
    BSP_LCD_DisplayStringAt(0, 179, (uint8_t *)text, LEFT_MODE);

    sprintf(text, "TriggerL2:%3d", ds4Info.triggerL2);
    BSP_LCD_DisplayStringAt(0, 195, (uint8_t *)text, LEFT_MODE);

    sprintf(text, "TriggerR2:%3d", ds4Info.triggerR2);
    BSP_LCD_DisplayStringAt(0, 211, (uint8_t *)text, LEFT_MODE);
  }
}

// TinyUSB Callbacks

// Invoked when device with hid interface is mounted
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance,
  uint8_t const *desc_report, uint16_t desc_len)
{
  BSP_LCD_Clear(LCD_COLOR_BLACK);

  uint16_t PID, VID;
  tuh_vid_pid_get(dev_addr, &VID, &PID);

  if (VID == SONY_VID && PID == PS3_DUALSHOCK3_PID)
  {
    BSP_LCD_SetFont(&LCD_DEFAULT_FONT);
    BSP_LCD_DisplayStringAt(0, 10, (uint8_t*)"DualShock3", CENTER_MODE);

    deviceType = DEVICE_TYPE_DS3;

    tuh_hid_set_report(dev_addr, 0, 0xF4, HID_REPORT_TYPE_FEATURE, ds3BootCode, 4);

#if GET_INPUT_REPOT
    tuh_hid_get_report(dev_addr, 0, 0x00, HID_REPORT_TYPE_INPUT, inputReport, 12);
#endif

    if (!tuh_hid_receive_report(dev_addr, instance))
    {
      printf("Error: cannot request to receive report\r\n");
    }
  }

  if (VID == SONY_VID && PID == PS4_DUALSHOCK4_PID)
  {
    BSP_LCD_SetFont(&LCD_DEFAULT_FONT);
    BSP_LCD_DisplayStringAt(0, 10, (uint8_t*)"DualShock4", CENTER_MODE);

    deviceType = DEVICE_TYPE_DS4;

    if (!tuh_hid_receive_report(dev_addr, instance))
    {
      printf("Error: cannot request to receive report\r\n");
    }
  }
}

// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance)
{
  BSP_LCD_Clear(LCD_COLOR_BLACK);

  deviceType = DEVICE_TYPE_NONE;
}

// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance,
  const uint8_t *report, uint16_t len)
{
  if (deviceType == DEVICE_TYPE_DS3)
  {
    dualshock3_report_parser(report, &ds3Info);
  }
  else if (deviceType == DEVICE_TYPE_DS4)
  {
    dualshock4_report_parser(report, &ds4Info);
  }

  // continue to request to receive report
  if (!tuh_hid_receive_report(dev_addr, instance))
  {
    printf("Error: cannot request to receive report\r\n");
  }
}

