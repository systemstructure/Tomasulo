#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTextStream>
#include <QInputDialog>
#include <QFile>
#include <QFileDialog>
#include <QDebug>

# pragma execution_character_set("utf-8")

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    t = new Tomasulo();


    QStringList colLabels;
    colLabels << "Name"<<"Desti"<<"Sourcej"<<"Sourcek"<<"发射指令"<<"执行完毕"<<"写回结果";
    ui->instrWidget->setHorizontalHeaderLabels(colLabels);
    for(int i = 0; i < 7; i++)
        ui->instrWidget->setColumnWidth(i, width);
    //instrArea->setWidget(instrWidget);

    QStringList LScolLabels;
    LScolLabels<<"isBusy"<<"Address";
    ui->LSQueueWidget->setHorizontalHeaderLabels(LScolLabels);
    QStringList LSrowLabels;
    LSrowLabels<<"Load1"<<"load2"<<"Load3"<<"Store1"<<"Store2"<<"Store3";
    ui->LSQueueWidget->setVerticalHeaderLabels(LSrowLabels);
    for(int i = 0; i < 3; i++)
        ui->LSQueueWidget->setColumnWidth(i, 1.5*width);

    QStringList memrowLabels;
    memrowLabels<<"Address"<<"data";
    ui->memoryWidget->setVerticalHeaderLabels(memrowLabels);
    ui->memoryWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList stationcolLabels;
    stationcolLabels<<"isBusy"<<"Time"<<"Op"<<"Vi"<<"Vk"<<"Qi"<<"Qk";
    ui->stationWidget->setHorizontalHeaderLabels(stationcolLabels);
    QStringList stationrowLabels;
    stationrowLabels<<"Add1"<<"Add2"<<"Add3"<<"Mult1"<<"Mult2";
    ui->stationWidget->setVerticalHeaderLabels(stationrowLabels);
    for(int i = 0; i < 7; i++)
        ui->stationWidget->setColumnWidth(i, 0.9*width);

    QStringList regrowLabels;
    regrowLabels<<"expr"<<"data";
    ui->registerWidget->setVerticalHeaderLabels(regrowLabels);
    QStringList regcolLabels;
    for(int i=0;i<11;i++)
        regcolLabels << QString("F%1").arg(i);
    ui->registerWidget->setHorizontalHeaderLabels(regcolLabels);
    for(int i = 0; i < 11; i++)
    {
        ui->registerWidget->setColumnWidth(i, width);
    }
    updateAll();
}

MainWindow::~MainWindow()
{
    delete ui; 
    delete t;
}

void MainWindow::on_addAction_triggered()
{
    bool ok = false;
    QString text = QInputDialog::getText(this,
                    tr( "input dialog" ),
                    tr( "Please enter one instruction(use captial letters)" ),
                    QLineEdit::Normal, QString::null, &ok);
    if (ok && !text.isEmpty())
    {
      t->addOneInstr(text);
      updateInstrWidget();
    }

}

