#ifndef KEYBOARDINTERPRETER_HPP
#define KEYBOARDINTERPRETER_HPP

#include <qfile>
#include <qtextstream.h>

#include "../DefinedStructs/ConstDataHolder.hpp"
#include "../HID/HIDDevices.hpp"
#include "BaseInterpreter.hpp"

/// <summary>
/// Interpreter for HID keyboard device.
/// </summary>
class KeyboardInterpreter : public BaseInterpreter
{
public:
	KeyboardInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel,
		const HIDReportDescriptorInputParse inputParser);
	void Interpret() override;
private:
	void InterpretModifierKey(TreeItem* child, const unsigned char* packet);
	void SetupUsages();

	/// <summary>
	/// Concrete HIDReportDescriptorInputParse struct to parse this input.
	/// </summary>
	HIDReportDescriptorInputParse inputParser;
	/// <summary>
	/// Pointer to DataHolder instance.
	/// </summary>
	DataHolder* holder;
	/// <summary>
	/// Pointer to HIDDevices instance.
	/// </summary>
	HIDDevices* hidDevices;
	/// <summary>
	/// Text representation of keyboard usages
	/// </summary>
	std::vector<QString> keyboardUsages;
};

#endif // !KEYBOARDINTERPRETER_HPP
