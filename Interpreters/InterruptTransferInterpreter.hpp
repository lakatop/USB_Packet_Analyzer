#ifndef INTERRUPTTRANSFERMODEL_HPP
#define INTERRUPTTRANSFERMODEL_HPP

#include "MouseInterpreter.hpp"
#include "KeyboardInterpreter.hpp"
#include "JoystickInterpreter.hpp"
#include "UnknownDeviceInterpreter.hpp"
#include "BaseInterpreter.hpp"

class InterruptTransferInterpreter : public BaseInterpreter
{
public:
	InterruptTransferInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel);
	void Interpret() override;
private:
	HIDReportDescriptorInputParse GetInputParser(int index);

	DataHolder* holder;
	HIDDevices* hidDevices;
};

#endif // !INTERRUPTTRANSFERMODEL_HPP
