#ifndef TOMASULO_H
#define TOMASULO_H

const int MAX_INSTR_NUM = 100;
#include <QString>


struct instruction
{
    int type;
    QString parameter[3];
    friend std::ostream & operator <<(std::ostream &out, instruction &a)
    {
        out << a.type;
        for(int i=0;i<3;i++)
        {
            out << a.parameter[i].toStdString() << ' ';
        }
        out << std::endl;
    }
};

struct LSStation
{
    bool isBusy;
    int address;
};


class Tomasulo
{
public:
    Tomasulo();
    instruction instr[MAX_INSTR_NUM];
    int instr_num;
    int curr_instr_pos;
    QString instr_name[6] = {"ADDD", "SUBD", "MULD", "DIVD", "LD", "ST"};

    LSStation load[3];
    LSStation store[3];

    int memory[4096];

public:
    void init();

    void addOneInstr(QString str);

};

#endif // TOMASULO_H
