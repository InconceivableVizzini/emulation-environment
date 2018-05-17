#include "i8080.h"

#include <QDebug>

int parity(uint8_t n) {
    uint8_t y;

    y = n^(n>>1);
    y = y^(y>>2);
    y = y^(y>>4);

    // 1 = odd,
    // 0 = even.
    return y & 1;
}

void SetFlagsZ(CPU *processor, int8_t val) {
    // Set Zero bit
    if (val == 0) {
        processor->flags |= (1<<0);
    } else {
        processor->flags &= ~(1<<0);
    }
}

void SetFlagsS(CPU *processor, int8_t val) {
    // Set Sign bit
    if ((val & 0x80) == 0x80) {
        processor->flags |= (1 << 1);
    } else {
        processor->flags &= ~(1 << 1);
    }
}

void SetFlagsP(CPU *processor, uint8_t val) {
    // Set parity bit
    if (parity(val) == 0) {
        processor->flags |= (1 << 2);
    } else {
        processor->flags &= ~(1 << 2);
    }
}

void SetFlagsAfterArithmeticOperation(CPU *processor, uint16_t val) {
    // The last instruction emulated changed the
    // processor state (a +b a, a - c, etc), so
    // update the various flags.

    // Set Carry bit
    if (val > 0xff) {
        processor->flags |= (1 << 3);
    } else {
        processor->flags &= ~(1 << 3);
    }

    // Set Zero bit
    if ((val&0xff) == 0) {
        processor->flags |= (1 << 0);
    } else {
        processor->flags &= ~(1 << 0);
    }

    // Set Sign bit
    if ((val & 0x80) == 0x80) {
        processor->flags |= (1 << 1);
    } else {
        processor->flags &= ~(1 << 1);
    }

    // Set Parity bit
    if (parity(val&0xff) == 0) {
        processor->flags |= (1 << 2);
    } else {
        processor->flags &= ~(1 << 2);
    }
}

void SetFlagsAfterLogicalOperation(CPU *processor) {
    // The last instruction emulated changed the
    // processor state (a ^ a, a & d8, etc), so
    // update the various flags.

    // Turn off Carry and Aux Carry bits
    processor->flags &= ~(1<<3);
    processor->flags &= ~(1<<4);

    // Set Zero bit
    if (processor->a == 0) {
        processor->flags |= (1 << 0);
    } else {
        processor->flags &= ~(1 << 0);
    }

    // Set Sign bit
    if ((processor->a & 0x80) == 0x80) {
        processor->flags |= (1 << 1);
    } else {
        processor->flags &= ~(1 << 1);
    }

    // Set Parity bit
    if (parity(processor->a) == 0) {
        processor->flags |= (1 << 2);
    } else {
        processor->flags &= ~(1 << 2);
    }
}

void SetFlagsZSP(CPU *processor, uint8_t val) {
    SetFlagsZ(processor, val);
    SetFlagsS(processor, val);
    SetFlagsP(processor, val);
}

int noop(INSTDEF) {
    Q_UNUSED(processor);
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    //qDebug() << "In noop() instruction handler.";

    processor->pc += inst_length;
    return inst_cycles;
}

// HLT
int hlt(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->flags &= ~(1 << 6); // Disable the processor

    processor->pc += inst_length;
    return inst_cycles;
}

// DCR A
int dcr_a(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t res = processor->a-1;

    SetFlagsZSP(processor, res);

    processor->a = res;

    processor->pc += inst_length;
    return inst_cycles;
}

// DCR B
int dcr_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t res = processor->b-1;

    SetFlagsZSP(processor, res);

    processor->b = res;

    processor->pc += inst_length;
    return inst_cycles;
}

// DCR C
int dcr_c(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t res = processor->c-1;

    SetFlagsZSP(processor, res);

    processor->c = res;

    processor->pc += inst_length;
    return inst_cycles;
}

// DCR D
int dcr_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t res = processor->d-1;

    SetFlagsZSP(processor, res);

    processor->d = res;

    processor->pc += inst_length;
    return inst_cycles;
}

// DCR E
int dcr_e(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t res = processor->e-1;

    SetFlagsZSP(processor, res);

    processor->e = res;

    processor->pc += inst_length;
    return inst_cycles;
}

// DCR H
int dcr_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t res = processor->h-1;

    SetFlagsZSP(processor, res);

    processor->h = res;

    processor->pc += inst_length;
    return inst_cycles;
}

// DCR L
int dcr_l(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t res = processor->l-1;

    SetFlagsZSP(processor, res);

    processor->l = res;

    processor->pc += inst_length;
    return inst_cycles;
}


// JNZ addr
int jnz(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    unsigned char highaddr = processor->memory->data.at(processor->pc+1);
    unsigned char lowaddr = processor->memory->data.at(processor->pc+2);

    if (processor->flags&(1<<0)) {
        processor->pc += inst_length;
    } else {
        processor->pc = (lowaddr<<8) | highaddr;
    }

    return inst_cycles;
}

// JMP addr
int jmp(INSTDEF) {
    int inst_cycles = opcode.at(2).toInt();
    unsigned char highaddr = processor->memory->data.at(processor->pc+1);
    unsigned char lowaddr = processor->memory->data.at(processor->pc+2);
    processor->pc = (lowaddr << 8) | highaddr;
    return inst_cycles;
}

// LXI B,d16
int lxi_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    unsigned char highaddr = processor->memory->data.at(processor->pc+1);
    unsigned char lowaddr = processor->memory->data.at(processor->pc+2);

    processor->c = highaddr;
    processor->b = lowaddr;

    processor->pc += inst_length;
    return inst_cycles;
}

// LXI D,d16
int lxi_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    unsigned char highaddr = processor->memory->data.at(processor->pc+1);
    unsigned char lowaddr = processor->memory->data.at(processor->pc+2);

    processor->e = highaddr;
    processor->d = lowaddr;

    processor->pc += inst_length;
    return inst_cycles;
}

// LDAX B
int ldax_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->b << 8) | processor->c;
    processor->a = processor->memory->data.at(addr);

    processor->pc += inst_length;
    return inst_cycles;
}
// LDAX D
int ldax_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->d << 8) | processor->e;
    processor->a = processor->memory->data.at(addr);

    processor->pc += inst_length;
    return inst_cycles;
}

