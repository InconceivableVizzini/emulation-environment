#include "machine.h"

#include <QFile>
#include <QDebug>

using namespace EE;

ShiftRegister shift_register_hw;

MemoryMap::MemoryMap()
{
}

MemoryMap::~MemoryMap()
{
}

int input_callback(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t port = processor->memory->data.at(processor->pc+1);
    switch (port) {
        case 0:
            processor->a = shift_register_hw.in_port_one;
            break;
        case 2:
            processor->a = shift_register_hw.in_port_two;
            break;
        case 3:
            uint16_t val = ((((shift_register_hw.out_port_four_high << 8) | shift_register_hw.out_port_four_low) << shift_register_hw.out_port_two) >> 8);
            processor->a = (val & 0xFF);
            break;
    }

    processor->pc += inst_length;
    return inst_cycles;
}
int output_callback(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t port = processor->memory->data.at(processor->pc+1);

    switch (port) {
        case 2:
            shift_register_hw.out_port_two = processor->a;
        break;
    case 3:
        shift_register_hw.out_port_three = processor->a;
        break;
    case 4:
        shift_register_hw.out_port_four_low = shift_register_hw.out_port_four_high;
        shift_register_hw.out_port_four_high = processor->a;
        break;
    case 5:
        shift_register_hw.out_port_five = processor->a;
        break;
    }

    processor->pc += inst_length;
    return inst_cycles;
}

Machine::Machine(QObject *parent) : QObject(parent)
{
    mutex = new QMutex();
    memory = new MemoryMap();
    cpu = new CPU(mutex, memory);
    //cpu->flags |= (1 << 6); // Enable the processor
    //cpu->flags |= (1 << 7); // Diagnostic mode
    //cpu->flags |= (1 << 8); // Capture cpu state changes

    cpu->setCallback(0xd3, output_callback);
    cpu->setCallback(0xdb, input_callback);

    // Load Space Invaders rom files.
    QFile invaders_h("://assets/roms/invaders.h");
    QFile invaders_g("://assets/roms/invaders.g");
    QFile invaders_f("://assets/roms/invaders.f");
    QFile invaders_e("://assets/roms/invaders.e");
    QFile diag("://assets/roms/cpudiag.bin");

    bool opened = true;
    opened = opened && invaders_h.open(QIODevice::ReadOnly);
    opened = opened && invaders_g.open(QIODevice::ReadOnly);
    opened = opened && invaders_f.open(QIODevice::ReadOnly);
    opened = opened && invaders_e.open(QIODevice::ReadOnly);
    opened = opened && diag.open(QIODevice::ReadOnly);
    if (!opened) {
      qDebug() << "This machine needs space invaders rom files to emulate.";
      exit(1);
    }

    // ROMs are mapped to memory at 0x0000 - 0x1FFF
    // 0x2000-0x23ff working RAM, 0x2400-0x3FFF video RAM
    mutex->lock();
    char *mem = memory->data.data();

    memcpy(mem,          invaders_h.readAll().data(), invaders_h.size());
    memcpy(mem + 0x800,  invaders_g.readAll().data(), invaders_g.size());
    memcpy(mem + 0x1000, invaders_f.readAll().data(), invaders_f.size());
    memcpy(mem + 0x1800, invaders_e.readAll().data(), invaders_e.size());

    // Load a diagnostic binary for diagnostic mode.
    if (cpu->flags&(1 << 7))  {
        memcpy(mem + 0x100, diag.readAll().data(), diag.size());
        mem[0] = 0xc3;
        mem[1] = 0;
        mem[2] = 0x01;
        mem[368] = 0x7;
        mem[0x59c] = 0xc3;
        mem[0x59d] = 0xc2;
        mem[0x59e] = 0x05;
    }
    mutex->unlock();

    cpu->moveToThread(&thread);
    connect(&thread, &QThread::started, cpu, &CPU::emulate);
    connect(this, &Machine::processorEnabled, cpu, &CPU::emulate);
    connect(&thread, &QThread::finished, cpu, &QObject::deleteLater);
    connect(&thread, &QThread::finished, &thread, &QThread::deleteLater);

    thread.start();
}
Machine::~Machine()
{
    //thread.requestInterruption();
    mutex->lock();
    cpu->flags &= ~(1<<6); // disable processor
    mutex->unlock();
    thread.quit();
    thread.wait();
    delete memory;
    delete mutex;
}
