#ifndef CONFIGDESCRIPTORSMODEL_HPP
#define CONFIGDESCRIPTORSMODEL_HPP

#include "../HID/HIDDevices.hpp"
#include "BaseInterpreter.hpp"

/// <summary>
/// Interpreter class for configuration descriptors
/// </summary>
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

	/// <summary>
	/// Pointer to DataHolder class instance
	/// </summary>
	DataHolder* holder;
	/// <summary>
	/// Pointer to HIDDevices class instance
	/// </summary>
	HIDDevices* hidDevices;
};

#endif // !CONFIGDESCRIPTORSMODEL_HPP
