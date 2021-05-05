#ifndef BASEINTERPRETER_HPP
#define BASEINTERPRETER_HPP

#include "../Models/AdditionalDataModel.hpp"
#include <qtablewidget.h>

/// <summary>
/// Base interpreter class.
/// </summary>
class BaseInterpreter
{
public:
	BaseInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel);
	/// <summary>
	/// Virtual function that every interpreter must implement. It is used to specific interpretation of packet data for given context.
	/// </summary>
	virtual void Interpret() = 0;
protected:
	/// <summary>
	/// Root tree item of tree view.
	/// </summary>
	TreeItem* rootItem;
	/// <summary>
	/// Item which holds packet data.
	/// </summary>
	QTableWidgetItem* item;
	/// <summary>
	/// Pointer to AdditionalDataModel instance for beign able to call 
	/// its specific methods like TreeItemBaseModel.ShowBits()
	/// or TreeItemBaseModel.CharToHexConvert()
	/// </summary>
	AdditionalDataModel* additionalDataModel;
};

#endif // !BASEINTERPRETER_HPP
