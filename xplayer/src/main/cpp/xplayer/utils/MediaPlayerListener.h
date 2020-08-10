//
// Created by 王越 on 2020/8/10.
//

#ifndef XPLAY_MEDIAPLAYERLISTENER_H
#define XPLAY_MEDIAPLAYERLISTENER_H


class MediaPlayerListener {
public:
    virtual void notify(int msg, int ext1, int ext2, void *obj) {}
};


#endif //XPLAY_MEDIAPLAYERLISTENER_H
