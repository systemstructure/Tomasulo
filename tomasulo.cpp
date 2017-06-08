#include "tomasulo.h"
#include <QStringList>
#include <string.h>

Tomasulo::Tomasulo()
{
    init();
    myTest();
}

void Tomasulo::init()
{
    instr_num = 0;
    memory_num = 0;
    curr_instr_pos = 0;
    addRunningNo = -1;
    mulRunningNo = -1;

    memset(memory,0,sizeof(memory));
    memset(Qi,0,sizeof(Qi));
    memset(reg,0,sizeof(reg));

    for(int i=1;i<6;i++)
        station[i].isBusy = false;
    for(int i=1;i<7;i++)
        lsStation[i].isBusy = false;
}

void Tomasulo::addOneInstr(QString str)
{
    QStringList list = str.split(' ');
    int l = list.size();
//    for(int i=0;i<l;i++)
//        qDebug()<<list[i];
    for(int i=0;i<6;i++)
    {
        if(list[0] == instr_name[i])
            instr[instr_num].type = i;  //Decide type of instruction
    }
    for(int i=1;i<l;i++)
        instr[instr_num].parameter[i-1] = list[i];

    switch (instr[instr_num].type) {
    case ADDD:
    case SUBD:
    case MULD:
    case DIVD:
    {
        instr[instr_num].rd = convParaToRegNo(instr[instr_num].parameter[0]);
        instr[instr_num].rs = convParaToRegNo(instr[instr_num].parameter[1]);
        instr[instr_num].rt = convParaToRegNo(instr[instr_num].parameter[2]);
        break;
    }
    case LD:
    {
        instr[instr_num].rd = convParaToRegNo(instr[instr_num].parameter[0]);
        instr[instr_num].addr = instr[instr_num].parameter[1].toInt();
        break;
    }
    case ST:
    {
        instr[instr_num].rs = convParaToRegNo(instr[instr_num].parameter[0]);
        instr[instr_num].addr = instr[instr_num].parameter[1].toInt();
        break;
    }
    default:
    {
        qDebug() << "Wrong case of instr type.";
    }
    }

    instr_num++;

}

void Tomasulo::addMemory(QString str)
{
    QStringList list = str.split(';');
    int l = list.size();
    for(int i=0;i<l;i++)
    {
        int pos = list[i].indexOf(":");
//        qDebug()<<list[i].left(pos);
//        qDebug()<<list[i].mid(pos);
        int address = list[i].left(pos).toInt();
        float data = list[i].mid(pos+1).toFloat();
        addOneMemory(address, data);
    }
}

void Tomasulo::addOneMemory(int address, float data)
{
    qDebug()<<address<<data;
    if(data == 0)
    {
        if(memory[address] != 0)
        {
            memory[address] = 0;
            memory_num--;
        }
        return;
    }
    if(memory[address] == 0)
        memory_num++;
    memory[address] = data;
}

void Tomasulo::runOneStep()
{
    issue();
    execute();
    writeBack();
}

void Tomasulo::issue()
{
    instruction cur_ins = instr[curr_instr_pos];

}

void Tomasulo::execute()
{

}

void Tomasulo::writeBack()
{

}

void Tomasulo::doWriteBack(int instr_no)
{
    instruction ins = instr[instr_no];
    switch (ins.type) {
    case ADDD:
    {
        reg[ins.rd] = reg[ins.rs] + reg[ins.rt];
        break;
    }
    case SUBD:
    {
        reg[ins.rd] = reg[ins.rs] - reg[ins.rt];
        break;
    }
    case MULD:
    {
        reg[ins.rd] = reg[ins.rs] * reg[ins.rt];
        break;
    }
    case DIVD:
    {
        reg[ins.rd] = reg[ins.rs] / reg[ins.rt];
        break;
    }
    case LD:
    {
        reg[ins.rd] = memory[ins.addr];
        break;
    }
    case ST:
    {
        memory[ins.addr] = reg[ins.rs];
        break;
    }
    default:
    {
        qDebug() << "Wrong case of instr type in doWriteBack().";
    }
    }
}


int Tomasulo::convParaToRegNo(QString str)
{
    QString no_str = str.mid(1);
    int reg_no = no_str.toInt();
    return reg_no;
}

void Tomasulo::printRegs()
{
    for(int i = 0; i < 11; i++)
    {
        QString ready_or_not = " is Ready.";
        if (Qi[i] > 0) ready_or_not = "is not ready..";
        qDebug() << "F" << i << ": " << reg[i] << " " << ready_or_not;
    }
}

int Tomasulo::findAvailableStation(int ins_type)
{
    switch (ins_type)
    {
    case ADDD:
    case SUBD:
    {
        for(int i = 1; i <= 3; ++i) {
            if(!station[i].isBusy) {
                return i;
            }
        }
        break;
    }
    case MULD:
    case DIVD:
    {
        for(int i = 4; i <= 5; ++i) {
            if(!station[i].isBusy) {
                return i;
            }
        }
    }
    case LD:
    case ST:
    {
        for(int i = 1; i <= 6; ++i) {
            if(!lsStation[i].isBusy) {
                return i;
            }
        }
    }
    }
}

void Tomasulo::myTest()
{
    memory[10] = 0.5;
    memory[11] = 1.5;
    memory[12] = 2.5;

    addOneInstr(QString("LD F1 11"));
    addOneInstr(QString("LD F2 12"));
    addOneInstr(QString("ADDD F0 F1 F2"));
    addOneInstr(QString("MULD F3 F1 F2"));
    addOneInstr(QString("ST F3 23"));

    for (int i = 0; i < instr_num; i++) {
        instr[i].print();
        doWriteBack(i);
    }

    printRegs();
    qDebug() << memory[23];
}
