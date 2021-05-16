#ifndef ADDITIONALDATAMODEL_HPP
#define ADDITIONALDATAMODEL_HPP

#include "TreeItemBaseModel.h"

#include <qtablewidget.h>

/// <summary>
/// Model class for additional packet data.
/// </summary>
class AdditionalDataModel : public TreeItemBaseModel
{
public:
	AdditionalDataModel(QTableWidgetItem* item, const HeaderDataType dataType, QObject* parent = Q_NULLPTR);

	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
	void SetupSpecifiedModelData();

	/// <summary>
	/// Item which holds data for this model.
	/// </summary>
	QTableWidgetItem* item;
	/// <summary>
	/// Data type of packet data in <see cref = "item"/>
	/// </summary>
	HeaderDataType dataType;
};

#endif // !ADDITIONALDATAMODEL_HPP