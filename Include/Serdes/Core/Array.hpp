#ifndef SERDES_CORE_ARRAY_HPP
#define SERDES_CORE_ARRAY_HPP
//------------------------------------------------------------------------------
/** @file

    @brief Шаблон седесов для массивов фиксированной длины

    @details В отличие от седеса Range, Array может сериализовывать диапазоны
        с одноразовыми (single-pass) итераторами

    @todo

    @author Niraleks

*/
//------------------------------------------------------------------------------
#include <ranges>
#include <array>
#include "Math.hpp"
#include "Concepts.hpp"
//------------------------------------------------------------------------------
namespace serdes
{
    //--------------------------------------------------------------------------
    /// @tparam TElementType Седес-тип элементов контейнера
    /// @tparam elementCount Количество сериализуемых элементов
    template<
        CSerdes TElementSerdes,
        uint32_t elementCount>
    struct Array
    {
        /// Количество сериализуемых элементов
        constexpr static
        uint32_t arraySize = elementCount;

        /// Седес для сериализации/десериализации элементов контейнера
        using ElementSerdes = TElementSerdes;

        /// Тип элементов контейнера
        using ElementBaseType = ValueT<ElementSerdes>;

        using ValueType = std::array<ElementBaseType, arraySize>;

        static consteval
        SerdesTypeId GetSerdesTypeId() { return SerdesTypeId::Array; }

        static consteval
        BufferType GetBufferType() { return ElementSerdes::GetBufferType(); }

        [[nodiscard]] static consteval
        uint32_t Sizeof()
        {
            return utils::Safe<utils::policy::MaxValue>::Mul(ElementSerdes::Sizeof(), static_cast<uint32_t>(arraySize));
        }

        /// @return Размер или WRONG_SIZE, если при вычислении результата произошло
        /// переполнение разрядной сетки или  длина диапазона меньше arraySize
        template<std::ranges::forward_range TRange>
        [[nodiscard]] static constexpr
        uint32_t Sizeof(const TRange &range)
        {
            // Размер сериализуемого диапазона должен быть не меньше arraySize
            if(std::ranges::size(range) < arraySize)
                return WRONG_SIZE;

            // Если седес элементов контейнера использует статический буфер
            if constexpr (ElementSerdes::GetBufferType() == BufferType::Static)
                return Sizeof();
            else // Cедес элементов контейнера использует динамический буфер
            {
                uint32_t bufSize = 0;
                auto element = std::ranges::begin(range);
                for(uint32_t i = 0; i < arraySize; i++)
                    bufSize = utils::Safe<utils::policy::Exception>::Add(bufSize, ElementSerdes::Sizeof(*element++));

                return bufSize;
            }
        }

        template<typename ...TValues>
        requires (sizeof...(TValues) == arraySize)
        [[nodiscard]] static constexpr
        uint32_t Sizeof(const TValues &...values)
        {
            // Если седес элементов контейнера использует статический буфер
            if constexpr (ElementSerdes::GetBufferType() == BufferType::Static)
                return Sizeof();
            else // Cедес элементов контейнера использует динамический буфер
            {
                uint32_t bufSize = 0;
                ((bufSize = utils::Safe<utils::policy::Exception>::Add(bufSize, ElementSerdes::Sizeof(values))), ...) ;
                return bufSize;
            }
        }

        /// @note Функция не проверяет выход за границы буфера и размер контейнера/диапазона. Для этого
        /// необходимо использовать функцию Array::Sizeof(const TRange &range)
        /// @note Размер контейнера/диапазона может быть больше arraySize, но будут сериализованы только первые arraySize элементов.
        template<COutputIterator TOutputIterator, std::ranges::range TRange>
        static constexpr
        auto SerializeTo(TOutputIterator bufpos, const TRange &range)
        {
            auto element = std::ranges::begin(range);
            uint32_t i;
            for(i = 0; i < arraySize && element != std::ranges::end(range); i++) // сериализация элементов диапазона
                bufpos = ElementSerdes::SerializeTo(bufpos, *element++);
            if(i == arraySize) // если сериализовано количество элементов равное размеру массива
                return bufpos;
            else throw 1;
        }

        template<COutputIterator TOutputIterator, typename ...TValues>
        requires (sizeof...(TValues) == arraySize)
        static constexpr
        auto SerializeTo(TOutputIterator bufpos, const TValues &...values)
        {
            ((bufpos = ElementSerdes::SerializeTo(bufpos, values)), ...);
            return bufpos;
        }

        template<COutputIterator TOutputIterator, typename TValue>
        static constexpr
        auto SerializeTo(TOutputIterator bufpos, const std::initializer_list<TValue> &value)
        {
            return SerializeTo(bufpos, std::ranges::subrange(value.begin(), value.end()));
        }

        template<CInputIterator TInputIterator, std::ranges::forward_range TRange>
        static constexpr
        auto DeserializeFrom(TInputIterator bufpos, TRange &range)
        {
            auto elementIt = std::ranges::begin(range);

            for(uint32_t i = 0; i < arraySize; i++)
                bufpos = ElementSerdes::DeserializeFrom(bufpos, *elementIt++);

            return bufpos;
        }

        template<CInputIterator TInputIterator, typename ...TValues>
        requires (sizeof...(TValues) == arraySize)
        static constexpr
        auto DeserializeFrom(TInputIterator bufpos, TValues &...values)
        {
            ((bufpos = ElementSerdes::DeserializeFrom(bufpos, values)), ...);
            return bufpos;
        }
    };
    //----------------------------------------------------------------------

} // serdes

//------------------------------------------------------------------------------
#endif

