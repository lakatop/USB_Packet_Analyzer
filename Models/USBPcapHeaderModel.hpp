#ifndef USBPCAPHEADERMODEL_HPP
#define USBPCAPHEADERMODEL_HPP

#include "TreeItemBaseModel.h"
#include "../DefinedStructs/ConstDataHolder.hpp"
#include <qtablewidget.h>

class USBPcapHeaderModel : public TreeItemBaseModel
{
public:
	USBPcapHeaderModel(QTableWidgetItem* item, QObject* parent = nullptr);
	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
private:
	void SetupModelData();

	DataHolder* holder;
	QTableWidgetItem* item;
};

#endif