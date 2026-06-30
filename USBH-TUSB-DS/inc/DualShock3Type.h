/*
 * DualShock3Type.h
 *
 *  Created on: 2026-06-15
 *      Author: ChiaYen
 */

#ifndef DUALSHOCK3TYPE_H_
#define DUALSHOCK3TYPE_H_

#include <stdint.h>

typedef struct
{
  union
  {
    uint8_t buttonGroup1;
    struct
    {
      uint8_t SELECT:1;
      uint8_t L3:1;
      uint8_t R3:1;
      uint8_t START:1;
      uint8_t UP:1;
      uint8_t RIGHT:1;
      uint8_t DOWN:1;
      uint8_t LEFT:1;
    };
  };

  union
  {
    uint8_t buttonGroup2;
    struct
    {
      uint8_t L2:1;
      uint8_t R2:1;
      uint8_t L1:1;
      uint8_t R1:1;
      uint8_t TRIANGLE:1;
      uint8_t CIRCLE:1;
      uint8_t CROSS:1;
      uint8_t SQUARE:1;
    };
  };

  union
  {
    uint8_t buttonGroup3;
    struct
    {
      uint8_t PS:1;
    };
  };

  uint8_t leftX;
  uint8_t leftY;
  uint8_t rightX;
  uint8_t rightY;

  uint8_t analogUp;
  uint8_t analogRight;
  uint8_t analogDown;
  uint8_t analogLeft;
  uint8_t analogL2;
  uint8_t analogR2;
  uint8_t analogL1;
  uint8_t analogR1;
  uint8_t analogTriangle;
  uint8_t analogCircle;
  uint8_t analogCross;
  uint8_t analogSquare;

  uint16_t accelerometerX;
  uint16_t accelerometerY;
  uint16_t accelerometerZ;

  uint16_t gyroscope;
}
DualShock3_Info_TypeDef;

void dualshock3_report_parser(const uint8_t *report, DualShock3_Info_TypeDef *dualshock3_info);

#endif /* DUALSHOCK3TYPE_H_ */
