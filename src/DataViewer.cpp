#include "DataViewer.h"
/// <summary>
/// Constructor fo DataViewer class.
/// </summary>
/// <param name="item">pointer to QTableWidgetItem which holds data for analysis</param>
/// <param name="additionalDataType">Specific type of data transfer</param>
/// <param name="dataHighlight">Determines whether data should be highlighted or not</param>
/// <param name="parent">Parent of this object</param>
DataViewer::DataViewer(QTableWidgetItem* item, HeaderDataType additionalDataType, bool dataHighlight, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	InitTables(item, additionalDataType, dataHighlight);
	InitTreeViews(item, additionalDataType);
	InitLabels();
}

/// <summary>
/// Destructor of DataViewer
/// </summary>
DataViewer::~DataViewer()
{
}

/// <summary>
/// Initializes labels for views.
/// </summary>
void DataViewer::InitLabels()
{
	ui.additionalDataLabel->setText("Additional Transfer Data");
	ui.additionalDataLabel->adjustSize();
	ui.addressLabel->setText("Address");
	ui.addressLabel->adjustSize();
	ui.colorMapLabel->setText("Color Map");
	ui.colorMapLabel->adjustSize();
	ui.hexaLabel->setText("Hexadecimal values");
	ui.hexaLabel->adjustSize();
	ui.printableLabel->setText("Printable");
	ui.printableLabel->adjustSize();
	ui.usbhLabel->setText("USBPCAP Header");
	ui.usbhLabel->adjustSize();
}

/// <summary>
/// Initializes table views. Sets models and delegates, connects signals and slots.
/// </summary>
/// <param name="item"><see cref="DataViewer()"/></param>
/// <param name="additionalDataType"><see cref="DataViewer()"/></param>
/// <param name="dataHighlight"><see cref="DataViewer()"/></param>
void DataViewer::InitTables(QTableWidgetItem* item, HeaderDataType additionalDataType, bool dataHighlight)
{
	byteTableModel = std::make_unique<HexdumpModel>(item, false, additionalDataType, this);
	hexTableModel = std::make_unique<HexdumpModel>(item, true, additionalDataType, this);
	dataTableDelegate = std::make_unique<HexdumpDelegate>(dataHighlight, this);

	connect(ui.byteTableView->verticalScrollBar(), SIGNAL(valueChanged(int)), ui.hexTableView->verticalScrollBar(), SLOT(setValue(int)));
	connect(ui.hexTableView->verticalScrollBar(), SIGNAL(valueChanged(int)), ui.byteTableView->verticalScrollBar(), SLOT(setValue(int)));

	ui.byteTableView->setItemDelegate(dataTableDelegate.get());
	ui.hexTableView->setItemDelegate(dataTableDelegate.get());

	InitTableViewer(ui.byteTableView, false);
	InitTableViewer(ui.hexTableView, true);

	connect(ui.byteTableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DataViewer::updateHexSelection);
	connect(ui.hexTableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DataViewer::updateByteSelection);
}

/// <summary>
/// Initializes tree views. Sets models and adjust columns width.
/// </summary>
/// <param name="item"><see cref="DataViewer()"/></param>
/// <param name="additionalDataType"><see cref="DataViewer()"/></param>
void DataViewer::InitTreeViews(QTableWidgetItem* item, HeaderDataType additionalDataType)
{
	colorMapModel = std::make_unique<ColorMapModel>(this);
	ui.colorMapTreeView->setModel(colorMapModel.get());
	ui.colorMapTreeView->header()->setVisible(false);

	usbhModel = std::make_unique<USBPcapHeaderModel>(item, this);
	ui.usbHeaderTreeView->setModel(usbhModel.get());
	ui.usbHeaderTreeView->resizeColumnToContents(1);
	int columnWidth = ((ui.usbHeaderTreeView->width() - ui.usbHeaderTreeView->columnWidth(1)) / 2) - 1;
	ui.usbHeaderTreeView->setColumnWidth(0, columnWidth + 50);
	ui.usbHeaderTreeView->setColumnWidth(2, columnWidth - 50);

	additionalDataModel = std::make_unique<AdditionalDataModel>(item, additionalDataType, this);
	ui.additionalDataTreeView->setModel(additionalDataModel.get());
	columnWidth = ((ui.additionalDataTreeView->width() - 100) / 2) - 1;
	ui.additionalDataTreeView->setColumnWidth(0, columnWidth);
	ui.additionalDataTreeView->setColumnWidth(1, columnWidth);
	ui.additionalDataTreeView->setColumnWidth(2, 100);
}

/// <summary>
/// Sets properties for concrete table view.
/// </summary>
/// <param name="table">Table view which properties should be set</param>
/// <param name="hexViewTable">true if table is hexTableView, false if it is byteTableView</param>
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

/// <summary>
/// Adjust size of table cells.
/// </summary>
/// <param name="table"><see cref="InitTableViewer()"/></param>
/// <param name="hexViewTable"><see cref="InitTableViewer()"/></param>
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

/// <summary>
/// Slot which is called when items in byteTableView are selected. It then selects and deselects corresponding items in hexTableView.
/// </summary>
/// <param name="selected">Selected items in byteTableView</param>
/// <param name="deselected">Deselected items in byteTableView</param>
void DataViewer::updateHexSelection(const QItemSelection& selected, const QItemSelection& deselected)
{
	auto hexSelectionModel = ui.hexTableView->selectionModel();
	hexSelectionModel->select(selected, QItemSelectionModel::Select);
	hexSelectionModel->select(deselected, QItemSelectionModel::Deselect);
}

/// <summary>
/// Slot which is called when items in hexTableView are selected. It then selects and deselects corresponding items in byteTableView.
/// </summary>
/// <param name="selected">Selected items in hexTableView</param>
/// <param name="deselected">Deselected items in hexTableView</param>
void DataViewer::updateByteSelection(const QItemSelection& selected, const QItemSelection& deselected)
{
	auto byteSelectionModel = ui.byteTableView->selectionModel();
	byteSelectionModel->select(selected, QItemSelectionModel::Select);
	byteSelectionModel->select(deselected, QItemSelectionModel::Deselect);
}