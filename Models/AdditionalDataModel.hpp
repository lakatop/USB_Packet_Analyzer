#ifndef ADDITIONALDATAMODEL_HPP
#define ADDITIONALDATAMODEL_HPP

#include "TreeItemBaseModel.h"
#include "FixedDescriptorTreeeModel.hpp"
#include "ReportDescriptorModel.hpp"
#include "ConfigDescriptorsModel.hpp"

#include <qlistwidget.h>

class AdditionalDataModel : public TreeItemBaseModel
{
public:
	AdditionalDataModel(QListWidgetItem* item, HeaderDataType dataType, QObject* parent = Q_NULLPTR);

	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
	void CreateSpecifiedModel();

	QListWidgetItem* item;
	HeaderDataType dataType;
};

#endif