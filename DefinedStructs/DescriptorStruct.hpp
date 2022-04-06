#ifndef DESCRIPTOR_STRUCT_HPP
#define DESCRIPTOR_STRUCT_HPP

#include <memory>
#include <string>
#include <fstream>
#include <type_traits>
#include <qstring.h>
#include <qvariant.h>
#include <qvector.h>

#include "PacketExternStructs.hpp"
#include "../Models/AdditionalDataModel.hpp"

/// <summary>
/// Abstract class representing 1 field in descriptor
/// </summary>
class AbstractDescriptorField
{
public:
	AbstractDescriptorField() {}
	virtual ~AbstractDescriptorField() noexcept {}
	virtual void FillUpField(std::ifstream& input) = 0;
	virtual std::size_t InterpretField(TreeItem* rootItem, const unsigned char* data, std::size_t size,
		AdditionalDataModel* additionalDataModel) = 0;
};

/// <summary>
/// Structure representing one segment of field which has bit level details
/// </summary>
template <typename T>
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
	std::map<T, std::string> descriptions;
};

/// <summary>
/// Conrete class representing 1 field in descriptor
/// </summary>
/// <typeparam name="T">type of concrete field (e.g. INT32, INT64, ...)</typeparam>
template <typename T>
class DescriptorField : public AbstractDescriptorField
{
public:
	DescriptorField(std::string descr) : description(descr), value() {}
	void FillUpField(std::ifstream& input) override;
	std::size_t InterpretField(TreeItem* rootItem, const unsigned char* data, std::size_t sizeLeft, 
		AdditionalDataModel* additionalDataModel) override;
private:
	void CharToNumberConvert(const unsigned char* addr) 
	{
		if constexpr (!std::is_same_v<std::string, T> && !std::is_same_v<std::wstring, T>)
		{
			value = T();
			for (int i = sizeof(T); i > 0; i--)
			{
				value = (value << 8) | addr[i - 1];
			}
		}
	}

	void CharToConcreteNumberConvert(const unsigned char* addr, T& value) 
	{
		if constexpr (!std::is_same_v<std::string, T> && !std::is_same_v<std::wstring, T>)
		{
			value = T();
			for (int i = sizeof(T); i > 0; i--)
			{
				value = (value << 8) | addr[i - 1];
			}
		}
	}

	T GetBitFieldValue(BitField<T>& field);

	std::string description;
	std::vector<BitField<T>> bitFields;
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
			BitField<T> b;
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
						T value;
						CharToConcreteNumberConvert((const unsigned char*)(ss.str().c_str()), value);
						b.descriptions[value] = ss.str();
					}
				}
			}
		}
	}
}

/// <summary>
/// Interpret concrete field (and its possible bit fields)
/// </summary>
/// <typeparam name="T">Type of descriptor field</typeparam>
/// <param name="rootItem">Root tree item of tree view</param>
/// <param name="data">Starting point of data to be interpreted</param>
/// <param name="sizeLeft">Size of data that are still valid and has not been proccessed yet</param>
/// <param name="additionalDataModel">Pointer to AdditionalDataModel instance</param>
/// <returns>Size of data that were interpreted</returns>
template <typename T>
std::size_t DescriptorField<T>::InterpretField(TreeItem* rootItem, const unsigned char* data, std::size_t sizeLeft,
	AdditionalDataModel* additionalDataModel)
{
	QString hexData;
	//wstring ... proccess all left data
	if (std::is_same_v<T, std::wstring>)
	{
		additionalDataModel->CharToHexConvert(&data, sizeLeft, hexData);
		std::wstring wString(data, data + sizeLeft);
		std::string bString(wString.begin(), wString.end());
		rootItem->AppendChild(new TreeItem(QVector<QVariant>{hexData, description.c_str(), bString.c_str()}, rootItem));
		return sizeLeft;
	}

	//used so we can work with std::stringstream and its operator << (it wouldnt work with wstring)
	if constexpr (!std::is_same_v<std::wstring, T> && !std::is_same_v<std::string, T>)
	{
		//some other data type
		//convert char* to number and fill value data field
		CharToNumberConvert(data);
		additionalDataModel->CharToHexConvert(&data, sizeof(T), hexData);
		std::stringstream ss;
		ss << value;
		rootItem->AppendChild(new TreeItem(QVector<QVariant>{hexData, description.c_str(), ss.str().c_str()}, rootItem));

		//check if it carries some bit defined information
		//if not, return proccessed size
		if (bitFields.empty())
		{
			return sizeof(T);
		}
		//else go through bitFields and interpret them
		TreeItem* bitFieldParent = rootItem->Child(rootItem->ChildCount() - 1);
		for (std::size_t i = 0; i < bitFields.size(); i++)
		{
			T bitValue = GetBitFieldValue(bitFields[i]);
			auto fieldDesc = bitFields[i].descriptions[bitValue];
			ss.clear();
			ss << value;
			std::stringstream ss2;
			ss2 << bitValue;
			bitFieldParent->AppendChild(new TreeItem(QVector<QVariant>{
				additionalDataModel->ShowBits(bitFields[i].start, bitFields[i].size, ss.str().c_str()),
					fieldDesc.c_str(), ss2.str().c_str()}, bitFieldParent));
		}
	}
}

/// <summary>
/// Get value of given bit-field
/// </summary>
/// <typeparam name="T">Type of descriptor field</typeparam>
/// <param name="field">Concrete field</param>
/// <returns>Value of given bit-field</returns>
template <typename T>
T DescriptorField<T>::GetBitFieldValue(BitField<T>& field)
{
	T bitmask = (1 << field.size) - 1;
	bitmask = bitmask << field.start;
	T bitValue = (value & bitmask) >> field.start;

	return bitValue;
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
	void InterpretData(TreeItem* rootItem, const QByteArray& data, AdditionalDataModel* additionalDataModel);
	BYTE descriptorType;
private:
	void FillUpFields();

	std::string filename;
	std::vector<std::unique_ptr<AbstractDescriptorField>> fields;
};

#endif // !DESCRIPTOR_STRUCT_HPP

