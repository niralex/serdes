#pragma once
//------------------------------------------------------------------------------
/**	@file

    @brief Helper constructs

	@details Intended to shorten the notation of certain template constructs.

    @todo

    @author Niraleks
*/
//------------------------------------------------------------------------------
#include <iterator>
#include <limits>
#include "Typeids.hpp"
#include "Concepts.hpp"

//------------------------------------------------------------------------------
namespace serdes
{

    /// Template providing a shorthand for accessing the base value type of a serdes
    template<CSerdes TSerdes>
    using ValueT = typename TSerdes::ValueType;

    template<CSerdes ...TSerdes>
    requires (sizeof...(TSerdes) < std::numeric_limits<uint16_t>::max())
    struct Tuple;

    namespace details
    {
        template<CSerdes... TSerdes>
        struct SerdesType;

        // Specialization for a single type — return the type itself
        template<CSerdes TSerdes>
        struct SerdesType<TSerdes>
        {
            using Type = TSerdes;
        };

        // Specialization for multiple types — return a Tuple
        template<CSerdes TSerdes1, CSerdes TSerdes2, CSerdes... TSerdesRest>
        struct SerdesType<TSerdes1, TSerdes2, TSerdesRest...>
        {
            using Type = Tuple<TSerdes1, TSerdes2, TSerdesRest...>;
        };

        // Specialization for an empty list — use an empty tuple
        template<>
        struct SerdesType<>
        {
            using Type = Tuple<>;
        };
    }

    /// Helper alias to simplify obtaining a serdes type.
    // The main purpose of this helper is to unify handling of serdes lists
    // of any size, including zero.
    template<CSerdes ...TSerdes>
    using SerdesT = typename details::SerdesType<TSerdes...>::Type;

} // serdes

//------------------------------------------------------------------------------


