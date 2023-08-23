/*
 * This file is part of the MicroPython for Frame project:
 *      https://github.com/brilliantlabsAR/frame-micropython
 *
 * Authored by: Rohit Rathnam / Silicon Witchery AB (rohit@siliconwitchery.com)
 *              Raj Nakarja / Brilliant Labs Ltd. (raj@brilliant.xyz)
 *
 * ISC Licence
 *
 * Copyright © 2023 Brilliant Labs Ltd.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
/*
 * Copyright (c) 2017 - 2023, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "error_helpers.h"
#include "nrfx.h"
#include "nrfx_log.h"
#include <soc/nrfx_atomic.h>
#include <soc/nrfx_coredep.h>

#define NRFX_ASSERT(expression) \
    do                          \
    {                           \
        if ((expression) == 0)  \
        {                       \
            app_err(ASSERT);    \
        }                       \
    } while (0)

#define NRFX_STATIC_ASSERT(expression) \
    _Static_assert(expression, "unspecified message")

#define NRFX_IRQ_PRIORITY_SET(irq_number, priority) \
    NVIC_SetPriority(irq_number, priority)

#define NRFX_IRQ_ENABLE(irq_number) \
    NVIC_EnableIRQ(irq_number)

#define NRFX_IRQ_IS_ENABLED(irq_number) \
    (0 != (NVIC->ISER[irq_number / 32] & (1UL << (irq_number % 32))))

#define NRFX_IRQ_DISABLE(irq_number) \
    NVIC_DisableIRQ(irq_number)

#define NRFX_IRQ_PENDING_SET(irq_number) \
    NVIC_SetPendingIRQ(irq_number)

#define NRFX_IRQ_PENDING_CLEAR(irq_number) \
    NVIC_ClearPendingIRQ(irq_number)

#define NRFX_IRQ_IS_PENDING(irq_number) \
    NVIC_GetPendingIRQ(irq_number)

#define NRFX_CRITICAL_SECTION_ENTER()
// TODO

#define NRFX_CRITICAL_SECTION_EXIT()
// TODO

#define NRFX_DELAY_DWT_BASED 0

#define NRFX_DELAY_US(us_time) \
    nrfx_coredep_delay_us(us_time)

#define nrfx_atomic_t \
    nrfx_atomic_u32_t

#define NRFX_ATOMIC_FETCH_STORE(p_data, value) \
    nrfx_atomic_u32_fetch_store(p_data, value)

#define NRFX_ATOMIC_FETCH_OR(p_data, value) \
    nrfx_atomic_u32_fetch_or(p_data, value)

#define NRFX_ATOMIC_FETCH_AND(p_data, value) \
    nrfx_atomic_u32_fetch_and(p_data, value)

#define NRFX_ATOMIC_FETCH_XOR(p_data, value) \
    nrfx_atomic_u32_fetch_xor(p_data, value)

#define NRFX_ATOMIC_FETCH_ADD(p_data, value) \
    nrfx_atomic_u32_fetch_add(p_data, value)

#define NRFX_ATOMIC_FETCH_SUB(p_data, value) \
    nrfx_atomic_u32_fetch_sub(p_data, value)

#define NRFX_CUSTOM_ERROR_CODES 0

#define NRFX_EVENT_READBACK_ENABLED 1

#define NRFY_CACHE_WB(p_buffer, size) \
    do                                \
    {                                 \
        (void)p_buffer;               \
        (void)size;                   \
    } while (0)

#define NRFY_CACHE_INV(p_buffer, size) \
    do                                 \
    {                                  \
        (void)p_buffer;                \
        (void)size;                    \
    } while (0)

#define NRFY_CACHE_WBINV(p_buffer, size) \
    do                                   \
    {                                    \
        (void)p_buffer;                  \
        (void)size;                      \
    } while (0)

#define NRFX_DPPI_CHANNELS_USED 0

#define NRFX_DPPI_GROUPS_USED 0

#define NRFX_PPI_CHANNELS_USED 0

#define NRFX_PPI_GROUPS_USED 0

#define NRFX_GPIOTE_CHANNELS_USED 0

#define NRFX_EGUS_USED 0

#define NRFX_TIMERS_USED 0