void MainWindow::updateInstrWidget()
{
    ui->instrWidget->setRowCount(t->instr_num);
    for(int i=0;i<t->instr_num;i++)
    {
        ui->instrWidget->setItem(i,0,
                             new QTableWidgetItem(t->instr_name[t->instr[i].type]));
        for(int j=1;j<4;j++)
        {
            ui->instrWidget->setItem(i,j,
                                 new QTableWidgetItem(t->instr[i].parameter[j-1]));
        }
        for(int j=5;j<7;j++)
        {
            if(t->instr[i].event[j-5])
                ui->instrWidget->setItem(i,j,
                                     new QTableWidgetItem("Ok");
            else
                ui->instrWidget->setItem(i,j,
                                    new QTableWidgetItem("");
        }
    }

}

void MainWindow::updateMemWidget()
{
    ui->memoryWidget->setColumnCount(t->memory_num);
    int col = 0;
    for(int i=0;i<4096&&col!=t->memory_num;i++)
    {
        if(t->memory[i]!=0)
        {
            ui->memoryWidget->setItem(0,col,
                                      new QTableWidgetItem(QString::number(i)));
            ui->memoryWidget->setItem(1,col,
                                  new QTableWidgetItem(QString::number(t->memory[i])));
            col++;
            //qDebug()<<t->memory[i];
        }

    }
}

void MainWindow::updateLSWidget()
{
    for(int i=1;i<7;i++)
    {
        if(t->lsStation[i].isBusy)
        {
            ui->LSQueueWidget->setItem(i-1,0,new QTableWidgetItem("Yes"));
            ui->LSQueueWidget->setItem(i-1,1,
                                  new QTableWidgetItem(QString::number(t->lsStation[i].address)));
        }
        else
        {
            ui->LSQueueWidget->setItem(i-1,0,new QTableWidgetItem("No"));
            ui->LSQueueWidget->setItem(i-1,1,new QTableWidgetItem(""));
        }

    }
}

void MainWindow::updateReStationWidget()
{
    for(int i=1;i<6;i++)
    {
        if(t->station[i].isBusy)
        {
            ui->stationWidget->setItem(i-1,0,new QTableWidgetItem("Yes"));
            ui->stationWidget->setItem(i-1,1,new QTableWidgetItem(QString::number(t->station[i].time)));
            ui->stationWidget->setItem(i-1,2,new QTableWidgetItem(t->instr_name[t->station[i].op]));
            ui->stationWidget->setItem(i-1,3,new QTableWidgetItem(QString::number(t->station[i].Vj)));
            ui->stationWidget->setItem(i-1,4,new QTableWidgetItem(QString::number(t->station[i].Vk)));
            ui->stationWidget->setItem(i-1,5,new QTableWidgetItem(t->station_name[t->station[i].Qj]));
            ui->stationWidget->setItem(i-1,6,new QTableWidgetItem(t->station_name[t->station[i].Qk]));
        }
        else
        {
            ui->stationWidget->setItem(i-1,0,new QTableWidgetItem("No"));
            for(int j=1;j<7;j++)
            {
                ui->stationWidget->setItem(i-1, j, new QTableWidgetItem(""));
            }
        }
    }
}

void MainWindow::updateRegister()
{
    for(int i=0;i<11;i++)
    {
        ui->registerWidget->setItem(0,i,new QTableWidgetItem(t->station_name[t->Qi[i]]));
        ui->registerWidget->setItem(1,i,new QTableWidgetItem(QString::number(t->reg[i])));
    }
}

void MainWindow::updateLabel()
{
    ui->pcLabel->setText("PC:"+QString::number(t->curr_pc));
}

void MainWindow::updateAll()
{
    updateInstrWidget();
    updateMemWidget();
    updateLSWidget();
    updateReStationWidget();
    updateRegister();
}

void MainWindow::on_loadAction_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                        NULL,
                                                        tr("txtFile (*.txt)"));
    if(!fileName.isEmpty())
    {
         QFile file(fileName);
         if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
         {
             return;
         }
         QTextStream in(&file);
         QString tmp;
         while((tmp = in.readLine()) != NULL)
         {
             t->addOneInstr(tmp);

         }
         updateInstrWidget();
    }


}

void MainWindow::on_addMemoryAction_triggered()
{
    bool ok = false;
    QString text = QInputDialog::getText(this,
                    tr( "input dialog" ),
                    tr( "Please enter memory address and data\n"
                        "(format address:data,use ';' to input multi memory)\n"
                        "eg  10:20;11:30"),
                    QLineEdit::Normal, QString::null, &ok);
    if (ok && !text.isEmpty())
    {
      t->addMemory(text);
      updateMemWidget();
    }
}

void MainWindow::on_deleteAction_triggered()
{
    t->init();
    updateAll();
}

void MainWindow::on_onestepAction_triggered()
{
    t->runOneStep();
    updateAll();
}

void MainWindow::on_multistepAction_triggered()
{
    bool ok = false;
    int num = QInputDialog::getInt(this,
                                   tr( "input dialog" ),
                                   tr( "Please enter the number of steps"),
                                   5,0,MAX_INSTR_NUM,1,&ok);
    if (ok)
    {
        for(int i=0;i<num;i++)
        {
            t->runOneStep();
        }
    }


}
