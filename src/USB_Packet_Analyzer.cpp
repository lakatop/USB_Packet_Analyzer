#include "USB_Packet_Analyzer.h"
#include "../stdafx.h"

USB_Packet_Analyzer::USB_Packet_Analyzer(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    Init();
}

void USB_Packet_Analyzer::Init()
{
    this->itemManager = ItemManager::GetItemManager(ui.listWidget, this);

    ui.OpenButton->setText("Open");
    ui.StartButton->setText("Start");
    ui.ClearButton->setText("Clear");
    ui.StopButton->setText("Stop");
    ui.PauseButton->setText("Pause");

    ui.liveCaptureRadioButton->setText("Live Capture");
    ui.fileCaptureRadioButton->setText("File Capture");
    ui.fileCaptureRadioButton->setChecked(true);
    ui.dataHighlightCheckBox->setText("Data Highlight");
    ui.dataHighlightCheckBox->setChecked(true);

    ui.openFileLabel->setText("");
}

void USB_Packet_Analyzer::Refresh()
{
    QCoreApplication::processEvents();
}

void USB_Packet_Analyzer::on_OpenButton_clicked()
{
    currentFile = QFileDialog::getOpenFileName(this, "Select source file", ".", "Pcap files (*.pcap)");
    QString fileName = QDir(currentFile).dirName();
    ui.openFileLabel->setText("Selected file: " + fileName);
    ui.progressBar->setValue(0);
}

void USB_Packet_Analyzer::on_StartButton_clicked()
{
    this->itemManager->ProcessFile(currentFile, ui.liveCaptureRadioButton->isChecked());
}

void USB_Packet_Analyzer::on_ClearButton_clicked()
{
    ui.listWidget->clear();
    ui.progressBar->setValue(0);
}

void USB_Packet_Analyzer::on_StopButton_clicked()
{
    this->itemManager->stopButtonClicked = true;
}

void USB_Packet_Analyzer::on_PauseButton_clicked()
{
    if (ui.PauseButton->text() == "Pause")
    {
        ui.PauseButton->setText("Continue");
        this->itemManager->pauseButtonClicked = true;
    }
    else
    {
        ui.PauseButton->setText("Pause");
        this->itemManager->pauseButtonClicked = false;
    }
}

QProgressBar* USB_Packet_Analyzer::GetProgressBar()
{
    return ui.progressBar;
}

void USB_Packet_Analyzer::on_listWidget_itemDoubleClicked(QListWidgetItem* item)
{
    int currentRow = ui.listWidget->row(item);
    QListWidgetItem* previousItem = ui.listWidget->item(currentRow - 1);
    if (!dataViewer.isNull())
    {
        dataViewer->deleteLater();
    }
    //using nullptr as parent because otherwise it wont show in task bar
    dataViewer = QPointer(new DataViewer(item, this->itemManager->GetDataType(item, previousItem), ui.dataHighlightCheckBox->isChecked()));
    dataViewer->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
    connect(this, &QObject::destroyed, dataViewer.data(), &QObject::deleteLater);
    dataViewer->show();
}