/*
 * SwitchProType.h
 *
 *  Created on: 2026-06-29
 *      Author: ChiaYen
 */

#ifndef SWITCHPROTYPE_H_
#define SWITCHPROTYPE_H_

#include <stdint.h>

typedef struct
{
  union
  {
    uint8_t buttonGroup1;
    struct
    {
      uint8_t Y :1;
      uint8_t X :1;
      uint8_t B :1;
      uint8_t A :1;
      uint8_t SR1 :1;
      uint8_t SL1 :1;
      uint8_t R :1;
      uint8_t ZR:1;
    };
  };

  union
  {
    uint8_t buttonGroup2;
    struct
    {
      uint8_t MINUS :1;
      uint8_t PLUS :1;
      uint8_t rightStick :1;
      uint8_t leftStick :1;
      uint8_t HOME :1;
      uint8_t CAPTURE :1;
      uint8_t NONE :1;
      uint8_t ChargingGrip:1;
    };
  };

  union
  {
    uint8_t buttonGroup3;
    struct
    {
      uint8_t DOWN :1;
      uint8_t UP :1;
      uint8_t RIGHT :1;
      uint8_t LEFT :1;
      uint8_t SR2 :1;
      uint8_t SL2 :1;
      uint8_t L :1;
      uint8_t ZL:1;
    };
  };

  uint16_t leftStickHorizontal;
  uint16_t leftStickVertical;

  uint16_t rightStickHorizontal;
  uint16_t rightStickVertical;

} SwitchPro_Info_TypeDef;

void switchpro_report_parser(const uint8_t *report, SwitchPro_Info_TypeDef *switchpro_info);

#endif /* SWITCHPROTYPE_H_ */
