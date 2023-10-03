/*
 * This file is a part https://github.com/brilliantlabsAR/frame-codebase
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

#include "error_helpers.h"
#include "interprocessor_messaging.h"
#include "nrfx_log.h"

#ifdef NRF5340_XXAA_APPLICATION
static const char *core = "Application";
#elif NRF5340_XXAA_NETWORK
static const char *core = "Network";
#endif

static void issue_reset(void)
{
#ifdef NRF5340_XXAA_APPLICATION
    NVIC_SystemReset();
#elif NRF5340_XXAA_NETWORK
    message_t reset = MESSAGE_WITHOUT_PAYLOAD(NETWORK_CORE_ERROR);
    push_message(reset);
#endif
}

const char *lookup_error_code(uint32_t error_code)
{
    switch (error_code)
    {
    case NRFX_SUCCESS:
        return "NRFX_SUCCESS";

    case NRFX_ERROR_INTERNAL:
        return "NRFX_ERROR_INTERNAL";

    case NRFX_ERROR_NO_MEM:
        return "NRFX_ERROR_NO_MEM";

    case NRFX_ERROR_NOT_SUPPORTED:
        return "NRFX_ERROR_NOT_SUPPORTED";

    case NRFX_ERROR_INVALID_PARAM:
        return "NRFX_ERROR_INVALID_PARAM";

    case NRFX_ERROR_INVALID_STATE:
        return "NRFX_ERROR_INVALID_STATE";

    case NRFX_ERROR_INVALID_LENGTH:
        return "NRFX_ERROR_INVALID_LENGTH";

    case NRFX_ERROR_TIMEOUT:
        return "NRFX_ERROR_TIMEOUT";

    case NRFX_ERROR_FORBIDDEN:
        return "NRFX_ERROR_FORBIDDEN";

    case NRFX_ERROR_NULL:
        return "NRFX_ERROR_NULL";

    case NRFX_ERROR_INVALID_ADDR:
        return "NRFX_ERROR_INVALID_ADDR";

    case NRFX_ERROR_BUSY:
        return "NRFX_ERROR_BUSY";

    case NRFX_ERROR_ALREADY_INITIALIZED:
        return "NRFX_ERROR_ALREADY_INITIALIZED";

    case NRFX_ERROR_DRV_TWI_ERR_OVERRUN:
        return "NRFX_ERROR_DRV_TWI_ERR_OVERRUN";

    case NRFX_ERROR_DRV_TWI_ERR_ANACK:
        return "NRFX_ERROR_DRV_TWI_ERR_ANACK";

    case NRFX_ERROR_DRV_TWI_ERR_DNACK:
        return "NRFX_ERROR_DRV_TWI_ERR_DNACK";

    default:
        return "UNKOWN_ERROR";
    }
}

void _app_err(nrfx_err_t error_code, const char *file, const int line)
{
    if (0xF000FFFF & (error_code))
    {
        if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)
        {
            LOG("%s core crashed at %s:%u. %s", core, file, line,
                lookup_error_code(error_code));
            __BKPT();
        }
        issue_reset();
    }
}

void _app_err_message(const char *message, const char *file, const int line)
{
    if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)
    {
        LOG("%s core crashed at %s:%u. %s", core, file, line, message);
        __BKPT();
    }
    issue_reset();
}

void HardFault_Handler(void)
{
    app_err_message("HARD_FAULT");
}