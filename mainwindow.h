#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QTableWidget>

#include "tomasulo.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_addAction_triggered();

    void on_loadAction_triggered();

    void on_addMemoryAction_triggered();

    void on_deleteAction_triggered();

    void on_onestepAction_triggered();

private:
    void updateInstrWidget();
    void updateMemWidget();
    void updateLSWidget();
    void updateReStationWidget();
    void updateRegister();
    void updateLabel();
    void updateAll();
private:
    const int width = 70;
    const int height = 35;
    Tomasulo *t;
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
