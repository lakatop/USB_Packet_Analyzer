#include "DescriptorStruct.hpp"
#include <sstream>

/// <summary>
/// Fills up fields for this concrete descriptor struct
/// </summary>
void DescriptorStruct::FillUpFields()
{
	std::ifstream input;
	input.open(filename);		
	std::string line;
	//read first line to get descriptor type and name
	if (input.good())
	{
		if (std::getline(input, line))
		{
			std::istringstream ss(line);
			std::string bDescType;
			ss >> bDescType;
			if (std::stoi(bDescType) != descriptorType)
			{
				return;
			}
			std::string descName = ss.str();
			fields.emplace_back(std::make_unique<DescriptorField<std::string>>(descName));
		}
	}
	while (input.good())
	{
		if (std::getline(input, line))
		{
			//last added field has more detailed structure (on bit level of detail) 
			//with given range and values (representing more fields) so fill them up
			if (line == "<")
			{
				fields[fields.size() - 1]->FillUpField(input);
			}
			else
			{
				std::istringstream ss(line);
				std::string firstWord;
				ss >> firstWord;
				if (!std::isdigit(firstWord[0]))
				{
					//means input will be read by the end and has no fixed length (e.g. wstring in String Descriptor)
					if (firstWord == "..")
					{
						std::string type;
						ss >> type;
						if (type == "wstring")
						{
							fields.emplace_back(std::make_unique<DescriptorField<std::wstring>>(ss.str()));
						}
					}
				}
				else
				{
					//according to length determine exact type
					//currently supporting 2 most common - BYTE and USHORT
					switch (std::stoi(firstWord))
					{
					case 1:
					{
						fields.emplace_back(std::make_unique<DescriptorField<BYTE>>(ss.str()));
						break;
					}
					case 2:
					{
						fields.emplace_back(std::make_unique<DescriptorField<USHORT>>(ss.str()));
						break;
					}
					default:
						break;
					}
				}
			}
		}
	}
}

/// <summary>
/// Interpret data according to concrete descriptor
/// </summary>
/// <param name="rootItem">Root tree item of tree view.</param>
/// <param name="data">Data to be interpreted</param>
void DescriptorStruct::InterpretData(TreeItem* rootItem, const unsigned char* data)
{

}