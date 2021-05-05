#ifndef COLORMAPMODEL_HPP
#define COLORMAPMODEL_HPP

#include "TreeItemBaseModel.h"
/// <summary>
/// Model class for Color Map
/// </summary>
class ColorMapModel : public TreeItemBaseModel
{
public:
	ColorMapModel(QObject* parent);
	~ColorMapModel();
	QVariant data(const QModelIndex& index, int role) const override;
	/// <summary>
	/// Return empty QVariant because we have no header in Color Map tree view.
	/// </summary>
	/// <param name="section">Column of header</param>
	/// <param name="orientation">Defines orientation of header</param>
	/// <param name="role">Defines Qt role</param>
	/// <returns>Empty QVariant</returns>
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override { return QVariant(); };
private:
	void SetupModelData();
};

#endif // !COLORMAPMODEL_HPP