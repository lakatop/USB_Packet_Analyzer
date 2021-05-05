#ifndef STRINGDESCRIPTORINTERPRETER_HPP
#define STRINGDESCRIPTORINTERPRETER_HPP

#include "BaseInterpreter.hpp"

/// <summary>
/// Interpreter class for String Descriptor.
/// </summary>
class StringDescriptorInterpreter : public BaseInterpreter
{
public:
	StringDescriptorInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel);
	void Interpret() override;
private:
	/// <summary>
	/// Pointer to DataHolder instance.
	/// </summary>
	DataHolder* holder;
};

#endif // !STRINGDESCRIPTORINTERPRETER_HPP
