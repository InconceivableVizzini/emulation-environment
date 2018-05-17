#ifndef APPFRAME_H
#define APPFRAME_H

#include <QObject>
#include <QWidget>
#include <QFrame>
#include <QToolButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QEvent>
#include <QMouseEvent>
#include <QSettings>
#include <QStyleOption>
#include <QPainter>
#include <QPixmap>
#include <QDebug>

#include "machine.h"
#include "disassemblystatelistwidget.h"
#include "sipainterframebufferview.h"

namespace EE {

enum AppUXMode { emulating, reversing };

class AppFrame : public QFrame
{
    Q_OBJECT
public:
    AppFrame();
    ~AppFrame();
    bool eventFilter(QObject *o, QEvent *e);
    void saveUXSettings();
    void restoreUXSettings();
    void setDisassemblyStateData();
    QWidget *contentAreaWidget();

    DisassemblyStateListWidget *disassemblyArea;
    SIPainterFrameBufferView *painterFrameBufferView;
private:
    QSettings settings;
    QTimer interactionTimer;
    QPoint prevMousePosition;
    QPixmap minPix, helpPix, configurePix, exitPix;
    bool active, mouseDown, left;
    bool right, bottom, top;
    int ee_app_inactive_timeout;
    AppUXMode mode;
    QToolButton *minimize, *help, *configure, *exit;
    QWidget *contentArea;
    QLabel *title;
    QHBoxLayout *titleBarLayout;
    QHBoxLayout *contentAreaLayout;
    QSpacerItem *alignContentLeftSpacer;
    QVBoxLayout *appWindowLayout;
    Machine machine;
public slots:
    void inactiveTimeout();
    void minimizeApp();
    void learnApp();
    void configureApp();
    void powerButtonToggled(bool checked);
    void pauseButtonToggled(bool checked);
    void captureButtonToggled(bool checked);
signals:
    void userActivity(bool active);
    void powerTurnedOn(void);
};

}

#endif // APPFRAME_H
