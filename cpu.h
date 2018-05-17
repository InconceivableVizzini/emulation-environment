#ifndef CPU_H
#define CPU_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QMap>
#include <QTime>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include <QVector>

#include "executedinstructionslistmodel.h"

#define INSTDEF CPU *processor, QList<QString> opcode

class Disassembler;

class MemoryMap {
public:
    QByteArray data;

    MemoryMap();
    ~MemoryMap();
    //ReadStuff
    //WriteStuff
};


class CPU : public QObject
{
    Q_OBJECT
/*
 Flags

 The 8080 has five flags, named Z (Zero), S (Sign),
 P (Parity), CY (Carry), and AC (Auxillary Carry).

 00000001 (1 << 0) Zero
   Set to 1 when the result of an instruction in the arithmetic group is zero.
 00000010 (1 << 1) Sign
   Set to 1 when bit 7 (the most significant bit) is set.
 00000100 (1 << 2) Parity
   Set to 1 when the result of an instruction in the arithmetic group has even parity.
   Set to 0 when the result has odd parity.
 00001000 (1 << 3) Carry
   Set to 1 when the result of an instruction in the arithmetic group overflows uint8_t.
 00010000 (1 << 4) Auxillary carry
   Space Invaders does not use this flag.
 00100000 (1 << 5) Interrupt Enabled
   The processor has an Interrupt Enable bit, and instructions
   for setting/clearing it.

 A few flags have been added for convenience.

 00000000000000000000000001000000 (1 << 6) Processor Enabled
   Used to disable the processor during processing.
 00000000000000000000000010000000 (1 << 7) Diagnostic Enabled
   Used to enable diagnostic mode. No bounds checks, etc.
 00000000000000000000000100000000 (1 << 8) Disassembly Reporting Enabled
   Used to enable storing disassembled instructions for inspection buy a GUI.


 Memory

 $0000-$3FFF

 Instruction Addressing Modes

 implied: STC (set carry flag)
 register addressing: MOV A,B (copy the contents of B to A)
 immediate addressing: MVI A,13H (load A with the value 13)
 direct addressing: LDA 1234H (note: in the machine code the low byte precedes the high byte of the address)
*/
public:
    uint8_t a, b, c, d,
            e, h, l;
    uint16_t sp, pc;
    int flags;
    QMap<QString, QVariant> instruction_list;
    MemoryMap *memory;
    Disassembler *disassembler;
    ExecutedInstructionsListModel *executed_instructions;
    CPU(QMutex *mu, MemoryMap *mem);
    ~CPU();
    void interrupt(int memory_vector);
    void setCallback(uint8_t opcode, int (*cb)(INSTDEF));
    void reset(void);
private:
    QTime now;
    QMutex *mutex;
    int (*instruction_handlers[0xFF]) (CPU* processor, QList<QString> opcode) = { 0 };
    int (*instruction_callbacks[0xFF]) (CPU* processor, QList<QString> opcode) = { 0 };
public slots:
    void emulate();
signals:
    void halted();
};

#endif // CPU_H
