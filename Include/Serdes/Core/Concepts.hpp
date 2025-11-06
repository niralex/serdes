#ifndef SERDES_CORE_CONCEPTS_HPP
#define SERDES_CORE_CONCEPTS_HPP
//------------------------------------------------------------------------------
/**	@file

    @brief  Concept definitions

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

    /// Serdes (serializer-deserializer) concept
    template<typename TSerdes>
    concept CSerdes = requires(
        uint8_t                           *bufpos,
        const uint8_t                     *cbufpos,
        typename TSerdes::ValueType       &value,
        const typename TSerdes::ValueType &cvalue)
    {
        /// Requirement: the base type for which the serdes is intended must be defined
        typename TSerdes::ValueType;

        /// Requirement: presence of a function to obtain the serdes type identifier
        { TSerdes::GetTypeId() } -> std::same_as<TypeId>;

        /// Requirement: functions to determine the serialized size of values must exist
        { TSerdes::Sizeof() } -> std::same_as<uint32_t>;       // maximum possible size
        { TSerdes::Sizeof(cvalue) } -> std::same_as<uint32_t>; // size for a specific value

        /// Requirement: functions to determine the buffer type must exist.
        { TSerdes::GetBufferType() } -> std::same_as<BufferType>;

        /// Requirement: a serialization function into a pre-allocated buffer must exist.
        /// @param bufpos Iterator pointing to the buffer position where serialization should start
        /// @param value Constant reference to the value to be serialized
        /// @return Iterator pointing to the buffer position immediately after the serialized value
        /// @note This function does not perform bounds checking.
        { TSerdes::SerializeTo(bufpos, cvalue) } -> std::output_iterator<uint8_t>;

        /// Requirement: a deserialization function from a given buffer must exist.
        /// @param cbufpos Iterator pointing to the serialized value in the buffer
        /// @param[out] value Reference to the variable where the deserialized value will be stored
        /// @return Iterator pointing to the buffer position immediately after the deserialized value
        /// @note This function does not perform bounds checking.
        { TSerdes::DeserializeFrom(cbufpos, value) } -> std::input_iterator;

    };

    /// Output iterator concept
    // Unlike the standard std::output_iterator, this concept takes only one parameter
    template<typename TIterator>
    concept COutputIterator = std::output_iterator<TIterator, uint8_t>
                              || std::output_iterator<TIterator, std::byte>;

    /// Input iterator concept
    // Identical to the standard concept; added for code consistency
    template<typename TIterator>
    concept CInputIterator = std::input_iterator<TIterator>;

    /// Concept requiring type T to be one of the types in the pack Ts...
    template<typename T, typename ... Ts>
    concept IsAnyOf = (std::same_as<T, Ts> || ...);

    /// POD types concept
    template<typename ...T>
    concept CPod = ((std::is_standard_layout_v<T> && std::is_trivial_v<T>) && ...);

    /// Concept checking whether type From can be explicitly converted to type To
    template<typename From, typename To>
    concept CExplicitlyConvertible = requires(From ob) { static_cast<To>(ob); };

    /// Concept checking whether type T is tuple-like
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

    /// Pointer-like concept for type TPointer pointing to type T
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


