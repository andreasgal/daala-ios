#ifndef __UVCPlayer__player__
#define __UVCPlayer__player__

class decode_thread;

class player {
    decode_thread *decode;
public:
    player();
    ~player();

    bool open(const char *name);
};


#endif /* defined(__UVCPlayer__player__) */
