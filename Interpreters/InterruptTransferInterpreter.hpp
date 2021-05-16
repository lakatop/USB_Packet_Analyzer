#ifndef INTERRUPTTRANSFERMODEL_HPP
#define INTERRUPTTRANSFERMODEL_HPP

#include "MouseInterpreter.hpp"
#include "KeyboardInterpreter.hpp"
#include "JoystickInterpreter.hpp"
#include "UnknownDeviceInterpreter.hpp"
#include "BaseInterpreter.hpp"

/// <summary>
/// Intepreter class for interrupt transfer
/// </summary>
class InterruptTransferInterpreter : public BaseInterpreter
{
public:
	InterruptTransferInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel);
	void Interpret() override;
private:
	HIDReportDescriptorInputParse GetInputParser(const int index);
	/// <summary>
	/// Pointer to DataHolder instance.
	/// </summary>
	DataHolder* holder;
	/// <summary>
	/// Pointer to HIDDevices instance.
	/// </summary>
	HIDDevices* hidDevices;
};

#endif // !INTERRUPTTRANSFERMODEL_HPP
