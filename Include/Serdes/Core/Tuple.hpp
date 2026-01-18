#pragma once
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
#include <limits>
#include "Math.hpp"
#include "Typeids.hpp"
#include "Concepts.hpp"
#include "Helpers.hpp"

//------------------------------------------------------------------------------
namespace serdes
{
    /// @tparam TSerdes Pack of serdes for tuple elements
    template<CSerdes ...TSerdes>
    requires (sizeof...(TSerdes) < std::numeric_limits<uint16_t>::max())
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
        uint16_t CountElements() { return sizeof...(TSerdes); }

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
        TOutputIterator Serialize(TOutputIterator bufpos, const TValue &value)
        {
            std::apply([&bufpos](const auto &...values)
            {
                ((bufpos = TSerdes::Serialize(bufpos, values)), ...);
            }, value);

            return bufpos;
        }

        /// Serialization overload for initializer lists
        /// Например Serialize({1, 2, 3});
        template<COutputIterator TOutputIterator, typename TValue>
        static constexpr
        TOutputIterator Serialize(TOutputIterator bufpos, const initializer_list<TValue> &value)
        {
            return Serialize(bufpos, std::tuple(value));
        }

        template<COutputIterator TOutputIterator, typename... TValues>
        requires (sizeof...(TSerdes) == sizeof...(TValues))
        static constexpr
        TOutputIterator Serialize(TOutputIterator bufpos, const TValues &...values)
        {
            ((bufpos = TSerdes::Serialize(bufpos, values)), ...);
            return bufpos;
        }

        template<CInputIterator TInputIterator, CTupleLike TValue>
        requires (sizeof...(TSerdes) == std::tuple_size_v<TValue>)
        static constexpr
        TInputIterator Deserialize(TInputIterator bufpos, TValue &tpl)
        {
            std::apply([&bufpos](auto &...values)
            {
                ((bufpos = TSerdes::Deserialize(bufpos, values)), ...);
            }, tpl);
            return bufpos;
        }

        template<CInputIterator TInputIterator, typename... TValues>
        requires (sizeof...(TSerdes) == sizeof...(TValues))
        static constexpr
        TInputIterator Deserialize(TInputIterator bufpos, TValues &...values)
        {
            ((bufpos = TSerdes::Deserialize(bufpos, values)), ...);
            return bufpos;
        }
    };

} // serdes

//------------------------------------------------------------------------------
