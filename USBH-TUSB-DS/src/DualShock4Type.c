/*
 * DualShock4Type.c
 *
 *  Created on: 2026-06-15
 *      Author: ChiaYen
 */


#include "DualShock4Type.h"
#include "HIDReportParser.h"

/*
 * https://www.psdevwiki.com/ps4/DS4-USB
 */

static const HID_Report_ItemTypedef ds4_leftX =
{
  1,     /*data_offset*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFF,  /*max value read can return*/
  0,     /*min vale device can report*/
  0xFF,  /*max value device can report*/
  1      /*resolution*/
};

static const HID_Report_ItemTypedef ds4_leftY =
{
  2,     /*data_offset*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFF,  /*max value read can return*/
  0,     /*min vale device can report*/
  0xFF,  /*max value device can report*/
  1      /*resolution*/
};

static const HID_Report_ItemTypedef ds4_rightX =
{
  3,     /*data_offset*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFF,  /*max value read can return*/
  0,     /*min vale device can report*/
  0xFF,  /*max value device can report*/
  1      /*resolution*/
};

static const HID_Report_ItemTypedef ds4_rightY =
{
  4,     /*data_offset*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFF,  /*max value read can return*/
  0,     /*min vale device can report*/
  0xFF,  /*max value device can report*/
  1      /*resolution*/
};

static const HID_Report_ItemTypedef ds4_digitalButton1 =
{
  5,     /*data_offset*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFF,  /*max value read can return*/
  0,     /*min vale device can report*/
  0xFF,  /*max value device can report*/
  1      /*resolution*/
};

static const HID_Report_ItemTypedef ds4_digitalButton2 =
{
  6,     /*data_offset*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFF,  /*max value read can return*/
  0,     /*min vale device can report*/
  0xFF,  /*max value device can report*/
  1      /*resolution*/
};

static const HID_Report_ItemTypedef ds4_digitalButton3 =
{
  7,     /*data_offset*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFF,  /*max value read can return*/
  0,     /*min vale device can report*/
  0xFF,  /*max value device can report*/
  1      /*resolution*/
};

static const HID_Report_ItemTypedef ds4_triggerL2 =
{
  8,     /*data_offset*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFF,  /*max value read can return*/
  0,     /*min vale device can report*/
  0xFF,  /*max value device can report*/
  1      /*resolution*/
};

static const HID_Report_ItemTypedef ds4_triggerR2 =
{
  9,     /*data_offset*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFF,  /*max value read can return*/
  0,     /*min vale device can report*/
  0xFF,  /*max value device can report*/
  1      /*resolution*/
};

void dualshock4_report_parser(const uint8_t *report, DualShock4_Info_TypeDef *dualshock4_info)
{
  dualshock4_info->leftX = ReadHIDReportItem(report, &ds4_leftX, 0);
  dualshock4_info->leftY = ReadHIDReportItem(report, &ds4_leftY, 0);
  dualshock4_info->rightX = ReadHIDReportItem(report, &ds4_rightX, 0);
  dualshock4_info->rightY = ReadHIDReportItem(report, &ds4_rightY, 0);

  dualshock4_info->buttonGroup1 = ReadHIDReportItem(report, &ds4_digitalButton1, 0);
  dualshock4_info->buttonGroup2 = ReadHIDReportItem(report, &ds4_digitalButton2, 0);
  dualshock4_info->buttonGroup3 = ReadHIDReportItem(report, &ds4_digitalButton3, 0);

  dualshock4_info->triggerL2 = ReadHIDReportItem(report, &ds4_triggerL2, 0);
  dualshock4_info->triggerR2 = ReadHIDReportItem(report, &ds4_triggerR2, 0);

  switch (dualshock4_info->DPAD)
  {
    case 0:
    {
      // UP
      dualshock4_info->UP = 1;
      break;
    }
    case 1:
    {
      // UP + RIGHT
      dualshock4_info->UP = dualshock4_info->RIGHT = 1;
      break;
    }
    case 2:
    {
      // RIGHT
      dualshock4_info->RIGHT = 1;
      break;
    }
    case 3:
    {
      // RIGHT + DOWN
      dualshock4_info->RIGHT = dualshock4_info->DOWN = 1;
      break;
    }
    case 4:
    {
      // DOWN
      dualshock4_info->DOWN = 1;
      break;
    }
    case 5:
    {
      // DOWN + LEFT
      dualshock4_info->DOWN = dualshock4_info->LEFT = 1;
      break;
    }
    case 6:
    {
      // LEFT
      dualshock4_info->LEFT = 1;
      break;
    }
    case 7:
    {
      // LEFT + UP
      dualshock4_info->LEFT = dualshock4_info->UP = 1;
      break;
    }
    default:
    {
      dualshock4_info->UP = dualshock4_info->RIGHT = dualshock4_info->DOWN = dualshock4_info->LEFT = 0;
      break;
    }
  }
}
