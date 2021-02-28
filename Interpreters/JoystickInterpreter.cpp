#include "JoystickInterpreter.hpp"

JoystickInterpreter::JoystickInterpreter(TreeItem* rootItem, QListWidgetItem* item, AdditionalDataModel* additionalDataModel,
	HIDReportDescriptorInputParse inputParser)
{
	this->rootItem = rootItem;
	this->item = item;
	this->inputParser = inputParser;
	this->additionalDataModel = additionalDataModel;
	this->hidDevices = HIDDevices::GetHIDDevices();
	this->holder = DataHolder::GetDataHolder();
}

void JoystickInterpreter::Interpret()
{
    QByteArray leftoverData = item->data(holder->TRANSFER_LEFTOVER_DATA).toByteArray();
    const unsigned char* packet = (const unsigned char*)leftoverData.constData();

    QString hexData;

    rootItem->AppendChild(new TreeItem(QVector<QVariant>{"JOYSTICK DEVICE", "", ""}, rootItem));
    TreeItem* joystickDeviceChild = rootItem->Child(rootItem->ChildCount() - 1);

    int hatswitch_size = 0;

    for (int i = 0; i < inputParser.inputValues.size(); i++)
    {
        if (inputParser.inputValues[i].LocalUsageNames.size() == 4)    //4 usage names, probably axes X,Y,Z,Rx
        {
            int checkCounter = 0;
            for (auto&& name : inputParser.inputValues[i].LocalUsageNames) //check if it really are axes
            {
                if ((name == X) || (name == Y) || (name == Z) || (name == RZ))
                {
                    checkCounter++;
                }
            }

            TreeItem* axisChild = nullptr;
            if (checkCounter == 4)
            {
                joystickDeviceChild->AppendChild(new TreeItem(QVector<QVariant>{"AXIS", "", ""}, joystickDeviceChild));
                axisChild = joystickDeviceChild->Child(joystickDeviceChild->ChildCount() - 1);
            }

            uint32_t size = inputParser.inputValues[i].ReportSize / 8;
            for (int j = 0; j < inputParser.inputValues[i].LocalUsageNames.size(); j++)
            {
                int value = 0;
                uint32_t name = inputParser.inputValues[i].LocalUsageNames[j];
                hidDevices->CharToNumberConvert(packet, value, size);
                additionalDataModel->CharToHexConvert(&packet, size, hexData);
                if (checkCounter == 4)
                {
                    QString axisName = (name == X) ? "X" :
                                           (name == Y) ? "Y" :
                                           (name == Z) ? "Z" : "Rz";
                    axisChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, axisName, value}, axisChild));
                }
                else
                {
                    joystickDeviceChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, std::to_string(name).c_str(), value}, joystickDeviceChild));
                }
            }
        }
        else if (inputParser.inputValues[i].LocalUsageNames.size() == 1)
        {
            if (inputParser.inputValues[i].LocalUsageNames[0] == HAT_SWITCH)
            {
                uint32_t size = inputParser.inputValues[i].ReportSize / 8; //this should be 0, as HAT_SWITCH is usually 4bits
                hatswitch_size += inputParser.inputValues[i].ReportSize;

                BYTE value = *packet;

                joystickDeviceChild->AppendChild(new TreeItem(QVector<QVariant>{"HAT_SWITCH", "", ""}, joystickDeviceChild));
                TreeItem* hatswitchChild = joystickDeviceChild->Child(joystickDeviceChild->ChildCount() - 1);
                if (size == 0)
                {
                    hatswitchChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(8 - hatswitch_size, hatswitch_size, value),
                        "hat_switch"}, hatswitchChild));
                }
                else
                {
                    additionalDataModel->CharToHexConvert(&packet, size, hexData);
                    hatswitchChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "Hat_switch", value}, hatswitchChild));
                }
            }
            else if (inputParser.inputValues[i].LocalUsageNames[0] == 0x09) //buttons, usually right after hat_switch
            {
                int buttonCounter = 0;
                int buttonNumber = 0;

                if ((hatswitch_size / 8) == 0)  //if hatswitch_size / 8 < 0, then first button bit is starting right after last hat_switch bit, not on position 0
                {
                    buttonCounter += hatswitch_size;
                }

                joystickDeviceChild->AppendChild(new TreeItem(QVector<QVariant>{"BUTTONS", "", ""}, joystickDeviceChild));
                TreeItem* buttonsChild = joystickDeviceChild->Child(joystickDeviceChild->ChildCount() - 1);

                for (int j = 0; j < inputParser.inputValues[i].ReportCount; j++)   //print button status
                {
                    BYTE value = *packet;
                    buttonsChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(8 - (buttonCounter % 8) - 1, 1, value), 
                                                                            ("Button " + std::to_string(buttonNumber)).c_str()}, buttonsChild));
                    if ((buttonCounter + 1) % 8 == 0) //if true, then this is last bit of 1B button sector, let packet go +1
                    {
                        packet += 1;
                    }
                    buttonCounter++;
                    buttonNumber++;
                }
            }
            else  //probably something vendor defined
            {
                uint32_t size = (inputParser.inputValues[i].ReportSize * inputParser.inputValues[i].ReportCount) /
                    (8 * inputParser.inputValues[i].LocalUsageNames.size());
                int value = 0;
                uint32_t name = inputParser.inputValues[i].LocalUsageNames[0];
                hidDevices->CharToNumberConvert(packet, value, size);
                additionalDataModel->CharToHexConvert(&packet, size, hexData);
                joystickDeviceChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, std::to_string(name).c_str(), value}, joystickDeviceChild));
            }
        }
        else //something unusual
        {
            uint32_t size = (inputParser.inputValues[i].ReportSize * inputParser.inputValues[i].ReportCount) /
                (8 * inputParser.inputValues[i].LocalUsageNames.size());
            for (int j = 0; j < inputParser.inputValues[i].LocalUsageNames.size(); j++)
            {
                int value = 0;
                uint32_t name = inputParser.inputValues[i].LocalUsageNames[0];
                hidDevices->CharToNumberConvert(packet, value, size);
                additionalDataModel->CharToHexConvert(&packet, size, hexData);
                joystickDeviceChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, std::to_string(name).c_str(), value}, joystickDeviceChild));
            }
        }
    }
}