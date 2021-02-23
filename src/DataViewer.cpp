#include "DataViewer.h"

DataViewer::DataViewer(QListWidgetItem* item, quint8 additionalDataType, bool dataHighlight, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	byteTableModel = std::make_unique<DataViewerModel>(item, false, additionalDataType, this);
	hexTableModel = std::make_unique<DataViewerModel>(item, true, additionalDataType, this);
	dataTableDelegate = std::make_unique<DataViewerDelegate>(dataHighlight, this);
	ui.byteTableView->setModel(byteTableModel.get());
	ui.hexTableView->setModel(hexTableModel.get());
	ui.byteTableView->setItemDelegate(dataTableDelegate.get());
	ui.hexTableView->setItemDelegate(dataTableDelegate.get());
}

DataViewer::~DataViewer()
{
}
