#include "sipainterframebufferview.h"

SIPainterFrameBufferView::SIPainterFrameBufferView(QWidget *parent, MemoryMap *memory_map)
    : QWidget(parent)
{
    memory = memory_map;

    image = QImage(224, 256, QImage::Format_RGB32);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    fb_update_timer = new QTimer(this);
    connect(fb_update_timer,
            &QTimer::timeout,
            this,
            &SIPainterFrameBufferView::updateFrameBufferTexture);
    fb_update_timer->start((1/60)*1000);

    installEventFilter(this);
}
bool SIPainterFrameBufferView::eventFilter(QObject *object, QEvent *event) {
    if(object==this && (event->type()==QEvent::Enter)) {
        setCursor(Qt::ArrowCursor);
        return true;
    }
    return false;
}

void SIPainterFrameBufferView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.drawImage(0, 0, image);
}

void SIPainterFrameBufferView::updateFrameBufferTexture(void)
{
    uchar *scanline;
    for (int i = 0; i < 256 * 224 / 8; i++) {
        const int y = i * 8 / 256;
        const int x = (i * 8) % 256;
        const uint8_t eight_pixels = memory->data[0x2400 + i];

        for (uint8_t bit = 0; bit < 8; bit++)
        {
            const int pixel_x = x + bit;
            const int pixel_y = y;

            // The X and Y coordinates are being swapped because
            // the framebuffer image is rotated 90degrees in memory.
            // The image is rotated in memory because the physical
            // display is rotated -90degrees in the arcade cabinet.

            scanline = image.scanLine(255 - pixel_x);

            ((uint*)scanline)[pixel_y] = 0xff000000; // Black pixel

            if ((eight_pixels >> bit) & 1) {
                ((uint*)scanline)[pixel_y] = 0xffffffff; // White pixel
            }
        }
    }
    update();
}

QSize SIPainterFrameBufferView::minimumSizeHint() const
{
    return QSize(224, 256);
}

QSize SIPainterFrameBufferView::sizeHint() const
{
    return QSize(224, 256);
}
