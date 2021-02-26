#ifndef INTERRUPTTRANSFERMODEL_HPP
#define INTERRUPTTRANSFERMODEL_HPP

#include "AdditionalDataModel.hpp"
#include "../Interpreters/MouseInterpreter.hpp"

class InterruptTransferModel
{
public:
	InterruptTransferModel(TreeItem* rootItem, QListWidgetItem* item, AdditionalDataModel* additionalDataModel);
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
