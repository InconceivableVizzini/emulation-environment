#include "cpu.h"
#include "disassembler.h"

#include "i8080.h"

#include <QDebug>

#include <QCoreApplication>

CPU::CPU(QMutex *mu, MemoryMap *mem)
{
    mutex = mu;
    memory = mem;
    memory->data.resize(0x10000); // 16 address pins, for 2^16=65536 (0x10000 in hex)
                                  // possible addresses.
                                  // Some memory cannot be written to (ROM),
                                  // so a custom QByteArray might be necessary.
    sp = 0;
    pc = 0;
    flags = 0;

    instruction_handlers[0x0] = noop;
    instruction_handlers[0x01] = lxi_b;
    instruction_handlers[0x02] = stax_b;
    instruction_handlers[0x03] = inx_b;
    instruction_handlers[0x04] = inr_b;
    instruction_handlers[0x05] = dcr_b;
    instruction_handlers[0x06] = mvi_b;
    instruction_handlers[0x07] = rlc;
    instruction_handlers[0x09] = dad_b;
    instruction_handlers[0x0a] = ldax_b;
    instruction_handlers[0x0b] = dcx_b;
    instruction_handlers[0x0c] = inr_c;
    instruction_handlers[0x0d] = dcr_c;
    instruction_handlers[0x0e] = mvi_c;
    instruction_handlers[0x0f] = rrc;
    instruction_handlers[0x11] = lxi_d;
    instruction_handlers[0x12] = stax_d;
    instruction_handlers[0x13] = inx_d;
    instruction_handlers[0x14] = inr_d;
    instruction_handlers[0x15] = dcr_d;
    instruction_handlers[0x16] = mvi_d;
    instruction_handlers[0x17] = ral;
    instruction_handlers[0x19] = dad_d;
    instruction_handlers[0x1a] = ldax_d;
    instruction_handlers[0x1b] = dcx_d;
    instruction_handlers[0x1c] = inr_e;
    instruction_handlers[0x1d] = dcr_e;
    instruction_handlers[0x1e] = mvi_e;
    instruction_handlers[0x1f] = rar;
    instruction_handlers[0x21] = lxi_h;
    instruction_handlers[0x22] = shld;
    instruction_handlers[0x23] = inx_h;
    instruction_handlers[0x24] = inr_h;
    instruction_handlers[0x25] = dcr_h;
    instruction_handlers[0x26] = mvi_h;
    instruction_handlers[0x29] = dad_h;
    instruction_handlers[0x2a] = lhld;
    instruction_handlers[0x2b] = dcx_h;
    instruction_handlers[0x2c] = inr_l;
    instruction_handlers[0x2d] = dcr_l;
    instruction_handlers[0x2e] = mvi_l;
    instruction_handlers[0x2f] = cma;
    instruction_handlers[0x31] = lxi_sp;
    instruction_handlers[0x32] = sta;
    instruction_handlers[0x33] = inx_sp;
    instruction_handlers[0x34] = inr_m;
    instruction_handlers[0x35] = dcr_m;
    instruction_handlers[0x36] = mvi_m;
    instruction_handlers[0x37] = stc;
    instruction_handlers[0x39] = dad_sp;
    instruction_handlers[0x3a] = lda;
    instruction_handlers[0x3b] = dcx_sp;
    instruction_handlers[0x3c] = inr_a;
    instruction_handlers[0x3d] = dcr_a;
    instruction_handlers[0x3e] = mvi_a;
    instruction_handlers[0x3f] = cmc;
    instruction_handlers[0x41] = mvb_c;
    instruction_handlers[0x42] = mvb_d;
    instruction_handlers[0x43] = mvb_e;
    instruction_handlers[0x44] = mvb_h;
    instruction_handlers[0x45] = mvb_l;
    instruction_handlers[0x46] = mvb_m;
    instruction_handlers[0x47] = mvb_a;
    instruction_handlers[0x48] = mvc_b;
    instruction_handlers[0x4a] = mvc_d;
    instruction_handlers[0x4b] = mvc_e;
    instruction_handlers[0x4c] = mvc_h;
    instruction_handlers[0x4d] = mvc_l;
    instruction_handlers[0x4e] = mvc_m;
    instruction_handlers[0x4f] = mvc_a;
    instruction_handlers[0x50] = mvd_b;
    instruction_handlers[0x51] = mvd_c;
    instruction_handlers[0x53] = mvd_e;
    instruction_handlers[0x54] = mvd_h;
    instruction_handlers[0x55] = mvd_l;
    instruction_handlers[0x56] = mvd_m;
    instruction_handlers[0x57] = mvd_a;
    instruction_handlers[0x58] = mve_b;
    instruction_handlers[0x59] = mve_c;
    instruction_handlers[0x5a] = mve_d;
    instruction_handlers[0x5c] = mve_h;
    instruction_handlers[0x5d] = mve_l;
    instruction_handlers[0x5e] = mve_m;
    instruction_handlers[0x5f] = mve_a;
    instruction_handlers[0x60] = mvh_b;
    instruction_handlers[0x61] = mvh_c;
    instruction_handlers[0x62] = mvh_d;
    instruction_handlers[0x63] = mvh_e;
    instruction_handlers[0x65] = mvh_l;
    instruction_handlers[0x66] = mvh_m;
    instruction_handlers[0x67] = mvh_a;
    instruction_handlers[0x68] = mvl_b;
    instruction_handlers[0x69] = mvl_c;
    instruction_handlers[0x6a] = mvl_d;
    instruction_handlers[0x6b] = mvl_e;
    instruction_handlers[0x6c] = mvl_h;
    instruction_handlers[0x6e] = mvl_m;
    instruction_handlers[0x6f] = mvl_a;
    instruction_handlers[0x70] = mvm_b;
    instruction_handlers[0x71] = mvm_c;
    instruction_handlers[0x72] = mvm_d;
    instruction_handlers[0x73] = mvm_e;
    instruction_handlers[0x74] = mvm_h;
    instruction_handlers[0x75] = mvm_l;
    instruction_handlers[0x76] = hlt;
    instruction_handlers[0x77] = mvm_a;
    instruction_handlers[0x78] = mva_b;
    instruction_handlers[0x79] = mva_c;
    instruction_handlers[0x7a] = mva_d;
    instruction_handlers[0x7b] = mva_e;
    instruction_handlers[0x7c] = mva_h;
    instruction_handlers[0x7d] = mva_l;
    instruction_handlers[0x7e] = mva_m;
    instruction_handlers[0x80] = add_b;
    instruction_handlers[0x81] = add_c;
    instruction_handlers[0x82] = add_d;
    instruction_handlers[0x83] = add_e;
    instruction_handlers[0x84] = add_h;
    instruction_handlers[0x85] = add_l;
    instruction_handlers[0x86] = add_m;
    instruction_handlers[0x87] = add_a;
    instruction_handlers[0x88] = adc_b;
    instruction_handlers[0x89] = adc_c;
    instruction_handlers[0x8a] = adc_d;
    instruction_handlers[0x8b] = adc_e;
    instruction_handlers[0x8c] = adc_h;
    instruction_handlers[0x8d] = adc_l;
    instruction_handlers[0x8e] = adc_m;
    instruction_handlers[0x8f] = adc_a;
    instruction_handlers[0x90] = sub_b;
    instruction_handlers[0x91] = sub_c;
    instruction_handlers[0x92] = sub_d;
    instruction_handlers[0x93] = sub_e;
    instruction_handlers[0x94] = sub_h;
    instruction_handlers[0x95] = sub_l;
    instruction_handlers[0x96] = sub_m;
    instruction_handlers[0x97] = sub_a;
    instruction_handlers[0x98] = sbb_b;
    instruction_handlers[0x99] = sbb_c;
    instruction_handlers[0x9a] = sbb_d;
    instruction_handlers[0x9b] = sbb_e;
    instruction_handlers[0x9c] = sbb_h;
    instruction_handlers[0x9d] = sbb_l;
    instruction_handlers[0x9e] = sbb_m;
    instruction_handlers[0x9f] = sbb_a;
    instruction_handlers[0xa0] = ana_b;
    instruction_handlers[0xa1] = ana_c;
    instruction_handlers[0xa2] = ana_d;
    instruction_handlers[0xa3] = ana_e;
    instruction_handlers[0xa4] = ana_h;
    instruction_handlers[0xa5] = ana_l;
    instruction_handlers[0xa6] = ana_m;
    instruction_handlers[0xa7] = ana_a;
    instruction_handlers[0xa8] = xra_b;
    instruction_handlers[0xa9] = xra_c;
    instruction_handlers[0xaa] = xra_d;
    instruction_handlers[0xab] = xra_e;
    instruction_handlers[0xac] = xra_h;
    instruction_handlers[0xad] = xra_l;
    instruction_handlers[0xae] = xra_m;
    instruction_handlers[0xaf] = xra_a;
    instruction_handlers[0xb0] = ora_b;
    instruction_handlers[0xb1] = ora_c;
    instruction_handlers[0xb2] = ora_d;
    instruction_handlers[0xb3] = ora_e;
    instruction_handlers[0xb4] = ora_h;
    instruction_handlers[0xb5] = ora_l;
    instruction_handlers[0xb6] = ora_m;
    instruction_handlers[0xb7] = ora_a;
    instruction_handlers[0xb8] = cmp_b;
    instruction_handlers[0xb9] = cmp_c;
    instruction_handlers[0xba] = cmp_d;
    instruction_handlers[0xbb] = cmp_e;
    instruction_handlers[0xbc] = cmp_h;
    instruction_handlers[0xbd] = cmp_l;
    instruction_handlers[0xbe] = cmp_m;
    instruction_handlers[0xbf] = cmp_a;
    instruction_handlers[0xc0] = rnz;
    instruction_handlers[0xc1] = pop_b;
    instruction_handlers[0xc2] = jnz;
    instruction_handlers[0xc3] = jmp;
    instruction_handlers[0xc4] = cnz;
    instruction_handlers[0xc5] = push_b;
    instruction_handlers[0xc6] = adi;
    instruction_handlers[0xc8] = rz;
    instruction_handlers[0xc9] = ret;
    instruction_handlers[0xca] = jz;
    instruction_handlers[0xcc] = cz;
    instruction_handlers[0xcd] = call;
    instruction_handlers[0xce] = aci;
    instruction_handlers[0xd0] = rnc;
    instruction_handlers[0xd1] = pop_d;
    instruction_handlers[0xd2] = jnc;
    instruction_handlers[0xd4] = cnc;
    instruction_handlers[0xd5] = push_d;
    instruction_handlers[0xd6] = sui;
    instruction_handlers[0xd8] = rc;
    instruction_handlers[0xda] = jc;
    instruction_handlers[0xdc] = cc;
    instruction_handlers[0xde] = sbi;
    instruction_handlers[0xe0] = rpo;
    instruction_handlers[0xe1] = pop_h;
    instruction_handlers[0xe2] = jpo;
    instruction_handlers[0xe3] = xthl;
    instruction_handlers[0xe4] = cpo;
    instruction_handlers[0xe5] = push_h;
    instruction_handlers[0xe6] = ani;
    instruction_handlers[0xe8] = rpe;
    instruction_handlers[0xe9] = pchl;
    instruction_handlers[0xea] = jpe;
    instruction_handlers[0xeb] = xchg;
    instruction_handlers[0xec] = cpe;
    instruction_handlers[0xee] = xri;
    instruction_handlers[0xf0] = rp;
    instruction_handlers[0xf1] = pop_psw;
    instruction_handlers[0xf2] = jp;
    instruction_handlers[0xf4] = cp;
    instruction_handlers[0xf5] = push_psw;
    instruction_handlers[0xf6] = ori;
    instruction_handlers[0xf8] = rm;
    instruction_handlers[0xf9] = sphl;
    instruction_handlers[0xfa] = jm;
    instruction_handlers[0xfb] = ei;
    instruction_handlers[0xfc] = cm;
    instruction_handlers[0xfe] = cpi;

    // Load instruction definitions. These are used by ::emulate() and
    // any Disassemblers. This loads data such as how many cycles
    // instructions take to execute.
    QFile cpu_defs_file("://assets/8080_instructions.json");
    cpu_defs_file.open(QIODevice::ReadOnly);
    QByteArray cpu_instruction_defs = cpu_defs_file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(cpu_instruction_defs));
    QJsonObject cpu_defs = doc.object();
    this->instruction_list = cpu_defs.value("opcodes").toVariant().toMap();

    this->executed_instructions = new ExecutedInstructionsListModel();

    disassembler = new Disassembler(this);
}

