//
// Created by 王越 on 2020/4/29.
//

#ifndef XPLAY_IOBSERVER_H
#define XPLAY_IOBSERVER_H

#include <vector>
#include <mutex>
#include "data/XData.h"
#include "XThread.h"

//观察者  主体
class IObserver : public XThread {
public:
    //观察者接受数据
    virtual void update(XData *data) {}

    //主体函数
    //添加观察者 线程安全的
    void addObserver(IObserver *observer);

    //通知所有观察者
    void notify(XData *data);

protected:
    std::vector<IObserver *> observers;
    std::mutex mutex;
};


#endif //XPLAY_IOBSERVER_H
