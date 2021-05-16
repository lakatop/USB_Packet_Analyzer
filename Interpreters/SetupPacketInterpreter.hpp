#ifndef SETUPPACKETINTERPRETER_HPP
#define SETUPPACKETINTERPRETER_HPP

#include "BaseInterpreter.hpp"
#include <winusb.h>

/// <summary>
/// Interpreter class for Setup Packet Descriptor
/// </summary>
class SetupPacketInterpreter : public BaseInterpreter
{
public:
	SetupPacketInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel);
	void Interpret() override;
private:
	void AppendSetupwValue(const unsigned char** it, const PWINUSB_SETUP_PACKET setupPacket, TreeItem* setupChild);
	void AppendSetupwIndex(const unsigned char** it, const PWINUSB_SETUP_PACKET setupPacket, TreeItem* setupChild);
	void AppendSetupwLength(const unsigned char** it, const PWINUSB_SETUP_PACKET setupPacket, TreeItem* setupChild);

	/// <summary>
	/// Pointer to DataHolder instance.
	/// </summary>
	DataHolder* holder;
};

#endif // !SETUPPACKETINTERPRETER_HPP
