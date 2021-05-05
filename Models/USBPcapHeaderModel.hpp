#ifndef USBPCAPHEADERMODEL_HPP
#define USBPCAPHEADERMODEL_HPP

#include "TreeItemBaseModel.h"
#include "../DefinedStructs/ConstDataHolder.hpp"
#include <qtablewidget.h>

/// <summary>
/// Model class for packet header.
/// </summary>
class USBPcapHeaderModel : public TreeItemBaseModel
{
public:
	USBPcapHeaderModel(QTableWidgetItem* item, QObject* parent = nullptr);
	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
private:
	void SetupModelData();

	/// <summary>
	/// Pointer to DataHolder class instance.
	/// </summary>
	DataHolder* holder;
	/// <summary>
	/// Pointer to item which holds packet data (alongside with header data)
	/// </summary>
	QTableWidgetItem* item;
};

#endif // !USBPCAPHEADERMODEL_HPP