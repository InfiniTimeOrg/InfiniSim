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

#ifndef NRF_GPIO_H__
#define NRF_GPIO_H__

#include <cstdint>

/**
 * @brief Enumerator used for selecting the pin to be pulled down or up at the time of pin configuration.
 */
// lv_sim: values copied from nrf52810_bifiellds.h
enum nrf_gpio_pin_pull_t
{
    NRF_GPIO_PIN_NOPULL   = 0, // GPIO_PIN_CNF_PULL_Disabled, ///<  Pin pull-up resistor disabled.
    NRF_GPIO_PIN_PULLDOWN = 1, // GPIO_PIN_CNF_PULL_Pulldown, ///<  Pin pull-down resistor enabled.
    NRF_GPIO_PIN_PULLUP   = 3, // GPIO_PIN_CNF_PULL_Pullup,   ///<  Pin pull-up resistor enabled.
};
constexpr uint16_t GPIO_PIN_CNF_PULL_Pulldown = 1;
constexpr uint16_t GPIO_PIN_CNF_PULL_Pullup = 3;

using nrf_gpiote_polarity_t = uint32_t;
constexpr uint16_t NRF_GPIOTE_POLARITY_HITOLO = 2;
constexpr uint16_t NRF_GPIOTE_POLARITY_TOGGLE = 3;

using nrfx_gpiote_pin_t = uint32_t;
typedef void (*nrfx_gpiote_event_handler_t)(nrfx_gpiote_pin_t, nrf_gpiote_polarity_t);

using nrf_gpio_pin_sense_t = uint32_t;
constexpr uint16_t GPIO_PIN_CNF_SENSE_Low = 3;

struct nrfx_gpiote_in_config_t {
  bool skip_gpio_setup = false;
  bool hi_accuracy = false;
  bool is_watcher = false;
  nrf_gpiote_polarity_t sense;
  nrf_gpio_pin_pull_t pull;
};

/**
 * @brief Function for resetting pin configuration to its default state.
 *
 * @param pin_number Specifies the pin number.
 */
void nrf_gpio_cfg_default(uint32_t pin_number);

/**
 * @brief Function for setting a GPIO pin.
 *
 * Note that the pin must be configured as an output for this function to have any effect.
 *
 * @param pin_number Specifies the pin number to set.
 */
void nrf_gpio_pin_set(uint32_t pin_number);

// read pin stub, intended to forward right mouse button as PinMap::Button
uint32_t nrf_gpio_pin_read(uint32_t pin_number);

/**
 * @brief Function for configuring the given GPIO pin number as output, hiding inner details.
 *        This function can be used to configure a pin as simple output with gate driving GPIO_PIN_CNF_DRIVE_S0S1 (normal cases).
 *
 * @param pin_number Specifies the pin number.
 *
 * @note  Sense capability on the pin is disabled and input is disconnected from the buffer as the pins are configured as output.
 */
void nrf_gpio_cfg_output(uint32_t pin_number);

/**
 * @brief Function for clearing a GPIO pin.
 *
 * Note that the pin must be configured as an output for this
 * function to have any effect.
 *
 * @param pin_number Specifies the pin number to clear.
 */
void nrf_gpio_pin_clear(uint32_t pin_number);

/**
 * @brief Function for configuring the GPIO pin range as input pins with given initial value set, hiding inner details.
 *        This function can be used to configure pin range as simple input.
 *
 * @param pin_range_start Specifies the start number (inclusive) in the range of pin numbers to be configured (allowed values 0-30).
 *
 * @param pin_range_end Specifies the end number (inclusive) in the range of pin numbers to be configured (allowed values 0-30).
 *
 * @param pull_config State of the pin range pull resistor (no pull, pulled down, or pulled high).
 *
 * @note  For configuring only one pin as input, use @ref nrf_gpio_cfg_input.
 *        Sense capability on the pin is disabled and input is connected to buffer so that the GPIO->IN register is readable.
 */
void nrf_gpio_range_cfg_input(uint32_t            pin_range_start,
                              uint32_t            pin_range_end,
                              nrf_gpio_pin_pull_t pull_config);

/**
 * @brief Function for configuring the given GPIO pin number as input, hiding inner details.
 *        This function can be used to configure a pin as simple input.
 *
 * @param pin_number Specifies the pin number.
 * @param pull_config State of the pin range pull resistor (no pull, pulled down, or pulled high).
 *
 * @note  Sense capability on the pin is disabled and input is connected to buffer so that the GPIO->IN register is readable.
 */
void nrf_gpio_cfg_input(uint32_t pin_number, nrf_gpio_pin_pull_t pull_config);

void nrfx_gpiote_in_init(uint32_t pin_number, nrfx_gpiote_in_config_t *config, nrfx_gpiote_event_handler_t);
void nrfx_gpiote_in_event_enable(uint32_t pin_number, bool enable);
void nrf_gpio_cfg_sense_input(uint32_t pin_number, nrf_gpio_pin_pull_t pin_pull, nrf_gpio_pin_sense_t sense);

bool nrfx_gpiote_is_init();
void nrfx_gpiote_init();

void APP_GPIOTE_INIT(uint32_t max_users);

#endif // NRF_GPIO_H__
