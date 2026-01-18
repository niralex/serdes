//------------------------------------------------------------------------------
/** @file

    @brief Demonstration of runtime descriptor inspection

    @details
     Shows how serialized data can be inspected and interpreted
     without knowing the original C++ type at compile time.

    @todo


    @author Niralex
*/
//------------------------------------------------------------------------------

#include <cassert>
#include <iostream>
#include <codecvt>
#include <locale>
#include <typeinfo>

#include <Serdes/Descriptors.hpp>

using namespace std;
using namespace serdes;

//------------------------------------------------------------------------------
consteval
std::array<std::string_view, 256> CreatePodNames()
{
    std::array<std::string_view, 256> names;

    names[(int)PodId::Unspecified] = "Unspecified";
    names[(int)PodId::Bool] = "Bool";
    names[(int)PodId::Char] = "Char";
    names[(int)PodId::UInt] = "UInt";
    names[(int)PodId::Int] = "Int";
    names[(int)PodId::Float] = "Float";
    names[(int)PodId::Day] = "Day";
    names[(int)PodId::Month] = "Month";
    names[(int)PodId::Weekday] = "Weekday";
    names[(int)PodId::Year] = "Year";
    names[(int)PodId::Date] = "Date";
    names[(int)PodId::Time] = "Time";
    names[(int)PodId::DateTime] = "DateTime";

    return names;
}
//------------------------------------------------------------------------------
void DescriptorHandler(Descriptor descriptor, unsigned indentLevel = 0);

//------------------------------------------------------------------------------
// Common handler — invoked for all descriptors
void CommonDescriptorHandler(string_view serdesTypeName, Descriptor descriptor, unsigned indentLevel = 0)
{
    cout << "\n" <<string(indentLevel, '\t') << "SerdesType:\t" << serdesTypeName << " (TypeId = " << (int)descriptor.GetTypeId() << ")" << endl;
    if(descriptor.GetBufferType() == BufferType::Static)
    {
        cout << string(indentLevel, '\t') << "BufferType:\tstatic" << endl;
        cout << string(indentLevel, '\t') << "BufferSize:\t" << descriptor.Sizeof() << "b" << endl;
    }
    else
    {
        cout << string(indentLevel, '\t') << "BufferType:\tdynamic" << endl;
        cout << string(indentLevel, '\t') << "BufferMaxSize:\t" << descriptor.Sizeof() << "b" << endl;
    }
}

//------------------------------------------------------------------------------
void PodDescriptorHandler(PodDescriptor descriptor, unsigned indentLevel = 0)
{
    constexpr static
    auto podNames = CreatePodNames();

    CommonDescriptorHandler("Pod", descriptor, indentLevel);
    cout << string(indentLevel, '\t') << "PodType:\t" << podNames[(int)descriptor.GetPodId()] << " (PodId = " << (int)descriptor.GetPodId() << ")"<< endl;
    cout << string(indentLevel, '\t') << "ByteOrder:\t" << (descriptor.GetEndianness() == std::endian::little ? "little-endian" : "big-endian") << endl;
}

//------------------------------------------------------------------------------
void RangeDescriptorHandler(RangeDescriptor descriptor, unsigned indentLevel = 0)
{
    CommonDescriptorHandler("Range", descriptor, indentLevel);

    cout << string(indentLevel, '\t') << "FieldSizeLen:\t" << (int)descriptor.GetSizeLen() << endl;
    cout << string(indentLevel, '\t') << "ElementDescriptor:\t" << endl;
    DescriptorHandler(descriptor.GetElementDescriptor(), indentLevel + 1);
}

//------------------------------------------------------------------------------
void ArrayDescriptorHandler(ArrayDescriptor descriptor, unsigned indentLevel = 0)
{
    CommonDescriptorHandler("Array", descriptor, indentLevel);

    cout << string(indentLevel, '\t') << "ArraySize:\t" << (int)descriptor.GetArraySize() << endl;
    cout << string(indentLevel, '\t') << "ElementDescriptor:\t" << endl;
    DescriptorHandler(descriptor.GetElementDescriptor(), indentLevel + 1);
}
//------------------------------------------------------------------------------
void TupleDescriptorHandler(TupleDescriptor descriptor, unsigned indentLevel = 0)
{
    CommonDescriptorHandler("Tuple", descriptor, indentLevel);

    cout << string(indentLevel, '\t') << "CountElements:\t" << (int)descriptor.CountElements() << endl;
    cout << string(indentLevel, '\t') << "ElementDescriptors:\t" << endl;
    for(auto d: descriptor)
        DescriptorHandler(d, indentLevel + 1);
}

