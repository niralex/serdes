#ifndef SERDES_CORE_RANGE_HPP
#define SERDES_CORE_RANGE_HPP
//------------------------------------------------------------------------------
/** @file

    @brief Base serdes template for ranges.

    @details
        The serialized data format is identical for all derived serdes.

    @todo

    @author Niraleks

*/
//------------------------------------------------------------------------------
#include <ranges>
#include "Math.hpp"
#include "Typeids.hpp"
#include "Concepts.hpp"
#include "Helpers.hpp"

//------------------------------------------------------------------------------
namespace serdes
{

    /// Serdes template for ranges
    /// @tparam TSizeSerdes Serdes used to serialize/deserialize the number of elements in the range
    /// @tparam TElementSerdes Serdes used to serialize/deserialize individual elements
    /// @tparam TValueType Range type
    template<
        CSerdes TSizeSerdes,
        CSerdes TElementSerdes,
        std::ranges::range TValueType>
    requires (TSizeSerdes::Sizeof() == 1 || TSizeSerdes::Sizeof() == 2 || TSizeSerdes::Sizeof() == 4)
    struct Range
    {
        /// Serdes for serializing/deserializing the range size
        using SizeSerdes = TSizeSerdes;

        /// Range size type
        using SizeType = ValueT<SizeSerdes>;

        /// Serdes for serializing/deserializing range elements
        using ElementSerdes = TElementSerdes;

        /// Element type of the range
        using ElementType = ValueT<ElementSerdes>;

        using ValueType = TValueType;

        /// Length of the size field (in bytes)
        static constexpr uint8_t sizelen = sizeof(ValueT<TSizeSerdes>);

        static consteval
        TypeId GetTypeId() { return TypeId::Range; }

        [[nodiscard]] static consteval
        BufferType GetBufferType() { return BufferType::Dynamic; }

        [[nodiscard]] static consteval
        uint32_t Sizeof()
        {
            return utils::Safe<utils::policy::MaxValue>::Add(
                    static_cast<uint32_t>(sizelen),
                    utils::Safe<utils::policy::MaxValue>::Mul(
                        ElementSerdes::Sizeof(),
                        static_cast<uint32_t>(std::numeric_limits<ValueT<TSizeSerdes>>::max())));
        }

        /// @note This function may return WRONG_SIZE if an overflow occurs during computation
        /// or if the range size exceeds the maximum allowed value
        template<std::ranges::forward_range TRange>
        [[nodiscard]] static constexpr
        uint32_t Sizeof(const TRange &range)
        {
            // Validate range size
            if(std::ranges::size(range) > std::numeric_limits<SizeType>::max())
                return WRONG_SIZE;

            uint32_t bufSize = SizeSerdes::Sizeof();

            // If the element serdes uses a static buffer
            if constexpr (ElementSerdes::GetBufferType() == BufferType::Static)
                bufSize = utils::Safe<utils::policy::MaxValue>::Add(bufSize,
                                    utils::Safe<utils::policy::MaxValue>::Mul(static_cast<uint32_t>(std::ranges::size(range)),
                                                                ElementSerdes::Sizeof()));

            else // The element serdes uses a dynamic buffer
                for(const auto &value: range)
                    bufSize = utils::Safe<utils::policy::MaxValue>::Add(bufSize, ElementSerdes::Sizeof(value));

            return bufSize;
        }

        template<COutputIterator TOutputIterator, std::ranges::forward_range TRange>
        static constexpr
        TOutputIterator SerializeTo(TOutputIterator bufpos, const TRange &range)
        {
            // Serialize the range size
            bufpos = SizeSerdes::SerializeTo(bufpos, std::ranges::size(range));

            // Serialize range elements
            for(const auto &element: range)
                bufpos = ElementSerdes::SerializeTo(bufpos, element);

            return bufpos;
        }
    };

} // namespace serdes

//------------------------------------------------------------------------------
#endif