CPU::~CPU()
{
    delete disassembler;
    delete this->executed_instructions;
}

void CPU::reset()
{
    mutex->lock();
    this->flags &= ~((1 << 0) & (1 << 1) & (1 << 2) & (1 << 3) & (1 << 4) & (1 << 5));
    this->pc = 0;
    this->sp = 0;
    this->a = 0;
    this->b = 0;
    this->c = 0;
    this->d = 0;
    this->e = 0;
    this->h = 0;
    this->l = 0;
    mutex->unlock();
}

void CPU::setCallback(uint8_t opcode, int (*cb)(CPU *, QList<QString>))
{
    instruction_callbacks[opcode] = cb;
}

void CPU::interrupt(int memory_vector) {
    //qDebug() << "interrupt - " << memory_vector;
    // PUSH PC
    // Set PC
    // DI
    this->memory->data[this->sp-1] = (this->pc&0xFF00)>>8;
    this->memory->data[this->sp-2] = (this->pc&0xFF);
    this->sp -= 2;
    this->pc = 8 * memory_vector;
    this->flags &= ~(1<<5);
}

void CPU::emulate() {
    int nowms = 0;
    int mspassed = 0;
    float roll_avg_mspassed = 0;
    int mstoint = 0;
    int whichinterrupt = 1;
    bool interrupts_enabled = true;
    forever {
        mutex->lock();

        if ( QThread::currentThread()->isInterruptionRequested() || !(this->flags&(1<<6)) ) {
            mutex->unlock();
            return;
        }

        // determine how much time has passed since the last iteration
        // determine how many cycles could be performed in that amount of time.
        // emulate instructions until that many cycles have been exhausted.

        if (!this->now.isValid()) {
            this->now.start();
        }

        nowms = this->now.elapsed();
        interrupts_enabled = (this->flags&(1<<5));

        if (mspassed == 0) {
          mspassed = nowms;
          mstoint = nowms + 16;
        }

        // generate interrupts
        if (interrupts_enabled && (nowms > mstoint))  {
            this->interrupt(whichinterrupt);
            whichinterrupt = (whichinterrupt == 1)?2:1;
            mstoint = nowms + 8;
        }

        //mutex->unlock();

        // i8080 clocked at 2MHz.
        // 2M cycles / 1 sec, or 2 cycles / 1MS
        // so 2*(elapsed ms).

        int cycles = 0;
        //int cycles_elapsed = (2*(nowms-mspassed));
        QList<QString> opcode;
        QList<QVariant> opcode_with_state;


        opcode = disassembler->Disassemble(this->pc);

        //mutex->lock();

        // Last check if the processor has been disabled,
        // e.g. by a machine or disassembler.
        if (!(this->flags&(1<<6))) {
            mutex->unlock();
            return;
        }

        // big array of byte values to instruction callbacks...
        uint8_t opcode_val = (unsigned char)(this->memory->data.at(this->pc));
        int (*inst_handler)(CPU*, QList<QString>) = this->instruction_handlers[opcode_val];
        int (*inst_cb)(CPU*, QList<QString>) = this->instruction_callbacks[opcode_val];

        if (inst_handler) {
          cycles += (*inst_handler)(this, opcode); // cpu provided instruction handler
        } else if (inst_cb) {
            cycles += (*inst_cb)(this, opcode); // machine provided instruction handler callback
                                                // (eg, for IN/OUT instructions, specific to the machine hardware)
        } else {
            qDebug() << "Unknown instruction" << opcode;
        }

        if (this->flags&(1 << 8)) {
            opcode_with_state.clear();
            opcode_with_state.append(QVariant(opcode.at(0)));
            opcode_with_state.append(QVariant(this->pc));
            opcode_with_state.append(QVariant(this->sp));
            opcode_with_state.append(QVariant(this->a));
            opcode_with_state.append(QVariant(this->b));
            opcode_with_state.append(QVariant(this->c));
            opcode_with_state.append(QVariant(this->d));
            opcode_with_state.append(QVariant(this->e));
            opcode_with_state.append(QVariant(this->h));
            opcode_with_state.append(QVariant(this->l));
            opcode_with_state.append(QVariant(this->flags));

            executed_instructions->addInstruction(opcode_with_state);
        }

        roll_avg_mspassed = ((1.0 / 25) * (nowms - mspassed)) + (1.0 - (1.0 / 25)) * roll_avg_mspassed;

        mspassed = nowms;

        mutex->unlock();
    }

}
