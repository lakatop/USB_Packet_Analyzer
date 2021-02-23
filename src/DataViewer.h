#pragma once

#include <QDialog>
#include "ui_DataViewer.h"
#include "../DefinedStructs/PacketExternStructs.hpp"
#include "../DefinedStructs/ConstDataHolder.hpp"
#include "../Models/DataViewerModel.h"
#include "../Delegates/DataViewerDelegate.h"
#include <qlistwidget.h>

class DataViewer : public QDialog
{
	Q_OBJECT

public:
	DataViewer(QListWidgetItem* item, quint8 additionalDataType, bool dataHighlight, QWidget *parent = Q_NULLPTR);
	~DataViewer();

private:
	Ui::DataViewer ui;
	std::unique_ptr<DataViewerModel> byteTableModel;
	std::unique_ptr<DataViewerModel> hexTableModel;
	std::unique_ptr<DataViewerDelegate> dataTableDelegate;
};
