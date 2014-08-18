#include <string>
#include <stack>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#include "player.h"
#include "daaladec.h"

using namespace std;

class decode_thread {
    string input_path;
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    stack<od_img*> imgs;

    void recycle_img(od_img *img);
    od_img *wait_img();
public:
    decode_thread(const char *name);
    ~decode_thread();

    void loop();
};

static void check(bool ok)
{
    assert(ok);
}

static void* start_decode_loop(void *arg) {
    reinterpret_cast<decode_thread *>(arg)->loop();
    return nullptr;
}

void
decode_thread::recycle_img(od_img *img)
{
    pthread_mutex_lock(&mutex);
    imgs.push(img);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

od_img *
decode_thread::wait_img()
{
    pthread_mutex_lock(&mutex);
    while (imgs.size() == 0)
        pthread_cond_wait(&cond, &mutex);
    od_img *img = imgs.top();
    imgs.pop();
    pthread_mutex_unlock(&mutex);
    return img;
}

decode_thread::decode_thread(const char *name) : input_path(name)
{
    check(pthread_create(&thread, nullptr, start_decode_loop, this) == 0);
    check(pthread_mutex_init(&mutex, nullptr) == 0);
    check(pthread_cond_init(&cond, nullptr) == 0);
    for (int n = 0; n < 3; ++n)
        recycle_img(new od_img());
}

decode_thread::~decode_thread()
{
}

void
decode_thread::loop()
{
    FILE *input = fopen(input_path.c_str(), "rb");
    if (!input)
        return;
    ogg_sync_state oy;
    ogg_sync_init(&oy);
    ogg_stream_state os;
    daala_info di;
    daala_comment dc;
    daala_setup_info *dsi = nullptr;
    bool header = true;
    daala_dec_ctx *dctx = nullptr;
    while (true) {
        ogg_page page;
        while (ogg_sync_pageout(&oy, &page) != 1) {
            if (feof(stdin))
                return;
            char *buffer = ogg_sync_buffer(&oy, 4096);
            size_t bytes = fread(buffer, 1, 4096, input);
            check(ogg_sync_wrote(&oy, bytes) == 0);
        }
        if (ogg_page_bos(&page)) {
            check(ogg_stream_init(&os, ogg_page_serialno(&page)) == 0);
            daala_info_init(&di);
            daala_comment_init(&dc);
        }
        check(ogg_stream_pagein(&os, &page) == 0);
        ogg_packet packet;
        while (ogg_stream_packetout(&os, &packet) == 1) {
            if (header) {
                int ret;
                check((ret = daala_decode_header_in(&di, &dc, &dsi, &packet)) >= 0);
                if (ret)
                    continue;
                check((dctx = daala_decode_alloc(&di, dsi)) != nullptr);
                header = false;
            }
            od_img *img = wait_img();
            check(daala_decode_packet_in(dctx, img, &packet) == 0);
            printf("decode!\n");
        }
    }
    check(ogg_sync_clear(&oy) == 0);
    fclose(input);
}

player::player()
{
}

player::~player()
{
}

bool
player::open(const char *name)
{
    decode = new decode_thread(name);
    return true;
}

