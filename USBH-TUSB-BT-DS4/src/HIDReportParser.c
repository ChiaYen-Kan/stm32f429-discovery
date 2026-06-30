/*
 * HIDReportParser.c
 *
 *  Created on: 2017-06-22
 *      Author: ChiaYen
 */

#include "HIDReportParser.h"

uint32_t ReadHIDReportItem(uint8_t *data, const HID_Report_ItemTypedef *ri, uint8_t ndx)
{
  uint32_t val = 0;
  uint32_t x = 0;
  uint32_t bofs;
  data += ri->data_offset;
  uint8_t shift = ri->shift;

  /* get the logical value of the item */

  /* if this is an array, wee may need to offset ri->data.*/
  if (ri->count > 0)
  {
    /* If app tries to read outside of the array. */
    if (ri->count <= ndx)
    {
      return(0);
    }

    /* calculate bit offset */
    bofs = ndx * ri->size;
    bofs += shift;
    /* calculate byte offset + shift pair from bit offset. */
    data += bofs / 8;
    shift = (uint8_t) (bofs % 8);
  }
  /* read data bytes in little endian order */
  for (x = 0; x < ((ri->size & 0x7) ? (ri->size / 8) + 1 : (ri->size / 8)); x++)
  {
    val = (uint32_t) (*data << (x * 8));
  }
  val = (val >> shift) & ((1 << ri->size) - 1);

  if (val < ri->logical_min || val > ri->logical_max)
  {
    return (0);
  }

  /* convert logical value to physical value */
  /* See if the number is negative or not. */
  if ((ri->sign) && (val & (1 << (ri->size - 1))))
  {
    /* yes, so sign extend value to 32 bits. */
    int vs = (int) ((-1 & ~((1 << (ri->size)) - 1)) | val);

    if (ri->resolution == 1)
    {
      return ((uint32_t) vs);
    }
    return ((uint32_t) (vs * ri->resolution));
  }
  else
  {
    if (ri->resolution == 1)
    {
      return (val);
    }
    return (val * ri->resolution);
  }
}
