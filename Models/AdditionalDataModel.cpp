#include "AdditionalDataModel.hpp"

#include "../Interpreters/InterpreterFactory.hpp"

AdditionalDataModel::AdditionalDataModel(QTableWidgetItem* item, HeaderDataType dataType, QObject* parent) :
	TreeItemBaseModel(parent)
{
	this->item = item;
	this->dataType = dataType;
	rootItem = std::make_unique<TreeItem>(QVector<QVariant>({ tr("Data"), tr("Meaning"), tr("Value") }));
    CreateSpecifiedModel();
}

QVariant AdditionalDataModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());

    auto a = index.column();
    auto b = index.row();

    return item->Data(index.column());
}

QVariant AdditionalDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->Data(section);

    return QVariant();
}

void AdditionalDataModel::CreateSpecifiedModel()
{
    InterpreterFactory factory(rootItem.get(), item, this,dataType);
    std::unique_ptr<BaseInterpreter> interpreter(factory.GetInterpreter());
    if (interpreter != nullptr)
    {
        interpreter->Interpret();
    }
}