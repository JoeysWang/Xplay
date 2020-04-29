//
// Created by 王越 on 2020/4/29.
//

#ifndef XPLAY_IDECODE_H
#define XPLAY_IDECODE_H

#include "../data/XParameter.h"
#include "../IObserver.h"

//解码接口 支持硬解码
class IDecode :public IObserver{
public:
    //打开解码器
    virtual bool open(XParameter parameter)=0;


};


#endif //XPLAY_IDECODE_H
