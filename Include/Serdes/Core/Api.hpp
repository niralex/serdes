#pragma once
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
    TOutputIterator Serialize(TOutputIterator bufpos, const TValues &...values)
    {
        return SerdesT<TSerdes...>::Serialize(bufpos, values...);
    }

    /// Serialization function that automatically deduces serdes from argument types using DefaultT
    template<COutputIterator TOutputIterator, typename ...TValues>
    constexpr inline
    TOutputIterator Serialize(TOutputIterator bufpos, const TValues &...values) // (2)
    {
        //return SerdesT<DefaultT<TValues>...>::Serialize(bufpos, values...);
        return DefaultT<TValues...>::Serialize(bufpos, values...);
    }

    /// Serialization for cases where arguments are initializer lists
    template<CSerdes ...TSerdes, COutputIterator TOutputIterator, typename... TValue>
    requires (sizeof...(TValue) > 0)
    constexpr inline
    TOutputIterator Serialize(TOutputIterator bufpos, const std::initializer_list<TValue> &...values)
    {
        return Serialize<TSerdes...>(bufpos, std::ranges::subrange(values.begin(), values.end())...);
    }

    /// Serialization into an automatically created std::array
    template<CSerdes ...TSerdes, typename ...TValues>
    requires (sizeof...(TSerdes) > 0)
    constexpr inline
    auto SerializeToArray(const TValues &...values)
    {
        std::array<uint8_t, Sizeof<TSerdes...>(values...)> buf;
        Serialize<TSerdes...>(buf.begin(), values...);
        return buf;
    }

    /// Serialization into an automatically created std::array with serdes automatically deduced
    /// from argument types using DefaultT
    template<typename ...TValues>
    constexpr inline
    auto SerializeToArray(const TValues &...values)
    {
        return SerializeToArray<DefaultT<TValues...>>(values...);
    }

    /// Serialization into an automatically created std::vector
    template<CSerdes ...TSerdes, typename ...TValues>
    requires (sizeof...(TSerdes) > 0)
    inline
    auto SerializeToVector(const TValues &...values)
    {
        std::vector<uint8_t> buf(Sizeof<TSerdes...>(values...));
        Serialize<TSerdes...>(buf.begin(), values...);
        return buf;
    }

	/// Serialization into an automatically created std::vector with serdes automatically deduced
    /// from argument types using DefaultT
    template<typename ...TValues>
    inline
    auto SerializeToVector(const TValues &...values)
    {
        return SerializeToVector<DefaultT<TValues...>>(values...);
    }

    //--------------------------------------------------------------------------
    /// Deserialization from an external buffer
    template<CSerdes ...TSerdes, CInputIterator TInputIterator, typename... TValues>
    requires (sizeof...(TValues) > 0 && sizeof...(TSerdes) > 0)
    constexpr inline
    TInputIterator Deserialize(TInputIterator bufpos, TValues &...values)
    {
        return SerdesT<TSerdes...>::Deserialize(bufpos, values...);
    }

    /// Deserialization from an external buffer with serdes automatically deduced
    /// from argument types using DefaultT
    template<CInputIterator TInputIterator, typename... TValues>
    constexpr inline
    TInputIterator Deserialize(TInputIterator bufpos, TValues &...values)
    {
        return DefaultT<TValues...>::Deserialize(bufpos, values...);
    }

    /// Deserialization from an external buffer with automatic value construction
    template<CSerdes ...TSerdes, CInputIterator TInputIterator>
    constexpr inline
    auto Deserialize(TInputIterator bufpos)
    {
        ValueT<SerdesT<TSerdes...>> values;
        SerdesT<TSerdes...>::Deserialize(bufpos, values);
        return values;
    }

}



