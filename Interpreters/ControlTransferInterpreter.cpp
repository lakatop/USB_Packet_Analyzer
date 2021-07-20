#include "ControlTransferInterpreter.hpp"
#include "../DefinedStructs/DescriptorStruct.hpp"


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
	BYTE descriptorType = (BYTE) * (packet++);
	DescriptorStruct* descStruct = GetDescriptorStruct(descriptorType);
	if (descStruct == nullptr)
	{
		InterpretUnknownDescriptor((const unsigned char*)packet);
	}
	else
	{
		descStruct->InterpretData(rootItem, data, additionalDataModel);
	}
}

/// <summary>
/// Gets struct that represents given descriptor. If it doesnt exists yet, try to load it
/// </summary>
/// <param name="descriptorType">Type of descriptor we want to get struct of</param>
/// <returns></returns>
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

/// <summary>
/// Inteprets unknown descriptor
/// </summary>
/// <param name="packet">Pointer to descriptor data</param>
void ControlTransferInterpreter::InterpretUnknownDescriptor(const unsigned char* packet)
{
	rootItem->AppendChild(new TreeItem(QVector<QVariant>{"UNKNOWN_DESCRIPTOR", "", ""}, rootItem));
	TreeItem* unknownDescriptorChild = rootItem->Child(rootItem->ChildCount() - 1);
	BYTE descriptorSize = (*packet);

	QString hexData;
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	unknownDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bLength", descriptorSize}, unknownDescriptorChild));

	BYTE descriptorType = (*packet);
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	unknownDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bDescriptorType", descriptorType}, unknownDescriptorChild));

	additionalDataModel->CharToHexConvert(&packet, descriptorSize - 1, hexData); // -1 for descriptorType
	unknownDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "unspecified"}, unknownDescriptorChild));
}