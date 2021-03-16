#include "USBPcapHeaderModel.hpp"

USBPcapHeaderModel::USBPcapHeaderModel(QTableWidgetItem* item, QObject* parent) : TreeItemBaseModel(parent)
{
    this->item = item;
    this->holder = DataHolder::GetDataHolder();
    SetupModelData();
}

QVariant USBPcapHeaderModel::data(const QModelIndex& index, int role) const
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

QVariant USBPcapHeaderModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->Data(section);

    return QVariant();
}

void USBPcapHeaderModel::SetupModelData()
{
    const unsigned char* it = (const unsigned char*)item->data(holder->USBPCAP_HEADER_DATA).toByteArray().constData();
    PUSBPCAP_BUFFER_PACKET_HEADER usbh = (PUSBPCAP_BUFFER_PACKET_HEADER)it;

    rootItem = std::make_unique<TreeItem>(QVector<QVariant>({ tr("Data"), tr("Meaning"), tr("Value") }));
    QString data;
    CharToHexConvert(&it, 2, data);
    rootItem->AppendChild(new TreeItem(QVector<QVariant>{data, "Header Length", usbh->headerLen}, rootItem.get()));
    CharToHexConvert(&it, 8, data);
    rootItem->AppendChild(new TreeItem(QVector<QVariant>{data, "IRP ID", usbh->irpId}, rootItem.get()));
    CharToHexConvert(&it, 4, data);
    rootItem->AppendChild(new TreeItem(QVector<QVariant>{data, "USBD_STATUS", usbh->status}, rootItem.get()));
    CharToHexConvert(&it, 2, data);
    rootItem->AppendChild(new TreeItem(QVector<QVariant>{data, "URB function", usbh->function}, rootItem.get()));
    CharToHexConvert(&it, 1, data);
    rootItem->AppendChild(new TreeItem(QVector<QVariant>{data, "IRP info", usbh->info}, rootItem.get()));

    TreeItem* infoChild = rootItem->Child(rootItem->ChildCount() - 1);
    infoChild->AppendChild(new TreeItem(QVector<QVariant>{ShowBits(0, 7, usbh->info), "Reserved 0x00"}, infoChild));
    infoChild->AppendChild(new TreeItem(QVector<QVariant>{ShowBits(7, 1, usbh->info), (usbh->info & 0x1) ? "Direction: PDO -> FDO (0x1)" :
        "Direction: FDO -> PDO (0x0)"}, infoChild));

    CharToHexConvert(&it, 2, data);
    rootItem->AppendChild(new TreeItem(QVector<QVariant>{data, "URB Bus ID", usbh->bus}, rootItem.get()));
    CharToHexConvert(&it, 2, data);
    rootItem->AppendChild(new TreeItem(QVector<QVariant>{data, "Device Address", usbh->device}, rootItem.get()));
    CharToHexConvert(&it, 1, data);
    rootItem->AppendChild(new TreeItem(QVector<QVariant>{data, "Endpoint", usbh->endpoint}, rootItem.get()));
    TreeItem* endpointChild = rootItem->Child(rootItem->ChildCount() - 1);
    endpointChild->AppendChild(new TreeItem(QVector<QVariant>{ShowBits(0, 1, usbh->endpoint), (usbh->endpoint & 0x80) ? "Direction: IN (1)" :
        "Direction: OUT(0)"}, endpointChild));
    endpointChild->AppendChild(new TreeItem(QVector<QVariant>{ShowBits(4, 4, usbh->endpoint), "Endpoit number: ", usbh->endpoint & 0x0F}, endpointChild));
    CharToHexConvert(&it, 1, data);
    rootItem->AppendChild(new TreeItem(QVector<QVariant>{data, "Transfer type", (usbh->transfer == 0) ? "ISOCHRONOUS (0x0)" :
        (usbh->transfer == 1) ? "INTERRUPT (0x1)" :
        (usbh->transfer == 2) ? "CONTROL (0x2)" : "BULK (0x3)"}, rootItem.get()));
    CharToHexConvert(&it, 4, data);
    rootItem->AppendChild(new TreeItem(QVector<QVariant>{data, "Packet data length", usbh->dataLength}, rootItem.get()));

    if (item->data(holder->TRANSFER_OPTIONAL_HEADER).isValid())
    {
        QByteArray additionalHeaderData = item->data(holder->TRANSFER_OPTIONAL_HEADER).toByteArray();
        const unsigned char* additionalData = (const unsigned char*)additionalHeaderData.constData();
        if (usbh->transfer == 2)
        {
            UCHAR stage = (UCHAR)(*additionalData);
            CharToHexConvert(&additionalData, additionalHeaderData.size(), data);
            rootItem->AppendChild(new TreeItem(QVector<QVariant>{data, "Additional Header Data", (stage == 0) ? "Setup stage (0)" :
                (stage == 1) ? "Data stage (1)" :
                (stage == 2) ? "Status stage (2)" : "Complete stage (3)"}, rootItem.get()));
        }
        else
        {
            CharToHexConvert(&additionalData, additionalHeaderData.size(), data);
            rootItem->AppendChild(new TreeItem(QVector<QVariant>{data, "Additional Header Data"}, rootItem.get()));
        }
    }
}