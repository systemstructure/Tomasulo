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
    bool event[3];
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
    int address;
    bool isRunning;
    int Qj;
    float Vj;
    int time;
    int instr_no;
    LSStation() {
        initLSStation();
    }
    void initLSStation() {
        isBusy = false;
        address = -1;
        isRunning = false;
        Qj = -1;
        Vj = 0;
        time = -1;
        instr_no = -1;
    }
};

struct ReStation
{
    int time;
    bool isBusy;
    int op;
    int Qj,Qk;
    float Vj,Vk;
    int instr_no;
    ReStation() {
        initStation();
    }
    void initStation() {
        time = -1;
        isBusy = false;
        op = -1;
        Qj = -1;
        Qk = -1;
        Vj = 0;
        Vk = 0;
        instr_no = -1;
    }
};

class Tomasulo
{
public:
    Tomasulo();
    instruction instr[MAX_INSTR_NUM];
    int instr_num;
    int curr_instr_pos;
    int curr_pc;
    const QString instr_name[7] = {"", "ADDD", "SUBD", "MULD", "DIVD", "LD", "ST"};
    const int clocktime[7] = {0,2,2,10,40,2,2};
    QString station_name[12] = {"","Add1", "Add2","Add3","Mult1","Mult2",
                    "Load1","Load2","Load3","Store1","Store2","Store3"};
    static const int ADDD = 1;
    static const int SUBD = 2;
    static const int MULD = 3;
    static const int DIVD = 4;
    static const int LD = 5;
    static const int ST = 6;

    LSStation lsStation[7];

    ReStation station[6];

    int addRunningNo;   //正在运行的保留站编号， -1表示没有运行的加减法保留站
    int mulRunningNo;
    std::queue<int> lsQueue;

    float memory[4096];
    int memory_num;

    float reg[11];
    int Qi[11]; //=0表示已准备好数据，>0表示保留站编号

    int clock;

public:
    void init();

    bool addOneInstr(QString str);

    bool addMemory(QString str);

    void addOneMemory(int address, float data);

    //算法运行相关函数

    void runOneStep();

    void issue();

    void execute();

    void writeBack();

    float calResult(ReStation _station);

    //以下是辅助函数

    int convParaToRegNo(QString str);

    void printRegs();

    void printInstr(instruction ins);

    int findAvailableStation(int ins_type); //返回保留站编号，返回0表示该type对应的保留站无空闲

    void checkIfFinishedAndUpdate(int& running_no);  //检查是否有加减或乘除的保留站执行完毕

    void checkIfFinishedLoadStoreAndUpdate(int ls_no);   //检查lsQueue队首是否执行完毕

    void updateRegQjQk(int r, float result);  //WriteBack中更新Reg数组、各保留站Qj,Qk

    bool isLoadInstr(int ls_no);

    bool allDone();

    bool judgeInstr(QString str);

    bool judgeMem(QString str);
};

#endif // TOMASULO_H
