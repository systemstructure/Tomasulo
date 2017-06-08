#include "tomasulo.h"
#include <QStringList>
#include <QDebug>
#include <string.h>

Tomasulo::Tomasulo()
{
    init();

}

void Tomasulo::init()
{
    instr_num = 0;
    memory_num = 0;
    memset(memory,0,sizeof(memory));
}

void Tomasulo::addOneInstr(QString str)
{
    QStringList list = str.split(' ');
    int l = list.size();
    for(int i=0;i<l;i++)
        qDebug()<<list[i];
    for(int i=0;i<6;i++)
    {
        if(list[0] == instr_name[i])
            instr[instr_num].type = i;
    }
    for(int i=1;i<l;i++)
        instr[instr_num].parameter[i-1] = list[i];

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
