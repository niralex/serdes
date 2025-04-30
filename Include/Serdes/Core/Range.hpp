#ifndef SERDES_CORE_RANGE_HPP
#define SERDES_CORE_RANGE_HPP
//------------------------------------------------------------------------------
/** @file

    @brief Шаблон седесов для динамических диапазонов

    @details
        При сериализации в начало буфера записывается количество элементов диапазона,
        а затем сериализуются сами элементы.

        @todo

    @author Niraleks

*/
//------------------------------------------------------------------------------
#include <ranges>
#include "Math.hpp"
#include "Typeids.hpp"
#include "Concepts.hpp"

//------------------------------------------------------------------------------
namespace serdes
{
    //--------------------------------------------------------------------------
    /// Специальное значение, которое возвращает функция Sizeof(),
    /// если размер значения превышает допустимый
    inline constexpr uint32_t WRONG_SIZE = std::numeric_limits<uint32_t>::max();

    //----------------------------------------------------------------------
    /// Шаблон седеса для диапазонов
    /// @tparam TSizeSerdes Седес для сериализации/десериализации количества элементов диапазона
    /// @tparam TElementSerdes Седес для сериализации/десериализации элементов
    /// @tparam TValueType Тип диапазона
    template<
        CSerdes TSizeSerdes,
        CSerdes TElementSerdes,
        std::ranges::range TValueType>
    requires (TSizeSerdes::Sizeof() == 1 || TSizeSerdes::Sizeof() == 2 || TSizeSerdes::Sizeof() == 4)
    struct Range
    {
        /// Седес для сериализации/десериализации размера диапазона
        using SizeSerdes = TSizeSerdes;

        /// Тип размера диапазона
        using SizeType = ValueT<SizeSerdes>;

        /// Седес для сериализации/десериализации элементов диапазона
        using ElementSerdes = TElementSerdes;

        /// Тип элементов диапазона
        using ElementType = ValueT<ElementSerdes>;

        using ValueType = TValueType;

        // Длина поля с размером диапазона
        static constexpr uint8_t sizelen = sizeof(ValueT<TSizeSerdes>);

        static consteval
        SerdesTypeId GetSerdesTypeId() { return SerdesTypeId::Range; }

        [[nodiscard]] static consteval
        BufferType GetBufferType() { return BufferType::Dynamic; }

        [[nodiscard]] static consteval
        uint32_t Sizeof()
        {
            return utils::Safe<utils::policy::MaxValue>::Add(
                    static_cast<uint32_t>(sizelen),
                    utils::Safe<utils::policy::MaxValue>::Mul(
                        ElementSerdes::Sizeof(), // размер буфера для одного элемента
                        static_cast<uint32_t>(std::numeric_limits<ValueT<TSizeSerdes>>::max()))); // вместимость контейнера
        }

        /// @note Функция может возвратить WRONG_SIZE, если при вычислении результата
        /// произошло переполнение разрядной сетки или размер диапазона больше допустимого
        template<std::ranges::forward_range TRange>
        [[nodiscard]] static constexpr
        uint32_t Sizeof(const TRange &range)
        {
            // Проверка размера диапазона
            if(std::ranges::size(range) > std::numeric_limits<SizeType>::max())
                return WRONG_SIZE;

            uint32_t bufSize = SizeSerdes::Sizeof();

            // Если седес элементов диапазона использует статический буфер
            if constexpr (ElementSerdes::GetBufferType() == BufferType::Static)
                bufSize = utils::Safe<utils::policy::MaxValue>::Add(bufSize,
                                    utils::Safe<utils::policy::MaxValue>::Mul(static_cast<uint32_t>(std::ranges::size(range)),
                                                                ElementSerdes::Sizeof()));

            else // Cедес элементов диапазона использует динамический буфер, поэтому нужно просуммировать размеры всех элементов
                for(const auto &value: range)
                    bufSize = utils::Safe<utils::policy::MaxValue>::Add(bufSize, ElementSerdes::Sizeof(value));

            return bufSize;
        }

        template<COutputIterator TOutputIterator, std::ranges::forward_range TRange>
        static constexpr
        TOutputIterator SerializeTo(TOutputIterator bufpos, const TRange &range)
        {
            // Сериализация размера диапазона
            bufpos = SizeSerdes::SerializeTo(bufpos, std::ranges::size(range));

            // Сериализация элементов диапазона
            for(const auto &element: range)
                bufpos = ElementSerdes::SerializeTo(bufpos, element);

            return bufpos;
        }

        /// @note Если десериализованный размер диапазона некорректен, то возможен выход за границы буфера
        /// @throw std::out_of_range Исключение происходит если у заданного диапазона нет функции insert()
        /// или resize(), а размер текущего диапазона меньше необходимого
        template<CInputIterator TInputIterator, std::ranges::forward_range TRange>
        static constexpr
        TInputIterator DeserializeFrom(TInputIterator bufpos, TRange &range)
        {
            // Десериализация размера последовательности
            SizeType rangeSize{0};
            bufpos = SizeSerdes::DeserializeFrom(bufpos, rangeSize);

            // Если range является ассоциативным контейнером типа std::map, std::set и подобным.
            if constexpr (!requires { range.resize(rangeSize); } &&
                           requires { range.insert(std::ranges::range_value_t<TRange>{}); })
            {
                // Последовательная десериализация и вставка элементов
                for(SizeType i = 0; i < rangeSize; i++)
                {
                    ElementType value{};
                    bufpos = ElementSerdes::DeserializeFrom(bufpos, value);
                    range.insert(std::move(static_cast<std::ranges::range_value_t<TRange>>(value)));
                }
            }
            else
            {
                // Если range поддерживает изменение размера
                if constexpr (requires { range.resize(rangeSize); })
                    range.resize(rangeSize);
                else
                {
                    // Диапазон должен иметь размер не меньше десериализованного размера rangeSize
                    size_t n = std::ranges::size(range);
                    if (n < rangeSize)
                        throw std::out_of_range("Serdes/Core/Range.hpp/Deserialize(): incorrect range size");
                }

                // Десериализация элементов
                auto element = std::ranges::begin(range);
                for(size_t i = 0; i < rangeSize; i++)
                    bufpos = ElementSerdes::DeserializeFrom(bufpos, *element++);

            }
            return bufpos;
        }
    };

} // namespace serdes

//------------------------------------------------------------------------------
#endif

