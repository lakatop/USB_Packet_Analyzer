#ifndef STRINGDESCRIPTORINTERPRETER_HPP
#define STRINGDESCRIPTORINTERPRETER_HPP

#include "BaseInterpreter.hpp"


class StringDescriptorInterpreter : public BaseInterpreter
{
public:
	StringDescriptorInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel);
	void Interpret() override;
private:
	DataHolder* holder;
};

#endif // !STRINGDESCRIPTORINTERPRETER_HPP
