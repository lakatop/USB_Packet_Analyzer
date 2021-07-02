#include "ControlTransferInterpreter.hpp"

ControlTransferInterpreter::ControlTransferInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel) :
	BaseInterpreter(rootItem, item, additionalDataModel)
{
	this->holder = DataHolder::GetDataHolder();
}

/// <summary>
/// Iterate through data and interpret known descriptors
/// </summary>
void ControlTransferInterpreter::Interpret()
{
	QByteArray data = item->data(holder->TRANSFER_LEFTOVER_DATA).toByteArray();
}