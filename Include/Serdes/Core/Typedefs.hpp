#ifndef SERDES_CORE_TYPEDEFS_HPP
#define SERDES_CORE_TYPEDEFS_HPP

//------------------------------------------------------------------------------
/** @file

	@brief Определение седесов для стандартных типов данных

	@details

    @todo


    @author Niraleks

*/

//------------------------------------------------------------------------------
#include <limits>
#include <type_traits>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <set>
#include <complex>
#include <chrono>

#include "Concepts.hpp"
#include "Typeids.hpp"
#include "Pod.hpp"
#include "Const.hpp"
#include "Range.hpp"
#include "Tuple.hpp"
#include "Array.hpp"
#include "Variant.hpp"
#include "Pointer.hpp"
#include "Reference.hpp"
#include "Struct.hpp"
#include "Custom.hpp"

//-----------------------------------------------------------------------------
namespace serdes
{
    //--------------------------------------------------------------------------
    // Определение седесов на базе шаблона Pod и декларация необходимых требований

    using Bool = Pod<bool, PodTypeId::Bool>;
    using UChar8 = Pod<unsigned char, PodTypeId::Char8>;
    using Char8 = Pod<char, PodTypeId::Char8>;
    using Char16 = Pod<char16_t, PodTypeId::Char16>;
    using Char16B = Pod<char16_t, PodTypeId::Char16B>;
    using Char32 = Pod<char32_t, PodTypeId::Char32>;
    using Char32B = Pod<char32_t, PodTypeId::Char32B>;

    using UInt8 = Pod<uint8_t, PodTypeId::UInt8>;
    using Int8 = Pod<int8_t, PodTypeId::Int8>;

    using UInt16 = Pod<uint16_t, PodTypeId::UInt16>;
    using UInt16B = Pod<uint16_t, PodTypeId::UInt16B>;

    using UInt32 = Pod<uint32_t, PodTypeId::UInt32>;
    using UInt32B = Pod< uint32_t, PodTypeId::UInt32B>;

    using UInt64 = Pod<uint64_t, PodTypeId::UInt64>;
    using UInt64B = Pod<uint64_t, PodTypeId::UInt64B>;

    using Int16 = Pod<int16_t, PodTypeId::Int64>;
    using Int16B = Pod<int16_t, PodTypeId::Int64B>;

    using Int32 = Pod<int32_t, PodTypeId::Int32>;
    using Int32B = Pod<int32_t, PodTypeId::Int32B>;

    using Int64 = Pod<int64_t, PodTypeId::Int64>;
    using Int64B = Pod<int64_t, PodTypeId::Int64B>;

    using Char = Char8;
    using Int = Int64;
    using IntB = Int64B;
    using UInt = UInt64;
    using UIntB = UInt64B;

    static_assert(sizeof(float) == 4);
    static_assert(std::numeric_limits<float>::digits == 24);  // IEEE 754 standard: binary32
    using Float = Pod<float, PodTypeId::Float>;
    using FloatB = Pod<float, PodTypeId::FloatB>;

    static_assert(sizeof(double) == 8);
    static_assert(std::numeric_limits<double>::digits == 53); // IEEE 754 double-precision binary floating-point format: binary64
    using Double = Pod<double, PodTypeId::Double>;
    using DoubleB = Pod<double, PodTypeId::DoubleB>;

    static_assert(sizeof(std::chrono::day) == 1);
    using Day = Pod<std::chrono::day, PodTypeId::Day>; // день месяца

    static_assert(sizeof(std::chrono::month) == 1);
    using Month = Pod<std::chrono::month, PodTypeId::Month>; // номер месяца в году (1 - январь, ..., 12 - декабрь)

    static_assert(sizeof(std::chrono::weekday) == 1);
    using Weekday = Pod<std::chrono::weekday, PodTypeId::Weekday>; // день недели (0 - воскресенье, ..., 6 - суббота)

    static_assert(sizeof(std::chrono::year) == 2);
    using Year = Pod<std::chrono::year, PodTypeId::Year>;
    using YearB = Pod<std::chrono::year, PodTypeId::YearB>;

    static_assert(sizeof(std::chrono::year_month_day) == 4);
    using Date = Pod<std::chrono::year_month_day, PodTypeId::Date>; // дата
    using DateB = Pod<std::chrono::year_month_day, PodTypeId::DateB>; //

    // Седесы времени дня(без даты) с точностью до миллисекунд
    using Time = Pod<std::chrono::duration<uint32_t, std::milli>, PodTypeId::Time>;
    using TimeB = Pod<std::chrono::duration<uint32_t, std::milli>, PodTypeId::TimeB>;

    // Седесы даты и времени в формате Unix с точностью до наносекунд
    using DateTime = Pod<std::chrono::nanoseconds, PodTypeId::DateTime>;
    using DateTimeB = Pod<std::chrono::nanoseconds, PodTypeId::DateTimeB>;

    //--------------------------------------------------------------------------
	// Определения седесов для строковых типов

