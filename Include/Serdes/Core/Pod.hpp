#ifndef SERDES_CORE_POD_HPP
#define SERDES_CORE_POD_HPP
//------------------------------------------------------------------------------
/** @file

    @brief  Шаблон седесов для Pod-типов(Plain Old Data)

    @details
        Никаких преобразований при сериализации/десериализации не выполняется, кроме
        изменения порядка байт если это необходимо(little-endian/big-endian).

    @todo

    @author Niraleks
*/

//------------------------------------------------------------------------------
#include "Concepts.hpp"
#include "Typeids.hpp"

//------------------------------------------------------------------------------
namespace serdes
{

    //--------------------------------------------------------------------------
    /// Шаблон седеса для POD-типов
    /// @tparam typeId Идентификатор POD-типа (Typeids.hpp)
    /// @tparam TValueType Базовый тип для которого инстанцируется седес
    template<CPod TValueType, PodTypeId typeId = PodTypeId::Unspecified>
    struct Pod
    {
        using ValueType = TValueType;

        static consteval
        SerdesTypeId GetSerdesTypeId() { return SerdesTypeId::Pod; }

        static consteval
        BufferType GetBufferType() { return BufferType::Static; }

        static consteval
        PodTypeId GetTypeId() { return typeId; }

        static consteval
        std::endian GetEndianness()
        {
            return static_cast<uint8_t>(GetTypeId()) & 1 ? std::endian::big : std::endian::little;
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
            // Определение типа буфера по типу итератора. Если тип определен как void
            // (возможно в случае использоввания ostream_iterator) то он заменяется на uint8_t.
            using IteratorValueType = typename std::iterator_traits<TOutputIterator>::value_type;
            using TBuffer = typename std::conditional<std::is_same_v<IteratorValueType, void>, std::uint8_t, IteratorValueType>::type;

            // Преобразование заданного значения в набор байт в виде std::array<uint8_t, N>
            auto bytes = std::bit_cast<std::array<uint8_t, sizeof(ValueType)>>(static_cast<ValueType>(value));

            // Если порядок байт на текущей платформе совпадает с целевым
            // то выполняется прямое копирование в буфер байт значения
            if constexpr (GetEndianness() == std::endian::native)
                for(auto it = bytes.begin(); it != bytes.end(); it++)
                    *bufpos++ = static_cast<TBuffer>(*it);

            else // иначе порядок байт на текущей платформе не совпадает с целевым
                 // и байты значения копируются в буфер в обратном порядке
                for(auto it = bytes.rbegin(); it != bytes.rend(); it++)
                    *bufpos++ = static_cast<TBuffer>(*it);

            return bufpos;
        }

        template<CInputIterator TInputIterator, CExplicitlyConvertible<ValueType> TValue>
        static constexpr
        TInputIterator DeserializeFrom(TInputIterator bufpos, TValue &value)
        {
            std::array<uint8_t, Sizeof()> bytes; // контейнер для байт десериализуеммого значения

            // Eсли порядок байт на текущей платформе совпадает с целевым
            if constexpr (GetEndianness() == std::endian::native)
                for(auto it = bytes.begin(); it != bytes.end(); it++) // копирование байт из буфера в том же порядке
                    *it = static_cast<uint8_t>(*bufpos++);

            else // порядок байт на текущей платформе не совпадает с целевым
                for(auto it = bytes.rbegin(); it != bytes.rend(); it++) // копирование байт из буфера в обратном порядке
                    *it = static_cast<uint8_t>(*bufpos++);

            value = static_cast<TValue>(std::bit_cast<ValueType>(bytes)); // преобразование набора байт в значение оригинального типа и приведение к заданному типу

            return bufpos;
        }

    };

    //--------------------------------------------------------------------------
    // Архитектуры с mixed-endian не поддерживаются
    static_assert(std::endian::native != std::endian::big || std::endian::native != std::endian::little);

} // serdes

//------------------------------------------------------------------------------
#endif
