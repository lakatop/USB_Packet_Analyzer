#pragma once

#include <QtWidgets/QWidget>
#include "ui_USB_Packet_Analyzer.h"
#include "ItemManager.hpp"

class USB_Packet_Analyzer : public QWidget
{
    Q_OBJECT

public:
    USB_Packet_Analyzer(QWidget *parent = Q_NULLPTR);
    void Refresh();

private:
    Ui::USB_Packet_AnalyzerClass ui;
    void Init();

private slots:
    //void on_Start_Button_clicked();
    void on_OpenButton_clicked();
    //void on_Clear_Button_clicked();
    //void on_Stop_Button_clicked();
    //void on_Pause_Button_clicked();
};
