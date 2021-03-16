#ifndef ADDITIONALDATAMODEL_HPP
#define ADDITIONALDATAMODEL_HPP

#include "TreeItemBaseModel.h"
#include "../Interpreters/FixedDescriptorTreeInterpreter.hpp"
#include "../Interpreters/ReportDescriptorInterpreter.hpp"
#include "../Interpreters/ConfigDescriptorsInterpreter.hpp"
#include "../Interpreters/InterruptTransferInterpreter.hpp"

#include <qtablewidget.h>

class AdditionalDataModel : public TreeItemBaseModel
{
public:
	AdditionalDataModel(QTableWidgetItem* item, HeaderDataType dataType, QObject* parent = Q_NULLPTR);

	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
	void CreateSpecifiedModel();

	QTableWidgetItem* item;
	HeaderDataType dataType;
};

#endif