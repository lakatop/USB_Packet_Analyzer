#pragma once

#include <QDialog>
#include "ui_DataViewer.h"
#include "../DefinedStructs/PacketExternStructs.hpp"
#include "../DefinedStructs/ConstDataHolder.hpp"
#include "../Models/DataViewerModel.h"
#include "../Models/ColorMapModel.hpp"
#include "../Models/USBPcapHeaderModel.hpp"
#include "../Models/AdditionalDataModel.hpp"
#include "../Delegates/DataViewerDelegate.h"
#include <qlistwidget.h>
#include <qscrollbar.h>

class DataViewer : public QDialog
{
	Q_OBJECT

public:
	DataViewer(QTableWidgetItem* item, HeaderDataType additionalDataType, bool dataHighlight, QWidget *parent = Q_NULLPTR);
	~DataViewer();

private:
	void InitTables(QTableWidgetItem* item, HeaderDataType additionalDataType, bool dataHighlight);
	void InitTreeViews(QTableWidgetItem* item, HeaderDataType additionalDataType);
	void InitTableViewer(QTableView* table, bool hexViewTable);
	void InitLabels();
	void AdjustTableSize(QTableView* table, bool hexViewTable);

	Ui::DataViewer ui;
	std::unique_ptr<DataViewerModel> byteTableModel;
	std::unique_ptr<DataViewerModel> hexTableModel;
	std::unique_ptr<DataViewerDelegate> dataTableDelegate;
	std::unique_ptr<ColorMapModel> colorMapModel;
	std::unique_ptr<USBPcapHeaderModel> usbhModel;
	std::unique_ptr<AdditionalDataModel> additionalDataModel;

private slots:
	void updateHexSelection(const QItemSelection& selected, const QItemSelection& deselected);
	void updateByteSelection(const QItemSelection& selected, const QItemSelection& deselected);
};
