#pragma once

//------------------------------------------------------------------------------
/** @file

    @brief  Serdes descriptor of type TypeId::Range

    @details

    @todo

    @author Niraleks
*/
//-----------------------------------------------------------------------------
#include <Serdes/Serdes.hpp>
#include "Descriptor.hpp"

//------------------------------------------------------------------------------
namespace serdes
{
    //--------------------------------------------------------------------------
    class RangeDescriptor : public Descriptor
    {
        private:
            using FSizeLen = details::Field<UInt8, 8>;

            constexpr static inline
            uint16_t elementDescriptorOffset = 9;

        public:

            constexpr inline
            RangeDescriptor(const uint8_t *id) : Descriptor(id)
            {
                if(GetTypeId() != TypeId::Range)
                    utils::Throw<std::invalid_argument>("Invalid Range-serdes identifier");
            }

            /// Length of array size field
            [[nodiscard]] constexpr inline
            uint8_t GetSizeLen()const
            {
                return GetValue<FSizeLen>();
            }

            /// Range element descriptor
            [[nodiscard]] constexpr inline
            Descriptor GetElementDescriptor()const
            {
                return _id + elementDescriptorOffset;
            }
    };

} // serdes

//------------------------------------------------------------------------------


