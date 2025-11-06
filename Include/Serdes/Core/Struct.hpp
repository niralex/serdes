#ifndef SERDES_CORE_STRUCT_HPP
#define SERDES_CORE_STRUCT_HPP
//------------------------------------------------------------------------------
/** @file

    @brief  Serdes template for simple user-defined structs

    @details
        The Struct template is not a standalone serdes type but allows defining serdes aliases
        for user-defined structs. The struct/class members must be either public,
        or the serdes must be declared as a friend of the class.
        The serdes on which the Struct-based serdes is built is called the base serdes (Struct::BaseSerdes).

    @todo

    @author Niraleks

*/
//------------------------------------------------------------------------------
#include "Typeids.hpp"
#include "Concepts.hpp"
#include "Helpers.hpp"

//------------------------------------------------------------------------------
namespace serdes
{

    /// Serdes template for structs
    /// @tparam TStruct Struct/class type for which the serdes is defined
    /// @tparam TSerdes Serdes used as the base serdes
    /// @tparam Fields Pointers to member fields of the struct/class
    template<typename TStruct, CSerdes TSerdes, auto ...Fields>
    struct Struct
    {
        using ValueType = TStruct;

        /// Base serdes used for serializing/deserializing the struct
        using BaseSerdes = TSerdes;

        static consteval
        TypeId GetTypeId() { return BaseSerdes::GetTypeId(); }

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

} // namespace serdes

//------------------------------------------------------------------------------
#endif
