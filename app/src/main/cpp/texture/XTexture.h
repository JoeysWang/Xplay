//
// Created by 王越 on 2020/4/30.
//

#ifndef XPLAY_XTEXTURE_H
#define XPLAY_XTEXTURE_H


class XTexture {
public:
    static XTexture *create();
    virtual bool init(void *win, float *matrix  ) = 0;
    virtual void draw(unsigned char *data[],int width[],int height)=0;
};


#endif //XPLAY_XTEXTURE_H
