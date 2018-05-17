#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <QVariant>
#include <QList>
#include <QSettings>

class CPU;

char *str_replace(char *orig, char const *rep, char const *with);

class Disassembler
{
public:
    Disassembler(CPU *processor);
    ~Disassembler();
    QList<QString> Disassemble(uint16_t addr);
    QList<QVariant> Disassemble(uint16_t addr, int diss_n_instructions);
    QList<QString> OpCode(uint16_t addr);
private:
    QSettings settings;
    CPU *cpu;
};


#endif // DISASSEMBLER_H
