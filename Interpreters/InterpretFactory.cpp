#include "InterpreterFactory.hpp"

InterpreterFactory::InterpreterFactory(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel,
    HeaderDataType dataType)
{
	this-> dataType = dataType;
    this->rootItem = rootItem;
    this->item = item;
    this->additionalDataModel = additionalDataModel;
}

BaseInterpreter* InterpreterFactory::GetInterpreter()
{
    switch (dataType)
    {
    case INTERR_TRANSFER:
    {
        return new InterruptTransferInterpreter(rootItem, item, additionalDataModel);
    }
    case IRP_INFO_TRANSFER:
    case CONTROL_TRANSFER_UNSPEC_DESC:
    case UNKNOWN_TRANSFER:
    case ISOCHRO_TRANSFER:
    case BULK_TRANSFER:
        return nullptr;
    case CONTROL_TRANSFER:
    case CONTROL_TRANSFER_CONFIG_DESC:
    case CONTROL_TRANSFER_INTERF_DESC:
    case CONTROL_TRANSFER_ENDPOI_DESC:
    case CONTROL_TRANSFER_HID_DESC:
    case CONTROL_TRANSFER_OTHER_SPEED_CONF_DESC:
    {
        return new ConfigDescriptorsInterpreter(rootItem, item, additionalDataModel);
    }
    case CONTROL_TRANSFER_DEVICE_DESC:
    {
        return new DeviceDescriptorInterpreter(rootItem, item, additionalDataModel);
    }
    break;
    case CONTROL_TRANSFER_STRING_DESC:
    {
        return new StringDescriptorInterpreter(rootItem, item, additionalDataModel);
    }
    break;
    case CONTROL_TRANSFER_HID_REPORT_DESC:
    {
        return new ReportDescriptorInterpreter(rootItem, item, additionalDataModel);
    }
    break;
    case CONTROL_TRANSFER_SETUP:
    {
        return new SetupPacketInterpreter(rootItem, item, additionalDataModel);
    }
    break;
    case CONTROL_TRANSFER_DEVICE_QUALIFIER_DESC:
    {
        return new DeviceQualifierDescriptorInterpreter(rootItem, item, additionalDataModel);
    }
    default:
        return nullptr;
    }
}