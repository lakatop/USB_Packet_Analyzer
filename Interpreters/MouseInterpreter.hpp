#ifndef MOUSEINTERPRETER_HPP
#define MOUSEINTERPRETER_HPP

#include "../DefinedStructs/ConstDataHolder.hpp"
#include "../HID/HIDDevices.hpp"
#include "BaseInterpreter.hpp"

/// <summary>
/// Interpreter for HID mouse device.
/// </summary>
class MouseInterpreter : public BaseInterpreter
{
public:
	MouseInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel,
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

#endif // !MOUSEINTERPRETER_HPP
