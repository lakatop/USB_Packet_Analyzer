#ifndef INTERPRETERFACTORY_HPP
#define INTERPRETERFACTORY_HPP

#include "BaseInterpreter.hpp"
#include "UnknownDeviceInterpreter.hpp"
#include "ConfigDescriptorsInterpreter.hpp"
#include "DeviceDescriptorInterpreter.hpp"
#include "InterruptTransferInterpreter.hpp"
#include "JoystickInterpreter.hpp"
#include "KeyboardInterpreter.hpp"
#include "MouseInterpreter.hpp"
#include "ReportDescriptorInterpreter.hpp"
#include "SetupPacketInterpreter.hpp"
#include "StringDescsriptorInterpreter.hpp"

class InterpreterFactory
{
public:
	InterpreterFactory(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel,HeaderDataType dataType);
	BaseInterpreter* GetInterpreter();
private:
	HeaderDataType dataType;
	TreeItem* rootItem;
	QTableWidgetItem* item;
	AdditionalDataModel* additionalDataModel;
};

#endif // !INTERPRETERFACTORY_HPP
