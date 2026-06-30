/*
 * DualShock4Type.h
 *
 *  Created on: 2026-06-12
 *      Author: ChiaYen
 */

#ifndef DUALSHOCK4TYPE_H_
#define DUALSHOCK4TYPE_H_

#include <stdint.h>

typedef struct
{
  uint8_t leftX;
  uint8_t leftY;
  uint8_t rightX;
  uint8_t rightY;

  union
  {
    uint8_t buttonGroup1;
    struct
    {
      uint8_t DPAD:4;
      uint8_t SQUARE :1;
      uint8_t CROSS :1;
      uint8_t CIRCLE :1;
      uint8_t TRIANGLE :1;
    };
  };

  struct
  {
    uint8_t UP:1;
    uint8_t RIGHT:1;
    uint8_t DOWN:1;
    uint8_t LEFT:1;
  };

  union
  {
    uint8_t buttonGroup2;
    struct
    {
      uint8_t L1 :1;
      uint8_t R1 :1;
      uint8_t L2 :1;
      uint8_t R2 :1;
      uint8_t SHARE :1;
      uint8_t OPT :1;
      uint8_t L3 :1;
      uint8_t R3 :1;
    };
  };

  union
  {
    uint8_t buttonGroup3;
    struct
    {
      uint8_t PS :1;
      uint8_t TPAD :1;
      uint8_t Counter :6;
    };
  };

  uint8_t triggerL2;
  uint8_t triggerR2;
}
DualShock4_Info_TypeDef;

void dualshock4_report_parser(const uint8_t *report, DualShock4_Info_TypeDef *dualshock4_info);

#endif /* DUALSHOCK4TYPE_H_ */
