#include "ReportDescriptorInterpreter.hpp"

/// <summary>
/// Constructor for ReportDescriptorInterpreter
/// </summary>
/// <param name="rootItem"><see cref="BaseInterpreter.rootItem"/></param>
/// <param name="item"><see cref="BaseInterpreter.item"/></param>
/// <param name="additionalDataModel"><see cref="BaseInterpreter.additionalDataModel"/></param>
ReportDescriptorInterpreter::ReportDescriptorInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel)
	:BaseInterpreter(rootItem, item, additionalDataModel)
{
	this->holder = DataHolder::GetDataHolder();
	this->reportArray = item->data(holder->TRANSFER_LEFTOVER_DATA).toByteArray();
	this->globalUsage = 0;
	this->reportDescriptor = ReportDescTreeStruct();
	ParseReportDescriptor(&reportDescriptor, 0);
}

/// <summary>
/// Interprets Report Descriptor. Starting point of recursive call InterpretReportDescriptor().
/// </summary>
void ReportDescriptorInterpreter::Interpret()
{
	rootItem->AppendChild(new TreeItem(QVector<QVariant>{"HID_REPORT_DESCRIPTOR", "", ""}, rootItem));
	TreeItem* reportDescriptorChild = rootItem->Child(rootItem->ChildCount() - 1);
	for (int i = 0; i < reportDescriptor.childs.size(); i++)
	{
		InterpretReportDescriptor(reportDescriptorChild, reportDescriptor.childs[i].get());
	}
}

/// <summary>
/// Recursive function to interpret Report Descriptor in a hierarchical way
/// </summary>
/// <param name="parent">Tree item to which we are appending items</param>
/// <param name="reportDescriptor">Node of ReportDescTreeStruct which we are interpreting</param>
void ReportDescriptorInterpreter::InterpretReportDescriptor(TreeItem* parent, ReportDescTreeStruct* reportDescriptor)
{

	AppendItem(parent, reportDescriptor);
	TreeItem* child = parent->Child(parent->ChildCount() - 1);
	for (int i = 0; i < reportDescriptor->childs.size(); i++)
	{
		InterpretReportDescriptor(child, reportDescriptor->childs[i].get());
	}
}

/// <summary>
/// Parsing method to fill up <see cref="reportDescriptor"/>
/// </summary>
/// <param name="root">Current node in reportDescriptor</param>
/// <param name="parsed">Value representing how much data of reportArray has been parsed already</param>
/// <returns>How much data has been already parsed</returns>
std::size_t ReportDescriptorInterpreter::ParseReportDescriptor(ReportDescTreeStruct* root, const std::size_t parsed)
{
	if (parsed == reportArray.size())
	{
		return 0;
	}
	bool afterCollection = false;
	int parsedAfterCollection = 0;
	BYTE header = reportArray.at(parsed);
	BYTE tag = (header & 0xF0) >> 4;  // header has 1 byte, but only 4 top bits represents tag
	BYTE size = (header & 0x03);   // header = 1 byte, size = bottom 2 bits
	BYTE itemType = (header & 0x0C) >> 2; // header = 1 byte, type = bit 2 and 3

	if (itemType == MAIN && tag == COLLECTION)
	{
		root->childs.emplace_back(std::make_shared<ReportDescTreeStruct>(tag, size, itemType, false));
		root->childs[root->childs.size() - 1]->data = reportArray.mid(parsed + 1, size); //+1 for header
		parsedAfterCollection = ParseReportDescriptor(root->childs[root->childs.size() - 1].get(), parsed + size + 1); //same here
		afterCollection = true;
	}
	else if (itemType == MAIN && tag == END_COLLECTION)
	{
		root->childs.emplace_back(std::make_shared<ReportDescTreeStruct>(tag, size, itemType, false));
		root->childs[root->childs.size() - 1]->data = reportArray.mid(parsed + 1, size); //+1 for header
		return parsed + size + 1;
	}
	else
	{
		root->childs.emplace_back(std::make_shared<ReportDescTreeStruct>(tag, size, itemType, false));
		root->childs[root->childs.size() - 1]->data = reportArray.mid(parsed + 1, size); //+1 for header
		return ParseReportDescriptor(root, parsed + 1 + size);
	}
	if (parsedAfterCollection != reportArray.size() && afterCollection) //when coming from END_COLLECTION, if not on end, continue parsing
	{
		return ParseReportDescriptor(root, parsedAfterCollection);
	}

	return parsed + size + 1;
}

