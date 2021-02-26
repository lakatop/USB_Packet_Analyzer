#include "AdditionalDataModel.hpp"

AdditionalDataModel::AdditionalDataModel(QListWidgetItem* item, HeaderDataType dataType, QObject* parent) : 
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
    switch (dataType)
    {
    case INTERR_TRANSFER:
    {
        InterruptTransferModel interruptModel(rootItem.get(), item, this);
        interruptModel.InterpretInterruptTransfer();
    }
        break;
    case ISOCHRO_TRANSFER:
        break;
    case BULK_TRANSFER:
        break;
    case CONTROL_TRANSFER:
    case CONTROL_TRANSFER_CONFIG_DESC:
    case CONTROL_TRANSFER_INTERF_DESC:
    case CONTROL_TRANSFER_ENDPOI_DESC:
    case CONTROL_TRANSFER_HID_DESC:
    case CONTROL_TRANSFER_OTHER_SPEED_CONF_DESC:
    {
        ConfigDescriptorsModel configDescsModel(rootItem.get(), item, this);
        configDescsModel.InterpretConfigDescriptors();
    }
    break;
    case CONTROL_TRANSFER_DEVICE_DESC:
    {
        FixedDescriptorTreeModel deviceDescModel(rootItem.get(), item, this);
        deviceDescModel.InterpretControlTransferDeviceDescriptor();
    }
    break;
    case CONTROL_TRANSFER_STRING_DESC:
    {
        FixedDescriptorTreeModel stringDescModel(rootItem.get(), item, this);
        stringDescModel.InterpretControlTransferStringDescriptor();
    }
    break;
    case CONTROL_TRANSFER_HID_REPORT_DESC:
    {
        ReportDescriptorModel reportDescModel(rootItem.get(), item, this);
        reportDescModel.InterpretReportDescriptor();
    }
    break;
    case CONTROL_TRANSFER_SETUP:
    {
        FixedDescriptorTreeModel setupModel(rootItem.get(), item, this);
        setupModel.InterpretControlTransferSetup();
    }
    break;
    case CONTROL_TRANSFER_DEVICE_QUALIFIER_DESC:
    {
        FixedDescriptorTreeModel deviceQualifierDescModel(rootItem.get(), item, this);
        deviceQualifierDescModel.InterpretControlTransferDeviceQualifierDescriptor();
    }
    break;
    case CONTROL_TRANSFER_UNSPEC_DESC:
    {
        FixedDescriptorTreeModel unspecDescModel(rootItem.get(), item, this);
        unspecDescModel.InterpretControlTransferUnspecifiedDescriptor();
    }
    break;
    case IRP_INFO_TRANSFER:
        break;
    case UNKNOWN_TRANSFER:
        break;
    default:
        break;
    }
}