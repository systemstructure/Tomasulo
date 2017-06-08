#ifndef TOMASULO_H
#define TOMASULO_H

const int MAX_INSTR_NUM = 10000;

#include <QString>


struct instruction
{

    int type;
    QString parameter[3];
    int rs, rt, rd;
    int addr;
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
    int clocktime;
    int address;
};

struct ReStation
{
    QString name;
    bool isBusy;
    int clocktime;
    int op;
    int Vj,Vk,Qi,Qk;
};

class Tomasulo
{
public:
    Tomasulo();
    instruction instr[MAX_INSTR_NUM];
    int instr_num;
    int curr_instr_pos;
    const QString instr_name[6] = {"ADDD", "SUBD", "MULD", "DIVD", "LD", "ST"};
    const int clocktime[6] = {2,2,10,40,2,2};

    const int ADDD = 0;
    const int SUBD = 1;
    const int MULD = 2;
    const int DIVD = 3;
    const int LD = 4;
    const int ST = 5;

    LSStation load[3];
    LSStation store[3];

    ReStation station[5];

    float memory[4096];
    int memory_num;

    float reg[11];

public:
    void init();

    void addOneInstr(QString str);

    void addMemory(QString str);

    void addOneMemory(int address, float data);

    void convParaToRegNo(QString str);
};

#endif // TOMASULO_H
