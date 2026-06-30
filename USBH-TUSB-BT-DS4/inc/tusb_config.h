/*
 * tusb_config.h
 *
 *  Created on: 2026-06-10
 *      Author: ChiaYen
 */

#ifndef TUSB_CONFIG_H_
#define TUSB_CONFIG_H_

#define CFG_TUSB_MCU      OPT_MCU_STM32F4
#define CFG_TUSB_OS       OPT_OS_NONE
#define CFG_TUSB_DEBUG    0
#define CFG_TUH_API_EDPT_XFER 1

#define CFG_TUH_ENABLED   1
#define CFG_TUH_BT        1

#define BOARD_TUH_RHPORT  1

#define CFG_TUSB_RHPORT0_MODE (OPT_MODE_HOST | OPT_MODE_FULL_SPEED)

#endif /* TUSB_CONFIG_H_ */
