#ifndef KEYBOARDINTERPRETER_HPP
#define KEYBOARDINTERPRETER_HPP

#include "../Models/AdditionalDataModel.hpp"
#include "../DefinedStructs/ConstDataHolder.hpp"
#include "../HID/HIDDevices.hpp"

class KeyboardInterpreter
{
public:
	KeyboardInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel,
		HIDReportDescriptorInputParse inputParser);
	void Interpret();
private:
	void InterpretModifierKey(TreeItem* child, const unsigned char* packet);

	TreeItem* rootItem;
	QTableWidgetItem* item;
	AdditionalDataModel* additionalDataModel;
	HIDReportDescriptorInputParse inputParser;
	HIDDevices* hidDevices;
	DataHolder* holder;
};

#endif // !KEYBOARDINTERPRETER_HPP
