#ifndef INTERRUPTTRANSFERMODEL_HPP
#define INTERRUPTTRANSFERMODEL_HPP

#include "../Models/AdditionalDataModel.hpp"
#include "MouseInterpreter.hpp"
#include "KeyboardInterpreter.hpp"
#include "JoystickInterpreter.hpp"
#include "UnknownDeviceInterpreter.hpp"

class InterruptTransferInterpreter
{
public:
	InterruptTransferInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel);
	void InterpretInterruptTransfer();
private:
	HIDReportDescriptorInputParse GetInputParser(int index);

	TreeItem* rootItem;
	QTableWidgetItem* item;
	AdditionalDataModel* additionalDataModel;
	HIDDevices* hidDevices;
	DataHolder* holder;
};

#endif // !INTERRUPTTRANSFERMODEL_HPP
