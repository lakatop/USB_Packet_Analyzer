#ifndef CONFIGDESCRIPTORSMODEL_HPP
#define CONFIGDESCRIPTORSMODEL_HPP

#include "../HID/HIDDevices.hpp"
#include "BaseInterpreter.hpp"

class ConfigDescriptorsInterpreter : public BaseInterpreter
{
public:
	ConfigDescriptorsInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel);

	void Interpret() override;
private:
	void InterpretConfigDescriptor(const unsigned char* packet);
	void InterpretInterfaceDescriptor(const unsigned char* packet);
	void InterpretEndpointDescriptor(const unsigned char* packet);
	void InterpretHIDDescriptor(const unsigned char* packet);
	void InterpretUnknownDescriptor(const unsigned char* packet);

	DataHolder* holder;
	HIDDevices* hidDevices;
};

#endif // !CONFIGDESCRIPTORSMODEL_HPP