// LXI H,d16
int lxi_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->l = processor->memory->data.at(processor->pc+1);
    processor->h = processor->memory->data.at(processor->pc+2);

    processor->pc += inst_length;
    return inst_cycles;
}

// LXI SP,d16
int lxi_sp(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    unsigned char highaddr = processor->memory->data.at(processor->pc+1);
    unsigned char lowaddr = processor->memory->data.at(processor->pc+2);

    processor->sp = (lowaddr << 8) | highaddr;

    processor->pc += inst_length;
    return inst_cycles;
}

// STA addr
int sta(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t highaddr = processor->memory->data.at(processor->pc+1);
    uint8_t lowaddr = processor->memory->data.at(processor->pc+2);

    uint16_t addr = (uint16_t)((uint16_t)(lowaddr<<8)|(uint16_t)highaddr);

    processor->memory->data[addr] = processor->a;

    processor->pc += inst_length;
    return inst_cycles;
}

// LDA addr
int lda(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t highaddr = processor->memory->data.at(processor->pc+1);
    uint8_t lowaddr = processor->memory->data.at(processor->pc+2);

    uint16_t addr = ((uint16_t)(lowaddr<<8)|(uint16_t)highaddr);

    processor->a = processor->memory->data[addr];

    processor->pc += inst_length;
    return inst_cycles;
}

// MVI A,d8
int mvi_a(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->memory->data.at(processor->pc+1);

    processor->pc += inst_length;
    return inst_cycles;
}

// MVI B,d8
int mvi_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->b = processor->memory->data.at(processor->pc+1);

    processor->pc += inst_length;
    return inst_cycles;
}

// MVI C,d8
int mvi_c(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->c = processor->memory->data.at(processor->pc+1);

    processor->pc += inst_length;
    return inst_cycles;
}

// MVI D,d8
int mvi_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->d = processor->memory->data.at(processor->pc+1);

    processor->pc += inst_length;
    return inst_cycles;
}

// MVI E,d8
int mvi_e(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->e = processor->memory->data.at(processor->pc+1);

    processor->pc += inst_length;
    return inst_cycles;
}


// MVI H,d8
int mvi_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->h = processor->memory->data.at(processor->pc+1);

    processor->pc += inst_length;
    return inst_cycles;
}

// MVI L,d8
int mvi_l(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->l = processor->memory->data.at(processor->pc+1);

    processor->pc += inst_length;
    return inst_cycles;

}

// MVI M,d8
int mvi_m(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t val = processor->memory->data.at(processor->pc+1);
    uint16_t addr = (processor->h << 8) | processor->l;
    processor->memory->data[addr] = val;

    processor->pc += inst_length;
    return inst_cycles;
}

// JZ addr
int jz(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t highaddr = processor->memory->data.at(processor->pc+1);
    uint8_t lowaddr = processor->memory->data.at(processor->pc+2);

    if (processor->flags&(1<<0)) {
        processor->pc = (uint16_t)((lowaddr << 8) | highaddr);
    } else {
        processor->pc += inst_length;
    }
    return inst_cycles;
}

// JNC addr
int jnc(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t highaddr = processor->memory->data.at(processor->pc+1);
    uint8_t lowaddr = processor->memory->data.at(processor->pc+2);

    if ((processor->flags&(1<<3)) == 0) {
        processor->pc = (uint16_t)((lowaddr << 8) | highaddr);
    } else {
        processor->pc += inst_length;
    }
    return inst_cycles;
}

// JPE addr
int jpe(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t highaddr = processor->memory->data.at(processor->pc+1);
    uint8_t lowaddr = processor->memory->data.at(processor->pc+2);

    if (processor->flags&(1<<2)) {
        processor->pc = (uint16_t)((lowaddr << 8) | highaddr);
    } else {
        processor->pc += inst_length;
    }
    return inst_cycles;
}

// JPO addr
int jpo(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t highaddr = processor->memory->data.at(processor->pc+1);
    uint8_t lowaddr = processor->memory->data.at(processor->pc+2);

    if ((processor->flags&(1<<2)) == 0) {
        processor->pc = (uint16_t)((lowaddr << 8) | highaddr);
    } else {
        processor->pc += inst_length;
    }
    return inst_cycles;
}

// JP addr
int jp(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t highaddr = processor->memory->data.at(processor->pc+1);
    uint8_t lowaddr = processor->memory->data.at(processor->pc+2);

    if ((processor->flags&(1<<1)) == 0) {
        processor->pc = (uint16_t)((lowaddr << 8) | highaddr);
    } else {
        processor->pc += inst_length;
    }
    return inst_cycles;
}

// JC addr
int jc(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t highaddr = processor->memory->data.at(processor->pc+1);
    uint8_t lowaddr = processor->memory->data.at(processor->pc+2);

    if (processor->flags&(1<<3)) {
        processor->pc = (uint16_t)((lowaddr << 8) | highaddr);
    } else {
        processor->pc += inst_length;
    }
    return inst_cycles;
}

// JM addr
int jm(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t highaddr = processor->memory->data.at(processor->pc+1);
    uint8_t lowaddr = processor->memory->data.at(processor->pc+2);

    if (processor->flags&(1<<1)) {
        processor->pc = (uint16_t)((lowaddr << 8) | highaddr);
    } else {
        processor->pc += inst_length;
    }
    return inst_cycles;
}

