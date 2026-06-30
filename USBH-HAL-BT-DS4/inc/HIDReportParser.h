/*
 * HIDReportParser.h
 *
 *  Created on: 2017-06-22
 *      Author: ChiaYen
 */

#ifndef HIDREPORTPARSER_H_
#define HIDREPORTPARSER_H_

#include <stdint.h>

typedef struct
{
  uint32_t data_offset;
  uint32_t size;
  uint8_t  shift;
  uint8_t  count;
  uint8_t  sign;
  uint32_t logical_min;  /*min value device can return*/
  uint32_t logical_max;  /*max value device can return*/
  uint32_t physical_min; /*min vale read can report*/
  uint32_t physical_max; /*max value read can report*/
  uint32_t resolution;
}
HID_Report_ItemTypedef;

uint32_t ReadHIDReportItem(uint8_t *data, const HID_Report_ItemTypedef *ri, uint8_t ndx);

#endif /* BTHIDREPORTPARSER_H_ */
