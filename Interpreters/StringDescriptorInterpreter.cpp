#include "StringDescsriptorInterpreter.hpp"

/// <summary>
/// Constructor of StringDescriptorInterpreter
/// </summary>
/// <param name="rootItem"><see cref="BaseInterpreter.rootItem"/></param>
/// <param name="item"><see cref="BaseInterpreter.item"/></param>
/// <param name="additionalDataModel"><see cref="BaseInterpreter.additionalDataModel"/></param>
StringDescriptorInterpreter::StringDescriptorInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel)
	:BaseInterpreter(rootItem, item, additionalDataModel)
{
	this->holder = DataHolder::GetDataHolder();
}

/// <summary>
/// Interprets String Descriptor.
/// </summary>
void StringDescriptorInterpreter::Interpret()
{
	QByteArray leftoverData = item->data(holder->TRANSFER_LEFTOVER_DATA).toByteArray();
	const unsigned char* packet = (unsigned char*)leftoverData.constData();
	PUSB_STRING_DESCRIPTOR stringDescriptor = (PUSB_STRING_DESCRIPTOR)packet;
	rootItem->AppendChild(new TreeItem(QVector<QVariant>{"STRING_DESCRIPTOR", "", ""}, rootItem));
	TreeItem* stringDescriptorChild = rootItem->Child(rootItem->ChildCount() - 1);

	QString hexData;
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	stringDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bLength", stringDescriptor->bLength}, stringDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	stringDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bDescriptorType (STRING_DESCRIPTOR)", stringDescriptor->bDescriptorType}, stringDescriptorChild));
	std::wstring wbString(stringDescriptor->bString);
	std::string bString(wbString.begin(), wbString.end());
	additionalDataModel->CharToHexConvert(&packet, leftoverData.size() - 2, hexData); //-2 for bLength and bDescriporType
	stringDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bString", bString.c_str()}, stringDescriptorChild));
}