// CALL addr
int call(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t highaddr = processor->memory->data.at(processor->pc+1);
    uint8_t lowaddr = processor->memory->data.at(processor->pc+2);
    uint16_t addr = processor->pc+inst_length;

    // Diagnostic Mode expects a string printing function at 0x5
    if ((processor->flags&(1 << 7)) && (5 == ((lowaddr << 8) | highaddr))) {
        if (processor->c == 9)
        {
            uint16_t addr = (processor->d<<8)|processor->e;
            unsigned char ayyy = processor->memory->data[addr+3];
            while(ayyy != '$') {
                ayyy = processor->memory->data[addr+3];
                printf("%c", ayyy);
                addr += 1;
            }
            printf("\n");
        }

        //processor->flags &= ~(1<<6);
        return inst_cycles;
    }

    processor->memory->data[processor->sp-1] = (addr >> 8) & 0xFF;
    processor->memory->data[processor->sp-2] = addr & 0xFF;
    processor->pc = (uint16_t)((lowaddr << 8) | highaddr);
    processor->sp -= 2;

    /*
    highaddr = processor->memory->data.at(processor->sp);
    lowaddr = processor->memory->data.at(processor->sp+1);
    qDebug() << hex << "hmm... " << (uint16_t)((lowaddr << 8) | highaddr);

    qDebug() << "In call, new addr - " << hex << processor->pc;
    qDebug() << "New return addr - " << hex << ((((uint8_t)(processor->memory->data[processor->sp+1])) << 8) | ((uint8_t)(processor->memory->data[processor->sp])));
    */

    return inst_cycles;
}

// RET
int ret(INSTDEF) {
    int inst_cycles = opcode.at(2).toInt();

    /*
    char debug[100];
    snprintf(debug, 100, "pc - 0x%04x", (unsigned char)(processor->pc));
    qDebug();
    snprintf(debug, 100, "data[sp] - 0x%02x", (unsigned char)(processor->memory->data.at(processor->sp)));
    qDebug() << debug;
    snprintf(debug, 100, "data[sp+1] - 0x%02x", (unsigned char)(processor->memory->data.at(processor->sp+1)));
    qDebug() << debug;

    uint8_t highaddr = processor->memory->data.at(processor->sp);
    uint8_t lowaddr = processor->memory->data.at(processor->sp+1);

    processor->pc = (uint16_t)((lowaddr << 8) | highaddr);

    snprintf(debug, 100, "pc' - 0x%04x", (uint16_t)(processor->pc));
    qDebug() << debug;

    qDebug() << hex << "0x" << processor->pc;
    */

    processor->pc = (uint16_t)(((uint8_t)processor->memory->data[processor->sp+1] << 8) | (uint8_t)processor->memory->data[processor->sp]);

    processor->sp += 2;

    return inst_cycles;
}

// ADI d8
int adi(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t val = processor->memory->data.at(processor->pc+1);

    uint16_t res = (uint16_t)processor->a + (uint16_t)val;
    SetFlagsZSP(processor, res);
    if (res>0xff) {
        processor->flags |= (1 << 3);
    } else {
        processor->flags &= ~(1 << 3);
    }
    processor->a = res & 0xff;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV M,A
int mvm_a(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->h << 8) | processor->l;
    processor->memory->data[addr] = processor->a;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV M,B
int mvm_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->h << 8) | processor->l;
    processor->memory->data[addr] = processor->b;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV M,C
int mvm_c(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->h << 8) | processor->l;
    processor->memory->data[addr] = processor->c;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV M,D
int mvm_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->h << 8) | processor->l;
    processor->memory->data[addr] = processor->d;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV M,E
int mvm_e(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->h << 8) | processor->l;
    processor->memory->data[addr] = processor->e;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV M,H
int mvm_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->h << 8) | processor->l;
    processor->memory->data[addr] = processor->h;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV M,L
int mvm_l(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->h << 8) | processor->l;
    processor->memory->data[addr] = processor->l;

    processor->pc += inst_length;
    return inst_cycles;
}

// ANA A
int ana_a(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->a & processor->a;

    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// ANA B
int ana_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->a & processor->b;

    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// ANA C
int ana_c(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->a & processor->c;

    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// ANA D
int ana_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->a & processor->d;

    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// ANA E
int ana_e(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->a & processor->e;

    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// ANA H
int ana_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->a & processor->h;

    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// ANA L
int ana_l(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->a & processor->l;

    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// ANA M
int ana_m(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->h << 8) | processor->l;
    processor->a = processor->a & processor->memory->data[addr];
    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// XRA A
int xra_a(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->a ^ processor->a;

    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// XRA B
int xra_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->a ^ processor->b;

    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// XRA C
int xra_c(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->a ^ processor->c;

    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// XRA D
int xra_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->a ^ processor->d;

    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// XRA E
int xra_e(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->a ^ processor->e;

    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// XRA H
int xra_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->a ^ processor->h;

    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// XRA L
int xra_l(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->a ^ processor->l;

    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// XRA M
int xra_m(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->h << 8) | processor->l;
    processor->a = processor->a ^ processor->memory->data[addr];
    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// INX B
int inx_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->c++;
    if (processor->c == 0) processor->b++;

    processor->pc += inst_length;
    return inst_cycles;
}

// INX H
int inx_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->l++;
    if (processor->l == 0) processor->h++;

    processor->pc += inst_length;
    return inst_cycles;
}

// INX D
int inx_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->e++;
    if (processor->e == 0) processor->d++;

    processor->pc += inst_length;
    return inst_cycles;
}

// INX SP
int inx_sp(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->sp++;

    processor->pc += inst_length;
    return inst_cycles;
}

// DCX B
int dcx_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->c--;
    if (processor->c == 0xFF) processor->b--;

    processor->pc += inst_length;
    return inst_cycles;
}

// DCX D
int dcx_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->e--;
    if (processor->e == 0xFF) processor->d--;

    processor->pc += inst_length;
    return inst_cycles;
}

// DCX H
int dcx_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->l--;
    if (processor->l == 0xFF) processor->h--;

    processor->pc += inst_length;
    return inst_cycles;
}

// DCX SP
int dcx_sp(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->sp--;

    processor->pc += inst_length;
    return inst_cycles;
}

// DAD B
int dad_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint32_t add = ((processor->h << 8) | processor->l) +
                   ((processor->b << 8) | processor->c);

    processor->h = (add & 0xff00) >> 8;
    processor->l = add & 0xff;

    if ((add & 0xffff0000) != 0) {
        processor->flags |= (1 << 3);
    } else {
        processor->flags &= ~(1 << 3);
    }

    processor->pc += inst_length;
    return inst_cycles;
}

// DAD D
int dad_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint32_t add = ((processor->h << 8) | processor->l) +
                   ((processor->d << 8) | processor->e);

    processor->h = (add & 0xff00) >> 8;
    processor->l = add & 0xff;

    if ((add & 0xffff0000) != 0) {
        processor->flags |= (1 << 3);
    } else {
        processor->flags &= ~(1 << 3);
    }

    processor->pc += inst_length;
    return inst_cycles;
}