//------------------------------------------------------------------------------
void VariantDescriptorHandler(VariantDescriptor descriptor, unsigned indentLevel = 0)
{
    CommonDescriptorHandler("Variant", descriptor, indentLevel);

    cout << string(indentLevel, '\t') << "CountTypes:\t" << (int)descriptor.CountTypes() << endl;
    cout << string(indentLevel, '\t') << "TypeDescriptors:\t" << endl;
    for(auto d: descriptor)
        DescriptorHandler(d, indentLevel + 1);
}

//------------------------------------------------------------------------------
void ConstDescriptorHandler(ConstDescriptor descriptor, unsigned indentLevel = 0)
{
    CommonDescriptorHandler("Const", descriptor, indentLevel);

    cout << string(indentLevel, '\t') << "ValueSerdes:" << endl;
    DescriptorHandler(descriptor.GetSerdesDescriptor(), indentLevel + 1);
}

//------------------------------------------------------------------------------
void DescriptorHandler(Descriptor descriptor, unsigned indentLevel)
{
    switch(descriptor.GetTypeId())
    {
        case TypeId::Void:
             CommonDescriptorHandler("Void", descriptor, indentLevel);
             break;

        case TypeId::Pod:
             PodDescriptorHandler(static_cast<PodDescriptor>(descriptor), indentLevel);
             break;

        case TypeId::Range:
             RangeDescriptorHandler(static_cast<RangeDescriptor>(descriptor), indentLevel);
             break;

        case TypeId::Array:
             ArrayDescriptorHandler(static_cast<ArrayDescriptor>(descriptor), indentLevel);
             break;

        case TypeId::Tuple:
             TupleDescriptorHandler(static_cast<TupleDescriptor>(descriptor), indentLevel);
             break;

        case TypeId::Variant:
             VariantDescriptorHandler(static_cast<VariantDescriptor>(descriptor), indentLevel);
             break;

        case TypeId::Const:
             ConstDescriptorHandler(static_cast<ConstDescriptor>(descriptor), indentLevel);
             break;
    }
}

//------------------------------------------------------------------------------
int main()
{
    using namespace serdes;
    using namespace std;

    Descriptor voidDescriptor = GetDescriptor<Void>();
    Descriptor podDescriptor = GetDescriptor<Int32>();
    Descriptor rangeDescriptor = GetDescriptor<Vector<Float>>();
    Descriptor arrayDescriptor = GetDescriptor<Array<UInt16B, 10>>();
    Descriptor tupleDescriptor = GetDescriptor<Tuple<Bool, Date>>();
    Descriptor variantDescriptor = GetDescriptor<Variant<DateTime, String, Void>>();
    Descriptor constDescriptor = GetDescriptor<Const<Double, 3.14>>();
    Descriptor complexDescriptor = GetDescriptor<
      Int,
      Tuple<Vector<UInt64>, String8>,
      Variant<ConstString<"Literal1">, ConstString<"Literal2">>>();

    cout << "\n******************************** Void ********************************" << endl;
    DescriptorHandler(voidDescriptor, 0);

    cout << "\n******************************** Int32 ********************************" << endl;
    DescriptorHandler(podDescriptor, 0);

    cout << "\n******************************** Vector<Float> ********************************" << endl;
    DescriptorHandler(rangeDescriptor, 0);

    cout << "\n******************************** Array<UInt16B, 10> ********************************" << endl;
    DescriptorHandler(arrayDescriptor, 0);

    cout << "\n******************************** Tuple<Bool, Date> ********************************" << endl;
    DescriptorHandler(tupleDescriptor, 0);

    cout << "\n******************************** Variant<DateTime, String, Void> ********************************\n" << endl;
    DescriptorHandler(variantDescriptor, 0);

    cout << "\n******************************** Const<Double, 3.14> ********************************" << endl;
    DescriptorHandler(constDescriptor, 0);

    cout << "\n******************************************************************************************************" << endl;
    cout << "**** Tuple<Int, Tuple<UInt64, String8>, Variant<ConstString<\"Literal1\">, ConstString<\"Literal2\">> ****" << endl;
    cout << "******************************************************************************************************" << endl;

    DescriptorHandler(complexDescriptor, 0);

    return 0;
}
