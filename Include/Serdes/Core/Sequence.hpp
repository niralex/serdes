#ifndef SERDES_CORE_SEQUENCE_HPP
#define SERDES_CORE_SEQUENCE_HPP
//------------------------------------------------------------------------------
/** @file

    @brief Serdes template for sequential containers

    @details Implemented using the Range template

    @todo

    @author Niraleks

*/
//------------------------------------------------------------------------------
#include <ranges>
#include "Math.hpp"
#include "Typeids.hpp"
#include "Concepts.hpp"
#include "Helpers.hpp"
#include "Range.hpp"

//------------------------------------------------------------------------------
namespace serdes
{
	
    /// @tparam TSizeSerdes Serdes used to serialize/deserialize the number of elements in the range
    /// @tparam TElementSerdes Serdes used to serialize/deserialize individual elements
    /// @tparam TValueType Range type
    template<
        CSerdes TSizeSerdes,
        CSerdes TElementSerdes,
        std::ranges::range TValueType>
    struct Sequence : public Range<TSizeSerdes, TElementSerdes, TValueType>
    {
        // Use SerializeTo from the base class
        using Range<TSizeSerdes, TElementSerdes, TValueType>::SerializeTo;

        template<CInputIterator TInputIterator, std::ranges::forward_range TSequence>
        static constexpr
        TInputIterator DeserializeFrom(TInputIterator bufpos, TSequence &sequence)
        {
            // Deserialize the sequence size
            ValueT<TSizeSerdes> sequenceSize{0};
            bufpos = TSizeSerdes::DeserializeFrom(bufpos, sequenceSize);

            sequence.resize(sequenceSize);

            // Deserialize elements
            auto element = std::ranges::begin(sequence);
            for(size_t i = 0; i < sequenceSize; i++)
                bufpos = TElementSerdes::DeserializeFrom(bufpos, *element++);

            return bufpos;
        }
    };

} // namespace serdes

//------------------------------------------------------------------------------
#endif