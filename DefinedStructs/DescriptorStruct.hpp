#ifndef DESCRIPTOR_STRUCT_HPP
#define DESCRIPTOR_STRUCT_HPP

#include <memory>
#include <string>
#include <fstream>
#include <type_traits>

#include "PacketExternStructs.hpp"

/// <summary>
/// FORWARD REFERENCES
/// Used in ControlTransferInterpreter class, where we include AdditionalDataModel 
/// along with TreeItem before using any of DescriptorStruct
/// </summary>
class AdditionalDataModel;
class TreeItem;

/// <summary>
/// Abstract class representing 1 field in descriptor
/// </summary>
class AbstractDescriptorField
{
public:
	AbstractDescriptorField() {}
	virtual ~AbstractDescriptorField() noexcept {}
	virtual void FillUpField(std::ifstream& input) = 0;
protected:
};

/// <summary>
/// Structure representing one segment of field which has bit level details
/// </summary>
struct BitField
{
	/// <summary>
	/// starting position of analyzing bits
	/// </summary>
	int start;
	/// <summary>
	/// size of analyzing bits
	/// </summary>
	int size;
	/// <summary>
	/// map holding descriptions to given values
	/// </summary>
	std::map<int, std::string> descriptions;
};

/// <summary>
/// Conrete class representing 1 field in descriptor
/// </summary>
/// <typeparam name="T">type of concrete field (e.g. INT32, INT64, ...)</typeparam>
template <typename T>
class DescriptorField : public AbstractDescriptorField
{
public:
	DescriptorField(std::string descr) : description(descr) {}
	void FillUpField(std::ifstream& input) override;
private:
	std::string description;
	std::vector<BitField> bitFields;
	T value;
};

/// <summary>
/// fill up one concrete fields along with its bit defined fields
/// </summary>
/// <typeparam name="T">Type of descriptor field</typeparam>
/// <param name="input">input stream</param>
template <typename T>
void DescriptorField<T>::FillUpField(std::ifstream& input)
{
	while (input.good())
	{
		std::string line;
		if (std::getline(input, line))
		{
			//end of the whole field
			if (line == ">")
			{
				return;
			}
			std::istringstream ss(line);
			BitField b;
			ss >> b.start;
			ss.get(); //separator
			ss >> b.size;
			char separator = input.get();
			if (input.good() && separator == '{')
			{
				while (input.good())
				{
					if (std::getline(input, line))
					{
						//end of bit field
						if (line == "}")
						{
							return;
						}
						//else fill up one bit field with value - description pairs
						ss = std::istringstream(line);
						int value = ss.get();
						b.descriptions[value] = ss.str();
					}
				}
			}
		}
	}
}

/// <summary>
/// Class representing concrete descriptor.
/// </summary>
class DescriptorStruct
{
public: 
	DescriptorStruct(std::string fName, BYTE dType) : filename(fName), descriptorType(dType) { FillUpFields(); }
	/// <summary>
	/// Fill up concrete tree representing this descriptor
	/// </summary>
	/// <param name="rootItem"></param>
	/// <param name="data"></param>
	void InterpretData(TreeItem* rootItem, const unsigned char* data);
	BYTE descriptorType;
private:
	void FillUpFields();

	std::string filename;
	std::vector<std::unique_ptr<AbstractDescriptorField>> fields;
};

#endif // !DESCRIPTOR_STRUCT_HPP

