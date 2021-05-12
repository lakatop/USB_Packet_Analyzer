#include "KeyboardInterpreter.hpp"

/// <summary>
/// Constructor for KeyboardInterpreter.
/// </summary>
/// <param name="rootItem"><see cref="BaseInterpreter.rootItem"/></param>
/// <param name="item"><see cref="BaseInterpreter.item"/></param>
/// <param name="additionalDataModel"><see cref="BaseInterpreter.additionalDataModel"/></param>
/// <param name="inputParser"><see cref="inputParser"/></param>
KeyboardInterpreter::KeyboardInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel,
    HIDReportDescriptorInputParse inputParser)
    :BaseInterpreter(rootItem, item, additionalDataModel)
{
	this->inputParser = inputParser;
    this->holder = DataHolder::GetDataHolder();
    this->hidDevices = HIDDevices::GetHIDDevices();
    SetupUsages();
}

/// <summary>
/// Interprets concrete input for HID keyboard device
/// </summary>
void KeyboardInterpreter::Interpret()
{
    QByteArray leftoverData = item->data(holder->TRANSFER_LEFTOVER_DATA).toByteArray();
    const unsigned char* packet = (unsigned char*)leftoverData.constData();

    rootItem->AppendChild(new TreeItem(QVector<QVariant>{"KEYBOARD DEVICE", "", ""}, rootItem));
    TreeItem* keyboardDeviceChild = rootItem->Child(rootItem->ChildCount() - 1);

    QString hexData;

    if (inputParser.inputValues.size() == 0)
    {
        return;
    }
    int interpreted = 0;
    
    //first byte should be modifier_key_status
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
        keyboardDeviceChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, inputParser.inputValues[0].LocalUsageNames[0], value}, keyboardDeviceChild));
    }

    interpreted += size;

    //check if input consists only of 1B (which is very unlikely)
    if (inputParser.inputValues.size() == 1)
    {
        return;
    }

    //now for the rest of the input
    bool isReserved = inputParser.inputValues[1].Constant;
    if (isReserved) //most keyboards will have second byte reserved
    {
        size = (inputParser.inputValues[1].ReportCount * inputParser.inputValues[1].ReportSize) / 8;
        int64_t value = 0;
        hidDevices->CharToNumberConvert(packet, value, size);
        additionalDataModel->CharToHexConvert(&packet, size, hexData);
        keyboardDeviceChild->AppendChild(new TreeItem(QVector<QVariant>{"RESERVED", "", ""}, keyboardDeviceChild));
        TreeItem* reservedChild = keyboardDeviceChild->Child(keyboardDeviceChild->ChildCount() - 1);
        reservedChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "Reserved", value}, reservedChild));
        interpreted += size;
    }

    //keyboard should either consists of byte-defined keyscans or by bit-defined map
    bool bitDefined = true;
    for (int i = (isReserved) ? 2 : 1; i < inputParser.inputValues.size(); i++)
    {
        //if every input item has ReportSize == 1, it should be bit-defined
        if (inputParser.inputValues[i].ReportSize != 1)
        {
            bitDefined = false;
        }
    }

    if (bitDefined)
    {
        size = inputParser.inputSize - interpreted + 1;
        for (int i = 0; i < size; i += 8)
        {
            byte value = 0;
            hidDevices->CharToNumberConvert(packet, value, 1);
            additionalDataModel->CharToHexConvert(&packet, 1, hexData);
            for (int j = 0; j < 8; j++)
            {
                keyboardDeviceChild->AppendChild(new TreeItem(QVector<QVariant>
                {additionalDataModel->ShowBits(j, 1, value), keyboardUsages[i + j], value}, keyboardDeviceChild));
            }
        }
    }
    else
    {
        uint32_t numberOfKeyScans = (isReserved) ? inputParser.inputValues[2].ReportCount :
            inputParser.inputValues[1].ReportCount;

        keyboardDeviceChild->AppendChild(new TreeItem(QVector<QVariant>{"KEYS", "", ""}, keyboardDeviceChild));
        TreeItem* keysChild = keyboardDeviceChild->Child(keyboardDeviceChild->ChildCount() - 1);

        for (int j = 0; j < numberOfKeyScans; j++)
        {
            int size = inputParser.inputValues[2].ReportSize / 8;
            int value = 0;
            hidDevices->CharToNumberConvert(packet, value, size);
            additionalDataModel->CharToHexConvert(&packet, size, hexData);
            keysChild->AppendChild(new TreeItem(QVector<QVariant>{
                hexData, (std::string("Key ").c_str() + (value<232) ? keyboardUsages[value] : "reserved"), value}, keysChild));
        }
    }
}

/// <summary>
/// Interprets keyboards modifier_key_status
/// </summary>
/// <param name="child">Tree item to which we will append interpreted data</param>
/// <param name="packet">Pointer to modifier_key_status data in packet data</param>
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

void KeyboardInterpreter::SetupUsages()
{
    QFile file("./DefinedStructs/KeyboardUsages.txt");
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        while (!stream.atEnd())
        {
            keyboardUsages.push_back(stream.readLine());
        }
        file.close();
    }
    else
    {
        auto v = file.errorString();
    }
}