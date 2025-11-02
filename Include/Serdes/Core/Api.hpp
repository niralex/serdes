#ifndef SERDES_CORE_API_HPP
#define SERDES_CORE_API_HPP

//------------------------------------------------------------------------------
/** @file

	@brief Library user interface

	@details

    @todo

    @author Niraleks

*/

//------------------------------------------------------------------------------
#include "Concepts.hpp"
#include "Helpers.hpp"
#include "Default.hpp"
#include "Typedefs.hpp"

//------------------------------------------------------------------------------
namespace serdes
{
    /// Function returns the buffer type (static or dynamic)
    template<CSerdes ...TSerdes>
    [[nodiscard]] static consteval
    BufferType GetBufferType() { return SerdesT<TSerdes...>::GetBufferType(); }

    /// Determines the buffer size required for serialization using the given serdes
    /// @tparam TSerdes Pack of serdes
    /// @return Buffer size or WRONG_SIZE if the allowed size is exceeded
    /// @note The function returns the maximum possible buffer size sufficient to store
    /// any values of the base types of the serdes
    template<CSerdes ...TSerdes>
    [[nodiscard]] consteval inline
    uint32_t Sizeof() { return SerdesT<TSerdes...>::Sizeof(); }

    /// Determines the buffer size required to serialize specific values
    /// @param values Values of base types or types implicitly convertible to them
    /// @return Buffer size or WRONG_SIZE if the allowed size is exceeded
    /// @note Unlike the parameterless version of Sizeof, this function returns the exact size
    /// required for the given values in the case of a dynamic buffer.
    template<CSerdes ...TSerdes, typename ...TValues>
    requires (sizeof...(TSerdes) > 0)
    [[nodiscard]] inline constexpr
    uint32_t Sizeof(const TValues &...values)
    {
        return SerdesT<TSerdes...>::Sizeof(values...);
    }

    /// Determines the buffer size required to serialize specific values,
    /// automatically deducing serdes from argument types using DefaultT
    template<typename ...TValues>
    [[nodiscard]] inline constexpr
    uint32_t Sizeof(const TValues &...values)
    {
        return DefaultT<TValues...>::Sizeof(values...);
    }

    /// Core serialization function (also called by other serialization functions)
    template<CSerdes ...TSerdes, COutputIterator TOutputIterator, typename ...TValues>
    requires (sizeof...(TSerdes) > 0)
    constexpr inline
    TOutputIterator SerializeTo(TOutputIterator bufpos, const TValues &...values) 
    {
        return SerdesT<TSerdes...>::SerializeTo(bufpos, values...);
    }

    // Serialization function that automatically deduces serdes from argument types using DefaultT
    template<COutputIterator TOutputIterator, typename ...TValues>
    constexpr inline
    TOutputIterator SerializeTo(TOutputIterator bufpos, const TValues &...values) 
    {
        return DefaultT<TValues...>::SerializeTo(bufpos, values...);
    }

    /// Serialization for cases where arguments are initializer lists
    template<CSerdes ...TSerdes, COutputIterator TOutputIterator, typename... TValue>
    requires (sizeof...(TValue) > 0)
    constexpr inline
    TOutputIterator SerializeTo(TOutputIterator bufpos, const std::initializer_list<TValue> &...values) // (1)
    {
        return SerializeTo<TSerdes...>(bufpos, std::ranges::subrange(values.begin(), values.end())...);
    }

    /// Serialization into an automatically created buffer
    /// The buffer type is chosen automatically based on the serdes type
    /// This function is intended for the simplest use cases. For more complex scenarios,
    /// explicit buffer management mechanisms should be used.
    template<CSerdes ...TSerdes, typename ...TValues>
    requires (sizeof...(TSerdes) > 0)
    constexpr inline
    auto Serialize(const TValues &...values)
    {
        if constexpr (GetBufferType<TSerdes...>() == BufferType::Static)
        {
            std::array<uint8_t, Sizeof<TSerdes...>()> buf;
            SerializeTo<TSerdes...>(buf.begin(), values...);
            return buf;
        }
        else
        {
            std::vector<uint8_t> buf(Sizeof<TSerdes...>(values...));
            SerializeTo<TSerdes...>(buf.begin(), values...);
            return buf;
        }
    }

    /// Serialization into an automatically created buffer with serdes automatically deduced
    /// from argument types using DefaultT
    template<typename ...TValues>
    constexpr inline
    auto Serialize(const TValues &...values)
    {
        return Serialize<DefaultT<TValues...>>(values...);
    }

    /// Deserialization from an external buffer
    template<CSerdes ...TSerdes, CInputIterator TInputIterator, typename... TValues>
    requires (sizeof...(TValues) > 0 && sizeof...(TSerdes) > 0)
    constexpr inline
    TInputIterator DeserializeFrom(TInputIterator bufpos, TValues &...values)
    {
        return SerdesT<TSerdes...>::DeserializeFrom(bufpos, values...);
    }

    /// Deserialization from an external buffer with serdes automatically deduced
    /// from argument types using DefaultT
    template<CInputIterator TInputIterator, typename... TValues>
    constexpr inline
    TInputIterator DeserializeFrom(TInputIterator bufpos, TValues &...values)
    {
        return DefaultT<TValues...>::DeserializeFrom(bufpos, values...);
    }

    /// Deserialization from an external buffer with automatic value construction
    template<CSerdes ...TSerdes, CInputIterator TInputIterator>
    constexpr inline
    auto DeserializeFrom(TInputIterator bufpos)
    {
        ValueT<SerdesT<TSerdes...>> values;
        SerdesT<TSerdes...>::DeserializeFrom(bufpos, values);
        return values;
    }

}

//------------------------------------------------------------------------------
#endif

