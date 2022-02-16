// original files copied from nRF5_SDK_15.3.0_59ac345/components/libraries/timer/app_timer.c
// modified to be simulated using SDL2 Timers by NeroBurner

/**
 * Copyright (c) 2012 - 2019, Nordic Semiconductor ASA
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
#include "app_timer.h"
#include <stdexcept>
#include <cstdlib>
#include <cstdint>

//uint32_t constexpr APP_TIMER_TICKS(uint32_t ms) {
//    return static_cast<uint32_t>(
//        static_cast<uint64_t>(ms) * configTICK_RATE_HZ / 1000
//    );
//}

ret_code_t app_timer_init(void) {
    return 0;
}
ret_code_t app_timer_create(app_timer_t *p_timer_id,
                            app_timer_mode_t            mode,
                            app_timer_timeout_handler_t timeout_handler) {
    if (mode != APP_TIMER_MODE_SINGLE_SHOT) {
        throw std::runtime_error("only mode 'APP_TIMER_MODE_SINGLE_SHOT' implemented");
    }
    p_timer_id->handler = timeout_handler;
    return 0;
}
Uint32 timeout_callback_wrapper(Uint32 interval, void *param)
{
  auto* timer_id = static_cast<app_timer_t*>(param);
  timer_id->handler(timer_id->p_context);
  return 0; // cancel timer
}
ret_code_t app_timer_start(app_timer_t &timer_id, uint32_t timeout_ticks, void * p_context) {
    timer_id.p_context = p_context;
    timer_id.sdl_timer_id = SDL_AddTimer(timeout_ticks, timeout_callback_wrapper, (void*)(&timer_id));
    return 0;
}
ret_code_t app_timer_stop(app_timer_t &timer_id) {
    return (SDL_RemoveTimer(timer_id.sdl_timer_id) == SDL_TRUE);
}
