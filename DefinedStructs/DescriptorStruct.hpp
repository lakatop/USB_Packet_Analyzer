#ifndef DESCRIPTOR_STRUCT_HPP
#define DESCRIPTOR_STRUCT_HPP

#include <memory>

/// <summary>
/// FORWARD REFERENCES
/// Used in ControlTRansferInterpreter class, where we include AdditionalDataModel 
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
	virtual void FillUpField() = 0;
protected:
};

/// <summary>
/// Conrete class representing 1 field in descriptor
/// </summary>
/// <typeparam name="T">type of concrete field (e.g. INT32, INT64, ...)</typeparam>
template <typename T>
class DescriptorField : public AbstractDescriptorField
{
public:
	DescriptorField(T val) : value(val) {}
	void FillUpField() override {}
private:
	T value;
};

/// <summary>
/// Class representing concrete descriptor.
/// </summary>
class DescriptorStruct
{
public: 
	DescriptorStruct(std::string fName, AdditionalDataModel* additionalDataModel) : filename(fName) {}
	/// <summary>
	/// Fill up concrete tree representing this descriptor
	/// </summary>
	/// <param name="rootItem"></param>
	/// <param name="data"></param>
	void InterpretData(TreeItem* rootItem, const unsigned char* data) {}
private:
	std::string filename;
	std::vector<std::unique_ptr<AbstractDescriptorField>> fields;
};

#endif // !DESCRIPTOR_STRUCT_HPP

