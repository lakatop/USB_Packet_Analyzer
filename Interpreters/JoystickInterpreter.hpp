#ifndef JOYSTICKINTERPRETER_HPP
#define JOYSTICKINTERPRETER_HPP

#include "../DefinedStructs/ConstDataHolder.hpp"
#include "../HID/HIDDevices.hpp"
#include "BaseInterpreter.hpp"

/// <summary>
/// Interpreter for HID joystick device.
/// </summary>
class JoystickInterpreter : public BaseInterpreter
{
public:
	JoystickInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel,
		const HIDReportDescriptorInputParse inputParser);
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

#endif // !JOYSTICKINTERPRETER_HPP
