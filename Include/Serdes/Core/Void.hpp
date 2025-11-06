#ifndef SERDES_CORE_VOID_HPP
#define SERDES_CORE_VOID_HPP
//------------------------------------------------------------------------------
/** @file

    @brief  Serdes template for the void type

    @details
        Used as a placeholder for the void type

    @todo

    @author Niraleks
*/

//------------------------------------------------------------------------------
#include <variant>
#include "Concepts.hpp"
#include "Typeids.hpp"

//------------------------------------------------------------------------------
namespace serdes
{
    //--------------------------------------------------------------------------
    struct Void
    {
        using ValueType = std::monostate;

        static consteval
        TypeId GetTypeId() { return TypeId::Void; }

        static consteval
        BufferType GetBufferType() { return BufferType::Static; }

        [[nodiscard]] static constexpr
        uint32_t Sizeof() { return 0; }

        [[nodiscard]] static constexpr
        uint32_t Sizeof(const ValueType &) { return Sizeof(); }

        template<COutputIterator TOutputIterator>
        static constexpr
        TOutputIterator SerializeTo(TOutputIterator bufpos, const ValueType &)
        {
            return bufpos;
        }

        template<COutputIterator TOutputIterator>
        static constexpr
        TOutputIterator SerializeTo(TOutputIterator bufpos) { return bufpos; }

        template<CInputIterator TInputIterator>
        static constexpr
        TInputIterator DeserializeFrom(TInputIterator bufpos, ValueType &value)
        {
            return bufpos;
        }

        template<CInputIterator TInputIterator>
        static constexpr
        TInputIterator DeserializeFrom(TInputIterator bufpos) { return bufpos; }

    };

} // serdes

//------------------------------------------------------------------------------
#endif
