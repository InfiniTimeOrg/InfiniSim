/**
 * Copyright (c) 2015 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
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
 */

#ifndef NRFX_TWI_H__
#define NRFX_TWI_H__

#include <cstdint>

/**
  * @brief I2C compatible Two-Wire Master Interface with EasyDMA 0 (TWIM0)
  */

struct NRF_TWIM_Type {                    /*!< (@ 0x40003000) TWIM0 Structure                                            */
  uint32_t  TASKS_STARTRX;                /*!< (@ 0x00000000) Start TWI receive sequence                                 */
  uint32_t  RESERVED;
  uint32_t  TASKS_STARTTX;                /*!< (@ 0x00000008) Start TWI transmit sequence                                */
  uint32_t  RESERVED1[2];
  uint32_t  TASKS_STOP;                   /*!< (@ 0x00000014) Stop TWI transaction. Must be issued while the
                                                              TWI master is not suspended.                               */
  uint32_t  RESERVED2;
  uint32_t  TASKS_SUSPEND;                /*!< (@ 0x0000001C) Suspend TWI transaction                                    */
  uint32_t  TASKS_RESUME;                 /*!< (@ 0x00000020) Resume TWI transaction                                     */
  uint32_t  RESERVED3[56];
  uint32_t  EVENTS_STOPPED;               /*!< (@ 0x00000104) TWI stopped                                                */
  uint32_t  RESERVED4[7];
  uint32_t  EVENTS_ERROR;                 /*!< (@ 0x00000124) TWI error                                                  */
  uint32_t  RESERVED5[8];
  uint32_t  EVENTS_SUSPENDED;             /*!< (@ 0x00000148) Last byte has been sent out after the SUSPEND
                                                              task has been issued, TWI traffic is now
                                                              suspended.                                                 */
  uint32_t  EVENTS_RXSTARTED;             /*!< (@ 0x0000014C) Receive sequence started                                   */
  uint32_t  EVENTS_TXSTARTED;             /*!< (@ 0x00000150) Transmit sequence started                                  */
  uint32_t  RESERVED6[2];
  uint32_t  EVENTS_LASTRX;                /*!< (@ 0x0000015C) Byte boundary, starting to receive the last byte           */
  uint32_t  EVENTS_LASTTX;                /*!< (@ 0x00000160) Byte boundary, starting to transmit the last
                                                              byte                                                       */
  uint32_t  RESERVED7[39];
  uint32_t  SHORTS;                       /*!< (@ 0x00000200) Shortcut register                                          */
  uint32_t  RESERVED8[63];
  uint32_t  INTEN;                        /*!< (@ 0x00000300) Enable or disable interrupt                                */
  uint32_t  INTENSET;                     /*!< (@ 0x00000304) Enable interrupt                                           */
  uint32_t  INTENCLR;                     /*!< (@ 0x00000308) Disable interrupt                                          */
  uint32_t  RESERVED9[110];
  uint32_t  ERRORSRC;                     /*!< (@ 0x000004C4) Error source                                               */
  uint32_t  RESERVED10[14];
  uint32_t  ENABLE;                       /*!< (@ 0x00000500) Enable TWIM                                                */
  uint32_t  RESERVED11;
///  TWIM_PSEL_Type PSEL;                    /*!< (@ 0x00000508) Unspecified                                                */
  uint32_t  RESERVED12[5];
  uint32_t  FREQUENCY;                    /*!< (@ 0x00000524) TWI frequency                                              */
  uint32_t  RESERVED13[3];
//  TWIM_RXD_Type RXD;                      /*!< (@ 0x00000534) RXD EasyDMA channel                                        */
//  TWIM_TXD_Type TXD;                      /*!< (@ 0x00000544) TXD EasyDMA channel                                        */
  uint32_t  RESERVED14[13];
  uint32_t  ADDRESS;                      /*!< (@ 0x00000588) Address used in the TWI transfer                           */
};                                              /*!< Size = 1420 (0x58c)                                                       */

#endif // NRFX_TWI_H__
