#ifndef CONTROLTRANSFERINTERPRETER_HPP
#define CONTROLTRANSFERINTERPRETER_HPP

#include "BaseInterpreter.hpp"

#include <algorithm>

class ControlTransferInterpreter : public BaseInterpreter
{
public:
	ControlTransferInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel);

	void Interpret() override;
private:
	DescriptorStruct* GetDescriptorStruct(BYTE descriptorType);
	void InterpretUnknownDescriptor(const unsigned char* packet);

	DataHolder* holder;
};

#endif // !CONTROLTRANSFERINTERPRETER_HPP
