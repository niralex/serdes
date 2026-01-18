#pragma once
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
        TOutputIterator Serialize(TOutputIterator bufpos, const ValueType &)
        {
            return bufpos;
        }

        template<COutputIterator TOutputIterator>
        static constexpr
        TOutputIterator Serialize(TOutputIterator bufpos) { return bufpos; }

        template<CInputIterator TInputIterator>
        static constexpr
        TInputIterator Deserialize(TInputIterator bufpos, ValueType &value)
        {
            return bufpos;
        }

        template<CInputIterator TInputIterator>
        static constexpr
        TInputIterator Deserialize(TInputIterator bufpos) { return bufpos; }

    };

} // serdes

//------------------------------------------------------------------------------
