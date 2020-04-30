//
// Created by 王越 on 2020/4/30.
//

#ifndef XPLAY_XEGL_H
#define XPLAY_XEGL_H


class XEGL {
public:
    virtual bool init(void *win) = 0;
    virtual void draw( ) = 0;

    static XEGL *get();

protected:
    XEGL(){}
};



#endif //XPLAY_XEGL_H
