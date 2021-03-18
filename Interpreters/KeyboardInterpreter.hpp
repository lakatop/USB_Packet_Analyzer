#ifndef KEYBOARDINTERPRETER_HPP
#define KEYBOARDINTERPRETER_HPP

#include "../DefinedStructs/ConstDataHolder.hpp"
#include "../HID/HIDDevices.hpp"
#include "BaseInterpreter.hpp"

class KeyboardInterpreter : public BaseInterpreter
{
public:
	KeyboardInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel,
		HIDReportDescriptorInputParse inputParser);
	void Interpret() override;
private:
	void InterpretModifierKey(TreeItem* child, const unsigned char* packet);

	HIDReportDescriptorInputParse inputParser;
	DataHolder* holder;
	HIDDevices* hidDevices;
};

#endif // !KEYBOARDINTERPRETER_HPP
