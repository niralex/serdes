#pragma once
//------------------------------------------------------------------------------
/** @file

    @brief Serdes template for strings

    @details
        This template is similar to Sequence but provides an additional overload
        for serializing string literals, where the null terminator '\0'
        is ignored during serialization and deserialization.

    @todo

    @author Niraleks

*/
//------------------------------------------------------------------------------
#include <ranges>
#include "Typeids.hpp"
#include "Concepts.hpp"
#include "Helpers.hpp"
#include "Sequence.hpp"

//------------------------------------------------------------------------------
namespace serdes
{
    /// @tparam TSizeSerdes Serdes used to serialize/deserialize the number of characters in the string
    /// @tparam TCharSerdes Serdes used to serialize/deserialize individual characters
    /// @tparam TAllocator Callable entity for memory allocation
    template<
        CPodSerdes TSizeSerdes,
        CSerdes TCharSerdes,
        typename TAllocator = std::allocator<ValueT<TCharSerdes>>>
    struct BaseString : public Sequence<TSizeSerdes, TCharSerdes, std::basic_string<ValueT<TCharSerdes>, std::char_traits<ValueT<TCharSerdes>>, TAllocator>>
    {
        // Use SerializeTo from the base class
        using Sequence<TSizeSerdes, TCharSerdes, std::basic_string<ValueT<TCharSerdes>, std::char_traits<ValueT<TCharSerdes>>, TAllocator>>::Serialize;

        // Overload for serializing string literals
        // Deserialization can be performed using any Range-based serdes into a compatible container
        template<COutputIterator TOutputIterator, typename TValue, size_t N>
        static constexpr
        TOutputIterator Serialize(TOutputIterator bufpos, const TValue (&cstr)[N])
        {
            // String length excluding the null terminator
            constexpr size_t sz = N-1;

            // Serialize the length
            bufpos = TSizeSerdes::Serialize(bufpos, sz);

            // Serialize the string characters
            for(size_t i = 0; i < sz; i++)
                bufpos = TCharSerdes::Serialize(bufpos, cstr[i]);

            return bufpos;
        }

    };

} // namespace serdes

//------------------------------------------------------------------------------

