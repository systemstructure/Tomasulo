#ifndef TOMASULO_H
#define TOMASULO_H

const int MAX_INSTR_NUM = 10000;

#include <QString>
#include <QDebug>
#include <queue>


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
    bool isRunning;
};

struct ReStation
{
    int time;
    bool isBusy;
    int clocktime;
    int op;
    int Vj,Vk,Qj,Qk;
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
    QString station_name[12] = {"","Add1", "Add2","Add3","Mult1","Mult2",
                    "Load1","Load2","Load3","Store1","Store2","Store3"};
    static const int ADDD = 0;
    static const int SUBD = 1;
    static const int MULD = 2;
    static const int DIVD = 3;
    static const int LD = 4;
    static const int ST = 5;

    LSStation lsStation[7];

    ReStation station[6];

    int addRunningNo;   //正在运行的保留站编号， -1表示没有运行的加减法保留站
    int mulRunningNo;
    std::queue<int> lsQueue;

    float memory[4096];
    int memory_num;

    float reg[11];
    int Qi[11]; //=0表示已准备好数据，>0表示保留站编号


public:
    void init();

    void addOneInstr(QString str);

    void addMemory(QString str);

    void addOneMemory(int address, float data);

    //算法运行相关函数

    void runOneStep();

    void issue();

    void execute();

    void writeBack();

    void doWriteBack(int instr_no);

    //以下是辅助函数

    int convParaToRegNo(QString str);

    void printRegs();

    int findAvailableStation(int ins_type); //返回保留站编号，返回0表示该type对应的保留站无空闲

    void myTest();
};

#endif // TOMASULO_H
