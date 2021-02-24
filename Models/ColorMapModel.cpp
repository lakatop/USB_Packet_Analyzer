#include "ColorMapModel.hpp"

ColorMapModel::ColorMapModel(QObject* parent) : TreeItemBaseModel(parent)
{
    SetupModelData();
}

ColorMapModel::~ColorMapModel()
{
}

QVariant ColorMapModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DecorationRole)
    {
        TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
        return GetDataTypeColor((HeaderDataType)item->Data(0).toInt());
    }

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
    return item->Data(1);
}

void ColorMapModel::SetupModelData()
{
    rootItem = std::make_unique<TreeItem>(QVector<QVariant>({ "" })); //adding 1 item to vector, because columnCount returns size of parents vector(which in this situation should be 1)
    rootItem->AppendChild(new TreeItem(QVector<QVariant>{ INTERR_TRANSFER, "Interrupt Transfer"}, rootItem.get()));
    rootItem->AppendChild(new TreeItem(QVector<QVariant>{ ISOCHRO_TRANSFER, "Isochronous Transfer"}, rootItem.get()));
    rootItem->AppendChild(new TreeItem(QVector<QVariant>{ BULK_TRANSFER, "Bulk Transfer"}, rootItem.get()));
    rootItem->AppendChild(new TreeItem(QVector<QVariant>{ CONTROL_TRANSFER, "Control Transfer"}, rootItem.get()));

    TreeItem* controlTransferItem = rootItem->Child(rootItem->ChildCount() - 1);
    controlTransferItem->AppendChild(new TreeItem(QVector<QVariant>{ CONTROL_TRANSFER_RESPONSE, "Response"}, controlTransferItem));
    controlTransferItem->AppendChild(new TreeItem(QVector<QVariant>{ CONTROL_TRANSFER_DEVICE_DESC, "Device Descriptor"}, controlTransferItem));
    controlTransferItem->AppendChild(new TreeItem(QVector<QVariant>{ CONTROL_TRANSFER_CONFIG_DESC, "Configuration Descriptor"}, controlTransferItem));
    controlTransferItem->AppendChild(new TreeItem(QVector<QVariant>{ CONTROL_TRANSFER_STRING_DESC, "String Descriptor"}, controlTransferItem));
    controlTransferItem->AppendChild(new TreeItem(QVector<QVariant>{ CONTROL_TRANSFER_INTERF_DESC, "Interface Descriptor"}, controlTransferItem));
    controlTransferItem->AppendChild(new TreeItem(QVector<QVariant>{ CONTROL_TRANSFER_ENDPOI_DESC, "Endpoint Descriptor"}, controlTransferItem));
    controlTransferItem->AppendChild(new TreeItem(QVector<QVariant>{ CONTROL_TRANSFER_HID_DESC, "HID Descriptor"}, controlTransferItem));
    controlTransferItem->AppendChild(new TreeItem(QVector<QVariant>{ CONTROL_TRANSFER_SETUP, "Setup Data"}, controlTransferItem));
    controlTransferItem->AppendChild(new TreeItem(QVector<QVariant>{ CONTROL_TRANSFER_UNSPEC_DESC, "Unspecified Descriptor"}, controlTransferItem));

    rootItem->AppendChild(new TreeItem(QVector<QVariant>{ CONTROL_TRANSFER_HID_REPORT_DESC, "HID Report Descriptor"}, rootItem.get()));
    rootItem->AppendChild(new TreeItem(QVector<QVariant>{ ADDITIONAL_HEADER_DATA, "Additional Header Data"}, rootItem.get()));
    rootItem->AppendChild(new TreeItem(QVector<QVariant>{ HEADER_DATA, "Header Data"}, rootItem.get()));
}