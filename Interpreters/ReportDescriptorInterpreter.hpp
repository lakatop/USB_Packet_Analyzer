#ifndef REPORTDESCRIPTORMODEL_HPP
#define REPORTDESCRIPTORMODEL_HPP

#include "../DefinedStructs/PacketExternStructs.hpp"
#include "../Models/AdditionalDataModel.hpp"
#include <qtablewidget.h>

class AdditionalDataModel; //forward declaration

class ReportDescriptorInterpreter
{
public:
	ReportDescriptorInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel);
	void InterpretReportDescriptor();
private:
	std::size_t ParseReportDescriptor(ReportDescTreeStruct* root, std::size_t parsed);
	void InterpretReportDescriptor(TreeItem* parent, ReportDescTreeStruct* reportDescriptor);
	void InterpretInputOutputFeature(BYTE tag, QByteArray& data, TreeItem* child);
	void InterpretHeader(ReportDescTreeStruct* reportDescriptor, TreeItem* child);
	void AppendItem(TreeItem* parent, ReportDescTreeStruct* reportDescriptor);
	std::string InterpretData(ReportDescTreeStruct* reportDescriptor);

	BYTE globalUsage;
	TreeItem* rootItem;
	AdditionalDataModel* additionalDataModel;
	ReportDescTreeStruct reportDescriptor;
	DataHolder* holder;
	QByteArray reportArray;
};

#endif