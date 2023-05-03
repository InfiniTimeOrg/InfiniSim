/*

Copyright (c) 2010 - 2018, Nordic Semiconductor ASA

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form, except as embedded into a Nordic
   Semiconductor ASA integrated circuit in a product or a software update for
   such product, must reproduce the above copyright notice, this list of
   conditions and the following disclaimer in the documentation and/or other
   materials provided with the distribution.

3. Neither the name of Nordic Semiconductor ASA nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

4. This software, with or without modification, must only be used with a
   Nordic Semiconductor ASA integrated circuit.

5. Any software provided in binary form under this license must not be reverse
   engineered, decompiled, modified and/or disassembled.

THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef __NRF52_BITS_H
#define __NRF52_BITS_H

// sim: trimmed down original for things we might use in simulator

/*lint ++flb "Enter library region" */

/* Peripheral: SPI */
/* Description: Serial Peripheral Interface 0 */

/* Register: SPI_INTENSET */
/* Description: Enable interrupt */

/* Bit 2 : Write '1' to Enable interrupt for READY event */
//#define SPI_INTENSET_READY_Pos (2UL) /*!< Position of READY field. */
//#define SPI_INTENSET_READY_Msk (0x1UL << SPI_INTENSET_READY_Pos) /*!< Bit mask of READY field. */
//#define SPI_INTENSET_READY_Disabled (0UL) /*!< Read: Disabled */
//#define SPI_INTENSET_READY_Enabled (1UL) /*!< Read: Enabled */
//#define SPI_INTENSET_READY_Set (1UL) /*!< Enable */
//
///* Register: SPI_INTENCLR */
///* Description: Disable interrupt */
//
///* Bit 2 : Write '1' to Disable interrupt for READY event */
//#define SPI_INTENCLR_READY_Pos (2UL) /*!< Position of READY field. */
//#define SPI_INTENCLR_READY_Msk (0x1UL << SPI_INTENCLR_READY_Pos) /*!< Bit mask of READY field. */
//#define SPI_INTENCLR_READY_Disabled (0UL) /*!< Read: Disabled */
//#define SPI_INTENCLR_READY_Enabled (1UL) /*!< Read: Enabled */
//#define SPI_INTENCLR_READY_Clear (1UL) /*!< Disable */
//
///* Register: SPI_ENABLE */
///* Description: Enable SPI */
//
///* Bits 3..0 : Enable or disable SPI */
//#define SPI_ENABLE_ENABLE_Pos (0UL) /*!< Position of ENABLE field. */
//#define SPI_ENABLE_ENABLE_Msk (0xFUL << SPI_ENABLE_ENABLE_Pos) /*!< Bit mask of ENABLE field. */
//#define SPI_ENABLE_ENABLE_Disabled (0UL) /*!< Disable SPI */
//#define SPI_ENABLE_ENABLE_Enabled (1UL) /*!< Enable SPI */
//
///* Register: SPI_PSEL_SCK */
///* Description: Pin select for SCK */
//
///* Bits 31..0 : Pin number configuration for SPI SCK signal */
//#define SPI_PSEL_SCK_PSELSCK_Pos (0UL) /*!< Position of PSELSCK field. */
//#define SPI_PSEL_SCK_PSELSCK_Msk (0xFFFFFFFFUL << SPI_PSEL_SCK_PSELSCK_Pos) /*!< Bit mask of PSELSCK field. */
//#define SPI_PSEL_SCK_PSELSCK_Disconnected (0xFFFFFFFFUL) /*!< Disconnect */
//
///* Register: SPI_PSEL_MOSI */
///* Description: Pin select for MOSI */
//
///* Bits 31..0 : Pin number configuration for SPI MOSI signal */
//#define SPI_PSEL_MOSI_PSELMOSI_Pos (0UL) /*!< Position of PSELMOSI field. */
//#define SPI_PSEL_MOSI_PSELMOSI_Msk (0xFFFFFFFFUL << SPI_PSEL_MOSI_PSELMOSI_Pos) /*!< Bit mask of PSELMOSI field. */
//#define SPI_PSEL_MOSI_PSELMOSI_Disconnected (0xFFFFFFFFUL) /*!< Disconnect */
//
///* Register: SPI_PSEL_MISO */
///* Description: Pin select for MISO */
//
///* Bits 31..0 : Pin number configuration for SPI MISO signal */
//#define SPI_PSEL_MISO_PSELMISO_Pos (0UL) /*!< Position of PSELMISO field. */
//#define SPI_PSEL_MISO_PSELMISO_Msk (0xFFFFFFFFUL << SPI_PSEL_MISO_PSELMISO_Pos) /*!< Bit mask of PSELMISO field. */
//#define SPI_PSEL_MISO_PSELMISO_Disconnected (0xFFFFFFFFUL) /*!< Disconnect */
//
///* Register: SPI_RXD */
///* Description: RXD register */
//
///* Bits 7..0 : RX data received. Double buffered */
//#define SPI_RXD_RXD_Pos (0UL) /*!< Position of RXD field. */
//#define SPI_RXD_RXD_Msk (0xFFUL << SPI_RXD_RXD_Pos) /*!< Bit mask of RXD field. */
//
///* Register: SPI_TXD */
///* Description: TXD register */
//
///* Bits 7..0 : TX data to send. Double buffered */
//#define SPI_TXD_TXD_Pos (0UL) /*!< Position of TXD field. */
//#define SPI_TXD_TXD_Msk (0xFFUL << SPI_TXD_TXD_Pos) /*!< Bit mask of TXD field. */
//
///* Register: SPI_FREQUENCY */
///* Description: SPI frequency */
//
///* Bits 31..0 : SPI master data rate */
//#define SPI_FREQUENCY_FREQUENCY_Pos (0UL) /*!< Position of FREQUENCY field. */
//#define SPI_FREQUENCY_FREQUENCY_Msk (0xFFFFFFFFUL << SPI_FREQUENCY_FREQUENCY_Pos) /*!< Bit mask of FREQUENCY field. */
//#define SPI_FREQUENCY_FREQUENCY_K125 (0x02000000UL) /*!< 125 kbps */
//#define SPI_FREQUENCY_FREQUENCY_K250 (0x04000000UL) /*!< 250 kbps */
//#define SPI_FREQUENCY_FREQUENCY_K500 (0x08000000UL) /*!< 500 kbps */
//#define SPI_FREQUENCY_FREQUENCY_M1 (0x10000000UL) /*!< 1 Mbps */
//#define SPI_FREQUENCY_FREQUENCY_M2 (0x20000000UL) /*!< 2 Mbps */
//#define SPI_FREQUENCY_FREQUENCY_M4 (0x40000000UL) /*!< 4 Mbps */
//#define SPI_FREQUENCY_FREQUENCY_M8 (0x80000000UL) /*!< 8 Mbps */
//
///* Register: SPI_CONFIG */
///* Description: Configuration register */
//
///* Bit 2 : Serial clock (SCK) polarity */
//#define SPI_CONFIG_CPOL_Pos (2UL) /*!< Position of CPOL field. */
//#define SPI_CONFIG_CPOL_Msk (0x1UL << SPI_CONFIG_CPOL_Pos) /*!< Bit mask of CPOL field. */
//#define SPI_CONFIG_CPOL_ActiveHigh (0UL) /*!< Active high */
//#define SPI_CONFIG_CPOL_ActiveLow (1UL) /*!< Active low */
//
///* Bit 1 : Serial clock (SCK) phase */
//#define SPI_CONFIG_CPHA_Pos (1UL) /*!< Position of CPHA field. */
//#define SPI_CONFIG_CPHA_Msk (0x1UL << SPI_CONFIG_CPHA_Pos) /*!< Bit mask of CPHA field. */
//#define SPI_CONFIG_CPHA_Leading (0UL) /*!< Sample on leading edge of clock, shift serial data on trailing edge */
//#define SPI_CONFIG_CPHA_Trailing (1UL) /*!< Sample on trailing edge of clock, shift serial data on leading edge */
//
///* Bit 0 : Bit order */
//#define SPI_CONFIG_ORDER_Pos (0UL) /*!< Position of ORDER field. */
//#define SPI_CONFIG_ORDER_Msk (0x1UL << SPI_CONFIG_ORDER_Pos) /*!< Bit mask of ORDER field. */
//#define SPI_CONFIG_ORDER_MsbFirst (0UL) /*!< Most significant bit shifted out first */
//#define SPI_CONFIG_ORDER_LsbFirst (1UL) /*!< Least significant bit shifted out first */
//
//
///* Peripheral: WDT */
///* Description: Watchdog Timer */
//
///* Register: WDT_INTENSET */
///* Description: Enable interrupt */
//
///* Bit 0 : Write '1' to Enable interrupt for TIMEOUT event */
//#define WDT_INTENSET_TIMEOUT_Pos (0UL) /*!< Position of TIMEOUT field. */
//#define WDT_INTENSET_TIMEOUT_Msk (0x1UL << WDT_INTENSET_TIMEOUT_Pos) /*!< Bit mask of TIMEOUT field. */
//#define WDT_INTENSET_TIMEOUT_Disabled (0UL) /*!< Read: Disabled */
//#define WDT_INTENSET_TIMEOUT_Enabled (1UL) /*!< Read: Enabled */
//#define WDT_INTENSET_TIMEOUT_Set (1UL) /*!< Enable */
//
///* Register: WDT_INTENCLR */
///* Description: Disable interrupt */
//
///* Bit 0 : Write '1' to Disable interrupt for TIMEOUT event */
//#define WDT_INTENCLR_TIMEOUT_Pos (0UL) /*!< Position of TIMEOUT field. */
//#define WDT_INTENCLR_TIMEOUT_Msk (0x1UL << WDT_INTENCLR_TIMEOUT_Pos) /*!< Bit mask of TIMEOUT field. */
//#define WDT_INTENCLR_TIMEOUT_Disabled (0UL) /*!< Read: Disabled */
//#define WDT_INTENCLR_TIMEOUT_Enabled (1UL) /*!< Read: Enabled */
//#define WDT_INTENCLR_TIMEOUT_Clear (1UL) /*!< Disable */
//
///* Register: WDT_RUNSTATUS */
///* Description: Run status */
//
///* Bit 0 : Indicates whether or not the watchdog is running */
//#define WDT_RUNSTATUS_RUNSTATUS_Pos (0UL) /*!< Position of RUNSTATUS field. */
//#define WDT_RUNSTATUS_RUNSTATUS_Msk (0x1UL << WDT_RUNSTATUS_RUNSTATUS_Pos) /*!< Bit mask of RUNSTATUS field. */
//#define WDT_RUNSTATUS_RUNSTATUS_NotRunning (0UL) /*!< Watchdog not running */
//#define WDT_RUNSTATUS_RUNSTATUS_Running (1UL) /*!< Watchdog is running */
//
///* Register: WDT_REQSTATUS */
///* Description: Request status */
//
///* Bit 7 : Request status for RR[7] register */
//#define WDT_REQSTATUS_RR7_Pos (7UL) /*!< Position of RR7 field. */
//#define WDT_REQSTATUS_RR7_Msk (0x1UL << WDT_REQSTATUS_RR7_Pos) /*!< Bit mask of RR7 field. */
//#define WDT_REQSTATUS_RR7_DisabledOrRequested (0UL) /*!< RR[7] register is not enabled, or are already requesting reload */
//#define WDT_REQSTATUS_RR7_EnabledAndUnrequested (1UL) /*!< RR[7] register is enabled, and are not yet requesting reload */
//
///* Bit 6 : Request status for RR[6] register */
//#define WDT_REQSTATUS_RR6_Pos (6UL) /*!< Position of RR6 field. */
//#define WDT_REQSTATUS_RR6_Msk (0x1UL << WDT_REQSTATUS_RR6_Pos) /*!< Bit mask of RR6 field. */
//#define WDT_REQSTATUS_RR6_DisabledOrRequested (0UL) /*!< RR[6] register is not enabled, or are already requesting reload */
//#define WDT_REQSTATUS_RR6_EnabledAndUnrequested (1UL) /*!< RR[6] register is enabled, and are not yet requesting reload */
//
///* Bit 5 : Request status for RR[5] register */
//#define WDT_REQSTATUS_RR5_Pos (5UL) /*!< Position of RR5 field. */
//#define WDT_REQSTATUS_RR5_Msk (0x1UL << WDT_REQSTATUS_RR5_Pos) /*!< Bit mask of RR5 field. */
//#define WDT_REQSTATUS_RR5_DisabledOrRequested (0UL) /*!< RR[5] register is not enabled, or are already requesting reload */
//#define WDT_REQSTATUS_RR5_EnabledAndUnrequested (1UL) /*!< RR[5] register is enabled, and are not yet requesting reload */
//
///* Bit 4 : Request status for RR[4] register */
//#define WDT_REQSTATUS_RR4_Pos (4UL) /*!< Position of RR4 field. */
//#define WDT_REQSTATUS_RR4_Msk (0x1UL << WDT_REQSTATUS_RR4_Pos) /*!< Bit mask of RR4 field. */
//#define WDT_REQSTATUS_RR4_DisabledOrRequested (0UL) /*!< RR[4] register is not enabled, or are already requesting reload */
//#define WDT_REQSTATUS_RR4_EnabledAndUnrequested (1UL) /*!< RR[4] register is enabled, and are not yet requesting reload */
//
///* Bit 3 : Request status for RR[3] register */
//#define WDT_REQSTATUS_RR3_Pos (3UL) /*!< Position of RR3 field. */
//#define WDT_REQSTATUS_RR3_Msk (0x1UL << WDT_REQSTATUS_RR3_Pos) /*!< Bit mask of RR3 field. */
//#define WDT_REQSTATUS_RR3_DisabledOrRequested (0UL) /*!< RR[3] register is not enabled, or are already requesting reload */
//#define WDT_REQSTATUS_RR3_EnabledAndUnrequested (1UL) /*!< RR[3] register is enabled, and are not yet requesting reload */
//
///* Bit 2 : Request status for RR[2] register */
//#define WDT_REQSTATUS_RR2_Pos (2UL) /*!< Position of RR2 field. */
//#define WDT_REQSTATUS_RR2_Msk (0x1UL << WDT_REQSTATUS_RR2_Pos) /*!< Bit mask of RR2 field. */
//#define WDT_REQSTATUS_RR2_DisabledOrRequested (0UL) /*!< RR[2] register is not enabled, or are already requesting reload */
//#define WDT_REQSTATUS_RR2_EnabledAndUnrequested (1UL) /*!< RR[2] register is enabled, and are not yet requesting reload */
//
///* Bit 1 : Request status for RR[1] register */
//#define WDT_REQSTATUS_RR1_Pos (1UL) /*!< Position of RR1 field. */
//#define WDT_REQSTATUS_RR1_Msk (0x1UL << WDT_REQSTATUS_RR1_Pos) /*!< Bit mask of RR1 field. */
//#define WDT_REQSTATUS_RR1_DisabledOrRequested (0UL) /*!< RR[1] register is not enabled, or are already requesting reload */
//#define WDT_REQSTATUS_RR1_EnabledAndUnrequested (1UL) /*!< RR[1] register is enabled, and are not yet requesting reload */
//
///* Bit 0 : Request status for RR[0] register */
//#define WDT_REQSTATUS_RR0_Pos (0UL) /*!< Position of RR0 field. */
//#define WDT_REQSTATUS_RR0_Msk (0x1UL << WDT_REQSTATUS_RR0_Pos) /*!< Bit mask of RR0 field. */
//#define WDT_REQSTATUS_RR0_DisabledOrRequested (0UL) /*!< RR[0] register is not enabled, or are already requesting reload */
//#define WDT_REQSTATUS_RR0_EnabledAndUnrequested (1UL) /*!< RR[0] register is enabled, and are not yet requesting reload */
//
///* Register: WDT_CRV */
///* Description: Counter reload value */
//
///* Bits 31..0 : Counter reload value in number of cycles of the 32.768 kHz clock */
//#define WDT_CRV_CRV_Pos (0UL) /*!< Position of CRV field. */
//#define WDT_CRV_CRV_Msk (0xFFFFFFFFUL << WDT_CRV_CRV_Pos) /*!< Bit mask of CRV field. */
//
///* Register: WDT_RREN */
///* Description: Enable register for reload request registers */
//
///* Bit 7 : Enable or disable RR[7] register */
//#define WDT_RREN_RR7_Pos (7UL) /*!< Position of RR7 field. */
//#define WDT_RREN_RR7_Msk (0x1UL << WDT_RREN_RR7_Pos) /*!< Bit mask of RR7 field. */
//#define WDT_RREN_RR7_Disabled (0UL) /*!< Disable RR[7] register */
//#define WDT_RREN_RR7_Enabled (1UL) /*!< Enable RR[7] register */
//
///* Bit 6 : Enable or disable RR[6] register */
//#define WDT_RREN_RR6_Pos (6UL) /*!< Position of RR6 field. */
//#define WDT_RREN_RR6_Msk (0x1UL << WDT_RREN_RR6_Pos) /*!< Bit mask of RR6 field. */
//#define WDT_RREN_RR6_Disabled (0UL) /*!< Disable RR[6] register */
//#define WDT_RREN_RR6_Enabled (1UL) /*!< Enable RR[6] register */
//
///* Bit 5 : Enable or disable RR[5] register */
//#define WDT_RREN_RR5_Pos (5UL) /*!< Position of RR5 field. */
//#define WDT_RREN_RR5_Msk (0x1UL << WDT_RREN_RR5_Pos) /*!< Bit mask of RR5 field. */
//#define WDT_RREN_RR5_Disabled (0UL) /*!< Disable RR[5] register */
//#define WDT_RREN_RR5_Enabled (1UL) /*!< Enable RR[5] register */
//
///* Bit 4 : Enable or disable RR[4] register */
//#define WDT_RREN_RR4_Pos (4UL) /*!< Position of RR4 field. */
//#define WDT_RREN_RR4_Msk (0x1UL << WDT_RREN_RR4_Pos) /*!< Bit mask of RR4 field. */
//#define WDT_RREN_RR4_Disabled (0UL) /*!< Disable RR[4] register */
//#define WDT_RREN_RR4_Enabled (1UL) /*!< Enable RR[4] register */
//
///* Bit 3 : Enable or disable RR[3] register */
//#define WDT_RREN_RR3_Pos (3UL) /*!< Position of RR3 field. */
//#define WDT_RREN_RR3_Msk (0x1UL << WDT_RREN_RR3_Pos) /*!< Bit mask of RR3 field. */
//#define WDT_RREN_RR3_Disabled (0UL) /*!< Disable RR[3] register */
//#define WDT_RREN_RR3_Enabled (1UL) /*!< Enable RR[3] register */
//
///* Bit 2 : Enable or disable RR[2] register */
//#define WDT_RREN_RR2_Pos (2UL) /*!< Position of RR2 field. */
//#define WDT_RREN_RR2_Msk (0x1UL << WDT_RREN_RR2_Pos) /*!< Bit mask of RR2 field. */
//#define WDT_RREN_RR2_Disabled (0UL) /*!< Disable RR[2] register */
//#define WDT_RREN_RR2_Enabled (1UL) /*!< Enable RR[2] register */
//
///* Bit 1 : Enable or disable RR[1] register */
//#define WDT_RREN_RR1_Pos (1UL) /*!< Position of RR1 field. */
//#define WDT_RREN_RR1_Msk (0x1UL << WDT_RREN_RR1_Pos) /*!< Bit mask of RR1 field. */
//#define WDT_RREN_RR1_Disabled (0UL) /*!< Disable RR[1] register */
//#define WDT_RREN_RR1_Enabled (1UL) /*!< Enable RR[1] register */
//
///* Bit 0 : Enable or disable RR[0] register */
//#define WDT_RREN_RR0_Pos (0UL) /*!< Position of RR0 field. */
//#define WDT_RREN_RR0_Msk (0x1UL << WDT_RREN_RR0_Pos) /*!< Bit mask of RR0 field. */
//#define WDT_RREN_RR0_Disabled (0UL) /*!< Disable RR[0] register */
//#define WDT_RREN_RR0_Enabled (1UL) /*!< Enable RR[0] register */

