#ifndef UNKNOWNDEVICEINTERPRETER_HPP
#define UNKNOWNDEVICEINTERPRETER_HPP

#include "../Models/AdditionalDataModel.hpp"
#include "../DefinedStructs/ConstDataHolder.hpp"
#include "../HID/HIDDevices.hpp"

class UnknownDeviceInterpreter
{
public:
	UnknownDeviceInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel,
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

#endif // !UNKNOWNDEVICEINTERPRETER_HPP
