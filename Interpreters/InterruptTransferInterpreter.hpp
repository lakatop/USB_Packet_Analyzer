#ifndef INTERRUPTTRANSFERMODEL_HPP
#define INTERRUPTTRANSFERMODEL_HPP

#include "../Models/AdditionalDataModel.hpp"
#include "../Interpreters/MouseInterpreter.hpp"

class InterruptTransferInterpreter
{
public:
	InterruptTransferInterpreter(TreeItem* rootItem, QListWidgetItem* item, AdditionalDataModel* additionalDataModel);
	void InterpretInterruptTransfer();
private:
	HIDReportDescriptorInputParse GetInputParser(int index);

	TreeItem* rootItem;
	QListWidgetItem* item;
	AdditionalDataModel* additionalDataModel;
	HIDDevices* hidDevices;
	DataHolder* holder;
};

#endif // !INTERRUPTTRANSFERMODEL_HPP