/* Register: WDT_CONFIG */
/* Description: Configuration register */

/* Bit 3 : Configure the watchdog to either be paused, or kept running, while the CPU is halted by the debugger */
#define WDT_CONFIG_HALT_Pos (3UL) /*!< Position of HALT field. */
#define WDT_CONFIG_HALT_Msk (0x1UL << WDT_CONFIG_HALT_Pos) /*!< Bit mask of HALT field. */
#define WDT_CONFIG_HALT_Pause (0UL) /*!< Pause watchdog while the CPU is halted by the debugger */
#define WDT_CONFIG_HALT_Run (1UL) /*!< Keep the watchdog running while the CPU is halted by the debugger */

/* Bit 0 : Configure the watchdog to either be paused, or kept running, while the CPU is sleeping */
#define WDT_CONFIG_SLEEP_Pos (0UL) /*!< Position of SLEEP field. */
#define WDT_CONFIG_SLEEP_Msk (0x1UL << WDT_CONFIG_SLEEP_Pos) /*!< Bit mask of SLEEP field. */
#define WDT_CONFIG_SLEEP_Pause (0UL) /*!< Pause watchdog while the CPU is sleeping */
#define WDT_CONFIG_SLEEP_Run (1UL) /*!< Keep the watchdog running while the CPU is sleeping */

///* Register: WDT_RR */
///* Description: Description collection[0]:  Reload request 0 */
//
///* Bits 31..0 : Reload request register */
//#define WDT_RR_RR_Pos (0UL) /*!< Position of RR field. */
//#define WDT_RR_RR_Msk (0xFFFFFFFFUL << WDT_RR_RR_Pos) /*!< Bit mask of RR field. */
//#define WDT_RR_RR_Reload (0x6E524635UL) /*!< Value to request a reload of the watchdog timer */


/*lint --flb "Leave library region" */
#endif
