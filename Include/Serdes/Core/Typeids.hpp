#ifndef SERDES_CORE_TYPEIDS_HPP
#define SERDES_CORE_TYPEIDS_HPP

//------------------------------------------------------------------------------
/** @file

	@brief Definitions of core types and special values

	@details

    @todo

    @author Niraleks

*/
//------------------------------------------------------------------------------
#include <cstdint>
#include <limits>

//------------------------------------------------------------------------------
namespace serdes
{
    /// Enumeration of serdes types
    enum class TypeId : uint8_t
    {
		Void,
        Pod,
        Range,
        Array,
        Tuple,
        Variant,
        Const,
    };

    /// Enumeration of value types for POD serdes
    // The least significant bit encodes endianness:
    // even IDs = little-endian, odd IDs = big-endian
    enum class PodTypeId : uint8_t
    {
        Char8     = 0,
        Char16    = 2,
        Char16B   = 3,
        Char32    = 4,
        Char32B   = 5,

        UInt8     = 16,
        Int8      = 17,
        UInt16    = 18,
        UInt16B   = 19,
        Int16     = 20,
        Int16B    = 21,
        UInt32    = 22,
        UInt32B   = 23,
        Int32     = 24,
        Int32B    = 25,
        UInt64    = 26,
        UInt64B   = 27,
        Int64     = 28,
        Int64B    = 29,

        Float     = 32,
        FloatB    = 33,
        Double    = 34,
        DoubleB   = 35,

        Day       = 64,
        Month     = 65,
        Weekday   = 66,
        Year      = 68,
        YearB     = 69,
        Date      = 70,
        DateB     = 71,
        Time      = 72,
        TimeB     = 73,
        DateTime  = 74,
        DateTimeB = 75,
        Bool      = 128,
        Unspecified = 130, // unspecified POD type
    };

    /// Enumeration of buffer types
    enum class BufferType : bool
    {
        Static,  // buffer size can be determined at compile time without a value
        Dynamic  // required buffer size can be determined only from a value
    };

    /// Special value returned by Sizeof() when the size exceeds the allowed limit
    inline constexpr uint32_t WRONG_SIZE = std::numeric_limits<uint32_t>::max();
}

//------------------------------------------------------------------------------
#endif
