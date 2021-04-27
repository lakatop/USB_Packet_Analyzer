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
    this->itemManager = ItemManager::GetItemManager(ui.tableWidget, this);

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

    QStringList headerList{ "Index", "Source", "Destination", "Length", "Transfer type", "Function" };
    ui.tableWidget->setColumnCount(headerList.size());
    ui.tableWidget->setHorizontalHeaderLabels(headerList);
    ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.tableWidget->verticalHeader()->setVisible(false);
    ui.tableWidget->setShowGrid(false);
    connect(ui.tableWidget, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(on_tableWidget_itemDoubleclicked(QTableWidgetItem*)));
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
    ui.tableWidget->resizeColumnsToContents();
}

void USB_Packet_Analyzer::on_ClearButton_clicked()
{
    ui.tableWidget->clearContents();
    ui.tableWidget->setRowCount(0);
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

void USB_Packet_Analyzer::on_tableWidget_itemDoubleclicked(QTableWidgetItem* item)
{
    int currentRow = ui.tableWidget->row(item);
    QTableWidgetItem* previousItem = ui.tableWidget->item(currentRow - 1, 0);
    if (!dataViewer.isNull())
    {
        dataViewer->deleteLater();
    }
    QTableWidgetItem* dataItem = ui.tableWidget->item(currentRow, 0);
    //using nullptr as parent because otherwise it wont show in task bar
    dataViewer = QPointer(new DataViewer(dataItem, this->itemManager->GetDataType(dataItem, previousItem), ui.dataHighlightCheckBox->isChecked()));
    dataViewer->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
    connect(this, &QObject::destroyed, dataViewer.data(), &QObject::deleteLater);
    dataViewer->show();
}