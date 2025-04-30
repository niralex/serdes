#ifndef SERDES_CORE_API_HPP
#define SERDES_CORE_API_HPP

//------------------------------------------------------------------------------
/** @file

	@brief Пользовательский интерфейс библиотеки

	@details

    @todo

    @author Niraleks

*/

//------------------------------------------------------------------------------
#include "Concepts.hpp"
#include "Typedefs.hpp"

//------------------------------------------------------------------------------
namespace serdes
{
    //--------------------------------------------------------------------------
    // Шаблон для формирования типа седеса из пакета параметров-типов седесов
    namespace details
    {
        template<CSerdes... TSerdes>
        struct SerdesType;

        // Специализация для одного типа — возвращаем сам тип
        template<CSerdes TSerdes>
        struct SerdesType<TSerdes>
        {
            using Type = TSerdes;
        };

        // Специализация для нескольких типов — возвращаем tuple
        template<CSerdes TSerdes1, CSerdes TSerdes2, CSerdes... TSerdesRest>
        struct SerdesType<TSerdes1, TSerdes2, TSerdesRest...>
        {
            using Type = Tuple<TSerdes1, TSerdes2, TSerdesRest...>;
        };

        // Специализация для пустого списка — возвращаем tuple
        template<>
        struct SerdesType<>
        {
            using Type = Tuple<>;
        };
    }

    template<CSerdes ...TSerdes>
    using SerdesT = typename details::SerdesType<TSerdes...>::Type;

    /// Функция возвращает тип буфера (статический или динамический)
    template<CSerdes ...TSerdes>
    [[nodiscard]] static consteval
    BufferType GetBufferType() { return SerdesT<TSerdes...>::GetBufferType(); }

    /// Определение размера буфера небходимого для сериализации с помощью данного седеса
    /// @tparam TSerdes Пакет седесов
    /// @return Размер буфера или WRONG_SIZE в случае превышения допустимого размера
    /// @note Функция возвращает максимально возможный размер буфера, достаточный для
    /// сохранения любых значений базовых типов седесов
    template<CSerdes ...TSerdes>
    [[nodiscard]] consteval inline
    uint32_t Sizeof() { return SerdesT<TSerdes...>::Sizeof(); }

    /// Определение размера буфера небходимого для сериализации конкретных значений
    /// @param values Значения базовых типов или типов, которые могут быть неявно к ним приведены
    /// @return Размер буфера или WRONG_SIZE в случае превышения допустимого размера
    /// @note В отличие от версии функции Sizeof без параметров, данная функция, в случае
    /// динамического буфера, возвращает точный размер, конкретно для указанных значений.
    template<CSerdes ...TSerdes, typename ...TValues>
    //requires (sizeof...(TSerdes) > 1)
    [[nodiscard]] inline constexpr
    uint32_t Sizeof(const TValues &...values)
    {
        return SerdesT<TSerdes...>::Sizeof(values...);
    }

    // Базовая функция сериализации(вызывается другими функциями сериализации)
    template<CSerdes ...TSerdes, COutputIterator TOutputIterator, typename ...TValues>
    //requires (sizeof...(TSerdes) > 1)
    //requires (!std::is_convertible_v<TValues, ValueT<TSerdes>> || ...) // типы не должны неявно приводится(для исключения неоднозначности)
    constexpr inline
    TOutputIterator SerializeTo(TOutputIterator bufpos, const TValues &...values) // (2)
    {
        return SerdesT<TSerdes...>::SerializeTo(bufpos, values...);
        //((bufpos = TSerdes::SerializeTo(bufpos, values)), ...);
        //return TSerdes::SerializeTo(bufpos, values...);
    }

    // Сериализация для случаев, когда аргументами являются списки инициализации
    template<CSerdes ...TSerdes, COutputIterator TOutputIterator, typename... TValue>
    requires (sizeof...(TValue) > 0)
    constexpr inline
    TOutputIterator SerializeTo(TOutputIterator bufpos, const std::initializer_list<TValue> &...values) // (1)
    {
        return SerializeTo<TSerdes...>(bufpos, std::ranges::subrange(values.begin(), values.end())...);
    }

