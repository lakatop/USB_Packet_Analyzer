#ifndef COLORMAPMODEL_HPP
#define COLORMAPMODEL_HPP

#include "TreeItemBaseModel.h"

class ColorMapModel : public TreeItemBaseModel
{
public:
	ColorMapModel(QObject* parent);
	~ColorMapModel();
	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override { return QVariant(); };
private:
	void SetupModelData();
};

#endif