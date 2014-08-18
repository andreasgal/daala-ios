#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#include "player.h"
#include "daala/daaladec.h"

using namespace std;

enum decode_result {
    DECODE_RESULT_DONE,
    DECODE_RESULT_ERROR,
    DECODE_RESULT_INVALID_OGG,
    DECODE_RESULT_INCOMPLETE,
    DECODE_RESULT_OK
};

enum decode_state {
    DECODE_STATE_NONE,
    DECODE_STATE_HEADER,
    DECODE_STATE_DATA
};

class decode_thread {
    FILE *input;
    string input_path;
    ogg_sync_state oy;
    ogg_stream_state os;
    daala_dec_ctx *dctx;
    daala_info di;
    daala_comment dc;
    decode_state ds;
    bool valid;

    bool stream_init(int serial);
    bool stream_clear();
public:
    decode_thread();
    ~decode_thread();

    bool open(const char *name);
    void close();
    void restart();
    decode_result next_frame(od_img *img);
};

decode_thread::decode_thread() : input(nullptr)
{
}

decode_thread::~decode_thread()
{
    close();
}

bool
decode_thread::open(const char *name)
{
    input = fopen(name, "rb");
    if (!input)
        return false;
    input_path = name;
    ogg_sync_init(&oy);
    valid = false;
    return true;
}

void
decode_thread::close()
{
    if (input) {
        fclose(input);
        input = nullptr;
    }
    ogg_sync_clear(&oy);
}

void
decode_thread::restart()
{
    close();
    open(input_path.c_str());
}

bool
decode_thread::stream_init(int serial)
{
    if (ogg_stream_init(&os, serial) != 0)
        return false;
    daala_info_init(&di);
    daala_comment_init(&dc);
    ds = DECODE_STATE_HEADER;
    return true;
}

bool
decode_thread::stream_clear()
{
    if (ds == DECODE_STATE_NONE)
        return false;
    ds = DECODE_STATE_NONE;
    daala_info_clear(&di);
    daala_comment_clear(&dc);
    if (dctx) {
        daala_decode_free(dctx);
        dctx = NULL;
    }
    ogg_stream_clear(&os);
    return true;
}

decode_result
decode_thread::next_frame(od_img *img) {
    ogg_page page;
    if (ogg_sync_pageout(&oy, &page) != 1)
        return DECODE_RESULT_INCOMPLETE;
    char *buffer = ogg_sync_buffer(&oy, 4096);
    if (!buffer)
        return DECODE_RESULT_ERROR;
    size_t bytes = fread(buffer, 1, 4096, input);
    if (bytes > 0) {
        if (ogg_sync_wrote(&oy, bytes) != 0)
            return DECODE_RESULT_ERROR;
    } else {
        if (!valid)
            return DECODE_RESULT_INVALID_OGG;
        if (!stream_clear())
            return DECODE_RESULT_ERROR;
        return DECODE_RESULT_DONE;
    }
    if (ogg_page_bos(&page)) {
        if (!stream_init(ogg_page_serialno(&page)))
            return DECODE_RESULT_ERROR;
    }
    if (ogg_page_eos(&page)) {
        if (!stream_clear())
            return DECODE_RESULT_ERROR;
        return DECODE_RESULT_DONE;
    }
    if (ogg_stream_pagein(&os, &page) != 0)
        return DECODE_RESULT_ERROR;
    ogg_packet packet;
    if (ogg_stream_packetout(&os, &packet) != 1)
        return DECODE_RESULT_INCOMPLETE;
    int ret;
    daala_setup_info *dsi = nullptr;
    switch (ds) {
    case DECODE_STATE_HEADER:
        ret = daala_decode_header_in(&di, &dc, &dsi, &packet);
        if (ret < 0)
            return DECODE_RESULT_ERROR;
        if (ret != 0)
            break;
        dctx = daala_decode_alloc(&di, dsi);
        if (!dctx)
            return DECODE_RESULT_ERROR;
        daala_setup_free(dsi);
        dsi = nullptr;
        ds = DECODE_STATE_DATA;
        // fall through
    default:
        ret = daala_decode_packet_in(dctx, img, &packet);
        if (ret != 0)
            return DECODE_RESULT_ERROR;
        valid = true;
    }
    return DECODE_RESULT_OK;
}

player::player()
{
    printf("Test!\n");
}

player::~player()
{
}
