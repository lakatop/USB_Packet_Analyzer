#ifndef JOYSTICKINTERPRETER_HPP
#define JOYSTICKINTERPRETER_HPP

#include "../Models/AdditionalDataModel.hpp"
#include "../DefinedStructs/ConstDataHolder.hpp"
#include "../HID/HIDDevices.hpp"

class JoystickInterpreter
{
public:
	JoystickInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel,
		HIDReportDescriptorInputParse inputParser);
	void Interpret();
private:
	TreeItem* rootItem;
	QTableWidgetItem* item;
	AdditionalDataModel* additionalDataModel;
	HIDReportDescriptorInputParse inputParser;
	HIDDevices* hidDevices;
	DataHolder* holder;
};

#endif // !JOYSTICKINTERPRETER_HPP
