#ifndef JOYSTICKINTERPRETER_HPP
#define JOYSTICKINTERPRETER_HPP

#include "../DefinedStructs/ConstDataHolder.hpp"
#include "../HID/HIDDevices.hpp"
#include "BaseInterpreter.hpp"

class JoystickInterpreter : public BaseInterpreter
{
public:
	JoystickInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel,
		HIDReportDescriptorInputParse inputParser);
	void Interpret() override;
private:
	HIDReportDescriptorInputParse inputParser;
	DataHolder* holder;
	HIDDevices* hidDevices;
};

#endif // !JOYSTICKINTERPRETER_HPP