// DAD H
int dad_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint32_t add = ((processor->h << 8) | processor->l) +
                   ((processor->h << 8) | processor->l);

    processor->h = (add & 0xff00) >> 8;
    processor->l = add & 0xff;

    if ((add & 0xffff0000) != 0) {
        processor->flags |= (1 << 3);
    } else {
        processor->flags &= ~(1 << 3);
    }

    processor->pc += inst_length;
    return inst_cycles;
}

// DAD SP
int dad_sp(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint32_t add = ( ((processor->h << 8) | processor->l) +
                     processor->sp );
    processor->h = (add & 0xff00) >> 8;
    processor->l = add & 0xff;
    if ((add & 0xffff0000) > 0) {
        processor->flags |= (1 << 3);
    } else {
        processor->flags &= ~(1 << 3);
    }

    processor->pc += inst_length;
    return inst_cycles;
}

// POP B
int pop_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->b = processor->memory->data[processor->sp+1];
    processor->c = processor->memory->data[processor->sp];
    processor->sp += 2;

    processor->pc += inst_length;
    return inst_cycles;
}

// PUSH B
int push_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    if (((processor->flags&(1 << 7)) == 0) && (((processor->sp-2) < 0x2000) || ((processor->sp-1) >= 0x4000))) {
        qDebug() << "Writing outside of safe memory";
        processor->flags &= ~(1 << 6); // Disable the processor.
    }

    processor->memory->data[processor->sp-1] = processor->b;
    processor->memory->data[processor->sp-2] = processor->c;
    processor->sp -= 2;

    processor->pc += inst_length;
    return inst_cycles;
}

// PUSH D
int push_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();
    if (((processor->flags&(1 << 7)) == 0) && (((processor->sp-2) < 0x2000) || ((processor->sp-1) >= 0x4000))) {
        qDebug() << "Writing outside of safe memory";
        processor->flags &= ~(1 << 6); // Disable the processor.
    }

    processor->memory->data[processor->sp-1] = processor->d;
    processor->memory->data[processor->sp-2] = processor->e;
    processor->sp -= 2;

    processor->pc += inst_length;
    return inst_cycles;
}

// POP H
int pop_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->h = processor->memory->data[processor->sp+1];
    processor->l = processor->memory->data[processor->sp];
    processor->sp += 2;

    processor->pc += inst_length;
    return inst_cycles;
}

// PUSH H
int push_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    if (((processor->flags&(1 << 7)) == 0) && (((processor->sp-2) < 0x2000) || ((processor->sp-1) >= 0x4000))) {
        qDebug() << "Writing outside of safe memory";
        processor->flags &= ~(1 << 6); // Disable the processor.
    }

    processor->memory->data[processor->sp-1] = processor->h;
    processor->memory->data[processor->sp-2] = processor->l;
    processor->sp -= 2;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV L,A
int mvl_a(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->l = processor->a;

    processor->pc += inst_length;
    return inst_cycles;
}