	using String8 = Range<UInt8, Char8, std::basic_string<ValueT<Char8>, std::char_traits<ValueT<Char8>>, std::allocator<ValueT<Char8>>>>;
    using String16 = Range<UInt16, Char8, std::basic_string<ValueT<Char8>, std::char_traits<ValueT<Char8>>, std::allocator<ValueT<Char8>>>>;
    using String32 = Range<UInt32, Char8, std::basic_string<ValueT<Char8>, std::char_traits<ValueT<Char8>>, std::allocator<ValueT<Char8>>>>;
	using U16String8 = Range<UInt8, Char16, std::basic_string<ValueT<Char16>, std::char_traits<ValueT<Char16>>, std::allocator<ValueT<Char16>>>>;
    using U16String16 = Range<UInt16, Char16, std::basic_string<ValueT<Char16>, std::char_traits<ValueT<Char16>>, std::allocator<ValueT<Char16>>>>;
    using U16String32 = Range<UInt32, Char16, std::basic_string<ValueT<Char16>, std::char_traits<ValueT<Char16>>, std::allocator<ValueT<Char16>>>>;
	using U32String8 = Range<UInt8, Char32, std::basic_string<ValueT<Char32>, std::char_traits<ValueT<Char32>>, std::allocator<ValueT<Char32>>>>;
    using U32String16 = Range<UInt16, Char32, std::basic_string<ValueT<Char32>, std::char_traits<ValueT<Char32>>, std::allocator<ValueT<Char32>>>>;
    using U32String32 = Range<UInt32, Char32, std::basic_string<ValueT<Char32>, std::char_traits<ValueT<Char32>>, std::allocator<ValueT<Char32>>>>;

    using String = String32;
    using U16String = U16String32;
	using U32String = U32String32;

    //--------------------------------------------------------------------------
    // Определения седесов для стандартных контейнеров

    template<CSerdes TElementSerdes, typename TAllocator = std::allocator<ValueT<TElementSerdes>>>
    using Vector8 = Range<UInt8, TElementSerdes, std::vector<ValueT<TElementSerdes>, TAllocator>>;

    template<CSerdes TElementSerdes, typename TAllocator = std::allocator<ValueT<TElementSerdes>>>
    using Vector16 = Range<UInt16, TElementSerdes, std::vector<ValueT<TElementSerdes>, TAllocator>>;

    template<CSerdes TElementSerdes, typename TAllocator = std::allocator<ValueT<TElementSerdes>>>
    using Vector32 = Range<UInt32, TElementSerdes, std::vector<ValueT<TElementSerdes>, TAllocator>>;

    template<CSerdes TElementSerdes, typename TAllocator = std::allocator<ValueT<TElementSerdes>>>
    using Vector = Vector32<TElementSerdes, TAllocator>;

    template<CSerdes TElementSerdes, typename TAllocator = std::allocator<ValueT<TElementSerdes>>>
    using Deque = Range<UInt32, TElementSerdes, std::deque<ValueT<TElementSerdes>, TAllocator>>;

    template<CSerdes TElementSerdes, typename TAllocator = std::allocator<ValueT<TElementSerdes>>>
    using List = Range<UInt32, TElementSerdes, std::list<ValueT<TElementSerdes>, TAllocator>>;

    template<CSerdes TKeySerdes,
             typename Compare = std::less<ValueT<TKeySerdes>>,
             typename Allocator = std::allocator<ValueT<TKeySerdes>>>
    using Set = Range<UInt32, TKeySerdes, std::set<ValueT<TKeySerdes>, Compare, Allocator>>;


    template<CSerdes TSerdes1, CSerdes TSerdes2>//
    using Pair = Struct<std::pair<ValueT<TSerdes1>, ValueT<TSerdes2>>, Tuple<TSerdes1, TSerdes2>,
                        &std::pair<ValueT<TSerdes1>, ValueT<TSerdes2>>::first, &std::pair<ValueT<TSerdes1>, ValueT<TSerdes2>>::second>;

    template<CSerdes TKeySerdes,
             CSerdes TValueSerdes,
             typename Compare = std::less<ValueT<TKeySerdes>>,
             typename Allocator = std::allocator<std::pair<const ValueT<TKeySerdes>, ValueT<TValueSerdes>>>>
    using Map = Range<UInt32,
                      Pair<TKeySerdes, TValueSerdes>,
                      std::map<ValueT<TKeySerdes>, ValueT<TValueSerdes>, Compare, Allocator>>;

    //--------------------------------------------------------------------------
    // Седесы для указателей

    template<CSerdes TSerdes>
    using Ptr = Pointer<TSerdes, ValueT<TSerdes> *>;

    template<CSerdes TSerdes>
    using UniquePtr = Pointer<TSerdes, std::unique_ptr<ValueT<TSerdes>>>;

    template<CSerdes TSerdes>
    using SharedPtr = Pointer<TSerdes, shared_ptr<ValueT<TSerdes>>>;

    //--------------------------------------------------------------------------
    // Седес для строковых констант
    template<ConstexprString str>
    using ConstString = Const<Array<Char, str.size()+1>, str>;

    //--------------------------------------------------------------------------
    // Седес для байтового массива, размер которого можно получить
    // с помощью функции-члена size() во время компиляции
    template<auto ArrayObject>
    using ByteArray = Array<UInt8, ArrayObject.size()>;

}

//------------------------------------------------------------------------------
#endif


