/*
 * This file is a part of: https://github.com/brilliantlabsAR/frame-codebase
 *
 * Authored by: Raj Nakarja / Brilliant Labs Ltd. (raj@brilliant.xyz)
 *              Rohit Rathnam / Silicon Witchery AB (rohit@siliconwitchery.com)
 *              Uma S. Gupta / Techno Exponent (umasankar@technoexponent.com)
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

#include <stdbool.h>
#include <nrfx.h>

bool disable_pin_interrupts_if_enabled(void)
{
    if (NRFX_IRQ_IS_ENABLED(GPIOTE_IRQn))
    {
        NRFX_IRQ_DISABLE(GPIOTE_IRQn);
        return true;
    }
    return false;
}

void enable_pin_interrupts_if(bool was_enabled)
{
    if (was_enabled)
    {
        NRFX_IRQ_ENABLE(GPIOTE_IRQn);
    }
}

bool disable_camera_timer_interrupt_if_enabled(void)
{
    if (NRFX_IRQ_IS_ENABLED(RTC2_IRQn))
    {
        NRFX_IRQ_DISABLE(RTC2_IRQn);
        return true;
    }
    return false;
}

void enable_camera_timer_interrupt_if(bool was_enabled)
{
    if (was_enabled)
    {
        NRFX_IRQ_ENABLE(RTC2_IRQn);
    }
}