    // Сериализация в автоматически создаваемый буфер
    // Тип буфера выбирается автоматически, в зависимости от типа седеса
    // Функция для простейших случаев. Для более сложных вариантов необходимо использовать
    // различные механизмы управления буфером
    template<CSerdes ...TSerdes, typename ...TValues>
    constexpr inline
    auto Serialize(const TValues &...values)
    {
        if constexpr (GetBufferType<TSerdes...>() == BufferType::Static)
        {
            std::array<uint8_t, Sizeof<TSerdes...>()> buf;
            SerializeTo<TSerdes...>(buf.begin(), values...);
            return buf;
        }
        else
        {
            std::vector<uint8_t> buf(Sizeof<TSerdes...>(values...));
            SerializeTo<TSerdes...>(buf.begin(), values...);
            return buf;
        }
    }

    //--------------------------------------------------------------------------

    /// Десериализация из внешнего буфера
    template<CSerdes ...TSerdes, CInputIterator TInputIterator, typename... TValues>
    requires (sizeof...(TValues) > 0)
    constexpr inline
    TInputIterator DeserializeFrom(TInputIterator bufpos, TValues &...values)
    {
        return SerdesT<TSerdes...>::DeserializeFrom(bufpos, values...);
    }

    /// Десериализация из внешнего буфера с автоматическим созданием значений
    template<CSerdes ...TSerdes, CInputIterator TInputIterator>
    constexpr inline
    auto DeserializeFrom(TInputIterator bufpos)
    {
        ValueT<SerdesT<TSerdes...>> values;
        SerdesT<TSerdes...>::DeserializeFrom(bufpos, values);
        return values;
    }

//
//    /// Сериализация во внешний буфер
//    /// @param buffer Итератор на позицию в подготовленном буфере или буфер/контейнер/массив
//    /// достаточного размера, куда будет записан результат сериализации
//    /// @param values Ссылка на значения, которые необходимо сериализовать.
//    /// @return Итератор на позицию в буфере, следующую за сериализованными данными
//    /// @note Функция не проверяет выход за границы буфера
//    template<CSerdes TSerdes, COutputIterator TOutputIterator,  typename ...TValues>
//    constexpr inline
//    TOutputIterator Serialize(TOutputIterator bufpos, const TValues &...values)
//    {
//        return TSerdes::SerializeTo(bufpos, values...);
//    }
//
//    /// Десериализация из внешнего буфера
//    template<CSerdes TSerdes, CInputIterator TInputIterator, typename ...TValues>
//    constexpr inline
//    TInputIterator Deserialize(TInputIterator bufpos, TValues &...values)
//    {
//        return TSerdes::DeserializeFrom(bufpos, values...);
//    }
//
//    /// Сериализация в serdes::Buffer
//    template<CBuffer TBuffer, typename ...TValues>
//    constexpr inline
//    void Serialize(TBuffer &buffer, const TValues &...values)
//    {
//        buffer.Serialize(values...);
//    }
//
//    /// Десериализация из буфера serdes::Buffer с автоматическим созданием значений
//    template<CBuffer TBuffer>
//    [[nodiscard]] constexpr inline
//    auto Deserialize(const TBuffer &buffer)
//    {
//        return Deserialize<typename TBuffer::Serdes>(buffer.begin());
//    }

//    template<CSerdes ...TSerdes>
//    struct Serdes
//    {
//        /// Функции сериализации в подготовленный заранее буфер
//        /// @param buffer Итератор на позицию в подготовленном буфере или буфер(контейнер, массив)
//        /// достаточного размера, куда будет записан результат сериализации
//        /// @param value(s) Ссылка на значения, которые необходимо сериализовать.
//        /// @return Итератор на позицию в буфере, следующую за сериализованными данными
//        /// @note Функции не проверяет выход за границы буфера
//        /// @note Несколько версий функции реализованы для различных комбинаций параметров и списков параметров
//        template<typename TValue, COutputBuffer TOutputBuffer>
//        constexpr
//        auto Serialize(TOutputBuffer &&buffer, const TValue &value)
//        {
//            return TSerdes::Serialize(Begin(buffer), value);
//        }
//    }

}

//------------------------------------------------------------------------------
#endif

