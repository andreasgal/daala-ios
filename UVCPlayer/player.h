#ifndef __UVCPlayer__player__
#define __UVCPlayer__player__

class decode_thread;
class od_img;

class player {
    decode_thread *decoder;
public:
    player();
    ~player();

    bool open(const char *name);
    od_img *next_frame();
    void recycle_frame(od_img *img);
};


#endif /* defined(__UVCPlayer__player__) */