/// <summary>
/// Interprets Input/Output/Feature item.
/// </summary>
/// <param name="tag">Tag of Report Descriptor item</param>
/// <param name="data">Data of Report Descriptor item</param>
/// <param name="child">Tree item to which we are appending items</param>
void ReportDescriptorInterpreter::InterpretInputOutputFeature(const BYTE tag, const QByteArray& data, TreeItem* child)
{
	const unsigned char* packet = (unsigned char*)data.constData();
	USHORT value = (USHORT)*packet;

	std::string dataConstant = std::string("Data/Constant: ") + std::string((value & 0x01) ? "Constant (1)" : "Data (0)");
	std::string arrayVariable = std::string("Data Type: ") + std::string((value & 0x02) ? "Variable (1)" : "Array (0)");
	std::string absolute = std::string("Coordinates: ") + std::string((value & 0x04) ? "Relative (1)" : "Absolute (0)");
	std::string wrap = std::string("Min/max wraparound: ") + std::string((value & 0x08) ? "Wrap (1)" : "No Wrap (0)");
	std::string linear = std::string("Physical relationship to data: ") + std::string((value & 0x10) ? "Non Linear (1)" : "Linear (0)");
	std::string state = std::string("Preferred state: ") + std::string((value & 0x20) ? "Non Preferred (1)" : "Preferred State (0)");
	std::string position = std::string("Has null position: ") + std::string((value & 0x40) ? "Null State (1)" : "No Null Position (0)");
	std::string vol = std::string((tag == MAIN_INPUT) ? "Reserved (0)" : (value & 0x80) ? "Volatile (1)" : "Non Volatile (0)");
	std::string field = std::string("Bits/Bytes: ") + std::string((value & 0x100) ? "Buffered Bytes (1)" : "Bit Field (0)");

	child->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(7, 1, value).right(19), field.c_str(), }, child));
	child->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(8, 1, value).right(19), vol.c_str(), }, child));
	child->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(9, 1, value).right(19), position.c_str(), }, child));
	child->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(10, 1, value).right(19), state.c_str(), }, child));
	child->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(11, 1, value).right(19), linear.c_str(), }, child));
	child->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(12, 1, value).right(19), wrap.c_str(), }, child));
	child->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(13, 1, value).right(19), absolute.c_str(), }, child));
	child->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(14, 1, value).right(19), arrayVariable.c_str(), }, child));
	child->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(15, 1, value).right(19), dataConstant.c_str(), }, child));
}

/// <summary>
/// Interprets header of Report Descriptor item.
/// </summary>
/// <param name="reportDescriptor">Current node in reportDescriptor whose header should be interpreted</param>
/// <param name="child">Tree item to which we are appending items</param>
void ReportDescriptorInterpreter::InterpretHeader(ReportDescTreeStruct* reportDescriptor, TreeItem* child)
{
	child->AppendChild(new TreeItem(QVector<QVariant>{"HEADER", "", ""}, child));
	TreeItem* headerChild = child->Child(child->ChildCount() - 1);
	BYTE value = reportDescriptor->size | (reportDescriptor->itemType << 2) | (reportDescriptor->tag << 4);
	headerChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(0, 4, value), (std::string("bTag: ") +
		holder->GetReportTag(reportDescriptor->tag, reportDescriptor->itemType)).c_str(), reportDescriptor->tag}, headerChild));
	headerChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(4, 2, value), (std::string("bType: ") +
		holder->GetReportItemType(reportDescriptor->itemType)).c_str(), reportDescriptor->itemType}, headerChild));
	headerChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(6, 2, value), "bSize", reportDescriptor->size}, headerChild));
}

/// <summary>
/// Interprets data of current item (if its not Input/Output/Feature item)
/// </summary>
/// <param name="reportDescriptor">Current node in reportDescriptor whose data should be interpreted</param>
/// <returns>String of data which will be appended to concrete item as interpreted</returns>
std::string ReportDescriptorInterpreter::InterpretData(ReportDescTreeStruct* reportDescriptor)
{
	std::stringstream stream;
	stream << holder->GetReportTag(reportDescriptor->tag, reportDescriptor->itemType) << ": ";
	if (reportDescriptor->tag == USAGE_PAGE) //usage
	{
		if (reportDescriptor->itemType == GLOBAL)
		{
			stream << holder->GetGlobalUsagePage(*(reportDescriptor->data.constData()));
		}
		else
		{
			stream << holder->GetUsage(globalUsage, *(reportDescriptor->data.constData()));
		}
	}
	else if (reportDescriptor->tag == COLLECTION) //collection
	{
		stream << holder->GetReportCollectionType(*(reportDescriptor->data.constData()));
	}
	else
	{
		QString data;
		const unsigned char* charData = (const unsigned char*)reportDescriptor->data.constData();
		additionalDataModel->CharToHexConvert(&charData, reportDescriptor->size, data);
		stream << data.toStdString();
	}

	return stream.str();
}

/// <summary>
/// Handles interpreting one item - calling all specific item interpreting methods.
/// </summary>
/// <param name="parent">Tree item to which we are appending item</param>
/// <param name="reportDescriptor">Current node in reportDescriptor which should be interpreted</param>
void ReportDescriptorInterpreter::AppendItem(TreeItem* parent, ReportDescTreeStruct* reportDescriptor)
{
	parent->AppendChild(new TreeItem(QVector<QVariant>{(holder->GetReportItemType(reportDescriptor->itemType) +
		" item ( " + holder->GetReportTag(reportDescriptor->tag, reportDescriptor->itemType) + " )").c_str(), "", ""}, parent));
	TreeItem* child = parent->Child(parent->ChildCount() - 1);
	InterpretHeader(reportDescriptor, child);
	if (reportDescriptor->itemType == GLOBAL && reportDescriptor->tag == USAGE_PAGE)
	{
		globalUsage = *(reportDescriptor->data.data());
	}
	if (reportDescriptor->itemType == MAIN && (reportDescriptor->tag == MAIN_INPUT || reportDescriptor->tag == OUTPUT || reportDescriptor->tag == FEATURE))
	{
		InterpretInputOutputFeature(reportDescriptor->tag, reportDescriptor->data, child);
	}
	else
	{
		child->AppendChild(new TreeItem(QVector<QVariant>{InterpretData(reportDescriptor).c_str(), "", ""}, child));
	}
}