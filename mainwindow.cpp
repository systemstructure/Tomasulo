#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTextStream>
#include <QInputDialog>
#include <QFile>
#include <QFileDialog>

# pragma execution_character_set("utf-8")

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    t = new Tomasulo();
    instrArea = new QScrollArea(this);
    instrArea->setGeometry(60,50,8*width,350);
    instrArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    instrWidget = new QTableWidget(0,7,instrArea);
    instrWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    instrWidget->setGeometry(60,50,8*width,(t->instr_num+2)*height);
    QStringList colLabels;
    colLabels << "Name"<<"Desti"<<"Sourcej"<<"Sourcek"<<"发射指令"<<"执行完毕"<<"写回结果";
    instrWidget->setHorizontalHeaderLabels(colLabels);
    for(int i = 0; i < 7; i++)
        instrWidget->setColumnWidth(i, width);
    instrArea->setWidget(instrWidget);

    QStringList LScolLabels;
    LScolLabels<<"isBusy"<<"Address";
    ui->LSQueueWidget->setHorizontalHeaderLabels(LScolLabels);
    QStringList LSrowLabels;
    LSrowLabels<<"Load1"<<"load2"<<"Load3"<<"Store1"<<"Store2"<<"Store3";
    ui->LSQueueWidget->setVerticalHeaderLabels(LSrowLabels);
    for(int i = 0; i < 3; i++)
        ui->LSQueueWidget->setColumnWidth(i, 1.5*width);

    memoryWidget = new QTableWidget(0,2,this);
    memoryWidget->setGeometry(100,500,300,200);
    QStringList memcolLabels;
    memcolLabels<<"Address"<<"data";
    memoryWidget->setHorizontalHeaderLabels(memcolLabels);
    memoryWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

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
    instrWidget->setGeometry(60,50,8*width,(t->instr_num+2)*height);
    int row = instrWidget->rowCount();
    instrWidget->setRowCount(row+1);
    instrWidget->setItem(row,0,
                         new QTableWidgetItem(t->instr_name[t->instr[row].type]));
    for(int i=1;i<4;i++)
    {
        instrWidget->setItem(row,i,
                             new QTableWidgetItem(t->instr[row].parameter[i-1]));
    }
}

void MainWindow::on_loadAction_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                        NULL,
                                                        tr("txtFile (*.* *.txt)"));
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
             updateInstrWidget();
         }
    }


}
