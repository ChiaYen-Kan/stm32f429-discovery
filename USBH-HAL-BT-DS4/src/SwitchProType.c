/*
 * SwitchProType.c
 *
 *  Created on: 2026-06-29
 *      Author: ChiaYen
 */

#include "SwitchProType.h"
#include "HIDReportParser.h"

/*
 * https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering/blob/master/bluetooth_hid_notes.md#standard-input-report-format
 */


static const HID_Report_ItemTypedef swp_digitalButton1 =
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

static const HID_Report_ItemTypedef swp_digitalButton2 =
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

static const HID_Report_ItemTypedef swp_digitalButton3 =
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

void switchpro_report_parser(const uint8_t *report, SwitchPro_Info_TypeDef *switchpro_info)
{
  switchpro_info->buttonGroup1 = ReadHIDReportItem(report, &swp_digitalButton1, 0);
  switchpro_info->buttonGroup2 = ReadHIDReportItem(report, &swp_digitalButton2, 0);
  switchpro_info->buttonGroup3 = ReadHIDReportItem(report, &swp_digitalButton3, 0);

  const uint8_t *data = report + 5;
  switchpro_info->leftStickHorizontal = data[0] | ((data[1] & 0xF) << 8);
  switchpro_info->leftStickVertical = (data[1] >> 4) | (data[2] << 4);

  data = report + 8;
  switchpro_info->rightStickHorizontal = data[0] | ((data[1] & 0xF) << 8);
  switchpro_info->rightStickVertical = (data[1] >> 4) | (data[2] << 4);
}
