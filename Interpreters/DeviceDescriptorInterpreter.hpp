#ifndef DEVICEDESCRIPTORINTERPRETER_HPP
#define DEVICEDESCRIPTORINTERPRETER_HPP

#include "BaseInterpreter.hpp"

class DeviceQualifierDescriptorInterpreter; //forward declaration

class DeviceDescriptorInterpreter : public BaseInterpreter
{
public:
	DeviceDescriptorInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel);
	void Interpret() override;
private:
	friend class DeviceQualifierDescriptorInterpreter;
	DataHolder* holder;
	template <typename T>
	static void InterpretControlTransferDeviceDescriptorBase(T* deviceDescriptor, const unsigned char** packet, 
		TreeItem* descriptorChild, AdditionalDataModel* additionalDataModel);
};

class DeviceQualifierDescriptorInterpreter : public BaseInterpreter
{
public:
	DeviceQualifierDescriptorInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel);
	void Interpret() override;
private:
	DataHolder* holder;
};


template <typename T>
void DeviceDescriptorInterpreter::InterpretControlTransferDeviceDescriptorBase(T* deviceDescriptor, const unsigned char** packet,
	TreeItem* descriptorChild, AdditionalDataModel* additionalDataModel)
{
	QString hexData;
	additionalDataModel->CharToHexConvert(packet, 1, hexData);
	descriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bLength", deviceDescriptor->bLength}, descriptorChild));
	additionalDataModel->CharToHexConvert(packet, 1, hexData);
	descriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, (std::string("bDescriptorType ") +
		std::string((deviceDescriptor->bDescriptorType == DEVICE_DESCRIPTOR) ? "(DEVICE_DESCRIPTOR)" : "(DEVICE_QUALIFIER_DESCRIPTOR)")).c_str(),
		deviceDescriptor->bDescriptorType}, descriptorChild));
	additionalDataModel->CharToHexConvert(packet, 2, hexData);
	descriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bcdUSB", deviceDescriptor->bcdUSB}, descriptorChild));
	additionalDataModel->CharToHexConvert(packet, 1, hexData);
	descriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bDeviceClass", deviceDescriptor->bDeviceClass}, descriptorChild));
	additionalDataModel->CharToHexConvert(packet, 1, hexData);
	descriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bDeviceSubClass", deviceDescriptor->bDeviceSubClass}, descriptorChild));
	additionalDataModel->CharToHexConvert(packet, 1, hexData);
	descriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bDeviceProtocol", deviceDescriptor->bDeviceProtocol}, descriptorChild));
	additionalDataModel->CharToHexConvert(packet, 1, hexData);
	descriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bMaxPacketSize0", deviceDescriptor->bMaxPacketSize0}, descriptorChild));
}


#endif // !DEVICEDESCRIPTORINTERPRETER_HPP
