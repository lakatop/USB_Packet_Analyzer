#include "DeviceDescriptorInterpreter.hpp"

DeviceDescriptorInterpreter::DeviceDescriptorInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel)
	:BaseInterpreter(rootItem, item, additionalDataModel)
{
	this->holder = DataHolder::GetDataHolder();
}

void DeviceDescriptorInterpreter::Interpret()
{
	QByteArray leftoverData = item->data(holder->TRANSFER_LEFTOVER_DATA).toByteArray();
	const unsigned char* packet = (unsigned char*)leftoverData.constData();
	PUSB_DEVICE_DESCRIPTOR deviceDescriptor = (PUSB_DEVICE_DESCRIPTOR)packet;
	rootItem->AppendChild(new TreeItem(QVector<QVariant>{"DEVICE_DESCRIPTOR", "", ""}, rootItem));
	TreeItem* deviceDescriptorChild = rootItem->Child(rootItem->ChildCount() - 1);

	QString hexData;
	InterpretControlTransferDeviceDescriptorBase(deviceDescriptor, &packet, deviceDescriptorChild, additionalDataModel);
	additionalDataModel->CharToHexConvert(&packet, 2, hexData);
	deviceDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "idVendor", deviceDescriptor->idVendor}, deviceDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 2, hexData);
	deviceDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "idProduct", deviceDescriptor->idProduct}, deviceDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 2, hexData);
	deviceDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bcdDevice", deviceDescriptor->bLength}, deviceDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	deviceDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "iManufacturer", deviceDescriptor->iManufacturer}, deviceDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	deviceDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "iProduct", deviceDescriptor->iProduct}, deviceDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	deviceDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "iSerialNumber", deviceDescriptor->iSerialNumber}, deviceDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	deviceDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bNumConfigurations", deviceDescriptor->bNumConfigurations}, deviceDescriptorChild));
}

DeviceQualifierDescriptorInterpreter::DeviceQualifierDescriptorInterpreter(TreeItem* rootItem, QTableWidgetItem* item,
	AdditionalDataModel* additionalDataModel)
	:BaseInterpreter(rootItem, item, additionalDataModel)
{
	this->holder = DataHolder::GetDataHolder();
}

void DeviceQualifierDescriptorInterpreter::Interpret()
{
	QByteArray leftoverData = item->data(holder->TRANSFER_LEFTOVER_DATA).toByteArray();
	const unsigned char* packet = (unsigned char*)leftoverData.constData();
	PUSB_DEVICE_QUALIFIER_DESCRIPTOR deviceQualifierDescriptor = (PUSB_DEVICE_QUALIFIER_DESCRIPTOR)packet;
	rootItem->AppendChild(new TreeItem(QVector<QVariant>{"DEVICE_QUALIFIER_DESCRIPTOR", "", ""}, rootItem));
	TreeItem* deviceQualifierChild = rootItem->Child(rootItem->ChildCount() - 1);

	QString hexData;
	DeviceDescriptorInterpreter::InterpretControlTransferDeviceDescriptorBase(deviceQualifierDescriptor, &packet, deviceQualifierChild, additionalDataModel);
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	deviceQualifierChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bNumConfigurations", deviceQualifierDescriptor->bNumConfigurations}, deviceQualifierChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	deviceQualifierChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bReserved", deviceQualifierDescriptor->bReserved}, deviceQualifierChild));
}