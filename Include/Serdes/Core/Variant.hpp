#ifndef SERDES_CORE_VARIANT_HPP
#define SERDES_CORE_VARIANT_HPP
//------------------------------------------------------------------------------
/** @file

    @brief  Сериализатор/десериализатор одного из множества заденных типов

    @details

    @todo

    @author Niraleks
*/
//------------------------------------------------------------------------------
#include <variant>
#include <algorithm>
#include "Typeids.hpp"
#include "Concepts.hpp"
#include "Pod.hpp"

using namespace std;

//------------------------------------------------------------------------------
namespace serdes
{
    //--------------------------------------------------------------------------
    /// Шаблон седеса Variant
    /// @tparam TSerdes Список седесов для сериализации/десериализации
    template<CSerdes ...TSerdes>
    requires (sizeof...(TSerdes) > 0)
    struct Variant
    {
        using ValueType = std::variant<ValueT<TSerdes>...>;

        using SerdesList = std::tuple<TSerdes...>;

        /// Определение индекса седеса в списке TSerdes
        /// который подходит для сериализации/десериализации типа TValue
        template <typename TValue>
        requires std::disjunction_v<is_same<TValue, ValueT<TSerdes>>...>
        static consteval inline
        uint8_t GetIndex()
        {
            uint8_t index = 0;
            ((is_same_v<TValue, ValueT<TSerdes>> ? false : (index++, true)) && ...);
            return index;
        }

        template<typename T>
        using MatchSerdes = std::tuple_element_t<GetIndex<T>(), SerdesList>;

        static consteval
        SerdesTypeId GetSerdesTypeId() { return SerdesTypeId::Variant; }

        static consteval
        BufferType GetBufferType()
        {
            // Если один из составляющих седесов динамический или
            // размеры всех седесов не равны, то седес является динамическим
            // Т.е. седес Variant может быть статическим только если все составляющие
            // его седесы статические и имеют одинаковый размер Sizeof()
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

        /// Сериализация значения одного из базовых типов
        // Тип TValue должен быть одним из базовых типов седесов
        // составляющих список типов Variant
        template<COutputIterator TOutputIterator, typename TValue>
        requires std::disjunction_v<is_same<TValue, ValueT<TSerdes>>...>
        static constexpr
        TOutputIterator SerializeTo(TOutputIterator bufpos, const TValue &value)
        {
            // Сериализация индекса
            bufpos = Pod<uint8_t>::SerializeTo(bufpos, GetIndex<TValue>());

            // Сериализация значения
            return MatchSerdes<TValue>::SerializeTo(bufpos, value);
        }

        /// Сериализация базового типа
        template<COutputIterator TOutputIterator>
        static constexpr
        TOutputIterator SerializeTo(TOutputIterator bufpos, const ValueType &value)
        {
            return std::visit([bufpos](const auto &v) { return SerializeTo(bufpos, v); }, value);
        }

        /// Десериализация базового типа
        // Функция не проверяет десериализованный индекс типа
        // В случае, если индекс больше либо равен количесту типов составляющих Variant
        // десериализации не происходит, значение value сохраняется и возвращается итератор равный bufpos
        template<CInputIterator TInputIterator>
        static constexpr
        TInputIterator DeserializeFrom(TInputIterator bufpos, ValueType &value)
        {
            // Десериализация индекса седеса
            uint8_t index;
            bufpos = Pod<uint8_t>::DeserializeFrom(bufpos, index);

            // Последовательное сравнение десериализованного индекса с индексами седесов в пакете.
            // Когда индексы совпадают, выполняется десериализация значения std::variant соответствующего типа
            return [index, &value]<size_t ...I>(TInputIterator bufpos, std::index_sequence<I...>)constexpr
            {
                ((index == I ? (bufpos = std::tuple_element_t<I, SerdesList>::DeserializeFrom(bufpos, value.template emplace<I>()), false) : true) && ...);
                return bufpos;
            }(bufpos, std::make_index_sequence<sizeof...(TSerdes)>{});
        }

        /// Десериализация значения одного из составляющих типов
        // Функция может использоваться когда тип сериализованного
        // значения точно известен или когда значение одного типа нужно
        // десериализовать как значение другого(при условии совпадения размеров).
        // Ошибка типа может приводить к выходу за границы буфера.
        template<CInputIterator TInputIterator, typename TValue>
        requires std::disjunction_v<is_same<TValue, ValueT<TSerdes>>...>
        static constexpr
        TInputIterator DeserializeFrom(TInputIterator bufpos, TValue &value)
        {
            return MatchSerdes<TValue>::DeserializeFrom(bufpos + 1, value);
        }

    };

} // serdes

//------------------------------------------------------------------------------
#endif
