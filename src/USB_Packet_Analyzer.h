#ifndef USBPACKETANALYZER_HPP
#define USBPACKETANALYZER_HPP

#include <QtWidgets/QWidget>
#include <qpointer.h>
#include "ui_USB_Packet_Analyzer.h"
#include "ItemManager.hpp"
#include "DataViewer.h"

class ItemManager; //forward declaration

/// <summary>
/// Main class for application.
/// </summary>
class USB_Packet_Analyzer : public QWidget
{
    Q_OBJECT

public:
    USB_Packet_Analyzer(QWidget *parent = Q_NULLPTR);
    void Refresh();
    QProgressBar* GetProgressBar();

protected:
    void closeEvent(QCloseEvent* event);
private:
    Ui::USB_Packet_AnalyzerClass ui;
    /// <summary>
    /// Pointer to ItemManager instance.
    /// </summary>
    ItemManager* itemManager;
    /// <summary>
    /// Name of current file which is being processed.
    /// </summary>
    QString currentFile;
    /// <summary>
    /// QPointer to pop-up DataViewer dialog.
    /// </summary>
    QPointer<DataViewer> dataViewer;

    void Init();

private slots:
    void on_StartButton_clicked();
    void on_OpenButton_clicked();
    void on_ClearButton_clicked();
    void on_StopButton_clicked();
    void on_PauseButton_clicked();
    void on_tableWidget_itemDoubleclicked(QTableWidgetItem* item);
};

#endif // !USBPACKETANALYZER_HPP