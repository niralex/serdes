#ifndef SERDES_CORE_POD_HPP
#define SERDES_CORE_POD_HPP
//------------------------------------------------------------------------------
/** @file

    @brief  Serdes template for POD types (Plain Old Data)

    @details
        No conversions are performed during serialization/deserialization except
        for byte order adjustment (little-endian vs. big-endian) when necessary.

    @todo

    @author Niraleks
*/

//------------------------------------------------------------------------------
#include "Concepts.hpp"
#include "Helpers.hpp"
#include "Typeids.hpp"

//------------------------------------------------------------------------------
namespace serdes
{
    //--------------------------------------------------------------------------
    /// Serdes template for POD types
    /// @tparam TValueType Base type for which the serdes is instantiated
    /// @tparam typeId POD type identifier (from Typeids.hpp)
    template<CPod TValueType, PodTypeId typeId = PodTypeId::Unspecified>
    struct Pod
    {
        using ValueType = TValueType;

        static consteval
        TypeId GetTypeId() { return TypeId::Pod; }

        static consteval
        PodTypeId GetPodId() { return typeId; }

        static consteval
        BufferType GetBufferType() { return BufferType::Static; }

        static consteval
        std::endian GetEndianness()
        {
            return static_cast<uint8_t>(GetPodId()) & 1 ? std::endian::big : std::endian::little;
        }

        [[nodiscard]] static constexpr
        uint32_t Sizeof() { return sizeof(ValueType); }

        template<CExplicitlyConvertible<ValueType> TValue>
        [[nodiscard]] static constexpr
        uint32_t Sizeof(const TValue &) { return Sizeof(); }

        template<COutputIterator TOutputIterator, CExplicitlyConvertible<ValueType> TValue>
        static constexpr
        TOutputIterator SerializeTo(TOutputIterator bufpos, const TValue &value)
        {
            using IteratorValueType = typename std::iterator_traits<TOutputIterator>::value_type;
            using TBuffer = typename std::conditional<std::is_same_v<IteratorValueType, void>, std::uint8_t, IteratorValueType>::type;

            auto bytes = std::bit_cast<std::array<uint8_t, sizeof(ValueType)>>(static_cast<ValueType>(value));

            if constexpr (GetEndianness() == std::endian::native)
                for(auto it = bytes.begin(); it != bytes.end(); it++)
                    *bufpos++ = static_cast<TBuffer>(*it);

            else
                for(auto it = bytes.rbegin(); it != bytes.rend(); it++)
                    *bufpos++ = static_cast<TBuffer>(*it);

            return bufpos;
        }

        template<CInputIterator TInputIterator, CExplicitlyConvertible<ValueType> TValue>
        static constexpr
        TInputIterator DeserializeFrom(TInputIterator bufpos, TValue &value)
        {
            std::array<uint8_t, Sizeof()> bytes;

            if constexpr (GetEndianness() == std::endian::native)
                for(auto it = bytes.begin(); it != bytes.end(); it++)
                    *it = static_cast<uint8_t>(*bufpos++);

            else
                for(auto it = bytes.rbegin(); it != bytes.rend(); it++)
                    *it = static_cast<uint8_t>(*bufpos++);

            value = static_cast<TValue>(std::bit_cast<ValueType>(bytes));

            return bufpos;
        }
    };

    //--------------------------------------------------------------------------
    // Mixed-endian architectures are not supported
    static_assert(std::endian::native != std::endian::big || std::endian::native != std::endian::little);

} // namespace serdes

//------------------------------------------------------------------------------
#endif
