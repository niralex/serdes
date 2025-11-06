#ifndef SERDES_CORE_DEFAULT_HPP
#define SERDES_CORE_DEFAULT_HPP

//------------------------------------------------------------------------------
/** @file

    @brief Default serdes definitions for standard types

    @details 
        This module defines the Default template and its specializations for
        certain standard types. This template enables automatic retrieval of default
        serdes without requiring the user to specify them explicitly—for example, during serialization.

        The default serdes depend on the endianness of the target platform.

        Users may add their own specializations for custom types.

    @todo

    @author Niraleks

*/

//------------------------------------------------------------------------------
#include <limits>
#include <type_traits>
#include "Typedefs.hpp"

//-----------------------------------------------------------------------------
namespace serdes
{
    /// Primary template (undefined)
    // Users must rely on provided specializations or supply their own
    // for more complex types
    template<typename ...Ts>
    struct Default;

    /// Helper alias to obtain the serdes type from Default
    template<typename ...Ts>
    using DefaultT = typename Default<Ts...>::Type;

    /// Helper to select the serdes version matching the platform's native endianness
    template<CSerdes TLittleSerdes, typename TBigSerdes>
    using NativeEndianSerdes = std::conditional_t<std::endian::native != std::endian::big, TLittleSerdes, TBigSerdes>;

    /// Specialization for an empty type list
    template<>
    struct Default<> { using Type = SerdesT<>; };

    // Specialization for type void
    template<>
    struct Default<void> { using Type = Void; };

    /// Concept for integral types with specified properties
    template<typename T, std::size_t Size, bool isSigned>
    concept CIntegral =
        std::integral<T> &&
        sizeof(T) == Size &&
        std::is_signed_v<T> == isSigned;

    /// Specializations for POD types
    template<>
    struct Default<bool> {  using Type = Bool; };

    template<>
    struct Default<char> { using Type = Char; };

    template<>
    struct Default<std::byte> { using Type = UInt8; };

    template<>
    struct Default<char16_t> { using Type = NativeEndianSerdes<Char16, Char16B>; };

    template<>
    struct Default<char32_t> { using Type =  NativeEndianSerdes<Char32, Char32B>; };

    template<typename T>
    requires (std::integral<T> && sizeof(T) == 1 && std::is_unsigned_v<T>)
    struct Default<T> { using Type = UInt8; };

    template<typename T>
    requires (std::integral<T> && sizeof(T) == 1 && std::is_signed_v<T>)
    struct Default<T> { using Type = Int8; };

    template<typename T>
    requires (std::integral<T> && sizeof(T) == 2 && std::is_signed_v<T>)
    struct Default<T> { using Type = NativeEndianSerdes<Int16, Int16B>; };

    template<typename T>
    requires (std::integral<T> && sizeof(T) == 2 && std::is_unsigned_v<T>)
    struct Default<T> { using Type = NativeEndianSerdes<UInt16, UInt16B>; };

    template<typename T>
    requires (std::integral<T> && sizeof(T) == 4 && std::is_signed_v<T>)
    struct Default<T> { using Type = NativeEndianSerdes<Int32, Int32B>; };

    template<typename T>
    requires (std::integral<T> && sizeof(T) == 4 && std::is_unsigned_v<T>)
    struct Default<T> { using Type = NativeEndianSerdes<UInt32, UInt32B>; };

    template<typename T>
    requires (std::integral<T> && sizeof(T) == 8 && std::is_signed_v<T>)
    struct Default<T> { using Type = NativeEndianSerdes<Int64, Int64B>; };

    template<typename T>
    requires (std::integral<T> && sizeof(T) == 8 && std::is_unsigned_v<T>)
    struct Default<T> { using Type = NativeEndianSerdes<UInt64, UInt64B>; };

    template<>
    struct Default<float> { using Type = NativeEndianSerdes<Float, FloatB>; };

    template<>
    struct Default<double> { using Type = NativeEndianSerdes<Double, DoubleB>; };

    template<>
    struct Default<std::chrono::day> { using Type = Day; };

    template<>
    struct Default<std::chrono::month> { using Type = Month; };

    template<>
    struct Default<std::chrono::weekday> { using Type = Weekday; };

    template<>
    struct Default<std::chrono::year> { using Type = NativeEndianSerdes<Year, YearB>; };

    template<>
    struct Default<std::chrono::year_month_day> { using Type = NativeEndianSerdes<Date, DateB>; };

    template<>
    struct Default<std::chrono::duration<uint32_t, std::milli>> { using Type = NativeEndianSerdes<Time, TimeB>; };

    template<>
    struct Default<std::chrono::nanoseconds> { using Type = NativeEndianSerdes<DateTime, DateTimeB>; };


    /// Enumerations
    template<typename TEnum>
    requires std::is_enum_v<TEnum>
    struct Default<TEnum> { using Type = DefaultT<std::underlying_type_t<TEnum>>; };


