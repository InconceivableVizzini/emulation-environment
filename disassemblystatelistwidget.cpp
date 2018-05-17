#include "disassemblystatelistwidget.h"

DisassemblyStateListWidget::DisassemblyStateListWidget(QWidget *parent) :
    QWidget(parent), executed_instructions_list(new QListView(this))
{
    layout = new QVBoxLayout(this);

    powerButton = new QToolButton(this);
    QIcon powerButtonIcon;
    powerButtonIcon.addPixmap(QPixmap("://assets/images/power-icon-on.svg"), QIcon::Normal, QIcon::On);
    powerButtonIcon.addPixmap(QPixmap("://assets/images/power-icon-off.svg"), QIcon::Normal, QIcon::Off);
    powerButton->setIcon(powerButtonIcon);
    powerButton->setIconSize(QSize(24, 24));
    powerButton->setCheckable(true);

    pauseButton = new QToolButton(this);
    QIcon pauseButtonIcon;
    pauseButtonIcon.addPixmap(QPixmap("://assets/images/pause-icon-on.svg"), QIcon::Normal, QIcon::On);
    pauseButtonIcon.addPixmap(QPixmap("://assets/images/pause-icon-off.svg"), QIcon::Normal, QIcon::Off);
    pauseButton->setIcon(pauseButtonIcon);
    pauseButton->setIconSize(QSize(24, 24));
    pauseButton->setCheckable(true);

    captureButton = new QToolButton(this);
    QIcon captureButtonIcon;
    captureButtonIcon.addPixmap(QPixmap("://assets/images/capture-icon-on.svg"), QIcon::Normal, QIcon::On);
    captureButtonIcon.addPixmap(QPixmap("://assets/images/capture-icon-off.svg"), QIcon::Normal, QIcon::Off);
    captureButton->setIcon(captureButtonIcon);
    captureButton->setIconSize(QSize(24, 24));
    captureButton->setCheckable(true);

    stepButtonOn = QIcon(QPixmap("://assets/images/step-icon-on.svg"));
    stepButtonOff = QIcon(QPixmap("://assets/images/step-icon-off.svg"));

    stepButton = new QPushButton(this);
    stepButton->setObjectName("disassemblystatelistwidgetstepbutton");
    stepButton->setIcon(stepButtonOff);
    stepButton->setIconSize(QSize(24, 24));
    stepButton->setFlat(true);
    stepButton->setFocusPolicy(Qt::NoFocus);

    connect(stepButton,
            SIGNAL(pressed()),
            this,
            SLOT(stepButtonPressed()));

    connect(stepButton,
            &QAbstractButton::pressed,
            this,
            &DisassemblyStateListWidget::stepButtonPressed);
    connect(stepButton,
            &QAbstractButton::released,
            this,
            &DisassemblyStateListWidget::stepButtonReleased);

    alignButtonsLeftSpacer = new QSpacerItem(1, 1, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    eeControlsButtonGroupLayout = new QHBoxLayout();
    eeControlsButtonGroupLayout->addWidget(powerButton);
    eeControlsButtonGroupLayout->addWidget(pauseButton);
    eeControlsButtonGroupLayout->addWidget(captureButton);
    eeControlsButtonGroupLayout->addSpacerItem(alignButtonsLeftSpacer);
    eeControlsButtonGroupLayout->addWidget(stepButton);

    executed_instructions_list->setUniformItemSizes(true);
    executed_instructions_list->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    executed_instructions_list->setFixedWidth(200);
    executed_instructions_list->setMinimumHeight(222);
    executed_instructions_list->show();

    layout->addLayout(eeControlsButtonGroupLayout);
    layout->addWidget(executed_instructions_list);

    setLayout(layout);
}

void DisassemblyStateListWidget::stepButtonPressed()
{
    stepButton->setIcon(stepButtonOn);
    stepButton->setIconSize(QSize(24, 24));
}
void DisassemblyStateListWidget::stepButtonReleased()
{
    stepButton->setIcon(stepButtonOff);
    stepButton->setIconSize(QSize(24, 24));
}
