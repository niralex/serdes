#ifndef SERDES_CORE_STRUCT_HPP
#define SERDES_CORE_STRUCT_HPP
//------------------------------------------------------------------------------
/** @file

    @brief  Шаблон седесов для простых пользовательских структур

    @details

        Шаблон Struct не является отдельным типом седеса, а только позволяет определить псевдоним другого седеса
        для пользовательской структуры/класса. Поля структуры/класса должны быть либо открытыми, либо седес должен
        быть объявлен другом.
        Седес, на основе которого опрелен Struct-седес, является базовым (Struct::BaseSerdes)

        Задача решаемая шаблоном Struct - представить пользовательский тип в виде комбинации
        базовых типов, для которых уже определены седесы. Struct-седесы имеют такие же идентификаторы и тот же
        дескриптор, как у базового седеса.

        @todo

    @author Niraleks

*/
//------------------------------------------------------------------------------
#include "Typeids.hpp"
#include "Concepts.hpp"

//------------------------------------------------------------------------------
namespace serdes
{
    //--------------------------------------------------------------------------
    /// Шаблон седеса для простых структур
    /// @tparam TStruct тип структуры/класса для которой определяется седес
    /// @tparam TSerdes Седес который будет использоваться как базовый
    /// @tparam Fields указатели на поля-члены структуры/класса
    template<typename TStruct, CSerdes TSerdes, auto ...Fields>
    struct Struct
    {
        using ValueType = TStruct;

        /// Базовый седес, который используется для сериализации/десериализации структуры
        using BaseSerdes = TSerdes;

        static consteval
        SerdesTypeId GetSerdesTypeId() { return BaseSerdes::GetSerdesTypeId(); }

        static consteval
        BufferType GetBufferType() { return BaseSerdes::GetBufferType(); }

        [[nodiscard]] static constexpr
        uint32_t Sizeof() { return BaseSerdes::Sizeof(); }


        [[nodiscard]] static constexpr
        uint32_t Sizeof(const ValueType &ob)
        {
            return BaseSerdes::Sizeof((ob.*Fields)...);
        }

        template<COutputIterator TOutputIterator>
        static constexpr
        TOutputIterator SerializeTo(TOutputIterator bufpos, const ValueType &ob)
        {
            return BaseSerdes::SerializeTo(bufpos, (ob.*Fields)...);
        }

        template<CInputIterator TInputIterator, typename TValue>
        static constexpr
        TInputIterator DeserializeFrom(TInputIterator bufpos, TValue &ob)
        {
            return BaseSerdes::DeserializeFrom(bufpos, (ob.*Fields)...);
        }
    };

} // serdes


//------------------------------------------------------------------------------
#endif

