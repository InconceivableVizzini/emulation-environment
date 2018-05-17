#include "appframe.h"

using namespace EE;

AppFrame::AppFrame()
{
    mouseDown = false;
    mode = AppUXMode::emulating;

    // Handle user inactivity timeouts
    ee_app_inactive_timeout = settings.value("UX/InactiveTimeout", 300000).toInt(); // 1000*60*5, 5 minutes.
    active = true;
    installEventFilter(this);
    interactionTimer.start(ee_app_inactive_timeout);
    connect(&interactionTimer, SIGNAL(timeout()), this, SLOT(inactiveTimeout()));

    setMouseTracking(true);

    // Get rid of standard window decoration.
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    appWindowLayout = new QVBoxLayout(this);
    titleBarLayout = new QHBoxLayout();

    contentAreaLayout = new QHBoxLayout();

    disassemblyArea = new DisassemblyStateListWidget(this);
    disassemblyArea->setCursor(Qt::ArrowCursor);
    disassemblyArea->setFocus();
    disassemblyArea->setObjectName("disassemblyArea");
    disassemblyArea->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    contentAreaLayout->addWidget(disassemblyArea);

    painterFrameBufferView = new SIPainterFrameBufferView(this, machine.cpu->memory);

    contentAreaLayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));

    contentAreaLayout->addWidget(painterFrameBufferView);

    alignContentLeftSpacer = new QSpacerItem(1, 1, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    contentAreaLayout->addSpacerItem(alignContentLeftSpacer);

    title = new QLabel(this);
    title->setObjectName("windowTitle");
    title->setText("Emulation Environment");
    this->setWindowTitle("Emulation Environment");

    minPix = QPixmap(":/assets/images/minimize-button.png");
    minimize = new QToolButton(this);
    minimize->setCursor(Qt::PointingHandCursor);
    minimize->setIcon(minPix);
    minimize->setMinimumHeight(20);
    helpPix = QPixmap(":/assets/images/help-button.png");
    help = new QToolButton(this);
    help->setCursor(Qt::PointingHandCursor);
    help->setIcon(helpPix);
    help->setMinimumHeight(20);
    configurePix = QPixmap(":/assets/images/settings-button.png");
    configure = new QToolButton(this);
    configure->setCursor(Qt::PointingHandCursor);
    configure->setIcon(configurePix);
    configure->setMinimumHeight(20);
    exitPix = QPixmap(":/assets/images/close-button.png");
    exit = new QToolButton(this);
    exit->setCursor(Qt::PointingHandCursor);
    exit->setIcon(exitPix);
    exit->setMinimumHeight(20);

    titleBarLayout->addWidget(title);
    titleBarLayout->addWidget(minimize);
    titleBarLayout->addWidget(help);
    titleBarLayout->addWidget(configure);
    titleBarLayout->addWidget(exit);
    titleBarLayout->insertStretch(1, 500);
    titleBarLayout->setSpacing(0);
    titleBarLayout->setMargin(0);

    appWindowLayout->addLayout(titleBarLayout);
    appWindowLayout->addLayout(contentAreaLayout);
    appWindowLayout->setSpacing(0);
    appWindowLayout->setMargin(5);

    setLayout(appWindowLayout);

    connect(exit, SIGNAL(clicked()), this, SLOT(close()));
    connect(minimize, SIGNAL(clicked()), this, SLOT(minimizeApp()));
    connect(help, SIGNAL(clicked()), this, SLOT(learnApp()));
    connect(configure, SIGNAL(clicked()), this, SLOT(configureApp()));

    connect(disassemblyArea->powerButton,
            &QAbstractButton::toggled,
            this,
            &AppFrame::powerButtonToggled);

    connect(disassemblyArea->pauseButton,
            &QAbstractButton::toggled,
            this,
            &AppFrame::pauseButtonToggled);

    connect(disassemblyArea->captureButton,
            &QAbstractButton::toggled,
            this,
            &AppFrame::captureButtonToggled);

    connect(this,
            &AppFrame::powerTurnedOn,
            machine.cpu,
            &CPU::emulate);
}
AppFrame::~AppFrame() {
    this->saveUXSettings();
}

void AppFrame::powerButtonToggled(bool checked)
{
    if (checked) {
        disassemblyArea->pauseButton->blockSignals(true);
        disassemblyArea->pauseButton->setChecked(false);
        disassemblyArea->pauseButton->blockSignals(false);
        this->machine.cpu->flags |= (1 << 6); // enable the processor
        emit powerTurnedOn();
    } else {
        disassemblyArea->pauseButton->blockSignals(true);
        disassemblyArea->pauseButton->setChecked(true);
        disassemblyArea->pauseButton->blockSignals(false);
        this->machine.cpu->reset();
        this->machine.cpu->flags &= ~(1 << 6); // disable the processor
    }
}

