#ifndef UNKNOWNDEVICEINTERPRETER_HPP
#define UNKNOWNDEVICEINTERPRETER_HPP

#include "BaseInterpreter.hpp"
#include "../DefinedStructs/ConstDataHolder.hpp"
#include "../HID/HIDDevices.hpp"

/// <summary>
/// Interpreter class for unknown descriptor
/// </summary>
class UnknownDeviceInterpreter : public BaseInterpreter
{
public:
	UnknownDeviceInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel,
		HIDReportDescriptorInputParse inputParser);
	void Interpret() override;
private:
	/// <summary>
	/// Concrete HIDReportDescriptorInputParse struct to parse this input.
	/// </summary>
	HIDReportDescriptorInputParse inputParser;
	/// <summary>
	/// Pointer to DataHolder instance.
	/// </summary>
	DataHolder* holder;
	/// <summary>
	/// Pointer to HIDDevices instance.
	/// </summary>
	HIDDevices* hidDevices;
};

#endif // !UNKNOWNDEVICEINTERPRETER_HPP
