#include "DataViewer.h"

DataViewer::DataViewer(QListWidgetItem* item, HeaderDataType additionalDataType, bool dataHighlight, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	InitTables(item, additionalDataType, dataHighlight);
}

DataViewer::~DataViewer()
{
}

void DataViewer::InitTables(QListWidgetItem* item, HeaderDataType additionalDataType, bool dataHighlight)
{
	byteTableModel = std::make_unique<DataViewerModel>(item, false, additionalDataType, this);
	hexTableModel = std::make_unique<DataViewerModel>(item, true, additionalDataType, this);
	dataTableDelegate = std::make_unique<DataViewerDelegate>(dataHighlight, this);

	connect(ui.byteTableView->verticalScrollBar(), SIGNAL(valueChanged(int)), ui.hexTableView->verticalScrollBar(), SLOT(setValue(int)));
	connect(ui.hexTableView->verticalScrollBar(), SIGNAL(valueChanged(int)), ui.byteTableView->verticalScrollBar(), SLOT(setValue(int)));

	ui.byteTableView->setItemDelegate(dataTableDelegate.get());
	ui.hexTableView->setItemDelegate(dataTableDelegate.get());

	InitTableViewer(ui.byteTableView, false);
	InitTableViewer(ui.hexTableView, true);
}

void DataViewer::InitTableViewer(QTableView* table, bool hexViewTable)
{
	if (hexViewTable)
	{
		table->setModel(hexTableModel.get());
		table->verticalHeader()->setVisible(false);
		table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}
	else
	{
		table->setModel(byteTableModel.get());
	}

	table->horizontalHeader()->setMinimumSectionSize(5);
	table->verticalHeader()->setMinimumSectionSize(5);
	table->resizeColumnsToContents();
	table->resizeRowsToContents();
	table->setShowGrid(false);
	table->horizontalHeader()->setVisible(false);
	table->verticalScrollBar()->setFixedSize(QSize(20, table->height()));
	AdjustTableSize(table, hexViewTable);
}

void DataViewer::AdjustTableSize(QTableView* table, bool hexViewTable)
{
	size_t maxHeight = 0;
	size_t maxWidth = 0;
	for (int i = 0; i < table->model()->rowCount(); i++)
	{
		maxHeight += table->rowHeight(i);
	}

	if (!hexViewTable)
	{
		maxWidth += table->verticalHeader()->width();
		maxWidth += table->verticalScrollBar()->width();
	}
	for (int i = 0; i < table->model()->columnCount(); i++)
	{
		maxWidth += table->columnWidth(i);
	}


	table->setMaximumHeight(maxHeight + 2);
	table->setMaximumWidth(maxWidth + 3);
}