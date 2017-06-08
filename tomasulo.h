#ifndef TOMASULO_H
#define TOMASULO_H

const int MAX_INSTR_NUM = 10000;

#include <QString>
#include <QDebug>


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

    void print() {
        qDebug() << type << ' '<< parameter[0] << ' ' << parameter[1] << ' ' << parameter[2]
                 << ' ' << addr;
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
    int time;
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

    static const int ADDD = 0;
    static const int SUBD = 1;
    static const int MULD = 2;
    static const int DIVD = 3;
    static const int LD = 4;
    static const int ST = 5;

    LSStation lsStation[7];

    ReStation station[6];

    float memory[4096];
    int memory_num;

    float reg[11];
    int Qi[11]; //=0表示

public:
    void init();

    void addOneInstr(QString str);

    void addMemory(QString str);

    void addOneMemory(int address, float data);

    int convParaToRegNo(QString str);

    void doWriteBack(int instr_no);

    void printRegs();

    void myTest();
};

#endif // TOMASULO_H
