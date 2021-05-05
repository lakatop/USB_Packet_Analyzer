#ifndef REPORTDESCRIPTORMODEL_HPP
#define REPORTDESCRIPTORMODEL_HPP

#include "../DefinedStructs/PacketExternStructs.hpp"
#include "BaseInterpreter.hpp"

/// <summary>
/// Interpreter class for HID Report Descriptor
/// </summary>
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

	/// <summary>
	/// Holds Global Usage for Report Descriptor.
	/// </summary>
	BYTE globalUsage;
	/// <summary>
	/// Tree structure representing Report Descriptor in format so it can be easily interpreted
	/// </summary>
	ReportDescTreeStruct reportDescriptor;
	/// <summary>
	/// Pointer to DataHolder instance.
	/// </summary>
	DataHolder* holder;
	/// <summary>
	/// QByteArray holding data which represents Report Descriptor
	/// </summary>
	QByteArray reportArray;
};

#endif // !REPORTDESCRIPTORMODEL_HPP