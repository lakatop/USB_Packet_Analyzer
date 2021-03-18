#ifndef BASEINTERPRETER_HPP
#define BASEINTERPRETER_HPP

#include "../Models/AdditionalDataModel.hpp"
#include <qtablewidget.h>

class BaseInterpreter
{
public:
	BaseInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel);
	virtual void Interpret() = 0;
protected:
	TreeItem* rootItem;
	QTableWidgetItem* item;
	AdditionalDataModel* additionalDataModel;
};

#endif // !BASEINTERPRETER_HPP
