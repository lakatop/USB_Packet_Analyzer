#include "KeyboardInterpreter.hpp"

KeyboardInterpreter::KeyboardInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel,
	HIDReportDescriptorInputParse inputParser)
{
	this->rootItem = rootItem;
	this->item = item;
	this->inputParser = inputParser;
	this->additionalDataModel = additionalDataModel;
	this->hidDevices = HIDDevices::GetHIDDevices();
	this->holder = DataHolder::GetDataHolder();
}

void KeyboardInterpreter::Interpret()
{
    QByteArray leftoverData = item->data(holder->TRANSFER_LEFTOVER_DATA).toByteArray();
    const unsigned char* packet = (unsigned char*)leftoverData.constData();

    rootItem->AppendChild(new TreeItem(QVector<QVariant>{"KEYBOARD DEVICE", "", ""}, rootItem));
    TreeItem* keyboardDeviceChild = rootItem->Child(rootItem->ChildCount() - 1);

    QString hexData;

    if (inputParser.inputValues.size() == 3)    //usually keyboards have 3 (modifier_key_status, reserved and keyscan codes)
    {
        int size = (inputParser.inputValues[0].ReportCount * inputParser.inputValues[0].ReportSize) / 8;

        if (size == 1)
        {
            keyboardDeviceChild->AppendChild(new TreeItem(QVector<QVariant>{"MODIFIER KEY STATUS", "", ""}, keyboardDeviceChild));
            InterpretModifierKey(keyboardDeviceChild->Child(keyboardDeviceChild->ChildCount() - 1), packet);
            packet += 1;
        }
        else
        {
            int64_t value = 0;
            hidDevices->CharToNumberConvert(packet, value, size);
            additionalDataModel->CharToHexConvert(&packet, size, hexData);
            keyboardDeviceChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, inputParser.inputValues[0].LocalUsageNames[0] , value}, keyboardDeviceChild));
        }


        size = (inputParser.inputValues[1].ReportCount * inputParser.inputValues[1].ReportSize) / 8;  //from USB HID specification, this field should be reserved
        int64_t value = 0;
        hidDevices->CharToNumberConvert(packet, value, size);
        additionalDataModel->CharToHexConvert(&packet, size, hexData);
        keyboardDeviceChild->AppendChild(new TreeItem(QVector<QVariant>{"RESERVED", "", ""}, keyboardDeviceChild));
        TreeItem* reservedChild = keyboardDeviceChild->Child(keyboardDeviceChild->ChildCount() - 1);
        reservedChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "Reserved", value}, reservedChild));
        packet += size;

        uint32_t numberOfKeyScans = ((std::size_t)inputParser.inputValues[2].ReportCount * (std::size_t)inputParser.inputValues[2].ReportSize) /
            (8 * inputParser.inputValues[2].LocalUsageNames.size());
        keyboardDeviceChild->AppendChild(new TreeItem(QVector<QVariant>{"KEYS", "", ""}, keyboardDeviceChild));
        TreeItem* keysChild = keyboardDeviceChild->Child(keyboardDeviceChild->ChildCount() - 1);
        for (int j = 0; j < numberOfKeyScans; j++)
        {
            int size = inputParser.inputValues[2].ReportSize / 8;
            int value = 0;
            hidDevices->CharToNumberConvert(packet, value, size);
            additionalDataModel->CharToHexConvert(&packet, size, hexData);
            keysChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, (std::string("Key ") + std::to_string(j)).c_str(), value}, keysChild));
        }
    }
    else  //this is not usual, 
    {
        for (int i = 0; i < inputParser.inputValues.size(); i++)
        {
            int size = ((std::size_t)inputParser.inputValues[i].ReportSize * (std::size_t)inputParser.inputValues[i].ReportCount) /
                (8 * inputParser.inputValues[i].LocalUsageNames.size());
            int64_t value = 0;
            hidDevices->CharToNumberConvert(packet, value, size);
            additionalDataModel->CharToHexConvert(&packet, size, hexData);
            for (int j = 0; j < inputParser.inputValues[i].LocalUsageNames.size(); j++)
            {
                keyboardDeviceChild->AppendChild(new TreeItem(QVector<QVariant>
                {hexData, inputParser.inputValues[i].LocalUsageNames[j], value}, keyboardDeviceChild));
            }
        }
    }
}

void KeyboardInterpreter::InterpretModifierKey(TreeItem* child, const unsigned char* packet)
{
    BYTE modifierStatus = 0;
    hidDevices->CharToNumberConvert(packet, modifierStatus, 1);
    QString status0 = QString("Left Ctrl %1").arg(modifierStatus & 0x01);
    QString status1 = QString("Left Shift %1").arg(modifierStatus & (0x01 << 1));
    QString status2 = QString("Left Alt %1").arg(modifierStatus & (0x01 << 2));
    QString status3 = QString("Left GUI(Windows/Super key) %1").arg(modifierStatus & (0x01 << 3));
    QString status4 = QString("Roght Ctrl %1").arg(modifierStatus & (0x01 << 4));
    QString status5 = QString("Right Shift %1").arg(modifierStatus & (0x01 << 5));
    QString status6 = QString("Right Alt %1").arg(modifierStatus & (0x01 << 6));
    QString status7 = QString("Right GUI(Windows/Super key) %1").arg(modifierStatus & (0x01 << 7));

    child->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(0, 1, modifierStatus), status0}, child));
    child->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(1, 1, modifierStatus), status1}, child));
    child->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(2, 1, modifierStatus), status2}, child));
    child->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(3, 1, modifierStatus), status3}, child));
    child->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(4, 1, modifierStatus), status4}, child));
    child->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(5, 1, modifierStatus), status5}, child));
    child->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(6, 1, modifierStatus), status6}, child));
    child->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(7, 1, modifierStatus), status7}, child));
}