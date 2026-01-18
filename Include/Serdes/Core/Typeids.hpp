#pragma once
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
        Custom,
    };

    /// Enumeration of value types for POD serdes
    enum class PodId : uint8_t 
    {
        Unspecified = 0, 
        Bool      = 1,
        Char      = 2,
        UInt      = 3,
        Int       = 4,
        Float     = 5,
        Day       = 6,
        Month     = 7,
        Weekday   = 8,
        Year      = 9,
        Date      = 10,  
        Time      = 11,
        DateTime  = 12,
    };

    /// Enumeration of buffer types
    enum class BufferType : uint8_t
    {
        Static,  // buffer size can be determined at compile time without a value
        Dynamic  // required buffer size can be determined only from a value
    };

    /// Special value returned by Sizeof() when the size exceeds the allowed limit
    inline constexpr uint32_t WRONG_SIZE = std::numeric_limits<uint32_t>::max();

}

//------------------------------------------------------------------------------


