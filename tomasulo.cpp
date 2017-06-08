#include "tomasulo.h"
#include <QStringList>
#include <string.h>
#include <assert.h>

Tomasulo::Tomasulo()
{
    init();
}

void Tomasulo::init()
{
    clock = 0;

    for (int i = 0; i < MAX_INSTR_NUM; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            instr[i].event[j] = false;
        }
    }

    instr_num = 0;
    memory_num = 0;

    curr_pc = 0;
    curr_instr_pos = 0;

    addRunningNo = -1;
    mulRunningNo = -1;

    memset(memory,0,sizeof(memory));
    memset(Qi,0,sizeof(Qi));
    memset(reg,0,sizeof(reg));


    for(int i=1;i<6;i++)
        station[i].initStation();
    for(int i=1;i<7;i++)
        lsStation[i].initLSStation();
}

bool Tomasulo::addOneInstr(QString str)
{
    str = str.trimmed();
    str = str.replace("  ", " ");
    if (!judgeInstr(str)) return false;

    QStringList list = str.split(' ');
    int l = list.size();
//    for(int i=0;i<l;i++)
//        qDebug()<<list[i];
    for(int i=1;i<=6;i++)
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
    return true;
}

bool Tomasulo::addMemory(QString str)
{
    if (!judgeMem(str)) return false;
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
    return true;
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
    qDebug() << "";
    qDebug() << "###### CLock" << clock << " ######";
    qDebug() << "<<<<<< current instr pos = " << curr_instr_pos;
    writeBack();
    execute();
    issue();
    clock++;
    qDebug() << "Store1.Qj = " << lsStation[4].Qj;
}

void Tomasulo::issue()  // !!! Qi, Qj, Qk 一定是1~11 !!!
{
    qDebug() << "===== issue() =====";
    instruction cur_ins = instr[curr_instr_pos];
    printInstr(cur_ins);
    int r = findAvailableStation(cur_ins.type);
    if (r == 0) { //如果没有空闲保留站，直接返回
        qDebug() << "No available station found. Return.";
        return;
    }
    if (curr_instr_pos >= instr_num) return;
    qDebug() << "Free station/lsStation: no." << r;
    //如果有空闲保留站，编号为r，则当前指令cur_ins进入保留站r
    int rs = cur_ins.rs;
    int rt = cur_ins.rt;
    int rd = cur_ins.rd;
    int addr = cur_ins.addr;
    switch (cur_ins.type)
    {
    case ADDD:
    case SUBD:
    case MULD:
    case DIVD: //浮点数运算指令，r = 1~5
    {
        if (Qi[rs] != 0) {
            station[r].Qj = Qi[rs];
        } else {
            station[r].Vj = reg[rs];
            station[r].Qj = 0;
        }
        if (Qi[rt] != 0) {
            station[r].Qk = Qi[rt];
        } else {
            station[r].Vk = reg[rt];
            station[r].Qk = 0;
        }
        station[r].isBusy = true;
        station[r].op = cur_ins.type;
        station[r].time = clocktime[station[r].op];
        station[r].instr_no = curr_instr_pos;
        Qi[rd] = r;
        break;
    }
    case LD:  //LS指令，r = 1~6，需转化成6~10
    {
        lsStation[r].isBusy = true;
        lsStation[r].address = addr;
        lsStation[r].time = 2;
        lsStation[r].instr_no = curr_instr_pos;
        Qi[rd] = r + 5;
        lsQueue.push(r); // 该保留站编号加入LS队列
        break;
    }
    case ST:
    {
        qDebug() << "ST: Qi[rs] = Qi[" << rs <<"] = " << Qi[rs];
        if (Qi[rs] != 0) {
            lsStation[r].Qj = Qi[rs];
            qDebug() << "Store"<<r<<".Qj = " << lsStation[r].Qj;
        } else {
            lsStation[r].Vj = reg[rs];
            lsStation[r].Qj = 0;
        }
        lsStation[r].isBusy = true;
        lsStation[r].address = addr;
        lsStation[r].time = 2;
        lsStation[r].instr_no = curr_instr_pos;
        lsQueue.push(r); // 该保留站编号加入LS队列
        break;
    }
    default:
    {
        qDebug() << "Wrong type of instruction in issue().";
    }
    }

    instr[curr_instr_pos].event[0] = true;
    curr_instr_pos ++;
}


