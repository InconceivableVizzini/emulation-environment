#ifndef MACHINE_H
#define MACHINE_H

#include <stdint.h>

#include <QByteArray>
#include <QSettings>
#include <QMutex>
#include <QThread>
#include <QTime>

#include "cpu.h"

namespace EE {

class Disassembler;

/*
 Space Invaders arcade machine.

 CPU: Intel 8080 CPU @ 2MHz

 Interrupts:
  RST 8  ($cf, rendering reaches middle of screen)
  RST 10 ($d7, rendering reaches end of screen)

 Video: 256x224@60Hz 1bpp, 7168 bytes video memory, vertical monitor.

 Memory map:
  ROM
  $0000-$07ff:    invaders.h
  $0800-$0fff:    invaders.g
  $1000-$17ff:    invaders.f
  $1800-$1fff:    invaders.e

  RAM
  $2000-$23ff:    work RAM
  $2400-$3fff:    video RAM

  $4000-:     RAM mirror
*/

class ShiftRegister {
public:
    uint8_t out_port_two, out_port_three,
            out_port_four_low, out_port_four_high,
            out_port_five, in_port_one, in_port_two = 0;
};

class Machine : public QObject
{
    Q_OBJECT
public:
    Machine(QObject *parent = 0);
    ~Machine();
    CPU *cpu;
private:
    uint8_t shift_high, shift_low, shift_offset;
    QSettings settings;
    QThread thread;
    QMutex *mutex;
    MemoryMap *memory;
signals:
    void processorEnabled(void);
};

}

extern "C" {
    int input_callback(INSTDEF);
    int output_callback(INSTDEF);
}
#endif // MACHINE_H
