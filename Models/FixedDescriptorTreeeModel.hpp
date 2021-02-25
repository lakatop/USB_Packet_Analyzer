#ifndef FIXEDDESCRIPTORTREEINTERPRETER_HPP
#define FIXEDDESCRIPTORTREEINTERPRETER_HPP

#include "AdditionalDataModel.hpp"
#include <qlistwidget.h>
#include <winusb.h>

class AdditionalDataModel; //forward declaration

class FixedDescriptorTreeModel
{
public:
	FixedDescriptorTreeModel(TreeItem* rootItem, QListWidgetItem* item, AdditionalDataModel* additionalDataModel);
	void InterpretControlTransferUnspecifiedDescriptor();
	void InterpretControlTransferSetup();
	template <typename T>
	void InterpretControlTransferDeviceDescriptorBase(T* deviceDescriptor, const unsigned char** packet, TreeItem* descriptorChild);
	void InterpretControlTransferDeviceDescriptor();
	void InterpretControlTransferStringDescriptor();
	void InterpretControlTransferDeviceQualifierDescriptor();
private:
	void AppendSetupwValue(const unsigned char** it, PWINUSB_SETUP_PACKET setupPacket, TreeItem* setupChild);
	void AppendSetupwIndex(const unsigned char** it, PWINUSB_SETUP_PACKET setupPacket, TreeItem* setupChild);
	void AppendSetupwLength(const unsigned char** it, PWINUSB_SETUP_PACKET setupPacket, TreeItem* setupChild);

	TreeItem* rootItem;
	QListWidgetItem* item;
	AdditionalDataModel* additionalDataModel;
	DataHolder* holder;
};

#endif
