#ifndef UNKNOWNDEVICEINTERPRETER_HPP
#define UNKNOWNDEVICEINTERPRETER_HPP

#include "BaseInterpreter.hpp"
#include "../DefinedStructs/ConstDataHolder.hpp"
#include "../HID/HIDDevices.hpp"


class UnknownDeviceInterpreter : public BaseInterpreter
{
public:
	UnknownDeviceInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel,
		HIDReportDescriptorInputParse inputParser);
	void Interpret() override;
private:
	HIDReportDescriptorInputParse inputParser;
	HIDDevices* hidDevices;
	DataHolder* holder;
};

#endif // !UNKNOWNDEVICEINTERPRETER_HPP
