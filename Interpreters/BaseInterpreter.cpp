#include "BaseInterpreter.hpp"

BaseInterpreter::BaseInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel)
{
	this->rootItem = rootItem;
	this->item = item;
	this->additionalDataModel = additionalDataModel;
}