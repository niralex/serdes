#ifndef SERDES_CORE_TUPLE_HPP
#define SERDES_CORE_TUPLE_HPP
//------------------------------------------------------------------------------
/** @file

    @brief  Serdes template for tuples

    @details Nested serdes can be of any type, including other tuples.
        The main functions of the Tuple serdes have overloaded versions
        that accept tuple elements as a parameter pack.

    @todo

    @author Niraleks
*/
//------------------------------------------------------------------------------
#include <tuple>
#include "Math.hpp"
#include "Typeids.hpp"
#include "Concepts.hpp"
#include "Helpers.hpp"

//------------------------------------------------------------------------------
namespace serdes
{
    /// @tparam TSerdes Pack of serdes for tuple elements
    template<CSerdes ...TSerdes>
    struct Tuple
    {
        using ValueType = std::tuple<ValueT<TSerdes>...>;

        /// Type defining the list of serdes for tuple elements
        using SerdesList = std::tuple<TSerdes...>;

        static consteval
        TypeId GetTypeId() { return TypeId::Tuple; }

        static consteval
        BufferType GetBufferType()
        {
            return ((TSerdes::GetBufferType() == BufferType::Dynamic) || ... ) ? BufferType::Dynamic : BufferType::Static;
        }

        [[nodiscard]] static constexpr
        uint32_t Sizeof()
        {
            uint32_t n = 0;
            ((n = utils::Safe<utils::policy::MaxValue>::Add(TSerdes::Sizeof(), n)), ...);
            return n;
        }

        /// Number of tuple elements
        [[nodiscard]] static consteval
        size_t GetSize() { return sizeof...(TSerdes); }

        template<CTupleLike TValue>
        requires (sizeof...(TSerdes) == std::tuple_size_v<TValue>)
        [[nodiscard]] static constexpr
        uint32_t Sizeof(const TValue &tpl)
        {
            return std::apply([](auto &...values)
            {
                uint32_t valueSize = 0;
                ((valueSize = utils::Safe<utils::policy::MaxValue>::Add(TSerdes::Sizeof(values), valueSize)), ...);
                return valueSize;
            }, tpl);
        }

        template<typename... TValues>
        requires (sizeof...(TSerdes) == sizeof...(TValues))
        [[nodiscard]] static constexpr
        uint32_t Sizeof(const TValues &...values)
        {
            uint32_t valueSize = 0;
            ((valueSize = utils::Safe<utils::policy::MaxValue>::Add(TSerdes::Sizeof(values), valueSize)), ...);
            return valueSize;
        }

        template<COutputIterator TOutputIterator, CTupleLike TValue>
        requires (sizeof...(TSerdes) == std::tuple_size_v<TValue>)
        static constexpr
        TOutputIterator SerializeTo(TOutputIterator bufpos, const TValue &value)
        {
            std::apply([&bufpos](const auto &...values)
            {
                ((bufpos = TSerdes::SerializeTo(bufpos, values)), ...);
            }, value);

            return bufpos;
        }

        /// Serialization overload for initializer lists
        /// For example: SerializeTo({1, 2, 3});
        template<COutputIterator TOutputIterator, typename TValue>
        static constexpr
        TOutputIterator SerializeTo(TOutputIterator bufpos, const std::initializer_list<TValue> &value)
        {
            return SerializeTo(bufpos, std::tuple(value.begin(), value.end()));
        }

        template<COutputIterator TOutputIterator, typename... TValues>
        requires (sizeof...(TSerdes) == sizeof...(TValues))
        static constexpr
        TOutputIterator SerializeTo(TOutputIterator bufpos, const TValues &...values)
        {
            ((bufpos = TSerdes::SerializeTo(bufpos, values)), ...);
            return bufpos;
        }

        template<CInputIterator TInputIterator, CTupleLike TValue>
        requires (sizeof...(TSerdes) == std::tuple_size_v<TValue>)
        static constexpr
        TInputIterator DeserializeFrom(TInputIterator bufpos, TValue &tpl)
        {
            std::apply([&bufpos](auto &...values)
            {
                ((bufpos = TSerdes::DeserializeFrom(bufpos, values)), ...);
            }, tpl);
            return bufpos;
        }

        template<CInputIterator TInputIterator, typename... TValues>
        requires (sizeof...(TSerdes) == sizeof...(TValues))
        static constexpr
        TInputIterator DeserializeFrom(TInputIterator bufpos, TValues &...values)
        {
            ((bufpos = TSerdes::DeserializeFrom(bufpos, values)), ...);
            return bufpos;
        }
    };

} // namespace serdes

//------------------------------------------------------------------------------
#endif