// ANI d8
int ani(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t val = processor->memory->data.at(processor->pc+1);

    processor->a = processor->a & val;

    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// XCHG
int xchg(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t tmpd, tmpe;
    tmpd = processor->d;
    tmpe = processor->e;
    processor->d = processor->h;
    processor->e = processor->l;
    processor->h = tmpd;
    processor->l = tmpe;

    processor->pc += inst_length;
    return inst_cycles;
}

// EI
int ei(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->flags |= (1 << 5);

    processor->pc += inst_length;
    return inst_cycles;
}

// CPI d8
int cpi(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t val = processor->memory->data.at(processor->pc+1);

    uint8_t res = processor->a - val;

    SetFlagsZSP(processor, res);
    if (processor->a < val) {
        processor->flags |= (1 << 3);
    } else {
        processor-> flags &= ~(1 << 3);
    }

    processor->pc += inst_length;
    return inst_cycles;
}

// ACI d8
int aci(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t val = processor->memory->data.at(processor->pc+1);

    uint16_t res = (uint16_t)processor->a + (uint16_t)val + ((processor->flags&(1 << 3))?1:0);
    SetFlagsZSP(processor, res&0xff);
    if (res>0xff) {
        processor->flags |= (1 << 3);
    } else {
        processor->flags &= ~(1 << 3);
    }
    processor->a = res & 0xff;

    processor->pc += inst_length;
    return inst_cycles;
}

// SUI d8
int sui(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t val = processor->memory->data.at(processor->pc+1);

    uint8_t res = processor->a - val;

    SetFlagsZSP(processor, res);
    if (processor->a < val) {
        processor->flags |= (1 << 3);
    } else {
        processor->flags &= ~(1 << 3);
    }
    processor->a = res;

    processor->pc += inst_length;
    return inst_cycles;
}

// SBI d8
int sbi(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t val = processor->memory->data.at(processor->pc+1);

    uint16_t res = (uint16_t)processor->a - (uint16_t)val - ((processor->flags&(1 << 3))?1:0);

    SetFlagsZSP(processor, res&0xff);
    if (res>0xff) {
        processor->flags |= (1 << 3);
    } else {
        processor->flags &= ~(1 << 3);
    }
    processor->a = res&0xff;

    processor->pc += inst_length;
    return inst_cycles;
}

// ORI d8
int ori(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t val = processor->memory->data.at(processor->pc+1);
    uint8_t res = processor->a | val;
    SetFlagsZSP(processor, res);
    processor->flags &= ~(1 << 3);
    processor->a = res;

    processor->pc += inst_length;
    return inst_cycles;
}

// XRI d8
int xri(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t val = processor->memory->data.at(processor->pc+1);
    uint8_t res = processor->a ^ val;
    SetFlagsZSP(processor, res);
    processor->flags &= ~(1 << 3);
    processor->a = res;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV A,H
int mva_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->h;

    processor->pc += inst_length;
    return inst_cycles;
}

// POP D
int pop_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->d = processor->memory->data[processor->sp+1];
    processor->e = processor->memory->data[processor->sp];
    processor->sp += 2;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV E,M
int mve_m(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->h << 8) |  processor->l;
    processor->e = processor->memory->data.at(addr);

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV D,M
int mvd_m(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->h << 8) |  processor->l;
    processor->d = processor->memory->data.at(addr);

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV A,M
int mva_m(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->h << 8) |  processor->l;
    processor->a = processor->memory->data.at(addr);

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV H,M
int mvh_m(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->h << 8) |  processor->l;
    processor->h = processor->memory->data.at(addr);

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV A,D
int mva_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->d;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV A,E
int mva_e(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->e;

    processor->pc += inst_length;
    return inst_cycles;
}

// PUSH PSW
int push_psw(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->memory->data[processor->sp-1] = processor->a;
    processor->memory->data[processor->sp-2] = (uint8_t)processor->flags;
    processor->sp -= 2;

    processor->pc += inst_length;
    return inst_cycles;
}

// POP PSW
int pop_psw(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->flags = ((processor->flags & 0xff00) | processor->memory->data[processor->sp]);
    processor->a = processor->memory->data[processor->sp+1];
    processor->sp += 2;

    processor->pc += inst_length;
    return inst_cycles;
}

// RLC
int rlc(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t val = processor->a;
    processor->a = ((val & 0x80) >> 7) | (val << 1);
    if ((val&0x80) == 0x80) {
        processor->flags |= (1 << 3);
    } else {
        processor->flags &= ~(1 << 3);
    }

    processor->pc += inst_length;
    return inst_cycles;
}

// RRC
int rrc(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t val = processor->a;
    processor->a = ((val & 1) << 7) | (val >> 1);
    if (1 == (val&1)) {
        processor->flags |= (1 << 3);
    } else {
        processor->flags &= ~(1 << 3);
    }

    processor->pc += inst_length;
    return inst_cycles;
}

// RAL
int ral(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t val = processor->a;
    processor->a = ((processor->flags&(1 << 3))?1:0) | (val << 1);
    if ((val&0x80) == 0x80) {
        processor->flags |= (1 << 3);
    } else {
        processor->flags &= ~(1 << 3);
    }

    processor->pc += inst_length;
    return inst_cycles;
}

// RAR
int rar(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t val = processor->a;
    processor->a = (((processor->flags&(1 << 3))?1:0) << 7) | (val >> 1);
    if ((val&1) == 1) {
        processor->flags |= (1 << 3);
    } else {
        processor->flags &= ~(1 << 3);
    }

    processor->pc += inst_length;
    return inst_cycles;
}

// CC addr
int cc(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    if (processor->flags&(1 << 3)) {
        uint8_t highaddr = processor->memory->data.at(processor->pc+1);
        uint8_t lowaddr = processor->memory->data.at(processor->pc+2);
        uint16_t addr = processor->pc+inst_length;

        processor->memory->data[processor->sp-1] = (addr >> 8) & 0xFF;
        processor->memory->data[processor->sp-2] = addr & 0xFF;
        processor->pc = (uint16_t)((lowaddr << 8) | highaddr);
        processor->sp -= 2;

        return inst_cycles;
    }

    processor->pc += inst_length;
    return 11;
}

// CPO addr
// Call if parity odd
int cpo(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    if ((processor->flags&(1 << 2)) == 0) {
        uint8_t highaddr = processor->memory->data.at(processor->pc+1);
        uint8_t lowaddr = processor->memory->data.at(processor->pc+2);
        uint16_t addr = processor->pc+inst_length;

        processor->memory->data[processor->sp-1] = (addr >> 8) & 0xFF;
        processor->memory->data[processor->sp-2] = addr & 0xFF;
        processor->pc = (uint16_t)((lowaddr << 8) | highaddr);
        processor->sp -= 2;

        return inst_cycles;
    }

    processor->pc += inst_length;
    return 11;
}

// CM addr
// Call if minus
// Call if sign is set (last operation was negative)
int cm(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    if (processor->flags&(1 << 1)) {
        uint8_t highaddr = processor->memory->data.at(processor->pc+1);
        uint8_t lowaddr = processor->memory->data.at(processor->pc+2);
        uint16_t addr = processor->pc+inst_length;

        processor->memory->data[processor->sp-1] = (addr >> 8) & 0xFF;
        processor->memory->data[processor->sp-2] = addr & 0xFF;
        processor->pc = (uint16_t)((lowaddr << 8) | highaddr);
        processor->sp -= 2;

        return inst_cycles;
    }

    processor->pc += inst_length;
    return 11;
}

// CNZ addr
// Call if zero is not set
int cnz(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    if ((processor->flags&(1 << 0)) == 0) {
        uint8_t highaddr = processor->memory->data.at(processor->pc+1);
        uint8_t lowaddr = processor->memory->data.at(processor->pc+2);
        uint16_t addr = processor->pc+inst_length;

        processor->memory->data[processor->sp-1] = (addr >> 8) & 0xFF;
        processor->memory->data[processor->sp-2] = addr & 0xFF;
        processor->pc = (uint16_t)((lowaddr << 8) | highaddr);
        processor->sp -= 2;

        return inst_cycles;
    }

    processor->pc += inst_length;
    return 11;
}

// CNC addr
// Call if carry is not set
int cnc(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    if ((processor->flags&(1 << 3)) == 0) {
        uint8_t highaddr = processor->memory->data.at(processor->pc+1);
        uint8_t lowaddr = processor->memory->data.at(processor->pc+2);
        uint16_t addr = processor->pc+inst_length;

        processor->memory->data[processor->sp-1] = (addr >> 8) & 0xFF;
        processor->memory->data[processor->sp-2] = addr & 0xFF;
        processor->pc = (uint16_t)((lowaddr << 8) | highaddr);
        processor->sp -= 2;

        return inst_cycles;
    }

    processor->pc += inst_length;
    return 11;
}

// CPE addr
// Call if parity even
int cpe(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    if (processor->flags&(1 << 2)) {
        uint8_t highaddr = processor->memory->data.at(processor->pc+1);
        uint8_t lowaddr = processor->memory->data.at(processor->pc+2);
        uint16_t addr = processor->pc+inst_length;

        processor->memory->data[processor->sp-1] = (addr >> 8) & 0xFF;
        processor->memory->data[processor->sp-2] = addr & 0xFF;
        processor->pc = (uint16_t)((lowaddr << 8) | highaddr);
        processor->sp -= 2;

        return inst_cycles;
    }

    processor->pc += inst_length;
    return 11;
}

// CP addr
// Call if plus
// Call if sign bit is not set (last operation was positive)
int cp(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    if ((processor->flags&(1 << 1)) == 0) {
        uint8_t highaddr = processor->memory->data.at(processor->pc+1);
        uint8_t lowaddr = processor->memory->data.at(processor->pc+2);
        uint16_t addr = processor->pc+inst_length;

        processor->memory->data[processor->sp-1] = (addr >> 8) & 0xFF;
        processor->memory->data[processor->sp-2] = addr & 0xFF;
        processor->pc = (uint16_t)((lowaddr << 8) | highaddr);
        processor->sp -= 2;

        return inst_cycles;
    }

    processor->pc += inst_length;
    return 11;
}

// CZ addr
// Call if zero is set
int cz(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    if (processor->flags&(1 << 0)) {
        uint8_t highaddr = processor->memory->data.at(processor->pc+1);
        uint8_t lowaddr = processor->memory->data.at(processor->pc+2);
        uint16_t addr = processor->pc+inst_length;

        processor->memory->data[processor->sp-1] = (addr >> 8) & 0xFF;
        processor->memory->data[processor->sp-2] = addr & 0xFF;
        processor->pc = (uint16_t)((lowaddr << 8) | highaddr);
        processor->sp -= 2;

        return inst_cycles;
    }

    processor->pc += inst_length;
    return 11;
}

// RPE
// Return if parity is set
int rpe(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    if (processor->flags&(1 << 2)) {
        processor->pc = (uint16_t)(((uint8_t)processor->memory->data[processor->sp+1] << 8) | (uint8_t)processor->memory->data[processor->sp]);
        processor->sp += 2;

        return inst_cycles;
    }

    processor->pc += inst_length;
    return 5;
}

// RPO
// Return if parity is not set
int rpo(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    if ((processor->flags&(1 << 2)) == 0) {
        processor->pc = (uint16_t)(((uint8_t)processor->memory->data[processor->sp+1] << 8) | (uint8_t)processor->memory->data[processor->sp]);
        processor->sp += 2;

        return inst_cycles;
    }

    processor->pc += inst_length;
    return 5;
}

// RP
// Return if plus
// Return if sign bit is not set (last operation was positive)
int rp(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    if ((processor->flags&(1 << 1)) == 0) {
        processor->pc = (uint16_t)(((uint8_t)processor->memory->data[processor->sp+1] << 8) | (uint8_t)processor->memory->data[processor->sp]);
        processor->sp += 2;

        return inst_cycles;
    }

    processor->pc += inst_length;
    return 5;
}

// RM
// Return if minus
// Return if sign is set (last operation was negative)
int rm(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    if (processor->flags&(1 << 1)) {
        processor->pc = (uint16_t)(((uint8_t)processor->memory->data[processor->sp+1] << 8) | (uint8_t)processor->memory->data[processor->sp]);
        processor->sp += 2;

        return inst_cycles;
    }

    processor->pc += inst_length;
    return 5;
}

// RC
// Return if carry is set
int rc(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    if (processor->flags&(1 << 3)) {
        processor->pc = (uint16_t)(((uint8_t)processor->memory->data[processor->sp+1] << 8) | (uint8_t)processor->memory->data[processor->sp]);
        processor->sp += 2;

        return inst_cycles;

    }

    processor->pc += inst_length;
    return 5;
}

// RNC
// Return if carry is not set
int rnc(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    if ((processor->flags&(1 << 3)) == 0) {
        processor->pc = (uint16_t)(((uint8_t)processor->memory->data[processor->sp+1] << 8) | (uint8_t)processor->memory->data[processor->sp]);
        processor->sp += 2;

        return inst_cycles;

    }

    processor->pc += inst_length;
    return 5;
}

// RZ
// Return if zero is set
int rz(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    if (processor->flags&(1 << 0)) {
        processor->pc = (uint16_t)(((uint8_t)processor->memory->data[processor->sp+1] << 8) | (uint8_t)processor->memory->data[processor->sp]);
        processor->sp += 2;

        return inst_cycles;

    }

    processor->pc += inst_length;
    return 5;
}

// RNZ
// Return if zero is not set
int rnz(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    if ((processor->flags&(1 << 0)) == 0) {
        processor->pc = (uint16_t)(((uint8_t)processor->memory->data[processor->sp+1] << 8) | (uint8_t)processor->memory->data[processor->sp]);
        processor->sp += 2;

        return inst_cycles;

    }

    processor->pc += inst_length;
    return 5;
}

// INR A
// Increment A
int inr_a(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t res = processor->a+1;

    SetFlagsZSP(processor, res);

    processor->a = res;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV B,A
int mvb_a(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->b = processor->a;

    processor->pc += inst_length;
    return inst_cycles;
}

// INR B
// Increment B
int inr_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t res = processor->b+1;

    SetFlagsZSP(processor, res);

    processor->b = res;

    processor->pc += inst_length;
    return inst_cycles;
}

// INR M
int inr_m(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->h << 8) | processor->l;
    uint8_t res = processor->memory->data[addr] + 1;
    SetFlagsZSP(processor, res);
    processor->memory->data[addr] = res;

    processor->pc += inst_length;
    return inst_cycles;
}


