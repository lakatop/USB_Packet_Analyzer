#ifndef MOUSEINTERPRETER_HPP
#define MOUSEINTERPRETER_HPP

#include "../Models/AdditionalDataModel.hpp"
#include "../DefinedStructs/ConstDataHolder.hpp"
#include "../HID/HIDDevices.hpp"

class MouseInterpreter
{
public:
	MouseInterpreter(TreeItem* rootItem, QListWidgetItem* item, AdditionalDataModel* additionalDataModel,
		HIDReportDescriptorInputParse inputParser);
	void Interpret();
private:
	TreeItem* rootItem;
	QListWidgetItem* item;
	AdditionalDataModel* additionalDataModel;
	HIDReportDescriptorInputParse inputParser;
	HIDDevices* hidDevices;
	DataHolder* holder;
};

#endif // !MOUSEINTERPRETER_HPP
