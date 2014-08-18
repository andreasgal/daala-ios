#include <string>
#include <queue>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#include "player.h"
#include "daaladec.h"

using namespace std;

static void check(bool ok)
{
    assert(ok);
}

class sync_queue {
    queue<od_img*> q;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
public:
    sync_queue();
    ~sync_queue();

    void put(od_img *img);
    od_img *get();
};

sync_queue::sync_queue()
{
    check(pthread_mutex_init(&mutex, nullptr) == 0);
    check(pthread_cond_init(&cond, nullptr) == 0);
}

sync_queue::~sync_queue()
{
}

void
sync_queue::put(od_img *img)
{
    pthread_mutex_lock(&mutex);
    q.push(img);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

od_img *
sync_queue::get()
{
    pthread_mutex_lock(&mutex);
    while (q.empty())
        pthread_cond_wait(&cond, &mutex);
    od_img *img = q.front();
    q.pop();
    pthread_mutex_unlock(&mutex);
    return img;

}

class decode_thread {
    string input_path;
    pthread_t thread;
    sync_queue unused;
    sync_queue decoded;
public:
    decode_thread(const char *name);
    ~decode_thread();

    od_img *next_frame();
    void recycle_frame(od_img *img);

    void loop();
};

static void* start_decode_loop(void *arg) {
    reinterpret_cast<decode_thread *>(arg)->loop();
    return nullptr;
}

decode_thread::decode_thread(const char *name) : input_path(name)
{
    check(pthread_create(&thread, nullptr, start_decode_loop, this) == 0);
    for (int n = 0; n < 3; ++n)
        unused.put(new od_img());
}

decode_thread::~decode_thread()
{
}

od_img *
decode_thread::next_frame()
{
    return decoded.get();
}

void
decode_thread::recycle_frame(od_img *img)
{
    unused.put(img);
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
    int x = 0;
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
            od_img *img = unused.get();
            check(daala_decode_packet_in(dctx, img, &packet) == 0);
            decoded.put(img);
            printf("decode! %d\n",x++);
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
    decoder = new decode_thread(name);
    return true;
}

od_img *
player::next_frame()
{
    return decoder->next_frame();
}

void
player::recycle_frame(od_img *img)
{
    decoder->recycle_frame(img);
}
