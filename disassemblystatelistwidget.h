#ifndef DISASSEMBLYSTATELISTWIDGET_H
#define DISASSEMBLYSTATELISTWIDGET_H

#include <memory>

#include <QWidget>
#include <QListView>
#include <QVBoxLayout>
#include <QToolButton>
#include <QPushButton>
#include <QGroupBox>

class DisassemblyStateListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DisassemblyStateListWidget(QWidget *parent = nullptr);

    QVBoxLayout *layout;
    QHBoxLayout *eeControlsButtonGroupLayout;
    QToolButton *powerButton;
    QToolButton *pauseButton;
    QToolButton *captureButton;
    QPushButton *stepButton;
    QIcon        stepButtonOn;
    QIcon        stepButtonOff;
    QSpacerItem *alignButtonsLeftSpacer;
    QListView *executed_instructions_list;

signals:

public slots:
    void stepButtonPressed(void);
    void stepButtonReleased(void);
};

#endif // DISASSEMBLYSTATELISTWIDGET_H
