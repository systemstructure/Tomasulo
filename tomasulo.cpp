#include "tomasulo.h"
#include <QStringList>
#include <QDebug>

Tomasulo::Tomasulo()
{
    init();
    instr_num = 0;
}

void Tomasulo::init()
{

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
