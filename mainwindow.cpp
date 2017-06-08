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
    stationcolLabels<<"Name"<<"Time"<<"isBusy"<<"Op"<<"Vi"<<"Vk"<<"Qi"<<"Qk";
    ui->stationWidget->setHorizontalHeaderLabels(stationcolLabels);
    QStringList stationrowLabels;
    stationrowLabels<<"Add1"<<"Add2"<<"Add3"<<"Mult1"<<"Mult2";
    ui->stationWidget->setVerticalHeaderLabels(stationrowLabels);
    for(int i = 0; i < 8; i++)
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
    updateInstrWidget();
    updateMemWidget();
}
