#include "AdditionalDataModel.hpp"

#include "../Interpreters/InterpreterFactory.hpp"

/// <summary>
/// Constructor of AdditionaldataModel.
/// </summary>
/// <param name="item"><see cref="item"/></param>
/// <param name="dataType"><see cref="dataType"/></param>
/// <param name="parent">Dialog <see cref="DataViewer"/></param>
AdditionalDataModel::AdditionalDataModel(QTableWidgetItem* item, HeaderDataType dataType, QObject* parent) :
	TreeItemBaseModel(parent)
{
	this->item = item;
	this->dataType = dataType;
	rootItem = std::make_unique<TreeItem>(QVector<QVariant>({ tr("Data"), tr("Meaning"), tr("Value") }));
    SetupSpecifiedModelData();
}

/// <summary>
/// Returns data on specified index and role
/// </summary>
/// <param name="index">Index of data</param>
/// <param name="role">Role of data</param>
/// <returns>QVariant on specified index and role of <see cref="item"/></returns>
QVariant AdditionalDataModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());

    return item->Data(index.column());
}

/// <summary>
/// Returns header data which are stored as data in <see cref="rootItem"/>
/// </summary>
/// <param name="section">Column of header</param>
/// <param name="orientation">Defines orientation of header</param>
/// <param name="role">Defines Qt role</param>
/// <returns>QVariant with header data</returns>
QVariant AdditionalDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->Data(section);

    return QVariant();
}

/// <summary>
/// Thanks to <see cref="InterpreterFactory"/> chooses correct interpreter and interprets additional packet data.
/// </summary>
void AdditionalDataModel::SetupSpecifiedModelData()
{
    InterpreterFactory factory(rootItem.get(), item, this,dataType);
    std::unique_ptr<BaseInterpreter> interpreter(factory.GetInterpreter());
    if (interpreter != nullptr)
    {
        interpreter->Interpret();
    }
}