void Tomasulo::execute()
{
    qDebug() << "===== execute() =====";
    if(addRunningNo == -1)  //没有ADDD/SUBD在跑
    {
        for(int i=1;i<=3;i++)
        {
            if(station[i].Qj == 0 && station[i].Qk == 0)  //如果两个操作数都已准备好了
            {
                addRunningNo = i;   //就执行该保留站的指令
                qDebug() << "AddStation" << i << "Starts running. Time = " << station[i].time;
                break;
            }
        }
    }
    if(addRunningNo != -1)    //有ADDD/SUBD在跑，则直接time减一
    {
        qDebug() << "addRunningNo" ;
        station[addRunningNo].time--;
        if (station[addRunningNo].time == 0) instr[station[addRunningNo].instr_no].event[1] = true;
        qDebug() << "AddStation" << addRunningNo << "runs a clock, Time = " << station[addRunningNo].time;
    }

    if(mulRunningNo == -1)
    {
        for(int i=4;i<=5;i++)
        {
            if(station[i].Qj == 0 && station[i].Qk == 0)
            {
                mulRunningNo = i;
                qDebug() << "MulStation" << i << "Starts running. Time = " << station[i].time;
                break;
            }
        }
    }
    if(mulRunningNo != -1)
    {
        station[mulRunningNo].time--;
        if (station[mulRunningNo].time == 0) instr[station[mulRunningNo].instr_no].event[1] = true;
        qDebug() << "MulStation" << mulRunningNo << "runs a clock, Time = " << station[mulRunningNo].time;
    }

    if (!lsQueue.empty()) {
        int ls_no = lsQueue.front(); //队首
        qDebug() << "ls_front_no = " << ls_no;
        if(!lsStation[ls_no].isRunning && (isLoadInstr(ls_no) || (lsStation[ls_no].Qj == 0))) //不在跑，且已准备好数据
        {
            lsStation[ls_no].isRunning = true;
            qDebug() << "lsStation" << ls_no << "Starts running. Time = " << lsStation[ls_no].time;

        }
        if(lsStation[ls_no].isRunning) //在跑，则直接time减1
        {
            lsStation[ls_no].time--;
            if (lsStation[ls_no].time == 0) instr[lsStation[ls_no].instr_no].event[1] = true;
            qDebug() << "lsStation" << ls_no << "runs a clock, Time = " << lsStation[ls_no].time;
        }
    }
}


void Tomasulo::writeBack()
{
    qDebug() << "===== writeBack() =====";

    checkIfFinishedAndUpdate(addRunningNo);
    checkIfFinishedAndUpdate(mulRunningNo);
    if (!lsQueue.empty()) {
        int ls_no = lsQueue.front();
        checkIfFinishedLoadStoreAndUpdate(ls_no);
    }
}

float Tomasulo::calResult(ReStation _station)
{
    switch (_station.op)
    {
    case ADDD:
        return _station.Vj + _station.Vk;
    case SUBD:
        return _station.Vj - _station.Vk;
    case MULD:
        return _station.Vj * _station.Vk;
    case DIVD:
        return _station.Vj / _station.Vk;
    default:
        qDebug() << "Wrong type of station.op in calResult().";
    }
    return 0;
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

void Tomasulo::printInstr(instruction ins)
{
    qDebug() << instr_name[ins.type] << ' '<< ins.parameter[0] << ' ' << ins.parameter[1] << ' '
                                     << ins.parameter[2] << ' ' << ", addr = " << ins.addr;
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
        break;
    }
    case LD:
    {
        for(int i = 1; i <= 3; ++i) {
            if(!lsStation[i].isBusy) {
                return i;
            }
        }
        break;
    }
    case ST:
    {
        for(int i = 4; i <= 6; ++i) {
            if(!lsStation[i].isBusy) {
                return i;
            }
        }
        break;
    }
    default:
    {
        qDebug() << "Wrong type of instruction in findAvailableStation.";
    }
    }
    return 0;
}

void Tomasulo::checkIfFinishedAndUpdate(int& running_no) //WriteBack检查Add/Mul保留站
{
    int r = running_no;
    if (r <= 0) {
        qDebug() << "No add/mul running_no.";
        return;
    }
    if(station[r].time == 0) {
        qDebug() << "Finished station[" << r << "].";
        float result = calResult(station[r]);
        updateRegQjQk(r, result);
//        running_no = -1;
        instr[station[r].instr_no].event[2] = true;
        station[r].initStation();
        if (running_no >= 1 && running_no <= 3) {
            addRunningNo = -1;
        } else {
            mulRunningNo = -1;
        }
    }

}