void AppFrame::pauseButtonToggled(bool checked)
{
    if (checked) {
        disassemblyArea->powerButton->blockSignals(true);
        disassemblyArea->powerButton->setChecked(false);
        disassemblyArea->powerButton->blockSignals(false);
        this->machine.cpu->flags &= ~(1 << 6); // disable the processor
    } else {
        disassemblyArea->powerButton->blockSignals(true);
        disassemblyArea->powerButton->setChecked(true);
        disassemblyArea->powerButton->blockSignals(false);
        this->machine.cpu->flags |= (1 << 6); // enable the processor
        emit powerTurnedOn();
    }
}

void AppFrame::captureButtonToggled(bool checked)
{
    if (checked) {
        this->machine.cpu->flags |= (1 << 8);
    } else {
        this->machine.cpu->flags &= ~(1 << 8);
    }
}

void AppFrame::saveUXSettings()
{
    settings.setValue("UX/ApplicationFrame/Size", this->saveGeometry());
}

void AppFrame::restoreUXSettings()
{
    this->restoreGeometry(settings.value("UX/ApplicationFrame/Size").toByteArray());
}

void AppFrame::setDisassemblyStateData()
{
    disassemblyArea->executed_instructions_list->setModel(this->machine.cpu->executed_instructions);
}

void AppFrame::minimizeApp()
{
    this->showMinimized();
}
void AppFrame::learnApp()
{
}
void AppFrame::configureApp()
{
}
void AppFrame::inactiveTimeout()
{
    active = false;
    emit userActivity(active);
}
QWidget *AppFrame::contentAreaWidget()
{
    return contentArea;
}
bool AppFrame::eventFilter(QObject *o, QEvent *e)
{
    QMouseEvent *me;
    QRect r;
    bool horizontal;
    int x, y;

    // Handle user activity signals.
    switch(e->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::Wheel:
        if (!active) {
            active = true;
            emit userActivity(active);
        }
        interactionTimer.start(ee_app_inactive_timeout);
        break;
    default:
        break;
    }

    // Handle window resizing.
    switch(e->type()) {
    case QEvent::MouseButtonDblClick:
        if (top) this->close();
    case QEvent::MouseButtonPress:
        me = static_cast<QMouseEvent *>(e);
        prevMousePosition = me->pos();
        mouseDown = (me->button() == Qt::LeftButton);
        if (mouseDown) { // Prevent the mousemove filter from resizing the
                         // application frame using erroneous values if mouse
                         // focus moves outside left/right/bottom/top regions.
            x = me->x();
            y = me->y();
            r = rect();
            left = (qAbs(x - r.left()) <= 5);
            right = (qAbs(x - r.right()) <= 5);
            bottom = (qAbs(y - r.bottom()) <= 5);
            top = (qAbs(y - r.top()) <= 15);
            horizontal = left|right;
        }
        break;
    case QEvent::MouseButtonRelease:
        mouseDown = false;
        break;
    case QEvent::MouseMove:
        me = static_cast<QMouseEvent *>(e);
        x = me->x();
        y = me->y();

        if (mouseDown) {
            QRect g;
            int dx, dy;

            if (top) {
                this->move(me->globalPos() - prevMousePosition);
                //setCursor(Qt::ClosedHandCursor);
            } else {
                dx = x - prevMousePosition.x();
                dy = y - prevMousePosition.y();
                g = geometry();
                if (left) g.setLeft(g.left() + dx);
                if (right) g.setRight(g.right() + dx);
                if (bottom) g.setBottom(g.bottom() + dy);
                setGeometry(g);
                prevMousePosition = QPoint((!left?x:prevMousePosition.x()), y);
            }
        } else {
            r = rect();
            left = (qAbs(x - r.left()) <= 5);
            right = (qAbs(x - r.right()) <= 5);
            bottom = (qAbs(y - r.bottom()) <= 5);
            top = (qAbs(y - r.top()) <= 15);
            horizontal = left|right;

            if (horizontal && bottom) {
                if (left) setCursor(Qt::SizeBDiagCursor);
                else setCursor(Qt::SizeFDiagCursor);
            } else if (horizontal) {
                setCursor(Qt::SizeHorCursor);
            } else if (bottom) {
                setCursor(Qt::SizeVerCursor);
            } else if (top) {
                //setCursor(Qt::OpenHandCursor);
            } else {
                setCursor(Qt::ArrowCursor);
            }
        }
        break;
    default:
        break;
    }

    return QObject::eventFilter(o, e);
}