    /// Strings
    template<>
    struct Default<std::string> { using Type = String; };

    template<>
    struct Default<std::string_view> { using Type = String; };

    template<>
    struct Default<std::u16string> { using Type = U16String; };

    template<>
    struct Default<std::u32string> { using Type = U32String; };
	
    // String literals are handled using String
    template<std::size_t N>
    struct Default<char[N]> { using Type = String; };

    /// Tuple
    template<typename... Ts>
    struct Default<std::tuple<Ts...>> {  using Type = Tuple<DefaultT<Ts>...>; };

    template<typename... Ts>
    requires (sizeof...(Ts) > 1)
    struct Default<Ts...> { using Type = DefaultT<std::tuple<Ts...>>; };

    // std::array and fixed-size arrays
    template<typename T, std::size_t N>
    struct Default<std::array<T, N>> { using Type = Array<DefaultT<T>, N>; };

    template<typename T, std::size_t N>
    struct Default<T[N]> { using Type = Array<DefaultT<T>, N>; };


    /// Dynamic containers
    template<typename T, typename TAlloc>
    struct Default<std::vector<T, TAlloc>> { using Type = Vector<DefaultT<T>, TAlloc>; };

    template<typename T, typename TAlloc>
    struct Default<std::deque<T, TAlloc>> { using Type = Deque<DefaultT<T>, TAlloc>; };

    template<typename T, typename TAlloc>
    struct Default<std::list<T, TAlloc>> { using Type = List<DefaultT<T>, TAlloc>; };

    template<typename TFirst, typename TSecond>
    struct Default<std::pair<TFirst, TSecond>> { using Type = Pair<DefaultT<TFirst>, DefaultT<TSecond>>; };

    template<typename TKey, typename TCompare, typename TAlloc>
    struct Default<std::set<TKey, TCompare, TAlloc>> { using Type = Set<DefaultT<TKey>, TCompare, TAlloc>; };

    template<typename TKey, typename TCompare, typename TAlloc>
    struct Default<std::multiset<TKey, TCompare, TAlloc>> { using Type = MultiSet<DefaultT<TKey>, TCompare, TAlloc>; };

    template<typename TKey, typename TValue, typename TCompare, typename TAlloc>
    struct Default<std::map<TKey, TValue, TCompare, TAlloc>>
    {
        using Type = Map<DefaultT<TKey>, DefaultT<TValue>, TCompare, TAlloc>;
    };

    template<typename TKey, typename TValue, typename TCompare, typename TAlloc>
    struct Default<std::multimap<TKey, TValue, TCompare, TAlloc>>
    {
        using Type = MultiMap<DefaultT<TKey>, DefaultT<TValue>, TCompare, TAlloc>;
    };

    template<typename TKey, typename THash, typename TKeyEqual, typename TAlloc>
    struct Default<std::unordered_set<TKey, THash, TKeyEqual, TAlloc>>
    {
        using Type = UnorderedSet<DefaultT<TKey>, THash, TKeyEqual, TAlloc>;
    };

    template<typename TKey, typename THash, typename TKeyEqual, typename TAlloc>
    struct Default<std::unordered_multiset<TKey, THash, TKeyEqual, TAlloc>>
    {
        using Type = UnorderedMultiSet<DefaultT<TKey>, THash, TKeyEqual, TAlloc>;
    };

    template<typename TKey, typename TValue, typename THash, typename TKeyEqual, typename TAlloc>
    struct Default<std::unordered_map<TKey, TValue, THash, TKeyEqual, TAlloc>>
    {
        using Type = UnorderedMap<DefaultT<TKey>, DefaultT<TValue>, THash, TKeyEqual, TAlloc>;
    };

    template<typename TKey, typename TValue, typename THash, typename TKeyEqual, typename TAlloc>
    struct Default<std::unordered_multimap<TKey, TValue, THash, TKeyEqual, TAlloc>>
    {
        using Type = UnorderedMultiMap<DefaultT<TKey>, DefaultT<TValue>, THash, TKeyEqual, TAlloc>;
    };


    // Pointers
    template<typename T>
    struct Default<T*> { using Type = Ptr<DefaultT<T>>; };

    template<>
    struct Default<std::nullptr_t> { using Type = Ptr<UInt8>; }; // ?? Void

    template<typename T>
    struct Default<std::unique_ptr<T>> { using Type = UniquePtr<DefaultT<T>>; };

    template<typename T>
    struct Default<std::shared_ptr<T>> { using Type = SharedPtr<DefaultT<T>>; };

    // std::variant
    template<typename... Ts>
    struct Default<std::variant<Ts...>> { using Type = Variant<DefaultT<Ts>...>; };

}

//------------------------------------------------------------------------------
#endif
