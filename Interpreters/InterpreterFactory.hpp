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

/// <summary>
/// Factory class for interpreters
/// </summary>
class InterpreterFactory
{
public:
	InterpreterFactory(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel, const HeaderDataType dataType);
	BaseInterpreter* GetInterpreter();
private:
	/// <summary>
	/// Data type of packet data in <see cref = "item"/>
	/// </summary>
	HeaderDataType dataType;
	/// <summary>
	/// Root tree item of tree view.
	/// </summary>
	TreeItem* rootItem;
	/// <summary>
	/// Item which holds packet data
	/// </summary>
	QTableWidgetItem* item;
	/// <summary>
	/// Pointer to AdditionalDataModel instance for beign able to call 
	/// its specific methods like TreeItemBaseModel.ShowBits()
	/// or TreeItemBaseModel.CharToHexConvert()
	/// </summary>
	AdditionalDataModel* additionalDataModel;
};

#endif // !INTERPRETERFACTORY_HPP
