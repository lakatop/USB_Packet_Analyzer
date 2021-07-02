#ifndef CONTROLTRANSFERINTERPRETER_HPP
#define CONTROLTRANSFERINTERPRETER_HPP

#include "BaseInterpreter.hpp"

class ControlTransferInterpreter : public BaseInterpreter
{
public:
	ControlTransferInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel);

	void Interpret() override;
private:
	DataHolder* holder;
};

#endif // !CONTROLTRANSFERINTERPRETER_HPP
