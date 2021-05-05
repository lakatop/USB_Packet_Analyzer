#include "BaseInterpreter.hpp"

/// <summary>
/// Constructor of BaseInterpreter class.
/// </summary>
/// <param name="rootItem"><see cref="rootItem"/></param>
/// <param name="item"><see cref="item"/></param>
/// <param name="additionalDataModel"><see cref="additionalDataModel"/></param>
BaseInterpreter::BaseInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel)
{
	this->rootItem = rootItem;
	this->item = item;
	this->additionalDataModel = additionalDataModel;
}