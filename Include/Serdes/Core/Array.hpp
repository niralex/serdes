#ifndef SERDES_CORE_ARRAY_HPP
#define SERDES_CORE_ARRAY_HPP
//------------------------------------------------------------------------------
/** @file

    @brief Serdes template for fixed-size arrays

    @details

    @todo

    @author Niraleks

*/
//------------------------------------------------------------------------------
#include <ranges>
#include <array>
#include "Math.hpp"
#include "Concepts.hpp"
#include "Helpers.hpp"
#include "Exception.hpp"

//------------------------------------------------------------------------------
namespace serdes
{

    /// @tparam TElementSerdes Serdes type for container elements
    /// @tparam elementCount Number of elements to serialize
    template<
        CSerdes TElementSerdes,
        uint32_t elementCount>
    struct Array
    {
        /// Number of elements to serialize
        constexpr static
        uint32_t arraySize = elementCount;

        /// Serdes used to serialize/deserialize container elements
        using ElementSerdes = TElementSerdes;

       /// Base type of container elements
        using ElementBaseType = ValueT<ElementSerdes>;

        using ValueType = std::array<ElementBaseType, arraySize>;

        static consteval
        TypeId GetTypeId() { return TypeId::Array; }

        static consteval
        BufferType GetBufferType() { return ElementSerdes::GetBufferType(); }

        [[nodiscard]] static consteval
        uint32_t Sizeof()
        {
            return utils::Safe<utils::policy::MaxValue>::Mul(ElementSerdes::Sizeof(), static_cast<uint32_t>(arraySize));
        }

        /// @return Size or WRONG_SIZE if an overflow occurred during computation
        /// or if the range size is less than arraySize
        template<std::ranges::forward_range TRange>
        [[nodiscard]] static constexpr
        uint32_t Sizeof(const TRange &range)
        {
            // The size of the range to serialize must be at least arraySize
            if(std::ranges::size(range) < arraySize)
                return WRONG_SIZE;

            // If the element serdes uses a static buffer
            if constexpr (ElementSerdes::GetBufferType() == BufferType::Static)
                return Sizeof();
            else // the element serdes uses a dynamic buffer
            {
                uint32_t bufSize = 0;
                auto element = std::ranges::begin(range);
                for(uint32_t i = 0; i < arraySize; i++)
                    bufSize = utils::Safe<utils::policy::Exception>::Add(bufSize, ElementSerdes::Sizeof(*element++));

                return bufSize;
            }
        }

        template<typename ...TValues>
        requires (sizeof...(TValues) == arraySize)
        [[nodiscard]] static constexpr
        uint32_t Sizeof(const TValues &...values)
        {
            // If the element serdes uses a static buffer
            if constexpr (ElementSerdes::GetBufferType() == BufferType::Static)
                return Sizeof();
            else // the element serdes uses a dynamic buffer
            {
                uint32_t bufSize = 0;
                ((bufSize = utils::Safe<utils::policy::Exception>::Add(bufSize, ElementSerdes::Sizeof(values))), ...) ;
                return bufSize;
            }
        }

        /// @note This function does not check for buffer overruns or container/range size.
        /// Use Array::Sizeof(const TRange &range) for such validation.
        /// @note The container/range size may exceed arraySize, but only the first arraySize elements will be serialized.
        template<COutputIterator TOutputIterator, std::ranges::range TRange>
        static constexpr
        auto SerializeTo(TOutputIterator bufpos, const TRange &range)
        {
            auto element = std::ranges::begin(range);
            uint32_t i;
            for(i = 0; i < arraySize && element != std::ranges::end(range); i++)
                bufpos = ElementSerdes::SerializeTo(bufpos, *element++);
            if(i == arraySize)
                return bufpos;
            else
                utils::Throw<std::length_error>(std::format("input range shorter than expected array size (expected {}, got {})", arraySize, i));
        }

        /// Serialization of elements provided as a parameter pack
		template<COutputIterator TOutputIterator, typename ...TValues>
        requires (sizeof...(TValues) == arraySize)
        static constexpr
        auto SerializeTo(TOutputIterator bufpos, const TValues &...values)
        {
            ((bufpos = ElementSerdes::SerializeTo(bufpos, values)), ...);
            return bufpos;
        }

        /// Serialization of elements provided as an initializer list
		template<COutputIterator TOutputIterator, typename TValue>
        static constexpr
        auto SerializeTo(TOutputIterator bufpos, const std::initializer_list<TValue> &value)
        {
            return SerializeTo(bufpos, std::ranges::subrange(value.begin(), value.end()));
        }

        /// Deserialization into a given range
		template<CInputIterator TInputIterator, std::ranges::forward_range TRange>
        static constexpr
        auto DeserializeFrom(TInputIterator bufpos, TRange &range)
        {
            auto elementIt = std::ranges::begin(range);

            for(uint32_t i = 0; i < arraySize; i++)
                bufpos = ElementSerdes::DeserializeFrom(bufpos, *elementIt++);

            return bufpos;
        }

        /// Deserialization of elements provided as a parameter pack
		template<CInputIterator TInputIterator, typename ...TValues>
        requires (sizeof...(TValues) == arraySize)
        static constexpr
        auto DeserializeFrom(TInputIterator bufpos, TValues &...values)
        {
            ((bufpos = ElementSerdes::DeserializeFrom(bufpos, values)), ...);
            return bufpos;
        }
    };

} // serdes

//------------------------------------------------------------------------------
#endif
