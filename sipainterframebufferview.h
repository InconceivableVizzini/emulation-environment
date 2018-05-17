#ifndef SIPAINTERFRAMEBUFFERVIEW_H
#define SIPAINTERFRAMEBUFFERVIEW_H

#include <QWidget>
#include <QTimer>
#include <QImage>
#include <QPainter>
#include <QDebug>

#include "cpu.h"

class SIPainterFrameBufferView : public QWidget
{
    Q_OBJECT
public:
    SIPainterFrameBufferView(QWidget *parent = nullptr, MemoryMap *memory = 0);

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event);
signals:

public slots:
    void updateFrameBufferTexture(void);

private:
    QImage image;
    QTimer *fb_update_timer;
    MemoryMap *memory;
};

#endif // SIPAINTERFRAMEBUFFERVIEW_H
