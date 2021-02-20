#include "USB_Packet_Analyzer.h"
#include "stdafx.h"

USB_Packet_Analyzer::USB_Packet_Analyzer(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    Init();
}

void USB_Packet_Analyzer::Init()
{
    ui.OpenButton->setText("Open");
    //ui.pushButton->setObjectName("OpenButton");
    ui.pushButton_2->setText("Start");
    ui.pushButton_2->setObjectName("StartButton");
    ui.pushButton_3->setText("Clear");
    ui.pushButton_3->setObjectName("ClearButton");
    ui.pushButton_4->setText("Stop");
    ui.pushButton_4->setObjectName("StopButton");
    ui.pushButton_5->setText("Pause");
    ui.pushButton_5->setObjectName("PauseButton");

    ui.radioButton->setText("Live Capture");
    ui.radioButton_2->setText("File Capture");
    ui.radioButton_2->setChecked(true);
    ui.checkBox->setText("Data Highlight");
}

void USB_Packet_Analyzer::on_OpenButton_clicked()
{
    QString fil = QFileDialog::getOpenFileName(this, "Select source file", ".", "Pcap files (*.pcap)");
}