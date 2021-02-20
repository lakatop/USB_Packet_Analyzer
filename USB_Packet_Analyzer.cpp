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
    ui.StartButton->setText("Start");
    ui.ClearButton->setText("Clear");
    ui.StopButton->setText("Stop");
    ui.PauseButton->setText("Pause");

    ui.liveCaptureRadioButton->setText("Live Capture");
    ui.fileCaptureRadioButton->setText("File Capture");
    ui.fileCaptureRadioButton->setChecked(true);
    ui.dataHighlightCheckBox->setText("Data Highlight");
}

void USB_Packet_Analyzer::Refresh()
{
    QCoreApplication::processEvents();
}

void USB_Packet_Analyzer::on_OpenButton_clicked()
{
    QString fil = QFileDialog::getOpenFileName(this, "Select source file", ".", "Pcap files (*.pcap)");
    ItemManager manager(ui.listWidget, this);
    manager.ProcessFile(fil, ui.liveCaptureRadioButton->isChecked());
}