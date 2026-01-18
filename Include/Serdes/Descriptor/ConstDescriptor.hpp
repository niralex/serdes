#pragma once

//------------------------------------------------------------------------------
/** @file

    @brief  Serdes descriptor of type TypeId::Const

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
    class ConstDescriptor : public Descriptor
    {
        private:
            constexpr static inline
            uint16_t serdesDescriptorOffset = 8;

        public:

            constexpr inline
            ConstDescriptor(const uint8_t *id) : Descriptor(id)
            {
                if(GetTypeId() != TypeId::Const)
                    utils::Throw<std::invalid_argument>("Invalid Const-serdes identifier");
            }

            /// Serdes descriptor for constant value
            [[nodiscard]] constexpr inline
            Descriptor GetSerdesDescriptor()const
            {
                return Descriptor(_id + serdesDescriptorOffset);
            }

            /// Pointer to value buffer
            [[nodiscard]] constexpr inline
            const uint8_t *GetValueBuffer()const
            {
                return _id + serdesDescriptorOffset + GetSerdesDescriptor().GetSize();
            }
    };

} // serdes

//------------------------------------------------------------------------------


