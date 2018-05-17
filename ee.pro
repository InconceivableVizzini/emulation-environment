QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EE
TEMPLATE = app

LIBS += -lOpengl32

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
    appframe.cpp \
    machine.cpp \
    cpu.cpp \
    disassembler.cpp \
    i8080.cpp \
    executedinstructionslistmodel.cpp \
    disassemblystatelistwidget.cpp \
    sipainterframebufferview.cpp

HEADERS += \
    appframe.h \
    machine.h \
    cpu.h \
    disassembler.h \
    i8080.h \
    executedinstructionslistmodel.h \
    disassemblystatelistwidget.h \
    sipainterframebufferview.h

RESOURCES += \
    ee.qrc

DISTFILES += \
    ee.qss \
    assets/8080_instructions.json \
    assets/shaders/si.vert \
    assets/shaders/si.frag
