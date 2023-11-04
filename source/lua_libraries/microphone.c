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

#include <math.h>
#include <stdint.h>
#include "error_logging.h"
#include "lauxlib.h"
#include "lua.h"
#include "nrfx_config.h"
#include "nrfx_log.h"
#include "nrfx_pdm.h"
#include "pinout.h"
#include <haly/nrfy_pdm.h>
#include <haly/nrfy_gpio.h>

static lua_Number seconds;
static lua_Integer sample_rate;
static lua_Integer bit_depth = 16;

static const size_t fifo_total_size = 1000000;
static struct fifo
{
    int16_t buffer[fifo_total_size];
    size_t chunk_size;
    size_t head;
    size_t target_head;
    size_t tail;
} fifo = {
    .head = 0,
    .chunk_size = 100,
    .target_head = 0,
    .tail = 0,
};

static nrfy_pdm_config_t config = {
    .mode = NRF_PDM_MODE_MONO,
    .edge = NRF_PDM_EDGE_LEFTRISING,
    .pins =
        {
            .clk_pin = MICROPHONE_CLOCK_PIN,
            .din_pin = MICROPHONE_DATA_PIN,
        },
    .clock_freq = NRF_PDM_FREQ_1032K,
    .gain_l = NRF_PDM_GAIN_DEFAULT,
    .gain_r = NRF_PDM_GAIN_DEFAULT,
    .ratio = NRF_PDM_RATIO_64X,
    .skip_psel_cfg = false,
};

void PDM_IRQHandler(void)
{
    uint32_t evt_mask = nrfy_pdm_events_process(
        NRF_PDM0,
        NRFY_EVENT_TO_INT_BITMASK(NRF_PDM_EVENT_STARTED),
        NULL);

    if (evt_mask & NRFY_EVENT_TO_INT_BITMASK(NRF_PDM_EVENT_STARTED))
    {
        fifo.head += fifo.chunk_size;

        if (fifo.head > fifo_total_size)
        {
            fifo.head = 0;
        }

        if (fifo.head % 1000 == 0)
            LOG("Setting buffer to: fifo.buffer[%u]", fifo.head);

        nrfy_pdm_buffer_t buffer = {
            .length = fifo.chunk_size,
            .p_buff = fifo.buffer + fifo.head};

        nrfy_pdm_buffer_set(NRF_PDM0, &buffer);

        // If overflow, force completion on this sample
        // TODO this corrupts the one chunk at the tail
        if (fifo.head == fifo.tail)
        {
            LOG("FIFO write overflow");
            fifo.target_head = fifo.head;
        }

        if (fifo.head == fifo.target_head)
        {
            LOG("Head is now at: %u", fifo.head);
            nrfy_pdm_abort(NRF_PDM0, NULL);
        }
    }
}

static int frame_microphone_record(lua_State *L)
{
    luaL_checknumber(L, 1);
    seconds = lua_tonumber(L, 1);

    luaL_checkinteger(L, 2);
    sample_rate = lua_tointeger(L, 2);
    switch (sample_rate)
    {
    case 20000:
    case 10000:
    case 5000:
        config.clock_freq = NRF_PDM_FREQ_1280K;
        config.ratio = NRF_PDM_RATIO_64X;
        break;

    case 16000:
    case 8000:
    case 4000:
        config.clock_freq = NRF_PDM_FREQ_1280K;
        config.ratio = NRF_PDM_RATIO_80X;
        break;

    case 12500:
        config.clock_freq = NRF_PDM_FREQ_1000K;
        config.ratio = NRF_PDM_RATIO_80X;
        break;

    default:
        luaL_error(L, "invalid sample rate");
        break;
    }

    if (lua_gettop(L) > 2)
    {
        luaL_checkinteger(L, 3);
        bit_depth = lua_tointeger(L, 3);
        if (bit_depth != 16 && bit_depth != 8 && bit_depth != 4)
        {
            luaL_error(L, "invalid bit depth");
        }
    }

    size_t requested_samples = (size_t)ceil(seconds * sample_rate);

    // TODO round up to nearest chunksize

    if (requested_samples > fifo_total_size)
    {
        luaL_error(L, "exceeded maximum buffer size of %d", fifo_total_size);
    }

    fifo.target_head += requested_samples;

    if (fifo.target_head > fifo_total_size)
    {
        fifo.target_head -= fifo_total_size;
    }

    LOG("New target head at: %d", fifo.target_head);

    // TODO do we want to add gain controls?

    // nrfy_pdm_disable(NRF_PDM0);
    nrfy_pdm_periph_configure(NRF_PDM0, &config);

    // nrfy_pdm_int_init(NRF_PDM0,
    //                   NRF_PDM_INT_STARTED,
    //                   NRFX_PDM_DEFAULT_CONFIG_IRQ_PRIORITY,
    //                   true);

    nrfy_pdm_buffer_t buffer = {
        .length = fifo.chunk_size,
        .p_buff = fifo.buffer + fifo.head};

    nrfy_pdm_buffer_set(NRF_PDM0, &buffer);

    // nrfy_pdm_enable(NRF_PDM0);
    nrfy_pdm_start(NRF_PDM0, NULL);

    return 0;
}

static int frame_microphone_read(lua_State *L)
{
    // TODO get requested number of samples

    // TODO limit max number of samples which can be read out

    // TODO figure out number of samples available, return nil if none available
    // TODO adjust for byte packing
    lua_createtable(L, 10, 0);

    // TODO populate table with samples from tail
    for (int i = 0; i < 10; i++)
    {
        lua_pushinteger(L, i);
        lua_seti(L, -2, i);
    }

    return 1;
}

void open_frame_microphone_library(lua_State *L)
{
    if (fifo_total_size % fifo.chunk_size)
    {
        error_with_message("chunks don't fit evenly into fifo");
    }

    nrfy_gpio_pin_clear(config.pins.clk_pin);
    nrfy_gpio_cfg_output(config.pins.clk_pin);
    nrfy_gpio_cfg_input(config.pins.din_pin, NRF_GPIO_PIN_NOPULL);

    nrfy_pdm_int_init(NRF_PDM0,
                      NRF_PDM_INT_STARTED,
                      NRFX_PDM_DEFAULT_CONFIG_IRQ_PRIORITY,
                      true);

    nrfy_pdm_enable(NRF_PDM0);

    lua_getglobal(L, "frame");

    lua_newtable(L);

    lua_pushcfunction(L, frame_microphone_record);
    lua_setfield(L, -2, "record");

    lua_pushcfunction(L, frame_microphone_read);
    lua_setfield(L, -2, "read");

    lua_setfield(L, -2, "microphone");

    lua_pop(L, 1);
}
