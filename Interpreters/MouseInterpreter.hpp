#ifndef MOUSEINTERPRETER_HPP
#define MOUSEINTERPRETER_HPP

#include "../DefinedStructs/ConstDataHolder.hpp"
#include "../HID/HIDDevices.hpp"
#include "BaseInterpreter.hpp"


class MouseInterpreter : public BaseInterpreter
{
public:
	MouseInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel,
		HIDReportDescriptorInputParse inputParser);
	void Interpret() override;
private:
	HIDReportDescriptorInputParse inputParser;
	DataHolder* holder;
	HIDDevices* hidDevices;
};

#endif // !MOUSEINTERPRETER_HPP
