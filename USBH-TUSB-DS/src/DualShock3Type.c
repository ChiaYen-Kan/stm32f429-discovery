/*
 * DualShock3Type.c
 *
 *  Created on: 2026-06-15
 *      Author: ChiaYen
 */

#include "DualShock3Type.h"
#include "HIDReportParser.h"

static const HID_Report_ItemTypedef ds3_digitalButton1 =
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

static const HID_Report_ItemTypedef ds3_digitalButton2 =
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

static const HID_Report_ItemTypedef ds3_digitalButton3 =
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

static const HID_Report_ItemTypedef ds3_leftX =
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

static const HID_Report_ItemTypedef ds3_leftY =
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

static const HID_Report_ItemTypedef ds3_rightX =
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

static const HID_Report_ItemTypedef ds3_rightY =
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

static const HID_Report_ItemTypedef ds3_analogUp =
{
  14,    /*data_offset*/
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

static const HID_Report_ItemTypedef ds3_analogRight = {
  15,    /*data_offset*/
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

static const HID_Report_ItemTypedef ds3_analogDown =
{
  16,    /*data_offset*/
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

static const HID_Report_ItemTypedef ds3_analogLeft =
{
  17,    /*data_offset*/
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

static const HID_Report_ItemTypedef ds3_analogL2 =
{
  18,    /*data_offset*/
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

static const HID_Report_ItemTypedef ds3_analogR2 =
{
  19,    /*data_offset*/
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

static const HID_Report_ItemTypedef ds3_analogL1 =
{
  20,    /*data_offset*/
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

static const HID_Report_ItemTypedef ds3_analogR1 =
{
  21,    /*data_offset*/
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

static const HID_Report_ItemTypedef ds3_analogTriangle =
{
  22,    /*data_offset*/
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

static const HID_Report_ItemTypedef ds3_analogCircle =
{
  23,    /*data_offset*/
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

static const HID_Report_ItemTypedef ds3_analogCross =
{
  24,    /*data_offset*/
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

static const HID_Report_ItemTypedef ds3_analogSquare =
{
  25,    /*data_offset*/
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

static const HID_Report_ItemTypedef ds3_accelerometerX1 =
{
  41,    /*data_offset*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFFFF,/*max value read can return*/
  0,     /*min vale device can report*/
  0xFFFF,/*max value device can report*/
  1      /*resolution*/
};

static const HID_Report_ItemTypedef ds3_accelerometerX2 =
{
  42,    /*data_offset*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFFFF,/*max value read can return*/
  0,     /*min vale device can report*/
  0xFFFF,/*max value device can report*/
  1      /*resolution*/
};

static const HID_Report_ItemTypedef ds3_accelerometerY1 =
{
  43,    /*data_offset*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFFFF,/*max value read can return*/
  0,     /*min vale device can report*/
  0xFFFF,/*max value device can report*/
  1      /*resolution*/
};

static const HID_Report_ItemTypedef ds3_accelerometerY2 =
{
  44,    /*data_offset*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFFFF,/*max value read can return*/
  0,     /*min vale device can report*/
  0xFFFF,/*max value device can report*/
  1      /*resolution*/
};

static const HID_Report_ItemTypedef ds3_accelerometerZ1 =
{
  45,    /*data_offset*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFFFF,/*max value read can return*/
  0,     /*min vale device can report*/
  0xFFFF,/*max value device can report*/
  1      /*resolution*/
};

static const HID_Report_ItemTypedef ds3_accelerometerZ2 =
{
  46,    /*data_offset*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFFFF,/*max value read can return*/
  0,     /*min vale device can report*/
  0xFFFF,/*max value device can report*/
  1      /*resolution*/
};

static const HID_Report_ItemTypedef ds3_gyroscope1 =
{
  47,    /*data_offset*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFFFF,/*max value read can return*/
  0,     /*min vale device can report*/
  0xFFFF,/*max value device can report*/
  1      /*resolution*/
};

static const HID_Report_ItemTypedef ds3_gyroscope2 =
{
  48,    /*data_offset*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFFFF,/*max value read can return*/
  0,     /*min vale device can report*/
  0xFFFF,/*max value device can report*/
  1      /*resolution*/
};

void dualshock3_report_parser(const uint8_t *report, DualShock3_Info_TypeDef *dualshock3_info)
{
  dualshock3_info->buttonGroup1 = ReadHIDReportItem(report, &ds3_digitalButton1, 0);
  dualshock3_info->buttonGroup2 = ReadHIDReportItem(report, &ds3_digitalButton2, 0);
  dualshock3_info->buttonGroup3 = ReadHIDReportItem(report, &ds3_digitalButton3, 0);

  dualshock3_info->leftX = ReadHIDReportItem(report, &ds3_leftX, 0);
  dualshock3_info->leftY = ReadHIDReportItem(report, &ds3_leftY, 0);
  dualshock3_info->rightX = ReadHIDReportItem(report, &ds3_rightX, 0);
  dualshock3_info->rightY = ReadHIDReportItem(report, &ds3_rightY, 0);

  dualshock3_info->analogUp = ReadHIDReportItem(report, &ds3_analogUp, 0);
  dualshock3_info->analogRight = ReadHIDReportItem(report, &ds3_analogRight, 0);
  dualshock3_info->analogDown = ReadHIDReportItem(report, &ds3_analogDown, 0);
  dualshock3_info->analogLeft = ReadHIDReportItem(report, &ds3_analogLeft, 0);
  dualshock3_info->analogL2 = ReadHIDReportItem(report, &ds3_analogL2, 0);
  dualshock3_info->analogR2 = ReadHIDReportItem(report, &ds3_analogR2, 0);
  dualshock3_info->analogL1 = ReadHIDReportItem(report, &ds3_analogL1, 0);
  dualshock3_info->analogR1 = ReadHIDReportItem(report, &ds3_analogR1, 0);
  dualshock3_info->analogTriangle = ReadHIDReportItem(report, &ds3_analogTriangle, 0);
  dualshock3_info->analogCircle = ReadHIDReportItem(report, &ds3_analogCircle, 0);
  dualshock3_info->analogCross = ReadHIDReportItem(report, &ds3_analogCross, 0);
  dualshock3_info->analogSquare = ReadHIDReportItem(report, &ds3_analogSquare, 0);

  uint32_t x1 = ReadHIDReportItem(report, &ds3_accelerometerX1, 0);
  uint16_t x2 = ReadHIDReportItem(report, &ds3_accelerometerX2, 0);
  dualshock3_info->accelerometerX = (x2 << 2) | (x1 & 0x3);

  uint32_t y1 = ReadHIDReportItem(report, &ds3_accelerometerY1, 0);
  uint16_t y2 = ReadHIDReportItem(report, &ds3_accelerometerY2, 0);
  dualshock3_info->accelerometerY = (y2 << 2) | (y1 & 0x3);

  uint32_t z1 = ReadHIDReportItem(report, &ds3_accelerometerZ1, 0);
  uint16_t z2 = ReadHIDReportItem(report, &ds3_accelerometerZ2, 0);
  dualshock3_info->accelerometerZ = (z2 << 2) | (z1 & 0x3);

  uint32_t g1 = ReadHIDReportItem(report, &ds3_gyroscope1, 0);
  uint16_t g2 = ReadHIDReportItem(report, &ds3_gyroscope2, 0);
  dualshock3_info->gyroscope = (g2 << 2) | (g1 & 0x3);
}
