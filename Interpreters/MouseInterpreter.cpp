#include "MouseInterpreter.hpp"

/// <summary>
/// Constructor for MouseInterpreter.
/// </summary>
/// <param name="rootItem"><see cref="BaseInterpreter.rootItem"/></param>
/// <param name="item"><see cref="BaseInterpreter.item"/></param>
/// <param name="additionalDataModel"><see cref="BaseInterpreter.additionalDataModel"/></param>
/// <param name="inputParser"><see cref="inputParser"/></param>
MouseInterpreter::MouseInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel,
    const HIDReportDescriptorInputParse inputParser)
    :BaseInterpreter(rootItem, item, additionalDataModel)
{
	this->inputParser = inputParser;
    this->holder = DataHolder::GetDataHolder();
    this->hidDevices = HIDDevices::GetHIDDevices();
}

/// <summary>
/// Interprets concrete input for HID joystick device
/// </summary>
void MouseInterpreter::Interpret()
{
    QByteArray leftoverData = item->data(holder->TRANSFER_LEFTOVER_DATA).toByteArray();
    if (leftoverData.isEmpty())
    {
        return;
    }
    const unsigned char* packet = (unsigned char*)leftoverData.data();

    rootItem->AppendChild(new TreeItem(QVector<QVariant>{"MOUSE DEVICE", "", ""}, rootItem));
    TreeItem* mouseDeviceChild = rootItem->Child(rootItem->ChildCount() - 1);

    QString hexData;
    int buttonCounter = 0;

    if (inputParser.reportDefined)
    {
        additionalDataModel->CharToHexConvert(&packet, 1, hexData);
        mouseDeviceChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "Report ID", inputParser.reportID}, mouseDeviceChild));
    }

    for (int i = 0; i < inputParser.inputValues.size(); i++)
    {
        if (inputParser.inputValues[i].LocalUsageNames.size() == 1)    //only one usage, should be some button
        {
            if (inputParser.inputValues[i].LocalUsageNames[0] == BUTTON_PAGE)   //normal button on mouse
            {
                mouseDeviceChild->AppendChild(new TreeItem(QVector<QVariant>{"BUTTONS", "", ""}, mouseDeviceChild));
                TreeItem* buttonsChild = mouseDeviceChild->Child(mouseDeviceChild->ChildCount() - 1);
                BYTE buttonValue = *packet;
                if (inputParser.inputValues[i].Variable == true)    //touchable button
                {
                    for (int j = 0; j < inputParser.inputValues[i].ReportCount; j++)   //print button status
                    {
                        if ((buttonCounter + 1) % 8 == 0) //if true, then this is last bit of 1B button sector, let packet go +1
                        {
                            buttonsChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(7 - (j % 8), 1, buttonValue), std::string("Button " + std::to_string(buttonCounter)).c_str(), buttonValue}, buttonsChild));
                            packet += 1;
                        }
                        else   //else dont move packet
                        {
                            buttonsChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(7 - (j % 8), 1, buttonValue), std::string("Button " + std::to_string(buttonCounter)).c_str(), buttonValue}, buttonsChild));
                        }
                        buttonCounter++;
                    }
                }
                else //array of buttons -> padding
                {
                    //size of padding in bits
                    int size = inputParser.inputValues[i].ReportSize * inputParser.inputValues[i].ReportCount;
                    if (size < 8)  //padding is usually shorted than 1B, but i still need to move to the next byte
                    {
                        buttonsChild->AppendChild(new TreeItem(QVector<QVariant>{
                            additionalDataModel->ShowBits(0, 8 - (buttonCounter % 8), *packet), std::string("Button padding").c_str()}, buttonsChild));
                        packet += 1;
                    }
                    else
                    {
                        //append button padding item to round a byte
                        buttonsChild->AppendChild(new TreeItem(QVector<QVariant>{
                            additionalDataModel->ShowBits(buttonCounter % 8, 8 - (buttonCounter % 8), buttonValue), std::string("Button padding").c_str()}, buttonsChild));
                        
                        //starting from new byte, get size of padding in bits
                        size = size - (8 - (buttonCounter % 8));
                        //size of padding in bytes
                        size /= 8;
                        for (int i = 0; i < size; i++)
                        {
                            buttonValue = *packet;
                            packet++;
                            buttonsChild->AppendChild(new TreeItem(QVector<QVariant>{
                                additionalDataModel->ShowBits(0, 8, buttonValue), std::string("Button padding").c_str(), buttonValue}, buttonsChild));
                        }
                    }
                }
            }
            else if (inputParser.inputValues[i].LocalUsageNames[0] == WHEEL)    //mouse wheel
            {
                mouseDeviceChild->AppendChild(new TreeItem(QVector<QVariant>{"WHEEL", "", ""}, mouseDeviceChild));
                TreeItem* wheelChild = mouseDeviceChild->Child(mouseDeviceChild->ChildCount() - 1);
                
                //wheels have usually 1B input report size
                int size = (inputParser.inputValues[i].ReportSize * inputParser.inputValues[i].ReportCount) / 8;   
                int value = 0;
                hidDevices->CharToNumberConvert(packet, value, size);
                additionalDataModel->CharToHexConvert(&packet, size, hexData);
                wheelChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "Wheel", value}, wheelChild));
            }
            else  //something not very common
            {
                int size = (inputParser.inputValues[i].ReportSize * inputParser.inputValues[i].ReportCount) / 8;
                int value = 0;
                hidDevices->CharToNumberConvert(packet, value, size);
                additionalDataModel->CharToHexConvert(&packet, size, hexData);
                mouseDeviceChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, std::to_string(inputParser.inputValues[i].LocalUsageNames[0]).c_str(), value}, mouseDeviceChild));
            }
        }
        else  if (inputParser.inputValues[i].LocalUsageNames.size() == 2) // 2 usages, probably -> X and Y axes
        {
            if ((inputParser.inputValues[i].LocalUsageNames[0] == X &&
                inputParser.inputValues[i].LocalUsageNames[1] == Y)
                ||
                (inputParser.inputValues[i].LocalUsageNames[0] == Y &&
                    inputParser.inputValues[i].LocalUsageNames[1] == X))     //X and Y axes 
            {
                int size = (inputParser.inputValues[i].ReportSize * inputParser.inputValues[i].ReportCount) / 16;
                int value = 0;
                mouseDeviceChild->AppendChild(new TreeItem(QVector<QVariant>{"AXIS", "" ,""}, mouseDeviceChild));
                TreeItem* axisChild = mouseDeviceChild->Child(mouseDeviceChild->ChildCount() - 1);
                
                QString name = (inputParser.inputValues[i].LocalUsageNames[0] == X) ? "X" : "Y";
                hidDevices->CharToNumberConvert(packet, value, size);
                additionalDataModel->CharToHexConvert(&packet, size, hexData);
                axisChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, name, value}, axisChild));
                value = 0;
                hidDevices->CharToNumberConvert(packet, value, size);
                additionalDataModel->CharToHexConvert(&packet, size, hexData);
                name = (name == "X") ? "Y" : "X";
                axisChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, name, value}, axisChild));
            }
            else  //something not very common
            {
                int size = (inputParser.inputValues[i].ReportSize * inputParser.inputValues[i].ReportCount) / 16;
                for (int j = 0; j < 2; j++)
                {
                    int64_t value = 0;
                    hidDevices->CharToNumberConvert(packet, value, size);
                    additionalDataModel->CharToHexConvert(&packet, size, hexData);
                    mouseDeviceChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, std::to_string(inputParser.inputValues[i].LocalUsageNames[j]).c_str(), value}, mouseDeviceChild));
                }
            }
        }
        else  //something not very common
        {
            int size = (inputParser.inputValues[i].ReportSize * inputParser.inputValues[i].ReportCount) /
                (8 * inputParser.inputValues[i].LocalUsageNames.size());

            for (int j = 0; j < inputParser.inputValues[i].LocalUsageNames.size(); j++)
            {
                int value = 0;
                hidDevices->CharToNumberConvert(packet, value, size);
                additionalDataModel->CharToHexConvert(&packet, size, hexData);
                mouseDeviceChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, std::to_string(inputParser.inputValues[i].LocalUsageNames[j]).c_str(), value}, mouseDeviceChild));
            }
        }
    }
}