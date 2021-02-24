#pragma once

#include <QDialog>
#include "ui_DataViewer.h"
#include "../DefinedStructs/PacketExternStructs.hpp"
#include "../DefinedStructs/ConstDataHolder.hpp"
#include "../Models/DataViewerModel.h"
#include "../Models/ColorMapModel.hpp"
#include "../Models/USBPcapHeaderModel.hpp"
#include "../Delegates/DataViewerDelegate.h"
#include <qlistwidget.h>
#include <qscrollbar.h>

class DataViewer : public QDialog
{
	Q_OBJECT

public:
	DataViewer(QListWidgetItem* item, HeaderDataType additionalDataType, bool dataHighlight, QWidget *parent = Q_NULLPTR);
	~DataViewer();

private:
	void InitTables(QListWidgetItem* item, HeaderDataType additionalDataType, bool dataHighlight);
	void InitTreeViews(QListWidgetItem* item);
	void InitTableViewer(QTableView* table, bool hexViewTable);
	void AdjustTableSize(QTableView* table, bool hexViewTable);

	Ui::DataViewer ui;
	std::unique_ptr<DataViewerModel> byteTableModel;
	std::unique_ptr<DataViewerModel> hexTableModel;
	std::unique_ptr<DataViewerDelegate> dataTableDelegate;
	std::unique_ptr<ColorMapModel> colorMapModel;
	std::unique_ptr<USBPcapHeaderModel> usbhModel;
};