void Tomasulo::checkIfFinishedLoadStoreAndUpdate(int ls_no)
{
    int r = ls_no;
    assert(1 <= r && r <= 6);
    if(lsStation[r].isRunning && lsStation[r].time == 0) {
        if(isLoadInstr(r)) {  //LD指令执行完成
            qDebug() << "Finished lsStation[" << r << "].";
            float result = memory[lsStation[r].address];
            updateRegQjQk(r + 5, result);
        }
        else {  //ST指令执行完成
            qDebug() << "Finished lsStation[" << r-3 << "].";
            addOneMemory(lsStation[r].address, lsStation[r].Vj);
        }
        instr[lsStation[r].instr_no].event[2] = true;
        lsStation[r].initLSStation();
        lsQueue.pop();
    }
}

void Tomasulo::updateRegQjQk(int r, float result)
{
    for (int i = 0; i <= 10; i++) { //遍历11个寄存器
        if(Qi[i] == r) {
            reg[i] = result;
            Qi[i] = 0;
        }
    }
    for (int i = 1; i <= 5; i++) {  //遍历5个保留站
        if(station[i].Qj == r) {
            station[i].Vj = result;
            station[i].Qj = 0;
        }
        if(station[i].Qk == r) {
            station[i].Vk = result;
            station[i].Qk = 0;
        }
    }
    for (int i = 4; i <= 6; i++) {  //遍历3个ST缓冲区
        if(lsStation[i].Qj == r) {
            lsStation[i].Vj = result;
            lsStation[i].Qj = 0;
        }
    }
}

bool Tomasulo::isLoadInstr(int ls_no)
{
    assert(1 <= ls_no && ls_no <= 6);
    if (1 <= ls_no && ls_no <= 3) {
        return true;
    }
    return false;
}

bool Tomasulo::allDone()
{
    bool flag = true;
    for(int i = 0; i < instr_num; ++i) {
        if(!instr[i].event[2]) {
            flag = false;
            break;
        }
    }
    return flag;
}

bool Tomasulo::judgeInstr(QString str)
{
    int len[] = {0, 4,4,4,4,3,3};
    QString reg_name[11];
    for(int i = 0 ; i < 11; i ++)
    {
        reg_name[i] = "F" + QString::number(i);
    }
    QStringList list = str.split(' ');


    //判断指令名字是否符合规则
    int g = -1;
    for(int i = 1; i <= 6; i ++)
    {
        if(list[0] == instr_name[i])
        {
            g = i;
            break;
        }
    }
    if(g == -1)
        return false;

    //判断指令参数个数是否符合规则
    int l = list.size();
    if(l != len[g])
    {
        return false;
    }

    //判断指令参数格式是否符合规则
    if(g <= 4)
    {
        //只能为寄存器的名字
        for(int i = 1; i < l; i ++)
        {
            QString temp = list.at(i);
            int reg_g = -1;
            for(int j = 0; j < 11; j ++)
            {
                if(temp == reg_name[j])
                {
                    reg_g = j;
                    break;
                }
            }
            if(reg_g == -1)
                return false;
        }
    }
    else
    {
        //第一个为寄存器名字
        int reg_g = -1;
        for(int j = 0; j < 11; j ++)
        {
            if(list.at(1) == reg_name[j])
            {
                reg_g = j;
                break;
            }
        }
        if(reg_g == -1)
            return false;

        //第二个为地址
        if(list.at(2).size()>4)
            return false;
        int temp = list.at(2).toInt();
//        qDebug() << "errornum";
//        qDebug() << temp;
        if(list.at(2) != "0" && list.at(2) != "00" && list.at(2) != "000" && list.at(2) != "0000" && temp == 0)
            return false;
        if(temp < 0 || temp > 4095)
            return false;
    }
    return true;
}

bool Tomasulo::judgeMem(QString str)
{
    QStringList list = str.split(';');
    qDebug() << list;
    for(int i = 0; i < list.size(); i ++)
    {
        QString temp = list.at(i);
        int pos = temp.indexOf(":");
        int address = temp.left(pos).toInt();
        float data = temp.mid(pos+1).toFloat();
        qDebug() << address;
        qDebug() << data;
        if(temp.left(pos).size()>4)
            return false;
        if(address == 0 && temp.left(pos) != "0" && temp.left(pos) != "00" && temp.left(pos) != "000" && temp.left(pos) != "0000")
            return false;
        if(address < 0 || address > 4095)
            return false;
        if(data == 0)
            return false;

    }
    return true;
}
