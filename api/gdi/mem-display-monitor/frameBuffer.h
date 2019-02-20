#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <QWidget>
#include <thread>

class frameBuffer : public QWidget
{
    Q_OBJECT
public:
    explicit frameBuffer();
    virtual ~frameBuffer() override;
    static constexpr int FB_WIDTH = 240;
    static constexpr int FB_HEIGHT = 320;
    unsigned short mem[FB_WIDTH * FB_HEIGHT];

    static int getFB_WIDTH()  {  return FB_WIDTH;  }
    static int getFB_HEIGHT() {  return FB_HEIGHT; }

signals:

public slots:
private:
    void paintEvent(QPaintEvent *) override;
    void timerEvent(QTimerEvent *) override;

    int volatile exiting;
    std::thread thread;
};

#endif // FRAMEBUFFER_H
