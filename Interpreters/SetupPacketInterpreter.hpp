#ifndef SETUPPACKETINTERPRETER_HPP
#define SETUPPACKETINTERPRETER_HPP

#include "BaseInterpreter.hpp"
#include <winusb.h>

class SetupPacketInterpreter : public BaseInterpreter
{
public:
	SetupPacketInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel);
	void Interpret() override;
private:
	void AppendSetupwValue(const unsigned char** it, PWINUSB_SETUP_PACKET setupPacket, TreeItem* setupChild);
	void AppendSetupwIndex(const unsigned char** it, PWINUSB_SETUP_PACKET setupPacket, TreeItem* setupChild);
	void AppendSetupwLength(const unsigned char** it, PWINUSB_SETUP_PACKET setupPacket, TreeItem* setupChild);

	DataHolder* holder;
};

#endif // !SETUPPACKETINTERPRETER_HPP
