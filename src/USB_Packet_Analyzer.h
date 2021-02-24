#pragma once

#include <QtWidgets/QWidget>
#include "ui_USB_Packet_Analyzer.h"
#include "ItemManager.hpp"
#include "DataViewer.h"

class USB_Packet_Analyzer : public QWidget
{
    Q_OBJECT

public:
    USB_Packet_Analyzer(QWidget *parent = Q_NULLPTR);
    void Refresh();

private:
    Ui::USB_Packet_AnalyzerClass ui;
    QString currentFile;
    std::unique_ptr<DataViewer> dataViewer;

    void Init();

private slots:
    void on_StartButton_clicked();
    void on_OpenButton_clicked();
    //void on_ClearButton_clicked();
    //void on_StopButton_clicked();
    //void on_PauseButton_clicked();
    void on_listWidget_itemDoubleClicked(QListWidgetItem* item);
};