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
	const char* packet = data.constData();
	BYTE descriptorSize = (BYTE)*packet;
	BYTE descriptorType = (BYTE) * (packet++);
	DescriptorStruct* descStruct = GetDescriptorStruct(descriptorType);
}

DescriptorStruct* ControlTransferInterpreter::GetDescriptorStruct(BYTE descriptorType)
{
	auto predicate = [descriptorType](std::unique_ptr<DescriptorStruct>& desc) {return desc->descriptorType == descriptorType; };
	auto descStructIterator = std::find_if(holder->descriptors.begin(), holder->descriptors.end(), predicate);
	if (descStructIterator == holder->descriptors.end())
	{
		DescriptorStruct* descStruct = holder->TryLoadNewDescriptor(descriptorType);
		return descStruct;
	}
	else
	{
		return descStructIterator->get();
	}
}