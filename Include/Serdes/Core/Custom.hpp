#ifndef SERDES_CORE_CUSTOM_HPP
#define SERDES_CORE_CUSTOM_HPP
//------------------------------------------------------------------------------
/** @file

    @brief Шаблоны для определения пользовательских седесов

    @details Шаблон Custom не является отдельным типом седеса, а только позволяет определить
        псевдоним другого седеса для пользовательского типа.
        Седес, на основе которого опрелен Custom-седес является базовым (Custom::BaseSerdes).
        Седес может сериализовывать как пользовательские объекты, так и значения базового типа.

        Шаблон Custom является более гибким, но медленным по сравнению с седесом Struct.

        @todo

    @author Niraleks

*/
//------------------------------------------------------------------------------
#include "Concepts.hpp"

//------------------------------------------------------------------------------
namespace serdes
{
    //--------------------------------------------------------------------------
    /// Шаблон седеса для пользовательских типов
    /// @tparam TValue Пользовательский тип
    /// @tparam TSerdes Базовый седес, который используется для пользовательского типа
    /// @tparam ValueTo Вызываемая сущность(указатель на функцию, функтор или лямбда) для
    /// преобразования значения пользовательского типа к значению типа TSerdes::ValueType
    /// @tparam ValueFrom Вызываемая сущность(указатель на функцию, функтор или лямбда)
    /// для преобразования значения типа TSerdes::ValueType в значение типа TValue
    template<typename T, CSerdes TBaseSerdes,
             auto ConvToBase = [](const ValueT<TBaseSerdes> &baseValue){ return static_cast<ValueT<TBaseSerdes>>(baseValue); }, //[](const T &ob) { return static_cast<ValueT<TBaseSerdes>>(ob); },
             auto ConvFromBase = [](ValueT<TBaseSerdes> &&baseValue, T &ob){ ob = static_cast<T>(baseValue); }>
    struct Custom
    {
        // Тип значений
        using ValueType = T;

        // Базовый седес
        using BaseSerdes = TBaseSerdes;

        // Базовый тип
        using BaseValueType = ValueT<BaseSerdes>;

        static consteval
        SerdesTypeId GetSerdesTypeId() { return BaseSerdes::GetSerdesTypeId(); }

        static consteval
        BufferType GetBufferType() { return BaseSerdes::GetBufferType(); }

        [[nodiscard]] static constexpr
        uint32_t Sizeof() { return BaseSerdes::Sizeof(); }

        template<typename TValue>
        [[nodiscard]] static constexpr
        uint32_t Sizeof(const TValue &ob)
        {
            return BaseSerdes::Sizeof(ConvToBase(ob));
        }

        [[nodiscard]] static constexpr
        uint32_t Sizeof(const BaseValueType &ob)
        {
            return BaseSerdes::Sizeof(ob);
        }

        template<COutputIterator TOutputIterator, typename TValue>
        static constexpr
        TOutputIterator SerializeTo(TOutputIterator bufpos, const TValue &ob)
        {
            return BaseSerdes::SerializeTo(bufpos, ConvToBase(ob));
        }

        template<COutputIterator TOutputIterator>
        static constexpr
        TOutputIterator SerializeTo(TOutputIterator bufpos, const BaseValueType &ob)
        {
            return BaseSerdes::SerializeTo(bufpos, ob);
        }

        template<CInputIterator TInputIterator, typename TValue>
        static constexpr
        TInputIterator DeserializeFrom(TInputIterator bufpos, TValue &ob)
        {
            BaseValueType baseVal;
            bufpos = BaseSerdes::DeserializeFrom(bufpos, baseVal);
            ConvFromBase(std::move(baseVal), ob);
            return bufpos;
        }

        template<CInputIterator TInputIterator>
        static constexpr
        TInputIterator DeserializeFrom(TInputIterator bufpos, BaseValueType &ob)
        {
            return BaseSerdes::DeserializeFrom(bufpos, ob);
        }

    };

} // serdes


//------------------------------------------------------------------------------
#endif
