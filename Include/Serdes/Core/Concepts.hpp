#ifndef SERDES_CORE_CONCEPTS_HPP
#define SERDES_CORE_CONCEPTS_HPP
//------------------------------------------------------------------------------
/**	@file

    @brief  Определения концептов

	@details

    @todo

    @author Niraleks
*/
//------------------------------------------------------------------------------
#include <iterator>
#include "Typeids.hpp"

//------------------------------------------------------------------------------
namespace serdes
{
    //--------------------------------------------------------------------------
    /// Концепт седеса (сериализатора-десериализатора)
    template<typename TSerdes>
    concept CSerdes = requires(
        uint8_t                           *bufpos,
        const uint8_t                     *cbufpos,
        typename TSerdes::ValueType       &value,
        const typename TSerdes::ValueType &cvalue)
    {
        /// Требование определения базового типа для которого предназначен седес
        typename TSerdes::ValueType;

        /// Требование наличия функций определения размера значений в сериализованном виде.
        { TSerdes::Sizeof() } -> std::same_as<uint32_t>;       // максимально возможное
        { TSerdes::Sizeof(cvalue) } -> std::same_as<uint32_t>; // для конкретного значения

        /// Требование наличия функций определения типа буфера.
        { TSerdes::GetBufferType() } -> std::same_as<BufferType>;

        /// Требование определения функции сериализации в подготовленный буфер.
        /// @param bufpos Итератор на позицию в буфере, начиная с которой необходимо выводить сериализованное значение
        /// @param value константная ссылка на значение, которое необходимо сериализовать.
        /// @return Итератор на позицию в буфере, следующую за сериализованным значением
        /// @note Функция не проверяет выход за границы буфера.
        { TSerdes::SerializeTo(bufpos, cvalue) } -> std::output_iterator<uint8_t>;

        /// Требование наличия функции десериализации из указанного буфера.
        /// @param cbufpos Итератор на позицию в буфере с сериализованным значением.
        /// @param[out] value Ссылка на переменную, куда будет десериализовано значение
        /// @return Итератор на позицию в буфере, следующую за десериализованным значением
        /// @note Функция не проверяет выход за границы буфера.
        { TSerdes::DeserializeFrom(cbufpos, value) } -> std::input_iterator;

    };

    //--------------------------------------------------------------------------
    /// Концепт итератора вывода
    // в отличие стандартного(std::output_iterator) требует только один параметр
    template<typename TIterator>
    concept COutputIterator = std::output_iterator<TIterator, uint8_t>
                              || std::output_iterator<TIterator, std::byte>;

    //--------------------------------------------------------------------------
    /// Концепт итератора ввода
    // Идентичен стандартному, добавлен для поддержки единообразия кода
    template<typename TIterator>
    concept CInputIterator = std::input_iterator<TIterator>;

    //--------------------------------------------------------------------------
    /// Концепт, определяющий требовние к типу T, чтобы он был одним из типов в пакете Ts...
    template<typename T, typename ... Ts>
    concept IsAnyOf = (std::same_as<T, Ts> || ...);

    //--------------------------------------------------------------------------
    /// Концепт Pod-типов
    template<typename ...T>
    concept CPod = ((std::is_standard_layout_v<T> && std::is_trivial_v<T>) && ...);

    //--------------------------------------------------------------------------
    /// Концепт проверяющий возможность явной конвертации из типа From в тип To
    template<typename From, typename To>
    concept CExplicitlyConvertible = requires(From ob) { static_cast<To>(ob); };

    //--------------------------------------------------------------------------
    /// Концепт, проверяющий, является ли тип T подобным кортежу (tuple-like)
    namespace details
    {
        template<class T>
        inline constexpr bool isTupleLikeV = false;

        template<class... Elems>
        inline constexpr bool isTupleLikeV<std::tuple<Elems...>> = true;

        template<class T1, class T2>
        inline constexpr bool isTupleLikeV<std::pair<T1, T2>> = true;

        template<class T, size_t N>
        inline constexpr bool isTupleLikeV<std::array<T, N>> = true;

        template<class It, class Sent, std::ranges::subrange_kind Kind>
        inline constexpr bool isTupleLikeV<std::ranges::subrange<It, Sent, Kind>> = true;
    }

    template<class T>
    concept CTupleLike = details::isTupleLikeV<std::remove_cvref_t<T>>;

    //--------------------------------------------------------------------------
    // Концепт указателя на тип T
    template <typename TPointer, typename T>
    concept CPointerLike =
    requires (TPointer ptr)
    {
        { *ptr }-> std::convertible_to<T>;
        { static_cast<bool>(ptr) };
    }
    || std::same_as<std::nullptr_t, TPointer>;

} // serdes

//------------------------------------------------------------------------------
#endif


