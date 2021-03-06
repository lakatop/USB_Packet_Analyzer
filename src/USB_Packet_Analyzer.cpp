#include "USB_Packet_Analyzer.h"
#include "../stdafx.h"

/// <summary>
/// Constuctor for USB_Packet_Analyzer.
/// </summary>
/// <param name="parent">Parent of this class</param>
USB_Packet_Analyzer::USB_Packet_Analyzer(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    Init();
}

/// <summary>
/// Initializes components of main window.
/// </summary>
void USB_Packet_Analyzer::Init()
{
    this->itemManager = ItemManager::GetItemManager(ui.tableWidget, this);
    timer = new QTimer(this);

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
    connect(timer, SIGNAL(timeout()), this, SLOT(updateLiveFile()));
}

/// <summary>
/// Processes all pending events.
/// </summary>
void USB_Packet_Analyzer::Refresh()
{
    QCoreApplication::processEvents();
}

/// <summary>
/// Slot which will be called when user click Open Button
/// </summary>
void USB_Packet_Analyzer::on_OpenButton_clicked()
{
    currentFile = QFileDialog::getOpenFileName(this, "Select source file", ".", "Pcap files (*.pcap)");
    QString fileName = QDir(currentFile).dirName();
    ui.openFileLabel->setText("Selected file: " + fileName);
    ui.progressBar->setValue(0);
}

/// <summary>
/// Slot which will be called when user click Start Button
/// </summary>
void USB_Packet_Analyzer::on_StartButton_clicked()
{
    this->itemManager->ProcessFile(currentFile, ui.liveCaptureRadioButton->isChecked());
    ui.tableWidget->resizeColumnsToContents();
}

/// <summary>
/// Slot which will be called when user click Clear Button
/// </summary>
void USB_Packet_Analyzer::on_ClearButton_clicked()
{
    ui.tableWidget->clearContents();
    ui.tableWidget->setRowCount(0);
    ui.progressBar->setValue(0);
}

/// <summary>
/// Slot which will be called when user click Stop Button
/// </summary>
void USB_Packet_Analyzer::on_StopButton_clicked()
{
    this->itemManager->stopButtonClicked = true;
}

/// <summary>
/// Gets called by timer when analyzing live capture file
/// and checks whether there were any changes in file.
/// If there were any changes -> process them.
/// </summary>
void USB_Packet_Analyzer::updateLiveFile()
{
    if (!itemManager->processingFile && !itemManager->fileReader.EndOfFile())
    {
        itemManager->ProcessFileTillEnd(true);
    }
}
    
/// <summary>
/// Slot which will be called when user click Pause Button
/// </summary>
void USB_Packet_Analyzer::on_PauseButton_clicked()
{
    if (this->itemManager->pauseButtonClicked == false)
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

/// <summary>
/// Get pointer on progress bar.
/// </summary>
/// <returns>Pointer on applications progress bar</returns>
QProgressBar* USB_Packet_Analyzer::GetProgressBar()
{
    return ui.progressBar;
}

/// <summary>
/// Slot which will be called when user double-clicks on item in tableWidget
/// </summary>
/// <param name="item">Item on which user double-clicked</param>
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
    dataViewer = QPointer<DataViewer>(new DataViewer(dataItem, this->itemManager->GetDataType(dataItem, previousItem), ui.dataHighlightCheckBox->isChecked()));
    dataViewer->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
    //since parent of DataViewer is nullptr, this ensures that when USB_Packet_Analyzer instance is about to be destroyed, 
    //it will delete DataViewer instance and frees memory
    connect(this, &QObject::destroyed, dataViewer.data(), &QObject::deleteLater);
    dataViewer->show();
}