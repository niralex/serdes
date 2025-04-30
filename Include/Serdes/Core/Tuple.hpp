#ifndef SERDES_CORE_TUPLE_HPP
#define SERDES_CORE_TUPLE_HPP
//------------------------------------------------------------------------------
/** @file

    @brief  Шаблон седеса для кортежа

    @details Вложенные седесы могут быть любого типа, включая другие кортежи.
        Для основных функций есть версии, которые принимают элементы кортежа в виде пакета параметров.

    @todo

    @author Niraleks
*/
//------------------------------------------------------------------------------
#include <tuple>
#include "Math.hpp"
#include "Typeids.hpp"
#include "Concepts.hpp"

//------------------------------------------------------------------------------
namespace serdes
{
    //--------------------------------------------------------------------------
    /// @tparam Ts Пакет седесов для элементов кортежа
    template<CSerdes ...TSerdes>
    struct Tuple
    {
        using ValueType = std::tuple<ValueT<TSerdes>...>;

        /// Тип определяющий список седесов для элементов кортежа
        using SerdesList = std::tuple<TSerdes...>;

        static consteval
        SerdesTypeId GetSerdesTypeId() { return SerdesTypeId::Tuple; }

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

        /// Количество элементов кортежа
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

        /// Версия функции сериализации для списока инициализации
        /// Например SerializeTo({1, 2, 3});
        template<COutputIterator TOutputIterator, typename TValue>
        static constexpr
        TOutputIterator SerializeTo(TOutputIterator bufpos, const initializer_list<TValue> &value)
        {
            return SerializeTo(bufpos, std::tuple(value));
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

} // serdes

//------------------------------------------------------------------------------
#endif
