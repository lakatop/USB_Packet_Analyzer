#include "UnknownDeviceInterpreter.hpp"

/// <summary>
/// Constructor of UnknownDeviceInterpreter
/// </summary>
/// <param name="rootItem"><see cref="BaseInterpreter.rootItem"/></param>
/// <param name="item"><see cref="BaseInterpreter.item"/></param>
/// <param name="additionalDataModel"><see cref="BaseInterpreter.additionalDataModel"/></param>
/// <param name="inputParser"><see cref="inputParser"/></param>
UnknownDeviceInterpreter::UnknownDeviceInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel,
	HIDReportDescriptorInputParse inputParser)
	:BaseInterpreter(rootItem, item, additionalDataModel)
{
	this->inputParser = inputParser;
	this->hidDevices = HIDDevices::GetHIDDevices();
	this->holder = DataHolder::GetDataHolder();
}

/// <summary>
/// Interprets unknown descriptor.
/// </summary>
void UnknownDeviceInterpreter::Interpret()
{
	QByteArray leftoverData = item->data(holder->TRANSFER_LEFTOVER_DATA).toByteArray();
	const unsigned char* packet = (unsigned char*)leftoverData.constData();

	rootItem->AppendChild(new TreeItem(QVector<QVariant>{"UNKNOWN DEVICE", "", ""}, rootItem));
	TreeItem* unknownDeviceChild = rootItem->Child(rootItem->ChildCount() - 1);

	QString hexData;

	for (int i = 0; i < inputParser.inputValues.size(); i++)
	{
		int size = ((std::size_t)inputParser.inputValues[i].ReportSize * (std::size_t)inputParser.inputValues[i].ReportCount) /
			(8 * inputParser.inputValues[i].LocalUsageNames.size());
		int64_t value = 0;
		hidDevices->CharToNumberConvert(packet, value, size);
		additionalDataModel->CharToHexConvert(&packet, size, hexData);
		for (int j = 0; j < inputParser.inputValues[i].LocalUsageNames.size(); j++)
		{
			unknownDeviceChild->AppendChild(new TreeItem(QVector<QVariant>
			{hexData, inputParser.inputValues[i].LocalUsageNames[j], value}, unknownDeviceChild));
		}
	}
}