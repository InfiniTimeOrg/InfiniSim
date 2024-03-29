/*
 * Copyright (c) 2010 - 2018, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 * Semiconductor ASA integrated circuit in a product or a software update for
 * such product, must reproduce the above copyright notice, this list of
 * conditions and the following disclaimer in the documentation and/or other
 * materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 * Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 * engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @file     nrf52.h
 * @brief    CMSIS HeaderFile
 * @version  1
 * @date     08. February 2019
 * @note     Generated by SVDConv V3.3.18 on Friday, 08.02.2019 16:48:11
 *           from File 'nrf52.svd',
 *           last modified on Friday, 08.02.2019 15:48:07
 */



/** @addtogroup Nordic Semiconductor
  * @{
  */


/** @addtogroup nrf52
  * @{
  */


#ifndef NRF52_H
#define NRF52_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// define empty to make original WDT struct work
#define __OM
#define __IM
#define __IOM

/* =========================================================================================================================== */
/* ================                                            WDT                                            ================ */
/* =========================================================================================================================== */


/**
  * @brief Watchdog Timer (WDT)
  */

typedef struct {                                /*!< (@ 0x40010000) WDT Structure                                              */
  __OM  uint32_t  TASKS_START;                  /*!< (@ 0x00000000) Start the watchdog                                         */
  __IM  uint32_t  RESERVED[63];
  __IOM uint32_t  EVENTS_TIMEOUT;               /*!< (@ 0x00000100) Watchdog timeout                                           */
  __IM  uint32_t  RESERVED1[128];
  __IOM uint32_t  INTENSET;                     /*!< (@ 0x00000304) Enable interrupt                                           */
  __IOM uint32_t  INTENCLR;                     /*!< (@ 0x00000308) Disable interrupt                                          */
  __IM  uint32_t  RESERVED2[61];
  __IM  uint32_t  RUNSTATUS;                    /*!< (@ 0x00000400) Run status                                                 */
  __IM  uint32_t  REQSTATUS;                    /*!< (@ 0x00000404) Request status                                             */
  __IM  uint32_t  RESERVED3[63];
  __IOM uint32_t  CRV;                          /*!< (@ 0x00000504) Counter reload value                                       */
  __IOM uint32_t  RREN;                         /*!< (@ 0x00000508) Enable register for reload request registers               */
  __IOM uint32_t  CONFIG;                       /*!< (@ 0x0000050C) Configuration register                                     */
  __IM  uint32_t  RESERVED4[60];
  __OM  uint32_t  RR[8];                        /*!< (@ 0x00000600) Description collection[0]: Reload request 0                */
} NRF_WDT_Type;                                 /*!< Size = 1568 (0x620)                                                       */

// simulator specific pointer and initialization function
extern NRF_WDT_Type *NRF_WDT;
void init_NRF_WDT();


/* =========================================================================================================================== */
/* ================                                           POWER                                           ================ */
/* =========================================================================================================================== */


/**
  * @brief Power control (POWER)
  */

typedef struct {                                /*!< (@ 0x40000000) POWER Structure                                            */
//  __IM  uint32_t  RESERVED[30];
//  __OM  uint32_t  TASKS_CONSTLAT;               /*!< (@ 0x00000078) Enable constant latency mode                               */
//  __OM  uint32_t  TASKS_LOWPWR;                 /*!< (@ 0x0000007C) Enable low power mode (variable latency)                   */
//  __IM  uint32_t  RESERVED1[34];
//  __IOM uint32_t  EVENTS_POFWARN;               /*!< (@ 0x00000108) Power failure warning                                      */
//  __IM  uint32_t  RESERVED2[2];
//  __IOM uint32_t  EVENTS_SLEEPENTER;            /*!< (@ 0x00000114) CPU entered WFI/WFE sleep                                  */
//  __IOM uint32_t  EVENTS_SLEEPEXIT;             /*!< (@ 0x00000118) CPU exited WFI/WFE sleep                                   */
//  __IM  uint32_t  RESERVED3[122];
//  __IOM uint32_t  INTENSET;                     /*!< (@ 0x00000304) Enable interrupt                                           */
//  __IOM uint32_t  INTENCLR;                     /*!< (@ 0x00000308) Disable interrupt                                          */
//  __IM  uint32_t  RESERVED4[61];
  __IOM uint32_t  RESETREAS;                    /*!< (@ 0x00000400) Reset reason                                               */
//  __IM  uint32_t  RESERVED5[9];
//  __IM  uint32_t  RAMSTATUS;                    /*!< (@ 0x00000428) Deprecated register - RAM status register                  */
//  __IM  uint32_t  RESERVED6[53];
//  __OM  uint32_t  SYSTEMOFF;                    /*!< (@ 0x00000500) System OFF register                                        */
//  __IM  uint32_t  RESERVED7[3];
//  __IOM uint32_t  POFCON;                       /*!< (@ 0x00000510) Power failure comparator configuration                     */
//  __IM  uint32_t  RESERVED8[2];
//  __IOM uint32_t  GPREGRET;                     /*!< (@ 0x0000051C) General purpose retention register                         */
//  __IOM uint32_t  GPREGRET2;                    /*!< (@ 0x00000520) General purpose retention register                         */
//  __IOM uint32_t  RAMON;                        /*!< (@ 0x00000524) Deprecated register - RAM on/off register (this
//                                                                    register is retained)                                      */
//  __IM  uint32_t  RESERVED9[11];
//  __IOM uint32_t  RAMONB;                       /*!< (@ 0x00000554) Deprecated register - RAM on/off register (this
//                                                                    register is retained)                                      */
//  __IM  uint32_t  RESERVED10[8];
//  __IOM uint32_t  DCDCEN;                       /*!< (@ 0x00000578) DC/DC enable register                                      */
//  __IM  uint32_t  RESERVED11[225];
//  __IOM POWER_RAM_Type RAM[8];                  /*!< (@ 0x00000900) Unspecified                                                */
} NRF_POWER_Type;                               /*!< Size = 2432 (0x980)                                                       */

// simulator specific pointer and initialization function
extern NRF_POWER_Type *NRF_POWER;
void init_NRF_POWER();

#ifdef __cplusplus
}
#endif

#endif /* NRF52_H */


/** @} */ /* End of group nrf52 */

/** @} */ /* End of group Nordic Semiconductor */
