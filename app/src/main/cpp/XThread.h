//
// Created by 王越 on 2020/4/26.
//

#ifndef XPLAY_XTHREAD_H
#define XPLAY_XTHREAD_H

//c++ 11 线程库
class XThread {

public:
    //启动线程
    virtual void start();

    //安全停止线程，不一定成功
    virtual void stop();

    //入口主函数
    virtual void run() {}

protected:
    bool isExit = false;
    bool isRunning = false;

private:
    void threadMain();
};
void XSleep(int mis);

#endif //XPLAY_XTHREAD_H