// DCR M
int dcr_m(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->h << 8) | processor->l;
    uint8_t res = processor->memory->data[addr] - 1;
    SetFlagsZSP(processor, res);
    processor->memory->data[addr] = res;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV C,B
int mvc_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->c = processor->b;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV D,C
int mvd_c(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->d = processor->c;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV E,D
int mve_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->e = processor->d;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV H,E
int mvh_e(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->h = processor->e;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV L,H
int mvl_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->l = processor->h;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV A,L
int mva_l(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->l;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV C,A
int mvc_a(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->c = processor->a;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV E,C
int mve_c(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->e = processor->c;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV L,E
int mvl_e(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->l = processor->e;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV B,L
int mvb_l(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->b = processor->l;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV D,B
int mvd_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->d = processor->b;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV H,D
int mvh_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->h = processor->d;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV D,A
int mvd_a(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->d = processor->a;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV L,D
int mvl_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->l = processor->d;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV C,L
int mvc_l(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->c = processor->l;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV H,C
int mvh_c(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->h = processor->c;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV B,H
int mvb_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->b = processor->h;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV E,B
int mve_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->e = processor->b;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV E,A
int mve_a(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->e = processor->a;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV B,E
int mvb_e(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->b = processor->e;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV H,B
int mvh_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->h = processor->b;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV C,H
int mvc_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->c = processor->h;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV C,M
int mvc_m(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->c = processor->memory->data[((processor->h << 8) | processor->l)];

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV L,C
int mvl_c(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->l = processor->c;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV D,L
int mvd_l(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->d = processor->l;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV H,A
int mvh_a(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->h = processor->a;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV D,H
int mvd_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->d = processor->h;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV B,D
int mvb_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->b = processor->d;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV L,B
int mvl_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->l = processor->b;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV E,L
int mve_l(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->e = processor->l;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV C,E
int mvc_e(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->c = processor->e;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV A,C
int mva_c(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->c;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV H,L
int mvh_l(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->h = processor->l;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV E,H
int mve_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->e = processor->h;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV D,E
int mvd_e(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->d = processor->e;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV C,D
int mvc_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->c = processor->d;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV B,C
int mvb_c(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->b = processor->c;

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV A,B
int mva_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->b;

    processor->pc += inst_length;
    return inst_cycles;
}

// INR C
// Increment C
int inr_c(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t res = processor->c+1;

    SetFlagsZSP(processor, res);

    processor->c = res;

    processor->pc += inst_length;
    return inst_cycles;
}


// INR D
// Increment D
int inr_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t res = processor->d+1;

    SetFlagsZSP(processor, res);

    processor->d = res;

    processor->pc += inst_length;
    return inst_cycles;
}

// INR E
// Increment E
int inr_e(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t res = processor->e+1;

    SetFlagsZSP(processor, res);

    processor->e = res;

    processor->pc += inst_length;
    return inst_cycles;
}

// INR H
// Increment H
int inr_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t res = processor->h+1;

    SetFlagsZSP(processor, res);

    processor->h = res;

    processor->pc += inst_length;
    return inst_cycles;
}

// INR L
int inr_l(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t res = processor->l+1;

    SetFlagsZSP(processor, res);

    processor->l = res;

    processor->pc += inst_length;
    return inst_cycles;
}

// ADD A
int add_a(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a + (uint16_t)processor->a;

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// ADD B
int add_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a + (uint16_t)processor->b;

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// ADD C
int add_c(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a + (uint16_t)processor->c;

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// ADD D
int add_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a + (uint16_t)processor->d;

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// ADD E
int add_e(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a + (uint16_t)processor->e;

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// ADD H
int add_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a + (uint16_t)processor->h;

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// ADD L
int add_l(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a + (uint16_t)processor->l;

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// ADD M
int add_m(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->h << 8) | processor->l;
    uint16_t res = ((uint16_t)(processor->a)) + ((uint16_t)(processor->memory->data[addr]));
    SetFlagsAfterArithmeticOperation(processor, res);
    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// SUB B
int sub_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a - (uint16_t)processor->b;

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// SUB C
int sub_c(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a - (uint16_t)processor->c;

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// SUB D
int sub_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a - (uint16_t)processor->d;

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// SUB E
int sub_e(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a - (uint16_t)processor->e;

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// SUB H
int sub_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a - (uint16_t)processor->h;

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// SUB L
int sub_l(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a - (uint16_t)processor->l;

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// SUB A
int sub_a(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a - (uint16_t)processor->a;

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// SUB M
int sub_m(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->h << 8) | processor->l;
    uint16_t res = ((uint16_t)(processor->a)) - ((uint16_t)(processor->memory->data[addr]));
    SetFlagsAfterArithmeticOperation(processor, res);
    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// ADC A
int adc_a(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a + (uint16_t)processor->a + ((processor->flags&(1 << 3))?1:0);

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// ADC B
int adc_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a + (uint16_t)processor->b + ((processor->flags&(1 << 3))?1:0);

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}
// ADC C
int adc_c(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a + (uint16_t)processor->c + ((processor->flags&(1 << 3))?1:0);

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// ADC d
int adc_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a + (uint16_t)processor->d + ((processor->flags&(1 << 3))?1:0);

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// ADC E
int adc_e(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a + (uint16_t)processor->e + ((processor->flags&(1 << 3))?1:0);

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// ADC H
int adc_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a + (uint16_t)processor->h + ((processor->flags&(1 << 3))?1:0);

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// ADC L
int adc_l(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a + (uint16_t)processor->l + ((processor->flags&(1 << 3))?1:0);

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// ADC M
int adc_m(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->h << 8) | processor->l;
    uint16_t res = ((uint16_t)(processor->a)) + ((uint16_t)(processor->memory->data[addr])) + ((processor->flags&(1 << 3))?1:0);
    SetFlagsAfterArithmeticOperation(processor, res);
    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// SBB A
int sbb_a(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a - (uint16_t)processor->a - ((processor->flags&(1 << 3))?1:0);

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// SBB B
int sbb_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a - (uint16_t)processor->b - ((processor->flags&(1 << 3))?1:0);

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// SBB C
int sbb_c(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a - (uint16_t)processor->c - ((processor->flags&(1 << 3))?1:0);

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// SBB D
int sbb_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a - (uint16_t)processor->d - ((processor->flags&(1 << 3))?1:0);

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// SBB E
int sbb_e(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a - (uint16_t)processor->e - ((processor->flags&(1 << 3))?1:0);

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// SBB H
int sbb_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a - (uint16_t)processor->h - ((processor->flags&(1 << 3))?1:0);

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// SBB L
int sbb_l(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = (uint16_t)processor->a - (uint16_t)processor->l - ((processor->flags&(1 << 3))?1:0);

    SetFlagsAfterArithmeticOperation(processor, res);

    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// SBB M
int sbb_m(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->h << 8) | processor->l;
    uint16_t res = ((uint16_t)(processor->a)) - ((uint16_t)(processor->memory->data[addr])) - ((processor->flags&(1 << 3))?1:0);
    SetFlagsAfterArithmeticOperation(processor, res);
    processor->a = (res&0xFF);

    processor->pc += inst_length;
    return inst_cycles;
}

// ORA_A
int ora_a(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->a | processor->a;

    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// ORA_B
int ora_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->a | processor->b;

    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// ORA_C
int ora_c(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->a | processor->c;

    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// ORA_D
int ora_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->a | processor->d;

    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// ORA_E
int ora_e(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->a | processor->e;

    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// ORA_H
int ora_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->a | processor->h;

    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// ORA_L
int ora_l(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = processor->a | processor->l;

    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// ORA M
int ora_m(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->h << 8) | processor->l;
    processor->a = processor->a | processor->memory->data[addr];
    SetFlagsAfterLogicalOperation(processor);

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV B,M
int mvb_m(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->h << 8) | processor->l;
    processor->b = processor->memory->data[addr];

    processor->pc += inst_length;
    return inst_cycles;
}

// CMP A
int cmp_a(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = ((uint16_t)(processor->a)) - ((uint16_t)(processor->a));
    SetFlagsAfterArithmeticOperation(processor, res);

    processor->pc += inst_length;
    return inst_cycles;
}

// CMP B
int cmp_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = ((uint16_t)(processor->a)) - ((uint16_t)(processor->b));
    SetFlagsAfterArithmeticOperation(processor, res);

    processor->pc += inst_length;
    return inst_cycles;
}

// CMP C
int cmp_c(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = ((uint16_t)(processor->a)) - ((uint16_t)(processor->c));
    SetFlagsAfterArithmeticOperation(processor, res);

    processor->pc += inst_length;
    return inst_cycles;
}

// CMP D
int cmp_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = ((uint16_t)(processor->a)) - ((uint16_t)(processor->d));
    SetFlagsAfterArithmeticOperation(processor, res);

    processor->pc += inst_length;
    return inst_cycles;
}

// CMP E
int cmp_e(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = ((uint16_t)(processor->a)) - ((uint16_t)(processor->e));
    SetFlagsAfterArithmeticOperation(processor, res);

    processor->pc += inst_length;
    return inst_cycles;
}

// CMP H
int cmp_h(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = ((uint16_t)(processor->a)) - ((uint16_t)(processor->h));
    SetFlagsAfterArithmeticOperation(processor, res);

    processor->pc += inst_length;
    return inst_cycles;
}

// CMP L
int cmp_l(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t res = ((uint16_t)(processor->a)) - ((uint16_t)(processor->l));
    SetFlagsAfterArithmeticOperation(processor, res);

    processor->pc += inst_length;
    return inst_cycles;
}

// CMP M
int cmp_m(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->h << 8) | processor->l;
    uint16_t res = ((uint16_t)(processor->a)) - ((uint16_t)(processor->memory->data[addr]));
    SetFlagsAfterArithmeticOperation(processor, res);

    processor->pc += inst_length;
    return inst_cycles;
}

// MOV L,M
int mvl_m(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = (processor->h << 8) | processor->l;
    processor->l = processor->memory->data[addr];

    processor->pc += inst_length;
    return inst_cycles;
}

// LHLD addr
int lhld(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t highaddr = processor->memory->data.at(processor->pc+1);
    uint8_t lowaddr = processor->memory->data.at(processor->pc+2);
    uint16_t addr = (uint16_t)((lowaddr << 8) | highaddr);

    processor->h = processor->memory->data[addr+1];
    processor->l = processor->memory->data[addr];

    processor->pc += inst_length;
    return inst_cycles;
}

// SHLD addr
int shld(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t highaddr = processor->memory->data.at(processor->pc+1);
    uint8_t lowaddr = processor->memory->data.at(processor->pc+2);
    uint16_t addr = (uint16_t)((lowaddr << 8) | highaddr);

    processor->memory->data[addr+1] = processor->h;
    processor->memory->data[addr] = processor->l;

    processor->pc += inst_length;
    return inst_cycles;
}

// STAX B
int stax_b(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = ((processor->b << 8) | processor->c);
    processor->memory->data[addr] = processor->a;

    processor->pc += inst_length;
    return inst_cycles;
}

// STAX D
int stax_d(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint16_t addr = ((processor->d << 8) | processor->e);
    processor->memory->data[addr] = processor->a;

    processor->pc += inst_length;
    return inst_cycles;
}

// STC
int stc(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->flags |= (1 << 3);

    processor->pc += inst_length;
    return inst_cycles;
}

// CMC
int cmc(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    if (processor->flags&(1 << 3)) {
        processor->flags &= ~(1 << 3);
    } else {
        processor->flags |= (1 << 3);
    }

    processor->pc += inst_length;
    return inst_cycles;
}

// CMA
int cma(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->a = ~(processor->a);

    processor->pc += inst_length;
    return inst_cycles;
}

// SPHL
int sphl(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    processor->sp  = ((processor->h << 8) |  processor->l);

    processor->pc += inst_length;
    return inst_cycles;
}

// PCHL
int pchl(INSTDEF) {
    int inst_cycles = opcode.at(2).toInt();

    processor->pc  = ((processor->h << 8) |  processor->l);

    return inst_cycles;
}

// XTHL
int xthl(INSTDEF) {
    int inst_length = opcode.at(1).toInt();
    int inst_cycles = opcode.at(2).toInt();

    uint8_t h = processor->h;
    uint8_t l = processor->l;
    processor->h = processor->memory->data[processor->sp+1];
    processor->l = processor->memory->data[processor->sp];
    processor->memory->data[processor->sp+1] = h;
    processor->memory->data[processor->sp] = l;

    processor->pc += inst_length;
    return inst_cycles;
}
