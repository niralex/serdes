#ifndef SERDES_CORE_VARIANT_HPP
#define SERDES_CORE_VARIANT_HPP
//------------------------------------------------------------------------------
/** @file

    @brief  Serializer/deserializer for one of several predefined types

    @details

    @todo

    @author Niraleks
*/
//------------------------------------------------------------------------------
#include <variant>
#include <algorithm>
#include "Typeids.hpp"
#include "Concepts.hpp"
#include "Helpers.hpp"
#include "Pod.hpp"

using namespace std;

//------------------------------------------------------------------------------
namespace serdes
{
    //--------------------------------------------------------------------------
    /// Variant serdes template
    /// @tparam TSerdes List of serdes for serialization/deserialization
    template<CSerdes ...TSerdes>
    requires (sizeof...(TSerdes) > 0)
    struct Variant
    {
        using ValueType = std::variant<ValueT<TSerdes>...>;

        using SerdesList = std::tuple<TSerdes...>;

        /// Determines the index of the serdes in TSerdes that matches type TValue
        /// for serialization/deserialization
        template <typename TValue>
        requires std::disjunction_v<is_same<TValue, ValueT<TSerdes>>...>
        static consteval inline
        uint8_t GetIndex()
        {
            uint8_t index = 0;
            ((void)((is_same_v<TValue, ValueT<TSerdes>> ? false : (index++, true)) && ...));
            return index;
        }

        template<typename T>
        using MatchSerdes = std::tuple_element_t<GetIndex<T>(), SerdesList>;

        static consteval
        SerdesTypeId GetSerdesTypeId() { return SerdesTypeId::Variant; }

        static consteval
        BufferType GetBufferType()
        {
            // Variant serdes can be static only if all constituent serdes are static
            // and have the same Sizeof() value
            return (((TSerdes::GetBufferType() == BufferType::Dynamic) || ... ) ||
                    ((TSerdes::Sizeof() != std::tuple_element_t<0, std::tuple<TSerdes...>>::Sizeof()) || ...))
                    ? BufferType::Dynamic : BufferType::Static;
        }

        [[nodiscard]] static constexpr
        uint32_t Sizeof()
        {
           return std::max({TSerdes::Sizeof()...}) + 1;
        }

        template<typename TValue>
        requires std::disjunction_v<is_same<TValue, ValueT<TSerdes>>...>
        [[nodiscard]] static constexpr
        uint32_t Sizeof(const TValue &value)
        {
            return MatchSerdes<TValue>::Sizeof(value) + 1;
        }

        [[nodiscard]] static constexpr
        uint32_t Sizeof(const ValueType &value)
        {
            return std::visit([](const auto &v) { return Sizeof(v); }, value);
        }

        /// Serializes a value of one of the base types
        // TValue must be one of the base types defined by the serdes in the Variant's type list
        template<COutputIterator TOutputIterator, typename TValue>
        requires std::disjunction_v<is_same<TValue, ValueT<TSerdes>>...>
        static constexpr
        TOutputIterator SerializeTo(TOutputIterator bufpos, const TValue &value)
        {
            // Serialize the type index
            bufpos = Pod<uint8_t>::SerializeTo(bufpos, GetIndex<TValue>());

            // Serialize the value
            return MatchSerdes<TValue>::SerializeTo(bufpos, value);
        }

        /// Serializes a std::variant value
        template<COutputIterator TOutputIterator>
        static constexpr
        TOutputIterator SerializeTo(TOutputIterator bufpos, const ValueType &value)
        {
            return std::visit([bufpos](const auto &v) { return SerializeTo(bufpos, v); }, value);
        }

        /// Deserializes a std::variant value
        // This function does not validate the deserialized type index.
        // If the index is >= the number of types in the Variant,
        // no deserialization occurs, the `value` remains unchanged,
        // and the function returns the original `bufpos` iterator.
        template<CInputIterator TInputIterator>
        static constexpr
        TInputIterator DeserializeFrom(TInputIterator bufpos, ValueType &value)
        {
            // Deserialize the serdes index
            uint8_t index;
            bufpos = Pod<uint8_t>::DeserializeFrom(bufpos, index);

            // Sequentially compare the deserialized index with the indices of serdes in the pack.
            // When a match is found, deserialize into the std::variant at the corresponding type index.
            return [index, &value]<size_t ...I>(TInputIterator bufpos, std::index_sequence<I...>) constexpr
            {
                ((void)((index == I ? (bufpos = std::tuple_element_t<I, SerdesList>::DeserializeFrom(bufpos, value.template emplace<I>()), false) : true) && ...));
                return bufpos;
            }(bufpos, std::make_index_sequence<sizeof...(TSerdes)>{});
        }

        /// Deserializes a value of one of the constituent types
        // This function can be used when the serialized type is known exactly,
        // or when deserializing a value of one type as another (provided sizes match).
        // Type mismatches may lead to buffer overruns.
        template<CInputIterator TInputIterator, typename TValue>
        requires std::disjunction_v<is_same<TValue, ValueT<TSerdes>>...>
        static constexpr
        TInputIterator DeserializeFrom(TInputIterator bufpos, TValue &value)
        {
            return MatchSerdes<TValue>::DeserializeFrom(bufpos + 1, value);
        }
    };

} // namespace serdes

//------------------------------------------------------------------------------
#endif