#ifndef REPORTDESCRIPTORMODEL_HPP
#define REPORTDESCRIPTORMODEL_HPP

#include "../DefinedStructs/PacketExternStructs.hpp"
#include "BaseInterpreter.hpp"

class ReportDescriptorInterpreter : public BaseInterpreter
{
public:
	ReportDescriptorInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel);
	void Interpret() override;
private:
	std::size_t ParseReportDescriptor(ReportDescTreeStruct* root, std::size_t parsed);
	void InterpretReportDescriptor(TreeItem* parent, ReportDescTreeStruct* reportDescriptor);
	void InterpretInputOutputFeature(BYTE tag, QByteArray& data, TreeItem* child);
	void InterpretHeader(ReportDescTreeStruct* reportDescriptor, TreeItem* child);
	void AppendItem(TreeItem* parent, ReportDescTreeStruct* reportDescriptor);
	std::string InterpretData(ReportDescTreeStruct* reportDescriptor);

	BYTE globalUsage;
	ReportDescTreeStruct reportDescriptor;
	DataHolder* holder;
	QByteArray reportArray;
};

#endif // !REPORTDESCRIPTORMODEL_HPP