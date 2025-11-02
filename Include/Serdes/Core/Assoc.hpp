#ifndef SERDES_CORE_ASSOC_HPP
#define SERDES_CORE_ASSOC_HPP
//------------------------------------------------------------------------------
/** @file

    @brief Serdes template for associative containers

    @details  Implemented using Range

    @todo

    @author Niraleks

*/
//------------------------------------------------------------------------------
#include <ranges>
#include "Concepts.hpp"
#include "Helpers.hpp"
#include "Range.hpp"

//------------------------------------------------------------------------------
namespace serdes
{
    //----------------------------------------------------------------------
    /// @tparam TSizeSerdes Serdes used to serialize/deserialize the number of elements in the range
    /// @tparam TElementSerdes Serdes used to serialize/deserialize individual elements
    /// @tparam TValueType Range type
    template<
        CSerdes TSizeSerdes,
        CSerdes TElementSerdes,
        std::ranges::range TValueType>
    struct Assoc : public Range<TSizeSerdes, TElementSerdes, TValueType>
    {
        template<CInputIterator TInputIterator, std::ranges::forward_range TAssocContainer>
        static constexpr
        TInputIterator DeserializeFrom(TInputIterator bufpos, TAssocContainer &container)
        {
            // Deserialize container size
            ValueT<TSizeSerdes> containerSize{0};
            bufpos = TSizeSerdes::DeserializeFrom(bufpos, containerSize);

            container.clear();

            // Deserialize and insert elements sequentially
            for(ValueT<TSizeSerdes> i = 0; i < containerSize; i++)
            {
                ValueT<TElementSerdes> element{};
                bufpos = TElementSerdes::DeserializeFrom(bufpos, element);
                container.insert(std::move(static_cast<std::ranges::range_value_t<TAssocContainer>>(element)));
            }

            return bufpos;
        }
    };

} // namespace serdes

//------------------------------------------------------------------------------
#endif

