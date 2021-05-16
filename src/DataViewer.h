#ifndef DATAVIEWER_HPP
#define DATAVIEWER_HPP

#include <QDialog>
#include "ui_DataViewer.h"
#include "../DefinedStructs/PacketExternStructs.hpp"
#include "../DefinedStructs/ConstDataHolder.hpp"
#include "../Models/HexdumpModel.h"
#include "../Models/ColorMapModel.hpp"
#include "../Models/USBPcapHeaderModel.hpp"
#include "../Models/AdditionalDataModel.hpp"
#include "../Delegates/HexdumpDelegate.h"
#include <qlistwidget.h>
#include <qscrollbar.h>

/// <summary>
/// Representing pop-up window to view more detailed analysis of data.
/// </summary>
class DataViewer : public QDialog
{
	Q_OBJECT

public:
	DataViewer(QTableWidgetItem* item, const HeaderDataType additionalDataType, const bool dataHighlight, QWidget *parent = Q_NULLPTR);
	~DataViewer();

private:
	void InitTables(QTableWidgetItem* item, const HeaderDataType additionalDataType, const bool dataHighlight);
	void InitTreeViews(QTableWidgetItem* item, const HeaderDataType additionalDataType);
	void InitTableViewer(QTableView* table, const bool hexViewTable);
	void InitLabels();
	void AdjustTableSize(QTableView* table, const bool hexViewTable);

	Ui::DataViewer ui;
	/// <summary>
	/// unique_ptr to byte QTableView model class.
	/// </summary>
	std::unique_ptr<HexdumpModel> byteTableModel;
	/// <summary>
	/// unique_ptr to hex QTableView model class.
	/// </summary>
	std::unique_ptr<HexdumpModel> hexTableModel;
	/// <summary>
	/// unique_ptr to QTableView delegate class.
	/// </summary>
	std::unique_ptr<HexdumpDelegate> dataTableDelegate;
	/// <summary>
	/// unique_ptr to Color Map QTreeView model class.
	/// </summary>
	std::unique_ptr<ColorMapModel> colorMapModel;
	/// <summary>
	/// unique_ptr to packet header QTreeView model class.
	/// </summary>
	std::unique_ptr<USBPcapHeaderModel> usbhModel;
	/// <summary>
	/// unique_ptr to additional data QTreeView model class.
	/// </summary>
	std::unique_ptr<AdditionalDataModel> additionalDataModel;

private slots:
	void updateHexSelection(const QItemSelection& selected, const QItemSelection& deselected);
	void updateByteSelection(const QItemSelection& selected, const QItemSelection& deselected);
};

#endif // !DATAVIEWER_HPP