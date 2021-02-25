#ifndef CONFIGDESCRIPTORSMODEL_HPP
#define CONFIGDESCRIPTORSMODEL_HPP

#include "AdditionalDataModel.hpp"
#include "../HID/HIDDevices.hpp"

class ConfigDescriptorsModel
{
public:
	ConfigDescriptorsModel(TreeItem* rootItem, QListWidgetItem* item, AdditionalDataModel* additionalDataModel);

	void InterpretConfigDescriptors();
private:
	void InterpretConfigDescriptor(const unsigned char* packet);
	void InterpretInterfaceDescriptor(const unsigned char* packet);
	void InterpretEndpointDescriptor(const unsigned char* packet);
	void InterpretHIDDescriptor(const unsigned char* packet);

	TreeItem* rootItem;
	QListWidgetItem* item;
	AdditionalDataModel* additionalDataModel;
	DataHolder* holder;
	HIDDevices* hidDevices;
};

#endif // !CONFIGDESCRIPTORSMODEL_HPP
