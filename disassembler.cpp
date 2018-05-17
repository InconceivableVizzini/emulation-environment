#include "disassembler.h"
#include "cpu.h"

#include <QDebug>

#define INS_STR_BUFF_LEN 80



char *str_replace(char *orig, char const *rep, char const *with) {
    char *result;  // the return string
    char *ins;     // the next insert point
    char *tmp;     // varies
    int len_rep;   // length of rep (the string to remove)
    int len_with;  // length of with (the string to replace rep with)
    int len_front; // distance between rep and end of last rep
    int count;     // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = (char*)malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}


Disassembler::Disassembler(CPU *processor) {
    this->cpu = processor;
}

Disassembler::~Disassembler() {
}

QList<QString> Disassembler::OpCode(uint16_t addr) {
    char opcode_str[5];
    snprintf(opcode_str, 5, "0x%02x", (unsigned char)(this->cpu->memory->data.at(addr)));
    QVariant list_v = this->cpu->instruction_list.value(opcode_str);
    if (!list_v.isValid()) {
        qDebug() << "Unknown instruction " << opcode_str;
        if (settings.value("Disassembly/HaltAtUnknownInstruction", true).toBool()) {
            this->cpu->flags &= ~(1 << 6); // Disable the processor.
        }
        QList<QString> empty = QList<QString>();
        empty.append(QString(""));
        return empty;
    }
    QList<QString> opcode;
    QList<QVariant> opcode_def = list_v.toList();
    opcode.append(opcode_def.at(2).toString()); // length
    return opcode;
}
QList<QString> Disassembler::Disassemble(uint16_t addr) {
    char opcode_str[5];
    snprintf(opcode_str, 5, "0x%02x", (unsigned char)(this->cpu->memory->data.at(addr)));
    QVariant list_v = this->cpu->instruction_list.value(opcode_str);
    if (!list_v.isValid()) {
        qDebug() << "Unknown instruction " << opcode_str;
        if (settings.value("Disassembly/HaltAtUnknownInstruction", true).toBool()) {
            this->cpu->flags &= ~(1 << 6); // Disable the processor.
        }
        return QList<QString>();
    }

    QList<QVariant> opcode_def = list_v.toList();
    QString nemonic = opcode_def.at(0).toString();
    QString mode = opcode_def.at(1).toString();
    QString length = opcode_def.at(2).toString();
    QString duration = opcode_def.at(3).toString();

    char out[INS_STR_BUFF_LEN];
    char formatting[INS_STR_BUFF_LEN];
    char* tmp = NULL;
    if (mode == "IMM") {
        snprintf(out, INS_STR_BUFF_LEN, "%04X %02X %s", addr, (unsigned char)(this->cpu->memory->data.at(addr)), nemonic.toStdString().c_str());
        if (strlen(out)>3 && !strcmp(out+strlen(out)-3, "a16")) {
            tmp = str_replace(out, "a16", "$%02x%02x");
            if (tmp != NULL) {
                strncpy(out, tmp, sizeof(out));
                free(tmp);
                tmp = NULL;
            }
            snprintf(formatting, INS_STR_BUFF_LEN, out,
                     (unsigned char)(this->cpu->memory->data.at(addr+2)),
                     (unsigned char)(this->cpu->memory->data.at(addr+1)));
        } else if (strlen(out)>3 && !strcmp(out+strlen(out)-3, "d16")) {
            tmp = str_replace(out, "d16", "#$%02x%02x");
            if (tmp != NULL) {
                strncpy(out, tmp, sizeof(out));
                free(tmp);
                tmp = NULL;
            }
            snprintf(formatting, INS_STR_BUFF_LEN, out,
                     (unsigned char)(this->cpu->memory->data.at(addr+2)),
                     (unsigned char)(this->cpu->memory->data.at(addr+1)));
        } else if (strlen(out)>2 && !strcmp(out+strlen(out)-2, "d8")) {
            tmp = str_replace(out, "d8", "#$%02x");
            if (tmp != NULL) {
                strncpy(out, tmp, sizeof(out));
                free(tmp);
                tmp = NULL;
            }
            snprintf(formatting, INS_STR_BUFF_LEN, out,
                     (unsigned char)(this->cpu->memory->data.at(addr+1)));
        }

        strncpy(out, formatting, sizeof(out));
    } else {
        snprintf(out, INS_STR_BUFF_LEN, "%04X %02X %s", addr, (unsigned char)(this->cpu->memory->data.at(addr)), nemonic.toStdString().c_str());
    }
    //qDebug() << out;
    QList<QString> ret;
    ret.append(out);
    ret.append(length);
    ret.append(duration);
    return ret;
}

QList<QVariant> Disassembler::Disassemble(uint16_t addr, int diss_n_instructions) {
    Q_UNUSED(addr);
    Q_UNUSED(diss_n_instructions);
    QList<QVariant> empty;
    empty.append(QVariant());
    return empty;
}
