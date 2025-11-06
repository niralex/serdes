#ifndef SERDES_CORE_CUSTOM_HPP
#define SERDES_CORE_CUSTOM_HPP
//------------------------------------------------------------------------------
/** @file

    @brief Templates for defining custom serdes

    @details
		The Custom template is not a standalone serdes type; it only allows defining
        an alias of another serdes for a user-defined type.
        The serdes on which the Custom serdes is based is called the base serdes (Custom::BaseSerdes).
        The serdes can serialize both user-defined objects and values of the base type.
        To define a custom serializer, conversion functions must be provided.
        When defining a Custom serdes, the user must supply functors for converting
        between their type and the base serdes's ValueType. This approach ensures that
        serialization/deserialization is performed solely using already-defined serdes,
        preventing users from implementing their own algorithms or formats.
        Ultimately, this improves code reliability by relying on a known set of
        base serializers, with all custom serializers built on top of them.

        The Custom template is more flexible but slower compared to the Struct serdes.

    @todo

    @author Niraleks

*/
//------------------------------------------------------------------------------
#include "Concepts.hpp"
#include "Helpers.hpp"

//------------------------------------------------------------------------------
namespace serdes
{
    //--------------------------------------------------------------------------
    /// Serdes template for user-defined types
    /// @tparam T User-defined type
    /// @tparam TBaseSerdes Base serdes used for the user-defined type
    /// @tparam ConvToBase Callable entity (function pointer, functor, or lambda) that
    /// converts a value of the user-defined type to a value of type TBaseSerdes::ValueType
    /// @tparam ConvFromBase Callable entity (function pointer, functor, or lambda) that
    /// converts a value of type TBaseSerdes::ValueType to a value of type T
    template<typename T, CSerdes TBaseSerdes,
             auto ConvToBase = [](const ValueT<TBaseSerdes> &baseValue){ return static_cast<ValueT<TBaseSerdes>>(baseValue); },
             auto ConvFromBase = [](ValueT<TBaseSerdes> &&baseValue, T &ob){ ob = static_cast<T>(baseValue); }>
    struct Custom
    {
        // Value type
        using ValueType = T;

        // Base serdes
        using BaseSerdes = TBaseSerdes;

        // Base value type
        using BaseValueType = ValueT<BaseSerdes>;

        static consteval
        TypeId GetTypeId() { return BaseSerdes::GetTypeId(); }

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
